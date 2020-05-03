#ifndef __JVM_VM_CONSTANT_POOL_H__
#define __JVM_VM_CONSTANT_POOL_H__

/* ���������ͣ���Ϊ�����࣬
* 1��ֱ���ַ�������
* 2��������صġ�
*/
struct VMConstantItem {
	enum VMConstantItemType {
		TypeClassInfo, TypeFieldRef, TypeMethodRef, TypeInterfaceMothedRef, TypeString
	};
	VMConstantItemType type;

	~VMConstantItem() {}
};
#endif //__JVM_VM_CONSTANT_POOL_H__