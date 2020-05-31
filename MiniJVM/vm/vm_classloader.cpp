#include "vm_classloader.h"
#include "string_utils.h"
#include "log.h"
#include "vm.h"
#include "vm_method_area.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>

#undef DEBUGCLASSLOADER


ClassLoader::ClassLoader(weak_ptr<ClassLoader> p) : parent(p) {}

bool ClassLoader::classLoaded(const wstring &className)
{
	//check if class has been loaded.
	auto ma = VM::getVM().lock()->getMethodArea();
	return ma.lock()->classExists(className);
}

weak_ptr<VMClass> ClassLoader::getStoredClass(const wstring &className) const
{
	auto ma = VM::getVM().lock()->getMethodArea();
	return ma.lock()->get(className);
}

weak_ptr<VMClass> ClassLoader::defineClass(shared_ptr<Buffer> buf)
{
	auto cf = make_shared<ClassFile>(buf);
	if (cf != nullptr)
	{
		auto className = cf->getCanonicalClassName();
		if (classLoaded(className))
		{
#ifdef DEBUGCLASSLOADER
			spdlog::info("Class:{} had been loaded.", w2s(className));
#endif 
			return getStoredClass(className);
		}

		if (!cf->isJavaClassFile())
		{
			throw runtime_error("Is not supported class file:" + w2s(buf->getMappingFile()));
		}
		if (!cf->isSupportedClassFile())
		{
			throw runtime_error("Unsupported class file version:" + std::to_string(cf->major_version) + "." + std::to_string(cf->minor_version));
		}

		// 这里是需要真正存储class，所以用shared_ptr.
		shared_ptr<VMLoadableClass> clz = nullptr;

		if (cf->isInterface())
		{
			clz = make_shared<VMInterfaceClass>(className, getSharedPtr());
		}
		else
		{
			clz = make_shared<VMOrdinaryClass>(className, getSharedPtr());
		}
		auto ma = VM::getVM().lock()->getMethodArea().lock();

		if (ma->put(className, clz))
		{

			if (clz->loadClassInfo(cf))
			{
				// 把常量放到常量池中
				ma->putClassConstantPool(cf, clz);
				for (auto waiter = clz->loadWaitingList.begin(); waiter != clz->loadWaitingList.end(); waiter++)
				{
					if (!(*waiter).expired())
					{
						(*waiter).lock()->classLoaded(clz);
					}
				}
				clz->loadWaitingList.clear();
			}
			else
			{
				ma->remove(className);
				throw runtime_error("Unable to load class:" + w2s(className));
			}
		}
		else
		{
			throw runtime_error("Unable to put class:" + w2s(className));
		}
		return clz;
	}

	spdlog::error("Cannot load class:{}", w2s(buf->getMappingFile()));

	return std::weak_ptr<VMClass>();
}

weak_ptr<VMClass> ClassLoader::defineClass(const wstring &sym)
{
	if (sym.length() == 0)
	{
		throw runtime_error("Error in resolverArray, left sym is empty.");
	}
	auto ma = VM::getVM().lock()->getMethodArea().lock();

	weak_ptr<VMClass> newClass = ma->get(sym);

	// 如果类已经存在，则直接返回。
	if (!newClass.expired())
	{
		return newClass;
	}

	if (sym[0] == L'[')
	{
#if 0 
		/* 这代码有一个严重的问题，新创建的arrayclass还没有存放到ma里就失效了，所以需要新定义一个defineArrayClass来解决。 */
		wstring subName = sym.substr(1, sym.length() - 1);
		/* 这里有一个bug,必须要先把subComponentType变成share_ptr放到MA里，
		否则递归调用的时候会让这个产生的类被释放。 */
		auto subComponentType = defineClass(subName).lock();
		auto cl = getSharedPtr();
		auto newArrayClass = make_shared<VMArrayClass>(sym, subComponentType, cl);
		newArrayClass->super = loadClass(L"java/lang/Object");
		// 新创建了一个类，需要放到类常量池里。
		ma->put(sym, newArrayClass);
		newClass = newArrayClass;
#endif
		newClass = defineArrayClass(sym);
	}
	else if (sym[0] == L'L')
	{
		// 如果是正常的引用类型。
		wstring className = sym.substr(1, sym.length() - 2);
		newClass = loadClass(className);
	}
	else
	{
		// 到了这里应该只剩下一个字符了。
		assert(sym.length() == 1);
		if (!VMPrimitiveClass::isPrimitiveTypeSignature(sym))
		{
			throw runtime_error("Unsupported type:" + w2s(sym));
		}
		newClass = VMPrimitiveClass::getPrimitiveVMClass(sym);
	}
	return newClass;
}

