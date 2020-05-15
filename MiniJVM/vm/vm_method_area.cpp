#include "vm_method_area.h"
#include "log.h"
#include "string_utils.h"
#include "clazz_reader.h"
#include "pthread.h"
#include "vm_thread.h"
#include "vm_heap.h"

shared_ptr<VMClassConstantPool> VMMethodArea::createVMClassConstantPool(shared_ptr<ClassFile> cf, shared_ptr<VMClass> clz)
{
	auto cp = cf->constant_pool;
	vector<shared_ptr<VMConstantItem>> cs;
	cs.push_back(make_shared<VMConstantDummy>());
	// 因为第一个是无效的，所以从+1开始.
	for (auto c = cp.begin() + 1; c != cp.end(); c++)
	{
		auto p = *c;
		switch (p->tag)
		{
		case ConstantPoolType::Utf8:
		{
			auto utf8 = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(p);
			auto index = putConstantString(utf8->toUTF8String());
			cs.push_back(make_shared<VMConstantStringLiteral>(index, VMConstantItem::VMConstantItemType::TypeUtf8));
			break;
		}
		case ConstantPoolType::Integer:
		{
			auto i = std::dynamic_pointer_cast<CONSTANT_Integer_info>(p);
			cs.push_back(make_shared<VMConstantInteger>(i->bytes));
			break;
		}
		case ConstantPoolType::Float:
		{
			auto f = std::dynamic_pointer_cast<CONSTANT_Float_info>(p);
			cs.push_back(make_shared<VMConstantFloat>(f->bytes));
			break;
		}
		case ConstantPoolType::Long:
		{
			auto l = std::dynamic_pointer_cast<CONSTANT_Long_info>(p);
			cs.push_back(make_shared<VMConstantLongAndDouble>(l->high_bytes, l->low_bytes, VMConstantItem::VMConstantItemType::TypeLong));

			// 占用下一个slots，同时也要路过下一个。
			cs.push_back(make_shared<VMConstantDummy>());
			c++;
			break;
		}
		case ConstantPoolType::Double:
		{
			auto l = std::dynamic_pointer_cast<CONSTANT_Double_info>(p);
			cs.push_back(make_shared<VMConstantLongAndDouble>(l->high_bytes, l->low_bytes, VMConstantItem::VMConstantItemType::TypeDouble));

			// 占用下一个slots,同时也要路过下一个。
			cs.push_back(make_shared<VMConstantDummy>());
			c++;
			break;
		}
		case ConstantPoolType::Class:
		{
			auto clz = std::dynamic_pointer_cast<CONSTANT_Class_info>(p);
			auto clzName = cf->getUtf8String(clz->name_index);
			auto index = putConstantString(clzName);
			cs.push_back(make_shared<VMConstantStringLiteral>(index, VMConstantItem::VMConstantItemType::TypeClass));
			break;
		}
		case ConstantPoolType::String:
		{
			auto str = std::dynamic_pointer_cast<CONSTANT_String_info>(p);
			auto name = cf->getUtf8String(str->string_index);
			auto index = putConstantString(name);
			cs.push_back(make_shared<VMConstantStringLiteral>(index, VMConstantItem::VMConstantItemType::TypeString));
			break;
		}
		case ConstantPoolType::Fieldref:
		{
			auto ci = std::dynamic_pointer_cast<CONSTANT_Fieldref_info>(p);
			auto clzName = cf->getClassName(ci->class_index);
			auto nat = cf->getNameAndType(ci->name_and_type_index);

			auto cIndex = putConstantString(clzName);
			auto nIndex = putConstantString(nat.first);
			auto sIndex = putConstantString(nat.second);
			cs.push_back(make_shared<VMConstantFieldAndMethodRef>(cIndex, nIndex, sIndex, VMConstantItem::VMConstantItemType::TypeFieldRef));
			break;
		}
		case ConstantPoolType::Methodref:
		{
			auto ci = std::dynamic_pointer_cast<CONSTANT_Methodref_info>(p);
			auto clzName = cf->getClassName(ci->class_index);
			auto nat = cf->getNameAndType(ci->name_and_type_index);

			auto cIndex = putConstantString(clzName);
			auto nIndex = putConstantString(nat.first);
			auto sIndex = putConstantString(nat.second);
			cs.push_back(make_shared<VMConstantFieldAndMethodRef>(cIndex, nIndex, sIndex, VMConstantItem::VMConstantItemType::TypeMethodRef));
			break;
		}
		case ConstantPoolType::InterfaceMethodref:
		{
			auto ci = std::dynamic_pointer_cast<CONSTANT_InterfaceMethodref_info>(p);
			auto clzName = cf->getClassName(ci->class_index);
			auto nat = cf->getNameAndType(ci->name_and_type_index);

			auto cIndex = putConstantString(clzName);
			auto nIndex = putConstantString(nat.first);
			auto sIndex = putConstantString(nat.second);
			cs.push_back(make_shared<VMConstantFieldAndMethodRef>(cIndex, nIndex, sIndex, VMConstantItem::VMConstantItemType::TypeInterfaceMothedRef));
			break;
		}
		case ConstantPoolType::NameAndType:
		{
			// 因为我是和ClassFile里一一对应的，所以直接取下一个就是。
			auto nat = cf->getNameAndType(cs.size());

			auto nIndex = putConstantString(nat.first);
			auto sIndex = putConstantString(nat.second);
			cs.push_back(make_shared<VMConstantFieldAndMethodRef>(0, nIndex, sIndex, VMConstantItem::VMConstantItemType::TypeNameAndType));
			break;
		}
		case ConstantPoolType::MethodHandle:
		{
			auto ci = std::dynamic_pointer_cast<CONSTANT_MethodHandle_info>(p);
			u1 kind = ci->reference_kind;
			wstring clzName;
			std::pair<wstring, wstring> nat;

			switch (kind)
			{
			case VMConstantMethodHandle::RefType::REF_getField:
			case VMConstantMethodHandle::RefType::REF_putField:
			case VMConstantMethodHandle::RefType::REF_getStatic:
			case VMConstantMethodHandle::RefType::REF_putStatic:
			{
				// CONSTANT_Fieldref_info
				auto ri = std::dynamic_pointer_cast<CONSTANT_Fieldref_info>(cp[ci->reference_index]);
				clzName = cf->getClassName(ri->class_index);
				nat = cf->getNameAndType(ri->name_and_type_index);
				break;
			}
			case VMConstantMethodHandle::RefType::REF_invokeVirtual:
			case VMConstantMethodHandle::RefType::REF_newInvokeSpecial:
			case VMConstantMethodHandle::RefType::REF_invokeStatic:
			case VMConstantMethodHandle::RefType::REF_invokeSpecial:
			{
				// CONSTANT_Methodref_info
				/*
				If the value of the reference_kind item is 6 (REF_invokeStatic) or 7 (REF_invokeSpecial), 
				then if the class file version number is less than 52.0, 
				the constant_pool entry at that index must be a CONSTANT_Methodref_info structure representing a class's method for which a method handle is to be created;
				if the class file version number is 52.0 or above, 
				the constant_pool entry at that index must be either a CONSTANT_Methodref_info structure or a CONSTANT_InterfaceMethodref_info structure (§4.4.2)
				representing a class's or interface's method for which a method handle is to be created.
				*/
				auto methodRef = std::dynamic_pointer_cast<CONSTANT_Methodref_info>(cp[ci->reference_index]);
				auto interfaceMethodRef = std::dynamic_pointer_cast<CONSTANT_InterfaceMethodref_info>(cp[ci->reference_index]);
				if (methodRef != nullptr)
				{
					clzName = cf->getClassName(methodRef->class_index);
					nat = cf->getNameAndType(methodRef->name_and_type_index);
				}
				else
				{
					clzName = cf->getClassName(interfaceMethodRef->class_index);
					nat = cf->getNameAndType(interfaceMethodRef->name_and_type_index);
				}
				break;
			}
			case VMConstantMethodHandle::RefType::REF_invokeInterface:
			{
				// CONSTANT_InterfaceMethodref_info
				auto ri = std::dynamic_pointer_cast<CONSTANT_InterfaceMethodref_info>(cp[ci->reference_index]);
				clzName = cf->getClassName(ri->class_index);
				nat = cf->getNameAndType(ri->name_and_type_index);
				break;
			}
			default:
			{
				throw runtime_error("Unknown reference kind:" + kind);
			}
			}
			auto cIndex = putConstantString(clzName);
			auto nIndex = putConstantString(nat.first);
			auto sIndex = putConstantString(nat.second);
			cs.push_back(make_shared<VMConstantMethodHandle>(kind, cIndex, nIndex, sIndex, VMConstantItem::VMConstantItemType::TypeMethodHandle));
			break;
		}
		case ConstantPoolType::MethodType:
		{
			auto str = std::dynamic_pointer_cast<CONSTANT_MethodType_info>(p);
			auto name = cf->getUtf8String(str->descriptor_index);
			auto index = putConstantString(name);
			cs.push_back(make_shared<VMConstantStringLiteral>(index, VMConstantItem::VMConstantItemType::TypeMethodType));
			break;
		}
		case ConstantPoolType::Dynamic:
		{
			auto str = std::dynamic_pointer_cast<CONSTANT_Dynamic_info>(p);
			auto bma = std::dynamic_pointer_cast<BootstrapMethods_attribute>(cf->getAttributeByName(L"BootstrapMethods"));
			auto attr = bma->bootstrap_methods[str->bootstrap_method_attr_index];
			auto nat = cf->getNameAndType(str->name_and_type_index);
			auto cIndex = putConstantString(clz->className());
			auto nIndex = putConstantString(nat.first);
			auto sIndex = putConstantString(nat.second);

			// TODO 这里还没有完成解析。
			cs.push_back(make_shared<VMConstantDynamicFieldAndMethod>(cIndex, nIndex, sIndex, VMConstantItem::VMConstantItemType::TypeDynamic));

			break;
		}
		case ConstantPoolType::InvokeDynamic:
		{
			auto str = std::dynamic_pointer_cast<CONSTANT_InvokeDynamic_info>(p);
			auto bma = std::dynamic_pointer_cast<BootstrapMethods_attribute>(cf->getAttributeByName(L"BootstrapMethods"));
			auto attr = bma->bootstrap_methods[str->bootstrap_method_attr_index];
			auto nat = cf->getNameAndType(str->name_and_type_index);
			auto cIndex = putConstantString(clz->className());
			auto nIndex = putConstantString(nat.first);
			auto sIndex = putConstantString(nat.second);
			// TODO 这里还没有完成解析。
			cs.push_back(make_shared<VMConstantDynamicFieldAndMethod>(cIndex, nIndex, sIndex, VMConstantItem::VMConstantItemType::TypeInvokeDynamic));
			break;
		}
		case ConstantPoolType::Module:
		{
			auto str = std::dynamic_pointer_cast<CONSTANT_Module_info>(p);
			auto name = cf->getUtf8String(str->name_index);
			auto index = putConstantString(name);
			cs.push_back(make_shared<VMConstantStringLiteral>(index, VMConstantItem::VMConstantItemType::TypeModule));
			break;
		}
		case ConstantPoolType::Package:
		{
			auto str = std::dynamic_pointer_cast<CONSTANT_Package_info>(p);
			auto name = cf->getUtf8String(str->name_index);
			auto index = putConstantString(name);
			cs.push_back(make_shared<VMConstantStringLiteral>(index, VMConstantItem::VMConstantItemType::TypePackage));
			break;
		}
		default:
		{
			// 这里应该忽略路过的，但是为了简单我们先报错。
			throw runtime_error("Unknown constant pool:" + p->tag);
		}
		}
	}
	return make_shared<VMClassConstantPool>(clz, cs);
}

