#ifndef __JVM_VM_HEAP_H__
#define __JVM_VM_HEAP_H__

#include "vm_class.h"
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
struct VMHeapObject {
	enum VMHeapObjectType {
		Byte, Char, Double, Float, Int, Long, Class, Array, Interface, Short, Boolean
	};

	VMHeapObjectType type;

	virtual ~VMHeapObject() {};
};

struct PrimitiveVMHeapObject : public VMHeapObject {

};

// boolean, byte, char, short, int
struct IntegerVMHeapObject : public PrimitiveVMHeapObject {

};

struct FloatVMHeapObject : public PrimitiveVMHeapObject {

};

struct LongVMHeapObject : public PrimitiveVMHeapObject {

};

struct DoubleVMHeapObject : public PrimitiveVMHeapObject {

};

struct ReferenceVMHeapObject : public VMHeapObject {

};

struct ClassVMHeapObject : public ReferenceVMHeapObject {

};

struct ArrayVMHeapObject : public ReferenceVMHeapObject {

};

struct InterfaceVMHeapObject : public ReferenceVMHeapObject {

};

class VMHeapPool {
public:
	static shared_ptr< VMHeapObject> createVMHeapObject(const wstring& signature);
};
#endif // __JVM_VM_HEAP_H__