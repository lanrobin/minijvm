#ifndef __JVM_VM_CLASS_H__
#define __JVM_VM_CLASS_H__
#include "base_type.h"
#include "java_class_common.h"
#include "log.h"
#include "string_utils.h"
#include "clazz_reader.h"
#include "pthread.h"

#include <unordered_map>

class ClassLoader;
struct VMHeapObject;
struct VMClass;
struct VMJavaThread;

using std::unordered_map;

struct VMMethodExceptionTable
{
	u2 startPC;
	u2 endPC;
	u2 handlerPC;
	wstring catchType;
	VMMethodExceptionTable(shared_ptr<ClassFile> cf, shared_ptr<Code_attribute::ExceptionTable> et);
	void resolveSymbol(weak_ptr<ClassLoader> cl);
};

struct VMClassResolvable
{
public: // fields;
	u2 accessFlags = 0;
	wstring name;
	wstring signature;
	bool deprecated = false;
	weak_ptr<VMClass> ownerClass;

public: // methods
	virtual void resolveSymbol();
	wstring lookupKey()
	{
		return makeLookupKey(signature, name);
	}

	VMClassResolvable(weak_ptr<VMClass> owner) : ownerClass(owner) {}
	virtual ~VMClassResolvable() {}
	virtual vector<wstring> splitSignature() = 0;

	virtual bool isPublicOrProtected() const = 0;

public:
	static wstring  makeLookupKey(const wstring& sig, const wstring & name)
	{
		return sig + L"@" + name;
	}
protected:
	/*
	 check if contains any bit of flags.
	*/
	bool hasAnyAccessFlags(u2 flags) const { return (accessFlags & flags) != 0; }
};

struct VMClassMethod : public VMClassResolvable
{
	bool isStatic() const  { return ((accessFlags & METHOD_ACC_STATIC) == METHOD_ACC_STATIC); }
	bool isSyncrhonized() const  { return ((accessFlags & METHOD_ACC_SYNCHRONIZED) == METHOD_ACC_SYNCHRONIZED); }
	bool isNative() const  { return ((accessFlags & METHOD_ACC_NATIVE) == METHOD_ACC_NATIVE); }
	bool isStrict() const  { return ((accessFlags & METHOD_ACC_STRICT) == METHOD_ACC_STRICT); }
	bool isAbstract() const  { return ((accessFlags & METHOD_ACC_ABSTRACT) == METHOD_ACC_ABSTRACT); }
	bool isVarAgurs() const  { return ((accessFlags & METHOD_ACC_VARARGS) == METHOD_ACC_VARARGS); }
	bool isFinal() const { return ((accessFlags & METHOD_ACC_FINAL) == METHOD_ACC_FINAL);}
	bool isDeprected() const { return deprecated; }
	bool isPrivate() const { return ((accessFlags & METHOD_ACC_PRIVATE) == METHOD_ACC_PRIVATE); }

	bool isPublicOrProtected() const override{ return hasAnyAccessFlags(METHOD_ACC_PROTECTED | METHOD_ACC_PUBLIC); }
	//const shared_ptr<VMClass> clazz;
	vector<u1> codes;
	u2 maxStack;
	u2 maxLocals;
	/*这里面存放着函数的参数和返回值的拆分签名，最后一个是返回值的签名。*/
	vector<wstring> splittedSignatures;
	vector<shared_ptr<VMMethodExceptionTable>> exceptionTable;
	vector<wstring> throwExceptions;
	VMClassMethod(shared_ptr<ClassFile> cf, shared_ptr<Method_Info> mi, weak_ptr<VMClass> owner);

	vector<wstring> splitSignature() override;

	void resolveSymbol() override;
};

/*
所有的VMClass都保存在MethodArea里，所以其它地方引用它使用weak_ptr，防止循环引用。
*/
struct VMClassField : public VMClassResolvable
{
	bool isStatic() const { return ((accessFlags & FIELD_ACC_STATIC) == FIELD_ACC_STATIC); }
	bool isFinal() const { return ((accessFlags & FIELD_ACC_FINAL) == FIELD_ACC_FINAL); }
	bool isPublicOrProtected() const override { return hasAnyAccessFlags(FIELD_ACC_PROTECTED | FIELD_ACC_PUBLIC); }
	vector<wstring> splitSignature() override;
	VMClassField(shared_ptr<ClassFile> cf, shared_ptr<Field_Info> fi, weak_ptr<VMClass> owner);
	/*
	如果这个字段是static final并且是primitive或是String,需要有一个初始化的constantAttribute来初始化它。
	0表示无效。
	*/
	u2 initializeAttribute = 0;
};

