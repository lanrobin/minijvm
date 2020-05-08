#ifndef __JVM_VM_CLASS_H__
#define __JVM_VM_CLASS_H__
#include "base_type.h"
#include "clazz_reader.h"
#include "vm_classloader.h"
#include "vm_field.h"
#include "vm_heap.h"

#include <unordered_map>

using std::unordered_map;

class ClassLoader;
struct VMClass;


struct VMMethodExceptionTable {
	u2 startPC;
	u2 endPC;
	u2 handler_pc;
	wstring catchType;
};

struct VMClassMethod {
	bool isStatic() { return ((accessFlags & METHOD_ACC_STATIC) == METHOD_ACC_STATIC); }
	bool isSyncrhonized() { return ((accessFlags & METHOD_ACC_SYNCHRONIZED) == METHOD_ACC_SYNCHRONIZED); }
	bool isNative() { return ((accessFlags & METHOD_ACC_NATIVE) == METHOD_ACC_NATIVE); }
	bool isStrict() { return ((accessFlags & METHOD_ACC_STRICT) == METHOD_ACC_STRICT); }
	bool isAbstract() { return ((accessFlags & METHOD_ACC_ABSTRACT) == METHOD_ACC_ABSTRACT); }
	bool isVarAgurs() { return ((accessFlags & METHOD_ACC_VARARGS) == METHOD_ACC_VARARGS); }
	bool isDeprected() const { return deprecated; }

	const u2 accessFlags;
	const bool deprecated = false;
	const wstring name;
	const wstring signature;
	//const shared_ptr<VMClass> clazz;
	const vector<u1> codes;
	const u2 maxStack;
	const u2 maxLocals;
	vector<VMMethodExceptionTable> exceptionTable;
	vector<wstring> throwExceptions;
	VMClassMethod(shared_ptr<Method_Info> mi);
};

struct VMClassField {
	u2 accessFlags;
	wstring name;
	wstring signature;
	const bool deprecated = false;
};

struct VMClass {

	enum ClassType {
		ClassTypeOrdinaryClass,
		ClassTypeInterface,
		ClassTypeArrayClass
	};

	const wstring& className() const { return name; };
	VMClass(shared_ptr< ClassFile> cf, shared_ptr<ClassLoader> cl);
	virtual ~VMClass() {};
protected:
	wstring name;
	u2 accessFlags;
	shared_ptr<VMClass> super;
	vector<shared_ptr<VMClass>> interfaces;
	ClassType classType;

	static unordered_map<wstring, shared_ptr< VMClassField>> classStaticFieldLayout;

	static unordered_map<wstring, shared_ptr<VMHeapObject>> classStaticFields;

	static unordered_map<wstring, shared_ptr< VMClassField>> classInstanceFieldLayout;

};

struct VMOrdinaryClass : public VMClass {
	VMOrdinaryClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl);
protected:
	// 这里保存各个对象的属性性。
	unordered_map<wstring, shared_ptr<VMHeapObject>> classFields;
};

struct VMInterfaceClass : public VMClass {
	VMInterfaceClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl);
};

struct VMArrayClass : public VMClass {
	VMArrayClass(shared_ptr<ClassFile> cf);
	shared_ptr<VMClass> componentType;
	u4 lenghth;
	vector <shared_ptr<VMHeapObject>> elements;
};

#endif //__JVM_VM_CLASS_H__