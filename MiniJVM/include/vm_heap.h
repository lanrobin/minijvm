#ifndef __JVM_VM_HEAP_H__
#define __JVM_VM_HEAP_H__

#include "vm_class.h"
/*
�����������ʾJava���з����һ�����󣬿��Ա�GC��̬���յġ�
һ�������¼������ͣ�

����һ��
PrimitiveType( boolean, byte, char, short, int, float, long, double)
���� boolean, byte, char, short, int ͳһ��һ������, boolean spec��û˵��������
���ǵ�ʵ����int������
float, long, double�Լ�һ�����͡�

���Ͷ���
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