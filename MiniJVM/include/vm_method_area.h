#ifndef __JVM_VM_METHOD_AREA_H__
#define __JVM_VM_METHOD_AREA_H__
#include "base_type.h"
#include "vm_class.h"
#include "vm_constant_pool.h"
#include "configurations.h"

/*
 ����JVMS�����������������Է�Ϊ�̶���С�ĺͿ���չ�Ĵ�С�ģ�Ϊ�˼�ʵ�֣�����ʵ�ֿ���չ��
*/

class VMMethodArea {
public:
	virtual shared_ptr<VMClass> put(const wstring& className, shared_ptr<VMClass> clz) = 0;
	virtual shared_ptr<VMClass> get(const wstring& className) = 0;

	virtual std::pair<size_t, shared_ptr<VMConstantItem>>putConstant(const wstring & t) = 0;
	virtual std::pair<size_t, shared_ptr<VMConstantItem>> putConstantDouble(u4 high, u4 low) = 0;
	virtual std::pair<size_t, shared_ptr<VMConstantItem>> putConstantLong(u4 high, u4 low) = 0;
	virtual std::pair<size_t, shared_ptr<VMConstantItem>> putConstant(u4 v) = 0;
	virtual std::pair<size_t, shared_ptr<VMConstantItem>> putConstant(u2 v) = 0;
	virtual std::pair<size_t, shared_ptr<VMConstantItem>> putConstant(u1 v) = 0;
	virtual std::pair<size_t, shared_ptr<VMConstantItem>> putConstantChar(u2 v) = 0;
	virtual shared_ptr<VMConstantItem> getConstant(size_t index) = 0;
	virtual ~VMMethodArea() {};
};


class VMExtensibleMethodArea : public VMMethodArea {
public:
	shared_ptr<VMClass> put(const wstring& className, shared_ptr<VMClass> clz) override;
	shared_ptr<VMClass> get(const wstring& className) override;
	std::pair<size_t, shared_ptr<VMConstantItem>>putConstant(const wstring& t) override;
	std::pair<size_t, shared_ptr<VMConstantItem>> putConstantDouble(u4 high, u4 low) override;
	std::pair<size_t, shared_ptr<VMConstantItem>> putConstantLong(u4 high, u4 low) override;
	std::pair<size_t, shared_ptr<VMConstantItem>> putConstant(u4 v) override;
	std::pair<size_t, shared_ptr<VMConstantItem>> putConstant(u2 v) override;
	std::pair<size_t, shared_ptr<VMConstantItem>> putConstant(u1 v) override;
	std::pair<size_t, shared_ptr<VMConstantItem>> putConstantChar(u2 v) override;
	shared_ptr<VMConstantItem> getConstant(size_t index)override;
	~VMExtensibleMethodArea();

private:
	unordered_map<wstring, shared_ptr<VMClass>> classes;
	unordered_map<shared_ptr<VMConstantItem>, size_t> contantsIndex;
	vector< shared_ptr<VMConstantItem>> contants;
};

class VMMethodAreaFactory {
public:
	static shared_ptr<VMMethodArea> createMethodArea(shared_ptr<Configurations> conf);
};
#endif // __JVM_VM_METHOD_AREA_H__