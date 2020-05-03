#ifndef __JVM_VM_CONSTANT_POOL_H__
#define __JVM_VM_CONSTANT_POOL_H__

/* 常量池类型，分为两大类，
* 1、直接字符串类型
* 2、数字相关的。
*/
struct VMConstantItem {
	enum VMConstantItemType {
		TypeClassInfo, TypeFieldRef, TypeMethodRef, TypeInterfaceMothedRef, TypeString
	};
	VMConstantItemType type;

	~VMConstantItem() {}
};
#endif //__JVM_VM_CONSTANT_POOL_H__