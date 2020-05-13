#ifndef __JVM_VM_CONSTANT_POOL_H__
#define __JVM_VM_CONSTANT_POOL_H__

#include "base_type.h"
#include <bitset>
/* 常量池类型，分为两大类，
* 1、直接字符串类型
* 2、数字相关的。
*/
struct VMConstantItem {
	enum class VMConstantItemType : u1{
		// 这个是为了占用第0个用的，因为我们0表示无效的值。
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
占用第一个和long/double的下一个。
*/
struct VMConstantDummy: public VMConstantItem {
	VMConstantDummy() : VMConstantItem(VMConstantItemType::TypeDummy) {}
};

/*
这是字符通用的常量。为TypeClassInfo, TypeUtf8, TypeString, TypeMethodType, TypeModule, TypePackage使用。
*/
struct VMConstantStringLiteral : public VMConstantItem {
	VMConstantStringLiteral(size_t index, VMConstantItemType type) : VMConstantItem(type), literalStringIndex(index) {};
	const size_t literalStringIndex;
};

/*
这个类为 TypeFieldRef, TypeMethodRef, TypeInterfaceMothedRef, TypeNameAndType的通用结构体
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
TypeDynamic 和 TypeInvokeDynamic
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
这样转不知道是不是符合IEEE754和要求，但是为了简单，先这么干。
TypeLong和TypeDouble都用这个。
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