struct VMClass : public std::enable_shared_from_this<VMClass>
{

	enum class InitializeState
	{
		Created,
		NotInitialized,
		Initializing,
		Initialized
	};

	enum class ClassType
	{
		ClassTypeOrdinaryClass,
		ClassTypeInterface,
		ClassTypeArrayClass,
		ClassPrimitiveTypeClass // 这个只是用来表示数组最后一个Component来用的，没有什么实现意义。
	};

	weak_ptr<VMClass> super;
	vector<weak_ptr<VMClass>> interfaces;

	weak_ptr<ClassLoader> classLoader;

	const wstring& className() const { return name; }
	const wstring& getClassSignature() const { return signature; }

	VMClass(const wstring& name, weak_ptr<ClassLoader> classLoader);

	weak_ptr<ClassLoader> getClassLoader() const { return classLoader; }
	virtual ~VMClass();

	virtual weak_ptr<VMClassMethod> findMethod(const wstring& signature, const wstring& name) const = 0;
	virtual weak_ptr<VMHeapObject> findStaticField(const wstring& signature, const wstring& name) const = 0;
	virtual weak_ptr< VMClassField> findFieldLayout(const wstring& signature, const wstring& name) const = 0;
	virtual bool putStaticField(const wstring& signature, const wstring& name, weak_ptr< VMHeapObject> value) { throw runtime_error("Unsupported operation for this class:" + w2s(name)); }

	virtual void resolveSymbol() = 0;

	virtual bool equals(weak_ptr<VMClass> other) const;
	bool isSubClassOf(weak_ptr<VMClass> super) const;
	bool implemented(weak_ptr<VMClass> interf) const;
	bool hasAccessibilityTo(weak_ptr<VMClass> other) const;

	bool isInterface() const { return ((accessFlags & CLASS_ACC_INTERFACE) == CLASS_ACC_INTERFACE); }
	bool isAbstract() const { return ((accessFlags & CLASS_ACC_ABSTRACT) == CLASS_ACC_ABSTRACT); }
	virtual bool isPublic() const { return ((accessFlags & CLASS_ACC_PUBLIC) == CLASS_ACC_PUBLIC); }
	bool isProtected() const { return !isPublic() && !isPrivate(); }
	bool isPrivate() const { return ((accessFlags & NESTED_CLASS_ACC_PRIVATE) == NESTED_CLASS_ACC_PRIVATE); }
	bool initialized() const { return state == InitializeState::Initialized; }

	bool needInitializing() const { return state == InitializeState::Created || state == InitializeState::NotInitialized; }
	/*
	只有LoadableClass真正需要初始化，所以会重载这个函数。
	这个函数做三件事：
	1. 先递归调用父类和实现的接口来做 2和3
	2. 给static 字段创建相应的对象。
	3. 调用<cinit>函数。
	*/
	void initialize(weak_ptr<VMJavaThread> thread);

	virtual void initializeStaticField();

private:
	/*因为JVM规定class在initialized的时候*/
	pthread_mutex_t initializeMutex;
	pthread_mutexattr_t initializeMutexAttr;
protected:
	wstring name;
	wstring packageName;
	wstring signature;
	u2 accessFlags;

	ClassType classType;

	// class是否调用了 <cinit>函数了。
	InitializeState state = InitializeState::Created;

	inline bool checkAccessFlag(u2 flag) const { return ((accessFlags & flag) == flag); }

	weak_ptr<VMClass> getSharedPtr() { return shared_from_this(); }

public:
	static wstring getNextDimensionSignature(const wstring& sig);
	static vector<wstring> splitSignatureToElement(const wstring& sig);
};

/*
只能通过equals比较。
*/
//bool operator==(const VMClass &lhs, const VMClass &rhs) = delete;
//bool operator!=(const VMClass &lhs, const VMClass &rhs) = delete;

struct VMReferenceClass : public VMClass
{

	VMReferenceClass(const wstring &name, weak_ptr<ClassLoader> cl) : VMClass(name, cl){}



