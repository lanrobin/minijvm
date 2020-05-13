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
	/*
	第一个具体的对象都对应一个类，如果是Primitive就对应VMPrimitiveClass的一个具体对象。
	*/
	const weak_ptr<VMClass> typeClass;

	VMHeapObject(weak_ptr<VMClass> typeClz) : typeClass(typeClz) {
		spdlog::info("New VMHeapObject:{}", w2s(typeClass.lock()->className()));
	}

	virtual ~VMHeapObject() {
		spdlog::info("Delete VMHeapObject:{}", w2s(typeClass.lock()->className()));
	};
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
	}

	// 这里存放这个类型的实体字段。
	unordered_map<wstring, shared_ptr<VMHeapObject>> fields;

	~ClassVMHeapObject() {
		fields.clear();
	}
};

struct ArrayVMHeapObject : public ReferenceVMHeapObject {
	ArrayVMHeapObject(weak_ptr<VMClass> typeClz) :ReferenceVMHeapObject(typeClz) {
	}
	weak_ptr<VMHeapObject> componentType;

	vector<shared_ptr< VMHeapObject>> elements;

	~ArrayVMHeapObject() {
		elements.clear();
	}
};

struct VMHeapPool {
public:
	virtual weak_ptr<VMHeapObject> createVMHeapObject(const wstring& signature) = 0;

	// 这里应该还GC相关函数，目前先不管。

	virtual ~VMHeapPool() { spdlog::info("VMHeapPool gone"); };

};

struct FixSizeVMHeapPool : public VMHeapPool {

	weak_ptr<VMHeapObject> createVMHeapObject(const wstring& signature) override;

	FixSizeVMHeapPool(size_t maxHeapSize):maxsize(maxHeapSize){

	}

	~FixSizeVMHeapPool() {
		maxsize = 0;
		objects.clear();
	}
private:
	long long maxsize;
	vector<shared_ptr< VMHeapObject>> objects;
};

class VMHeapPoolFactory {
	VMHeapPoolFactory() {}

public:
	static shared_ptr<VMHeapPool> createVMHeapPool(weak_ptr<Configurations> conf);
};
#endif // __JVM_VM_HEAP_H__