#ifndef __JVM_VM_METHOD_AREA_H__
#define __JVM_VM_METHOD_AREA_H__
#include "base_type.h"
#include "vm_method.h"
#include "vm_class.h"
#include "configurations.h"

/*
 ����JVMS�����������������Է�Ϊ�̶���С�ĺͿ���չ�Ĵ�С�ģ�Ϊ�˼�ʵ�֣�����ʵ�ֿ���չ��
*/
class VMMethodArea {
public:
	virtual shared_ptr<VMClass> put(const wstring& className, shared_ptr<VMClass> clz) = 0;
	virtual shared_ptr<VMClass> get(const wstring& className) = 0;

	virtual ~VMMethodArea() {};
};

class VMExtensibleMethodArea : public VMMethodArea {
public:
	shared_ptr<VMClass> put(const wstring& className, shared_ptr<VMClass> clz) override;
	shared_ptr<VMClass> get(const wstring& className) override;
	~VMExtensibleMethodArea();

private:
	unordered_map<wstring, shared_ptr<VMClass>> classes;
};

class VMMethodAreaFactory {
public:
	static shared_ptr<VMMethodArea> createMethodArea(shared_ptr<Configurations> conf);
};
#endif // __JVM_VM_METHOD_AREA_H__