	weak_ptr<VMClassMethod> findMethod(const wstring &methodSignature, const wstring &name) const override;
	weak_ptr<VMHeapObject> findStaticField(const wstring &methodSignature, const wstring &name) const override;
	weak_ptr< VMClassField> findFieldLayout(const wstring& methodSignature, const wstring& name) const override;
	void resolveSymbol() override;

	vector<weak_ptr<VMReferenceClass>> loadWaitingList;
	void classLoaded(weak_ptr<VMReferenceClass> other);

	~VMReferenceClass()
	{
		staticFieldLayout.clear();
		staticFields.clear();
		instanceFieldLayout.clear();
		methods.clear();
		allFieldLayout.clear();
	}
protected:
	unordered_map<wstring, shared_ptr<VMClassField>> staticFieldLayout;

	unordered_map<wstring, weak_ptr<VMHeapObject>> staticFields;

	unordered_map<wstring, shared_ptr<VMClassField>> instanceFieldLayout;

	unordered_map<wstring, shared_ptr<VMClassField>> allFieldLayout;

	unordered_map<wstring, shared_ptr<VMClassMethod>> methods;
};

struct VMLoadableClass : public VMReferenceClass
{

	VMLoadableClass(const wstring &name, weak_ptr<ClassLoader> cl) : VMReferenceClass(name, cl){}

	bool loadClassInfo(shared_ptr<ClassFile> cf);
	virtual void initializeStaticField() override;
};

struct VMOrdinaryClass : public VMLoadableClass
{
	VMOrdinaryClass(const wstring &name, weak_ptr<ClassLoader> cl) : VMLoadableClass(name, cl)
	{
		classType = VMClass::ClassType::ClassTypeOrdinaryClass;
	}
	bool putStaticField(const wstring& signature, const wstring& name, weak_ptr< VMHeapObject> value) override;
};

struct VMInterfaceClass : public VMLoadableClass
{
	VMInterfaceClass(const wstring &name, weak_ptr<ClassLoader> cl) : VMLoadableClass(name, cl)
	{
		classType = VMClass::ClassType::ClassTypeInterface;
	}
};

struct VMArrayClass : public VMReferenceClass
{
	VMArrayClass(const wstring &name, weak_ptr<VMClass> innerType, weak_ptr<ClassLoader> cl)
		: VMReferenceClass(name, cl), componentType(innerType)
	{
		classType = VMClass::ClassType::ClassTypeArrayClass;

		// 数组的签名就是类名本身。
		signature = name;
		state = InitializeState::Initialized;
	};
	const shared_ptr<VMClass> componentType;
};

struct VMPrimitiveClass : public VMClass
{
	VMPrimitiveClass(const wstring &name) : VMClass(name, std::weak_ptr<ClassLoader>())
	{
		classType = VMClass::ClassType::ClassPrimitiveTypeClass;
		state = InitializeState::Initialized;
	};

	weak_ptr<VMClassMethod> findMethod(const wstring &methodSignature, const wstring &name) const override { throw runtime_error("VMPrimitiveClass has no method."); }
	weak_ptr<VMHeapObject> findStaticField(const wstring &methodSignature, const wstring &name) const override { throw runtime_error("VMPrimitiveClass has no static fields."); }
	weak_ptr< VMClassField> findFieldLayout(const wstring& methodSignature, const wstring& name) const override { throw runtime_error("VMPrimitiveClass has no static fieldLayouts."); }
	void resolveSymbol() override { throw runtime_error("VMPrimitiveClass no need to resolveSymbol."); };
	// 所有的都是public.
	bool isPublic() const override { return true; }

	//只有与自己比较才相等。
	bool equals(weak_ptr<VMClass> other) const { return !other.expired() && this == other.lock().get(); }
	static weak_ptr<VMPrimitiveClass> getPrimitiveVMClass(const wstring& signature);
	static bool isPrimitiveTypeSignature(const wstring& signature);
	static bool isPrimitiveTypeSignature(wchar_t c);
private:
	static unordered_map<wstring, shared_ptr<VMPrimitiveClass>> AllPrimitiveClasses;
	static const unordered_map<wchar_t, int> PRIMITIVE_TYPES;
};

#endif //__JVM_VM_CLASS_H__