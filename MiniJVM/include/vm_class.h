#ifndef __JVM_VM_CLASS_H__
#define __JVM_VM_CLASS_H__
#include "base_type.h"
#include "java_class_common.h"
#include "vm_classloader.h"
#include "vm_heap.h"
#include "log.h"
#include "string_utils.h"
#include "clazz_reader.h"

#include <unordered_map>

/*
/// class file access flag
extern const u2 CLASS_ACC_PUBLIC; //Declared public; may be accessed from outside its package.
extern const u2 CLASS_ACC_FINAL; //Declared final; no subclasses allowed.
extern const u2 CLASS_ACC_SUPER; //Treat superclass methods specially when invoked by the invokespecial instruction.
extern const u2 CLASS_ACC_INTERFACE; //Is an interface, not a class.
extern const u2 CLASS_ACC_ABSTRACT; //Declared abstract; must not be instantiated.
extern const u2 CLASS_ACC_SYNTHETIC; //Declared synthetic; not present in the source code.
extern const u2 CLASS_ACC_ANNOTATION; //Declared as an annotation type.
extern const u2 CLASS_ACC_ENUM; //Declared as an enum type.
extern const u2 CLASS_ACC_MODULE; //Is a module, not a class or interface.
*/
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

struct VMClassResolvable {
public: // fields;
	u2 accessFlags = 0;
	wstring name;
	wstring signature;
	bool deprecated = false;
	shared_ptr<VMClass> ownerClass;
	static const unordered_map<wchar_t, int> PRIMITIVE_TYPES;
public: // methods
	void resolveSymbol(shared_ptr<VMClass> owner);
	shared_ptr<VMClass> resolveArray(const wstring& sym);
	wstring lookupKey() {
		return makeLookupKey(signature, name);
	}

	virtual ~VMClassResolvable() {}
	virtual vector<wstring> splitSignature() = 0;
public:
	static wstring makeLookupKey(const wstring& sig, const wstring name) {
		return sig + L"$" + name;
	}
};

struct VMClassMethod : public VMClassResolvable{
	bool isStatic() { return ((accessFlags & METHOD_ACC_STATIC) == METHOD_ACC_STATIC); }
	bool isSyncrhonized() { return ((accessFlags & METHOD_ACC_SYNCHRONIZED) == METHOD_ACC_SYNCHRONIZED); }
	bool isNative() { return ((accessFlags & METHOD_ACC_NATIVE) == METHOD_ACC_NATIVE); }
	bool isStrict() { return ((accessFlags & METHOD_ACC_STRICT) == METHOD_ACC_STRICT); }
	bool isAbstract() { return ((accessFlags & METHOD_ACC_ABSTRACT) == METHOD_ACC_ABSTRACT); }
	bool isVarAgurs() { return ((accessFlags & METHOD_ACC_VARARGS) == METHOD_ACC_VARARGS); }
	bool isDeprected() const { return deprecated; }

	//const shared_ptr<VMClass> clazz;
	vector<u1> codes;
	u2 maxStack;
	u2 maxLocals;
	vector<shared_ptr<VMMethodExceptionTable>> exceptionTable;
	vector<wstring> throwExceptions;
	VMClassMethod(shared_ptr< ClassFile> cf, shared_ptr<Method_Info> mi);

	vector<wstring> splitSignature() override;
	
};

struct VMClassField : public VMClassResolvable {
	bool isStatic() const { return ((accessFlags & FIELD_ACC_STATIC) == FIELD_ACC_STATIC); }
	vector<wstring> splitSignature() override;
	VMClassField(shared_ptr< ClassFile> cf, shared_ptr<Field_Info> fi);
};

struct VMClass: public std::enable_shared_from_this<VMClass>{

	enum class InitializeState {
		NotInitialized,
		Initializing,
		Initialized
	};

	enum class ClassType {
		ClassTypeOrdinaryClass,
		ClassTypeInterface,
		ClassTypeArrayClass,
		ClassPrimitiveTypeClass // 这个只是用来表示数组最后一个Component来用的，没有什么实现意义。
	};

	const wstring& className() const { return name; };
	VMClass(shared_ptr< ClassFile> cf, shared_ptr<ClassLoader> cl);
	VMClass(const wstring& signature);
	shared_ptr<VMClassMethod> findMethod(const wstring& methodSignature, const wstring &name) const;
	shared_ptr<VMHeapObject> findStaticField(const wstring& methodSignature, const wstring& name) const;

	void resolveSymbol();
	shared_ptr<ClassLoader> getClassLoader() const { return classLoader; };
	virtual ~VMClass() { spdlog::info("Class:{}, type:{} gone", w2s(name), classType); };

	bool equals(shared_ptr<VMClass> other) const;
	bool isSubClassOf(shared_ptr<VMClass> super) const;
	bool implemented(shared_ptr<VMClass> interf) const;
	bool hasAccessibilityTo(shared_ptr<VMClass> other) const;

	bool isInterface() const { return ((accessFlags & CLASS_ACC_INTERFACE) == CLASS_ACC_INTERFACE); }
	bool isAbstract() const { return ((accessFlags & CLASS_ACC_ABSTRACT) == CLASS_ACC_ABSTRACT); }
	bool isPublic() const { return ((accessFlags & CLASS_ACC_PUBLIC) == CLASS_ACC_PUBLIC); }
	bool isProtected() const { return  !isPublic() && !isPrivate(); }
	bool isPrivate() const { return ((accessFlags & NESTED_CLASS_ACC_PRIVATE) == NESTED_CLASS_ACC_PRIVATE); }
protected:
	wstring name;
	wstring packageName;
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

	inline bool checkAccessFlag(u2 flag) const { return ((accessFlags & flag) == flag); }

	shared_ptr<VMClass> getSharedPtr() { return shared_from_this(); }

public:
	static wstring getNextDimensionSignature(const wstring& sig);
	static vector<wstring> splitSignatureToElement(const wstring& sig);

	static shared_ptr<VMClass> LOADING_VMCLASS;

};

struct VMOrdinaryClass : public VMClass {
	VMOrdinaryClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl);
};

struct VMInterfaceClass : public VMClass {
	VMInterfaceClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl);
};

struct VMArrayClass : public VMClass {
	VMArrayClass(const wstring& compenentSignature);
	shared_ptr<VMClass> componentType;
};

struct VMPrimitiveClass : public VMClass {
	VMPrimitiveClass(const wstring& compenentSignature) :VMClass(compenentSignature)
	{ classType = VMClass::ClassType::ClassPrimitiveTypeClass; };
	static unordered_map<wstring, shared_ptr< VMPrimitiveClass>> AllPrimitiveClasses;
};

#endif //__JVM_VM_CLASS_H__