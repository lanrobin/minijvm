#ifndef __JVM_VM_CONSTANT_POOL_H__
#define __JVM_VM_CONSTANT_POOL_H__

#include "base_type.h"
#include <bitset>
/* ���������ͣ���Ϊ�����࣬
* 1��ֱ���ַ�������
* 2��������صġ�
*/
struct VMConstantItem {
	enum class VMConstantItemType : u1{
		// �����Ϊ��ռ�õ�0���õģ���Ϊ����0��ʾ��Ч��ֵ��
		TypeDummy = 0,
		TypeUtf8 = 1,
		TypeInteger = 3,
		TypeFloat = 4,
		TypeLong = 5,
		TypeDouble = 6,
		TypeClass = 7,
		TypeString = 8,
		TypeFieldRef = 9,
		TypeMethodRef = 10,
		TypeInterfaceMothedRef = 11,
		TypeNameAndType = 12,
		TypeMethodHandle = 15,
		TypeMethodType = 16,
		TypeDynamic = 17,
		TypeInvokeDynamic = 18,
		TypeModule = 19,
		TypePackage = 20
 
	};
	const VMConstantItemType type;

	VMConstantItem(VMConstantItemType t) : type(t) {}
	virtual ~VMConstantItem() {}
};

/*
ռ�õ�һ����long/double����һ����
*/
struct VMConstantDummy: public VMConstantItem {
	VMConstantDummy() : VMConstantItem(VMConstantItemType::TypeDummy) {}
};

/*
�����ַ�ͨ�õĳ�����ΪTypeClassInfo, TypeUtf8, TypeString, TypeMethodType, TypeModule, TypePackageʹ�á�
*/
struct VMConstantStringLiteral : public VMConstantItem {
	VMConstantStringLiteral(size_t index, VMConstantItemType type) : VMConstantItem(type), literalStringIndex(index) {};
	const size_t literalStringIndex;
};

/*
�����Ϊ TypeFieldRef, TypeMethodRef, TypeInterfaceMothedRef, TypeNameAndType��ͨ�ýṹ��
*/
struct VMConstantFieldAndMethodRef : public VMConstantItem {

	VMConstantFieldAndMethodRef(size_t classIndex, size_t nIndex, size_t sIndex, VMConstantItemType type) :
		VMConstantItem(type), classNameIndex(classIndex), nameIndex(nIndex), signatureIndex(sIndex) {}
	const size_t classNameIndex;
	const size_t nameIndex;
	const size_t signatureIndex;
};


struct VMConstantMethodHandle : public VMConstantFieldAndMethodRef {
	VMConstantMethodHandle(std::uint8_t kind, size_t classIndex, size_t nIndex, size_t sIndex, VMConstantItemType type) :
		VMConstantFieldAndMethodRef(classIndex, nIndex, sIndex, type), referenceKind(reinterpret_cast<RefType&>(kind)) {}
	enum RefType : std::uint8_t  {
		REF_getField = 1,
		REF_getStatic = 2,
		REF_putField = 3,
		REF_putStatic = 4,

		REF_invokeVirtual = 5,
		REF_newInvokeSpecial = 8,

		REF_invokeStatic = 6,
		REF_invokeSpecial = 7,

		REF_invokeInterface = 9
	};
	const RefType referenceKind;
};


/*
TypeDynamic �� TypeInvokeDynamic
*/
struct VMConstantDynamicFieldAndMethod : public VMConstantFieldAndMethodRef {

	VMConstantDynamicFieldAndMethod(size_t classIndex, size_t nIndex, size_t sIndex, VMConstantItemType type) :
		VMConstantFieldAndMethodRef(classIndex, nIndex, sIndex, type) {}

	struct BootstrapMethod {
		size_t nameIndex;
		size_t signatureIndex;
		vector<size_t> argments;
	};

	vector<BootstrapMethod> bootstrapMethods;
};

struct VMConstantInteger: public VMConstantItem {
	VMConstantInteger(u4 bytes) : VMConstantItem(VMConstantItemType::TypeInteger), value(bytes) {

	}
	const int value;
};

struct VMConstantFloat: public VMConstantItem {
	VMConstantFloat(u4 bytes) : VMConstantItem(VMConstantItemType::TypeFloat), value(reinterpret_cast<float&>(bytes)) {

	}
	const float value;
};

/**
����ת��֪���ǲ��Ƿ���IEEE754��Ҫ�󣬵���Ϊ�˼򵥣�����ô�ɡ�
TypeLong��TypeDouble���������
*/
struct VMConstantLongAndDouble : public VMConstantItem {
	VMConstantLongAndDouble(u4 high, u4 low, VMConstantItemType type) : VMConstantItem(type) {
		unsigned long long v = high;
		longValue = ((v << 32) | low);
	}

	union{
		double doubleValue;
		long long longValue;
	};
};

#endif //__JVM_VM_CONSTANT_POOL_H__