bool VMExtensibleMethodArea::put(const wstring &className, shared_ptr<VMClass> clz)
{
	bool ok = false;
	pthread_rwlock_wrlock(&classRWLock);
	auto existing = classes.find(className);
	if (existing == classes.end())
	{
		classes[className] = clz;
		spdlog::info("class:{} put in MethodArea, total:{}", w2s(className), classes.size());
		ok = true;
	}
	pthread_rwlock_unlock(&classRWLock);
	return ok;
}

shared_ptr<VMClass> VMExtensibleMethodArea::get(const wstring &className)
{
	shared_ptr<VMClass> clz = nullptr;
	pthread_rwlock_rdlock(&classRWLock);
	auto existing = classes.find(className);
	if (existing != classes.end())
	{
		clz = existing->second;
	}
	pthread_rwlock_unlock(&classRWLock);
	if (clz == nullptr)
	{
		spdlog::info("Class: {}  does not exist in MethodArea.", w2s(className));
	}
	return clz;
}

shared_ptr<VMClass> VMExtensibleMethodArea::remove(const wstring &className)
{
	pthread_rwlock_wrlock(&classRWLock);
	shared_ptr<VMClass> clz = nullptr;
	auto existing = classes.find(className);
	if (existing != classes.end())
	{
		spdlog::info("class: {} had been removed.", w2s(className));
		clz = existing->second;
		classes.erase(existing);
	}
	spdlog::info("mark class: {}", w2s(className));
	pthread_rwlock_unlock(&classRWLock);
	return clz;
}

