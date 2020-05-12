#include "vm_classloader.h"
#include "string_utils.h"
#include "log.h"
#include "vm.h"
#include "vm_method_area.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>

ClassLoader::ClassLoader(shared_ptr<ClassLoader> p) : parent(p) {}


bool ClassLoader::classLoaded(const wstring& className) {
	//check if class has been loaded.
	auto ma = VM::getVM()->getMethodArea();
	return ma->classExists(className);
}

shared_ptr<VMClass> ClassLoader::getStoredClass(const wstring& className) const{
	auto ma = VM::getVM()->getMethodArea();
	return ma->get(className);
}


shared_ptr<VMClass> ClassLoader::defineClass(shared_ptr<Buffer> buf) {
	auto cf = make_shared<ClassFile>(buf);
	if (cf != nullptr) {
		auto className = cf->getCanonicalClassName();
		if (classLoaded(className)) {
			spdlog::info("Class:{} had been loaded.", w2s(className));
			return getStoredClass(className);
		}

		if (!cf->isJavaClassFile()) {
			throw runtime_error("Is not supported class file:" + w2s(buf->getMappingFile()));
		}
		if (!cf->isSupportedClassFile()) {
			throw runtime_error("Unsupported class file version:" + std::to_string(cf->major_version) + "." + std::to_string(cf->minor_version));
		}
		shared_ptr<VMLoadableClass> clz = nullptr;
		
		if (cf->isInterface()) {
			clz = make_shared<VMInterfaceClass>(className, getSharedPtr());
		}
		else {
			clz = make_shared<VMOrdinaryClass>(className, getSharedPtr());
		}
		auto ma = VM::getVM()->getMethodArea();

		if (ma->put(className, clz)) {

			if (clz->loadClassInfo(cf)) {
				// 把常量放到常量池中
				ma->putClassConstantPool(cf, clz);
			}
			else {
				ma->remove(className);
				throw runtime_error("Unable to load class:" + w2s(className));
			}
		}
		else {
			throw runtime_error("Unable to put class:" + w2s(className));
		}
		return clz;
	}
	spdlog::error("Cannot load class:{}", w2s(buf->getMappingFile()));

	return nullptr;
}

shared_ptr<VMClass> ClassLoader::defineClass(const wstring & sym) {
	if (sym.length() == 0) {
		throw runtime_error("Error in resolverArray, left sym is empty.");
	}
	auto ma = VM::getVM()->getMethodArea();

	shared_ptr<VMClass> componentType = ma->get(sym);

	// 如果类已经存在，则直接返回。
	if (componentType != nullptr) {
		return componentType;
	}

	if (sym[0] == L'[') {
		// 如果还是数组，则递归
		wstring subName = sym.substr(1, sym.length() - 1);
		auto subComponentType = defineClass(subName);
		componentType = make_shared<VMArrayClass>(subName, subComponentType, getSharedPtr());
		componentType->super = loadClass(L"java/lang/Object");
		// 新创建了一个类，需要放到类常量池里。
		ma->put(sym, componentType);
	}
	else if (sym[0] == L'L') {
		// 如果是正常的引用类型。
		wstring className = sym.substr(1, sym.length() - 2);
		componentType = loadClass(className);
	}
	else {
		// 到了这里应该只剩下一个字符了。
		assert(sym.length() == 1);
		if (VMPrimitiveClass::AllPrimitiveClasses.find(sym) == VMPrimitiveClass::AllPrimitiveClasses.end()) {
			throw runtime_error("Unsupported type:" + w2s(sym));
		}
		componentType = VMPrimitiveClass::AllPrimitiveClasses.find(sym)->second;
	}
	return componentType;
}
BootstrapClassLoader::BootstrapClassLoader(const wstring& cp, shared_ptr<ClassLoader> p):
	ClassLoader(p), bootstrapClassPath(cp), classLoaderName(L"bootstrapClassLoader") {

}

BootstrapClassLoader::BootstrapClassLoader(shared_ptr<ClassLoader> p) :
	ClassLoader(p), classLoaderName(L"bootstrapClassLoader") {

}

shared_ptr<VMClass> BootstrapClassLoader::loadClass(const wstring& className) {
	wstring canonicalClassPath(className);
	// replace . with /
	replaceAll(canonicalClassPath, L".", L"/");

	if (classLoaded(canonicalClassPath)) {
		spdlog::info("Class:{} had been loaded.", w2s(canonicalClassPath));
		return getStoredClass(canonicalClassPath);
	}

	std::filesystem::path clazz(getClassRootPath() + L"/" + canonicalClassPath + L".class");

	spdlog::info("Tried to load class {} from BootstrapClassLoader", w2s(clazz.wstring()));

	if (std::filesystem::is_regular_file(clazz) && std::filesystem::exists(clazz)) {
		auto buffer = Buffer::fromFile(clazz.wstring());
		return loadClass(buffer);
	}
	spdlog::error("Cannot laod class:{}", w2s(clazz.wstring()));
	return nullptr;
}

shared_ptr<VMClass> BootstrapClassLoader::loadClass(shared_ptr<Buffer> buf) {
	return defineClass(buf);
}

AppClassLoader::AppClassLoader(const wstring& appClassPath, shared_ptr<ClassLoader> p):
	BootstrapClassLoader(p),
	appClassRootPath(appClassPath),
	classLoaderName(L"AppClassLoader"){

}

shared_ptr<VMClass> AppClassLoader::loadClass(const wstring& className) {

	auto parentReadClass = parent->loadClass(className);
	if (parentReadClass != nullptr) {
		spdlog::info("Class:{} read from parent classloader", w2s(className));
		return parentReadClass;
	}

	wstring canonicalClassPath(className);
	// replace . with /
	replaceAll(canonicalClassPath, L".", L"/");

	if (classLoaded(canonicalClassPath)) {
		spdlog::info("Class:{} had been loaded.", w2s(canonicalClassPath));
		return getStoredClass(canonicalClassPath);
	}

	std::filesystem::path clazz(getClassRootPath() + L"/" + canonicalClassPath + L".class");

	spdlog::info("Tried to load class {} from BootstrapClassLoader", w2s(clazz.wstring()));

	if (std::filesystem::is_regular_file(clazz) && std::filesystem::exists(clazz)) {
		auto buffer = Buffer::fromFile(clazz.wstring());
		return loadClass(buffer);
	}
	spdlog::error("Cannot laod class:{}", w2s(clazz.wstring()));
	return nullptr;
}

shared_ptr<VMClass> AppClassLoader::loadClass(shared_ptr<Buffer> buf) {
	auto parentReadClass = parent->loadClass(buf);
	if (parentReadClass != nullptr) {
		spdlog::info("Class:{} read from parent classloader", w2s(buf->getMappingFile()));
		return parentReadClass;
	}
	return defineClass(buf);
}