shared_ptr<VMClass> ClassLoader::defineArrayClass(const wstring& sym)
{
	if (sym.length() < 2 || sym[0] != L'[') {
		throw runtime_error("Not an array signature:" + w2s(sym));
	}
	wstring subName = sym.substr(1, sym.length() - 1);
	shared_ptr<VMClass> subComponent = nullptr;
	if (subName[0] == L'[')
	{
		// 如果还是数组，就递归创建。
		subComponent = defineArrayClass(subName);
	}
	else {
		// 如果不是数组类，那么这个类肯定能找到，而且已经有引用了。所以lock一定能成功。
		subComponent = defineClass(subName).lock();
	}
	auto ma = VM::getVM().lock()->getMethodArea().lock();
	auto newArrayClass = make_shared<VMArrayClass>(sym, subComponent, getSharedPtr());
	newArrayClass->super = loadClass(L"java/lang/Object");
	// 新创建了一个类，需要放到类常量池里。
	ma->put(sym, newArrayClass);
	return newArrayClass;
}

weak_ptr<VMModule> ClassLoader::defineModule(const wstring& classRootPath) {
	std::filesystem::path mi(classRootPath + L"/module-info.class");
	auto vm = VM::getVM().lock();
	if (std::filesystem::is_regular_file(mi) && std::filesystem::exists(mi))
	{
		auto buffer = Buffer::fromFile(mi.wstring());
		auto cf = make_shared<ClassFile>(buffer);
		if (cf->isModule()) {
			auto m = std::dynamic_pointer_cast<Module_attribute>(cf->getAttributeByName(L"Module"));
			auto name = cf->getModuleName(m->module_name_index);
			auto existingModule = vm->getModule(name);
			if (!existingModule.expired()) {
				return existingModule;
			}
			auto newModule = make_shared<VMModule>(name, getSharedPtr());
			newModule->flags = m->module_flags;
			newModule->version = cf->getUtf8String(m->module_version_index);

			// Load Module Info;
			for (auto r = m->requires.begin(); r != m->requires.end(); r++)
			{
				auto req = (*r);
				auto mr = make_shared<VMModuleRequire>();
				mr->flags = req->requires_flags;
				mr->name = cf->getModuleName(req->requires_index);
				mr->requiredVersion = cf->getUtf8String(req->requires_version_index);
				newModule->requires[mr->name] = mr;
			}

			for (auto e = m->exports.begin(); e != m->exports.end(); e++)
			{
				auto exp = (*e);
				auto me = make_shared<VMModuleExport>();
				me->name = cf->getPackageName(exp->exports_index);
				me->flags = exp->exports_flags;
				for (auto eto = exp->exports_to_index.begin(); eto != exp->exports_to_index.end(); eto++)
				{
					me->to.insert(cf->getModuleName(*eto));
				}
				newModule->exports[me->name] = me;
			}

			for (auto o = m->opens.begin(); o != m->opens.end(); o++) {
				auto op = (*o);
				auto mo = make_shared<VMModuleOpen>();
				mo->name = cf->getPackageName(op->opens_index);
				mo->flags = op->opens_flags;
				for (auto oto = op->opens_to_index.begin(); oto != op->opens_to_index.end(); oto++)
				{
					mo->to.insert(cf->getModuleName(*oto));
				}
				newModule->opens[mo->name] = mo;
			}

			for (auto u = m->uses_index.begin(); u != m->uses_index.end(); u++)
			{
				newModule->uses.insert(cf->getClassName(*u));
			}

			for (auto p = m->provides.begin(); p != m->provides.end(); p++)
			{
				auto pr = (*p);
				auto provide = make_shared<VMModuleProvide>();
				provide->name = cf->getClassName(pr->provides_index);
				for (auto pw = pr->provides_with_index.begin(); pw != pr->provides_with_index.end(); pw++)
				{
					provide->with.insert(cf->getClassName(*pw));
				}
				newModule->provides[provide->name] = provide;
			}

			// Load Main class
			auto mainClass = std::dynamic_pointer_cast<ModuleMainClass_attribute>(cf->getAttributeByName(L"ModuleMainClass"));
			if (mainClass != nullptr)
			{
				newModule->mainClassName = cf->getClassName(mainClass->main_class_index);
			}
			// Load packages.
			auto mp = std::dynamic_pointer_cast<ModulePackages_attribute>(cf->getAttributeByName(L"ModulePackages"));
			if(mp != nullptr)
			{
				for (auto pi = mp->package_index.begin(); pi != mp->package_index.end(); pi++)
				{
					newModule->packages.insert(cf->getPackageName(*pi));
				}
			}
			spdlog::info("Load module:{}", w2s(newModule->name));
			vm->putModule(newModule->name, newModule);
			return newModule;
		}
		spdlog::warn("File: {} is module a module file, we will make this folder unamed module.", w2s(mi.wstring()));
	}
	spdlog::warn("No module file: {} found, we will make this folder unamed module.", w2s(mi.wstring()));
	auto unnamed = vm->getModule(VMModule::UNNAMED_MODULE);
	if (unnamed.expired())
	{
		auto nm = make_shared<VMModule>(VMModule::UNNAMED_MODULE, getSharedPtr());
		vm->putModule(VMModule::UNNAMED_MODULE, nm);
		unnamed = nm;
		spdlog::info("Create unnamed module.");
	}
	return unnamed;
}