bool VMExtensibleMethodArea::classExists(const wstring &className)
{
	bool exists = false;
	pthread_rwlock_rdlock(&classRWLock);
	auto existing = classes.find(className);
	exists = (existing != classes.end());
	pthread_rwlock_unlock(&classRWLock);
	return exists;
}

size_t VMExtensibleMethodArea::putConstantString(const wstring &t)
{

	// 这里其实要加锁，因为有可能多线程一起操作。
	auto exists = stringsMap.find(t);
	if (exists == stringsMap.end())
	{
		stringsVector.push_back(t);
		stringsMap[t] = stringsVector.size();
	}
	return stringsMap[t];
}
wstring VMExtensibleMethodArea::getConstantString(size_t index)
{
	assert(index < stringsVector.size());
	return stringsVector[index];
}

shared_ptr<VMClassConstantPool> VMExtensibleMethodArea::putClassConstantPool(shared_ptr<ClassFile> cf, shared_ptr<VMClass> clz)
{
	wstring clzName = clz->className();
	auto exists = classContantsPoolsMap.find(clzName);
	if (exists == classContantsPoolsMap.end())
	{
		auto cp = createVMClassConstantPool(cf, clz);
		classContantsPoolsMap[clzName] = cp;
	}
	return classContantsPoolsMap[clzName];
}
shared_ptr<VMClassConstantPool> VMExtensibleMethodArea::getClassConstantPool(const wstring &className)
{
	auto exists = classContantsPoolsMap.find(className);
	if (exists == classContantsPoolsMap.end())
	{
		return nullptr;
	}
	return exists->second;
}
VMExtensibleMethodArea::VMExtensibleMethodArea()
{
	pthread_rwlock_init(&classRWLock, nullptr);
	spdlog::info("VMExtensibleMethodArea created.");
}

VMExtensibleMethodArea::~VMExtensibleMethodArea()
{
	pthread_rwlock_destroy(&classRWLock);
	classes.clear();
	stringsMap.clear();
	stringsVector.clear();
	spdlog::info("VMExtensibleMethodArea gone");
}

shared_ptr<VMMethodArea> VMMethodAreaFactory::createMethodArea(shared_ptr<Configurations> conf)
{
	if (conf->isExtensibleMethodArea())
	{
		return make_shared<VMExtensibleMethodArea>();
	}
	return nullptr;
}