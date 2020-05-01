#ifndef __JVM_VM_CLASS_H__
#define __JVM_VM_CLASS_H__
#include "base_type.h"
#include "clazz_reader.h"
#include "vm_method.h"
#include "vm_field.h"

#include <unordered_map>

using std::unordered_map;

struct VMClass {

	enum ClassType {
		ClassTypeOrdinaryClass, ClassTypeInterface,
	};

	const wstring& className() const { return name; };
	VMClass(shared_ptr< ClassFile>& cf);
	
protected:
	wstring name;
	shared_ptr<VMClass> parent;
	unordered_map<wstring, shared_ptr<VMClass>> interfaces;
	unordered_map<wstring, shared_ptr<VMMethod>> methods;
	unordered_map<wstring, shared_ptr<VMMethod>> fields;
};
#endif //__JVM_VM_CLASS_H__