BootstrapClassLoader::BootstrapClassLoader(const wstring &cp, weak_ptr<ClassLoader> p) : ClassLoader(p), bootstrapClassPath(cp), classLoaderName(L"bootstrapClassLoader")
{
}

BootstrapClassLoader::BootstrapClassLoader(weak_ptr<ClassLoader> p) : ClassLoader(p), classLoaderName(L"bootstrapClassLoader")
{
}

weak_ptr<VMClass> BootstrapClassLoader::loadClass(const wstring &className)
{
	wstring canonicalClassPath(className);
	// replace . with /
	replaceAll(canonicalClassPath, L".", L"/");

	if (classLoaded(canonicalClassPath))
	{
#ifdef DEBUGCLASSLOADER
		spdlog::info("Class:{} had been loaded.", w2s(canonicalClassPath));
#endif
		return getStoredClass(canonicalClassPath);
	}

	std::filesystem::path clazz(getClassRootPath() + L"/" + canonicalClassPath + L".class");
#ifdef DEBUGCLASSLOADER
	spdlog::info("Tried to load class {} from BootstrapClassLoader", w2s(clazz.wstring()));
#endif
	if (std::filesystem::is_regular_file(clazz) && std::filesystem::exists(clazz))
	{
		auto buffer = Buffer::fromFile(clazz.wstring());
		auto clz = loadClass(buffer);
		auto clzModule = defineModule(getClassRootPath());
		clz.lock()->setModule(clzModule);
		return clz;
	}
	spdlog::error("Cannot laod class:{}", w2s(clazz.wstring()));
	return std::weak_ptr<VMClass>();
}

weak_ptr<VMClass> BootstrapClassLoader::loadClass(shared_ptr<Buffer> buf)
{
	return defineClass(buf);
}

AppClassLoader::AppClassLoader(const wstring &appClassPath, weak_ptr<ClassLoader> p) : BootstrapClassLoader(p),
																					   appClassRootPath(appClassPath),
																					   classLoaderName(L"AppClassLoader")
{
}

weak_ptr<VMClass> AppClassLoader::loadClass(const wstring &className)
{

	auto parentReadClass = parent.lock()->loadClass(className);
	if (!parentReadClass.expired())
	{
#ifdef DEBUGCLASSLOADER
		spdlog::info("Class:{} read from parent classloader", w2s(className));
#endif 
		return parentReadClass;
	}

	wstring canonicalClassPath(className);
	// 如果是 Ljava/lang/String;这种形式，去掉头尾。
	if (canonicalClassPath[0] == L'L' && canonicalClassPath[canonicalClassPath.size() - 1] == L';') {
		canonicalClassPath = canonicalClassPath.substr(1, canonicalClassPath.size() - 2);
	}
	// replace . with /
	replaceAll(canonicalClassPath, L".", L"/");

	if (classLoaded(canonicalClassPath))
	{
#ifdef DEBUGCLASSLOADER
		spdlog::info("Class:{} had been loaded.", w2s(canonicalClassPath));
#endif 
		return getStoredClass(canonicalClassPath);
	}

	std::filesystem::path clazz(getClassRootPath() + L"/" + canonicalClassPath + L".class");
#ifdef DEBUGCLASSLOADER
	spdlog::info("Tried to load class {} from BootstrapClassLoader", w2s(clazz.wstring()));
#endif 
	if (std::filesystem::is_regular_file(clazz) && std::filesystem::exists(clazz))
	{
		auto buffer = Buffer::fromFile(clazz.wstring());
		auto clz = loadClass(buffer);
		auto clzModule = defineModule(getClassRootPath());
		clz.lock()->setModule(clzModule);
		return clz;
	}
	spdlog::error("Cannot laod class:{}", w2s(clazz.wstring()));
	return std::weak_ptr<VMClass>();
}

weak_ptr<VMClass> AppClassLoader::loadClass(shared_ptr<Buffer> buf)
{
	auto parentReadClass = parent.lock()->loadClass(buf);
	if (!parentReadClass.expired())
	{
#ifdef DEBUGCLASSLOADER
		spdlog::info("Class:{} read from parent classloader", w2s(buf->getMappingFile()));
#endif
		return parentReadClass;
	}
	return defineClass(buf);
}