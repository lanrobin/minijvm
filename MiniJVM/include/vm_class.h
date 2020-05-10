#ifndef __JVM_VM_CLASS_H__
#define __JVM_VM_CLASS_H__
#include "base_type.h"
#include "clazz_reader.h"
#include "vm_classloader.h"
#include "vm_field.h"
#include "vm_heap.h"
#include "log.h"
#include "string_utils.h"

#include <unordered_map>

using std::unordered_map;

class ClassLoader;
struct VMClass;
struct VMHeapObject;


struct VMMethodExceptionTable {
	u2 startPC;
	u2 endPC;
	u2 handlerPC;
	wstring catchType;
	VMMethodExceptionTable(shared_ptr< ClassFile> cf, shared_ptr<Code_attribute::ExceptionTable> et);
};

struct VMClassMethod {
	bool isStatic() { return ((accessFlags & METHOD_ACC_STATIC) == METHOD_ACC_STATIC); }
	bool isSyncrhonized() { return ((accessFlags & METHOD_ACC_SYNCHRONIZED) == METHOD_ACC_SYNCHRONIZED); }
	bool isNative() { return ((accessFlags & METHOD_ACC_NATIVE) == METHOD_ACC_NATIVE); }
	bool isStrict() { return ((accessFlags & METHOD_ACC_STRICT) == METHOD_ACC_STRICT); }
	bool isAbstract() { return ((accessFlags & METHOD_ACC_ABSTRACT) == METHOD_ACC_ABSTRACT); }
	bool isVarAgurs() { return ((accessFlags & METHOD_ACC_VARARGS) == METHOD_ACC_VARARGS); }
	bool isDeprected() const { return deprecated; }

	u2 accessFlags;
	bool deprecated = false;
	wstring name;
	wstring signature;
	//const shared_ptr<VMClass> clazz;
	vector<u1> codes;
	u2 maxStack;
	u2 maxLocals;
	vector<shared_ptr<VMMethodExceptionTable>> exceptionTable;
	vector<wstring> throwExceptions;
	VMClassMethod(shared_ptr< ClassFile> cf, shared_ptr<Method_Info> mi);
	wstring lookupKey() {
		return VMClassMethod::makeLookupKey(signature, name);
	}
public:
	static wstring makeLookupKey(const wstring& sig, const wstring name) {
		return sig + L"$" + name;
	}
};

struct VMClassField {
	bool isStatic() const { return ((accessFlags & FIELD_ACC_STATIC) == FIELD_ACC_STATIC); }
	u2 accessFlags;
	wstring name;
	wstring signature;
	bool deprecated = false;

	VMClassField(shared_ptr< ClassFile> cf, shared_ptr<Field_Info> fi);

	wstring lookupKey() {
		return VMClassField::makeLookupKey(signature, name);
	}
public:
	static wstring makeLookupKey(const wstring& sig, const wstring name) {
		return sig + L"$" + name;
	}
};

struct VMClass {

	enum InitializeState {
		NotInitialized,
		Initializing,
		Initialized
	};

	enum class ClassType {
		ClassTypeOrdinaryClass,
		ClassTypeInterface,
		ClassTypeArrayClass
	};

	const wstring& className() const { return name; };
	VMClass(shared_ptr< ClassFile> cf, shared_ptr<ClassLoader> cl);
	VMClass(const wstring& signature);
	shared_ptr<VMClassMethod> findMethod(const wstring& methodSignature, const wstring &name) const;
	shared_ptr<VMHeapObject> findStaticField(const wstring& methodSignature, const wstring& name) const;
	virtual ~VMClass() { spdlog::info("Class:{}, type:{} gone", w2s(name), classType); };
protected:
	wstring name;
	u2 accessFlags;
	shared_ptr<VMClass> super;
	vector<shared_ptr<VMClass>> interfaces;
	ClassType classType;
	shared_ptr<ClassLoader> classLoader;

	unordered_map<wstring, shared_ptr<VMClassField>> classStaticFieldLayout;

	unordered_map<wstring, shared_ptr<VMHeapObject>> classStaticFields;

	unordered_map<wstring, shared_ptr< VMClassField>> classInstanceFieldLayout;

	unordered_map<wstring, shared_ptr< VMClassMethod>> methods;

	// class是否调用了 <cinit>函数了。
	InitializeState state = InitializeState::NotInitialized;

public:
	static wstring getNextDimensionSignature(const wstring& sig);
	static vector<wstring> splitSignatureToElement(const wstring& sig);

};

struct VMOrdinaryClass : public VMClass {
	VMOrdinaryClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl);
};

struct VMInterfaceClass : public VMClass {
	VMInterfaceClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl);
};

struct VMArrayClass : public VMClass {
	VMArrayClass(const wstring& compenentSignature, u4 l);
	shared_ptr<VMClass> componentType;
	u4 lenghth;
	vector <shared_ptr<VMHeapObject>> elements;
};

#endif //__JVM_VM_CLASS_H__