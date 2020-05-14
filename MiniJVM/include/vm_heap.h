#ifndef __JVM_VM_HEAP_H__
#define __JVM_VM_HEAP_H__
#include "base_type.h"
#include "configurations.h"
#include "log.h"
#include "string_utils.h"
#include "vm_class.h"

#include <vector>
#include <unordered_map>

using std::unordered_map;
using std::vector;


struct VMClass;

/*
这个类用来表示Java堆中分配的一个对象，可以被GC动态回收的。
一共有以下几种类型：

类型一：
PrimitiveType( boolean, byte, char, short, int, float, long, double)
其中 boolean, byte, char, short, int 统一成一个整型, boolean spec中没说明具体宽度
我们的实现用int来做。
float, long, double自己一种类型。

类型二：
ReferenceType (class types, array types, interface types) https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-2.html#jvms-2.4
*/
struct VMHeapObject : std::enable_shared_from_this<VMHeapObject> {

	virtual weak_ptr< VMHeapObject> getField(const wstring& signature, const wstring& name) const { throw runtime_error("This type of VMHeapObject can not performance getField"); };
	virtual weak_ptr< VMHeapObject> getStaticField(const wstring& signature, const wstring& name) const { throw runtime_error("This type of VMHeapObject can not performance getStaticField"); };

	virtual void putField(const wstring& signature, const wstring& name, weak_ptr<VMHeapObject> value) { throw runtime_error("This type of VMHeapObject imcompatible type Reference object."); };
	virtual void putStaticField(const wstring& signature, const wstring& name, weak_ptr<VMHeapObject> value) { throw runtime_error("This type of VMHeapObject imcompatible type Reference object."); };

	virtual void putArray(size_t index, weak_ptr<VMHeapObject> value) { throw runtime_error("This type of VMHeapObject imcompatible type int."); };
	virtual weak_ptr< VMHeapObject> getArray(size_t index) const { throw runtime_error("This type of VMHeapObject imcompatible type long long."); };

	weak_ptr<VMHeapObject> getWeakPtr() {
		return shared_from_this();
	}
	/*
	第一个具体的对象都对应一个类，如果是Primitive就对应VMPrimitiveClass的一个具体对象。
	*/
	const weak_ptr<VMClass> typeClass;

	VMHeapObject(weak_ptr<VMClass> typeClz) : typeClass(typeClz) {
		spdlog::info("New VMHeapObject.");
	}

	virtual ~VMHeapObject() {
		spdlog::info("Delete VMHeapObject");
	};

protected:
	shared_ptr< VMHeapObject> getSharedPtr() {
		return shared_from_this();
	}

	wstring makeLookupKey(const wstring& sig, const wstring & name) const
	{
		return VMClassResolvable::makeLookupKey(sig, name);
	}
};


struct PrimitiveVMHeapObject : public VMHeapObject {
	PrimitiveVMHeapObject(weak_ptr<VMClass> typeClz) :VMHeapObject(typeClz) {
	}
};

// boolean, byte, char, short, int
struct IntegerVMHeapObject : public PrimitiveVMHeapObject {
	IntegerVMHeapObject(int v, const wstring& sig) : PrimitiveVMHeapObject(VMPrimitiveClass::getPrimitiveVMClass(sig))  {
		intValue = v;
	}
	union {
		int intValue;
		short shortValue;
		char byteValue;
		unsigned short charValue;
		int boolValue;
	};
};

struct FloatVMHeapObject : public PrimitiveVMHeapObject {
	FloatVMHeapObject(float v):PrimitiveVMHeapObject(VMPrimitiveClass::getPrimitiveVMClass(L"F")), value(v)
	{}
	float value;
};

struct LongVMHeapObject : public PrimitiveVMHeapObject {
	LongVMHeapObject(long long v) :PrimitiveVMHeapObject(VMPrimitiveClass::getPrimitiveVMClass(L"J")), value(v)
	{}
	long long value;
};

struct DoubleVMHeapObject : public PrimitiveVMHeapObject {
	DoubleVMHeapObject(double v) :PrimitiveVMHeapObject(VMPrimitiveClass::getPrimitiveVMClass(L"D")), value(v)
	{}
	double value;
};

struct VoidVMHeapObject : public PrimitiveVMHeapObject {
	VoidVMHeapObject() :PrimitiveVMHeapObject(VMPrimitiveClass::getPrimitiveVMClass(L"V"))
	{}
};

struct ReferenceVMHeapObject : public VMHeapObject {
	ReferenceVMHeapObject(weak_ptr<VMClass> typeClz) :VMHeapObject(typeClz) {
	}
};

struct ClassVMHeapObject : public ReferenceVMHeapObject {
	ClassVMHeapObject(weak_ptr<VMClass> typeClz) :ReferenceVMHeapObject(typeClz) {
		isInterface = typeClz.lock()->isInterface();
	}


	~ClassVMHeapObject() {
		fields.clear();
	}

	weak_ptr< VMHeapObject> getField(const wstring& signature, const wstring& name) const override;
	weak_ptr< VMHeapObject> getStaticField(const wstring& signature, const wstring& name) const override;

	void putField(const wstring& signature, const wstring& name, weak_ptr<VMHeapObject> value) override;
	void putStaticField(const wstring& signature, const wstring& name, weak_ptr<VMHeapObject> value) override;
protected:
	// 这里存放这个类型的实体字段。
	unordered_map<wstring, weak_ptr<VMHeapObject>> fields;

private:
	bool isInterface = false;
};

struct ArrayVMHeapObject : public ReferenceVMHeapObject {
	ArrayVMHeapObject(weak_ptr<VMClass> typeClz, size_t size) :ReferenceVMHeapObject(typeClz), maxsize(size) {
		elements.reserve(size);
	}
	weak_ptr<VMHeapObject> componentType;

	vector<weak_ptr< VMHeapObject>> elements;
	size_t maxsize;
	~ArrayVMHeapObject() {
		elements.clear();
	}

	void putArray(size_t index, weak_ptr<VMHeapObject> value) override;
	weak_ptr<VMHeapObject> getArray(size_t index) const override;
};

struct NullVMHeapObject : public ReferenceVMHeapObject {
	NullVMHeapObject() :ReferenceVMHeapObject(std::weak_ptr<VMClass>()) {
	}
};


struct VMHeapPool {
public:
	template<typename T>
	weak_ptr<VMHeapObject> createVMHeapObject(weak_ptr<VMClass> clz, T& param);
	virtual weak_ptr<VMHeapObject> getNullVMHeapObject() const = 0;

	// 这里应该还GC相关函数，目前先不管。

	virtual ~VMHeapPool() { spdlog::info("VMHeapPool gone"); };

protected:
	virtual void storeObject(shared_ptr< VMHeapObject> obj) = 0;
};


struct FixSizeVMHeapPool : public VMHeapPool {

	weak_ptr<VMHeapObject> getNullVMHeapObject() const override;

	FixSizeVMHeapPool(size_t maxHeapSize):maxsize(maxHeapSize){

		// 第一个元素就是NullVMHeapObject， 全体用这个。
		objects.push_back(make_shared< NullVMHeapObject>());
	}

	~FixSizeVMHeapPool() {
		maxsize = 0;
		objects.clear();
	}
private:
	long long maxsize;
	vector<shared_ptr< VMHeapObject>> objects;

protected:
	void storeObject(shared_ptr< VMHeapObject> obj) override;
};

class VMHeapPoolFactory {
	VMHeapPoolFactory() {}

public:
	static shared_ptr<VMHeapPool> createVMHeapPool(weak_ptr<Configurations> conf);
};
#endif // __JVM_VM_HEAP_H__