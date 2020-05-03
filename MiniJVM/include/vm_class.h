#ifndef __JVM_VM_CLASS_H__
#define __JVM_VM_CLASS_H__
#include "base_type.h"
#include "clazz_reader.h"
#include "vm_field.h"
#include "vm_heap.h"

#include <unordered_map>

using std::unordered_map;

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
		ClassTypeOrdinaryClass, ClassTypeInterface,
	};

	const wstring& className() const { return name; };
	VMClass(shared_ptr< ClassFile>& cf);
	
protected:
	wstring name;
	u2 accessFlags;
	shared_ptr<VMClass> super;
	vector<shared_ptr<VMClass>> interfaces;

	static unordered_map<wstring, shared_ptr< VMClassField>> classStaticFieldLayout;

	static unordered_map<wstring, shared_ptr<VMHeapObject>> classStaticFields;

	static unordered_map<wstring, shared_ptr< VMClassField>> classInstanceFieldLayout;
};
#endif //__JVM_VM_CLASS_H__