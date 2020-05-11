#ifndef __JVM_VM_METHOD_AREA_H__
#define __JVM_VM_METHOD_AREA_H__
#include "base_type.h"
#include "vm_class.h"
#include "vm_constant_pool.h"
#include "configurations.h"

struct VMClassConstantPool {
	wstring className;
	shared_ptr<VMClass> ownerClass;
	vector<shared_ptr<VMConstantItem>> constants;
	VMClassConstantPool(shared_ptr<VMClass> ownerClazz, const vector<shared_ptr<VMConstantItem>>& cs):
		ownerClass(ownerClazz), constants(cs){
		className = ownerClass->className();
	}
};

/*
 ����JVMS�����������������Է�Ϊ�̶���С�ĺͿ���չ�Ĵ�С�ģ�Ϊ�˼�ʵ�֣�����ʵ�ֿ���չ��
*/
class VMMethodArea {
public:
	virtual shared_ptr<VMClass> put(const wstring& className, shared_ptr<VMClass> clz) = 0;
	virtual shared_ptr<VMClass> get(const wstring& className) = 0;
	virtual bool mark(const wstring& className) = 0;
	virtual bool isClassLoading(const wstring& className) = 0;
	virtual bool classExists(const wstring& className) const = 0;

	/*
	��ΪJVM�涨�����Ե��ַ�������ͳһ����������ClassFile���UTF8 constant�����������档
	*/
	virtual size_t putConstantString(const wstring & t) = 0;
	virtual wstring getConstantString(size_t index) = 0;

	virtual shared_ptr<VMClassConstantPool> putClassConstantPool(shared_ptr<ClassFile> cf, shared_ptr<VMClass> clz) = 0;
	virtual shared_ptr<VMClassConstantPool> getClassConstantPool(const wstring& className) = 0;
	virtual ~VMMethodArea() {};
protected:
	shared_ptr<VMClassConstantPool> createVMClassConstantPool(shared_ptr<ClassFile> cf, shared_ptr<VMClass> clz);
};


class VMExtensibleMethodArea : public VMMethodArea {
public:
	shared_ptr<VMClass> put(const wstring& className, shared_ptr<VMClass> clz) override;
	shared_ptr<VMClass> get(const wstring& className) override;
	bool mark(const wstring& className) override;
	bool isClassLoading(const wstring& className) override;
	bool classExists(const wstring& className) const override;


	size_t putConstantString(const wstring& t) override;
	wstring getConstantString(size_t index)override;

	shared_ptr<VMClassConstantPool> putClassConstantPool(shared_ptr<ClassFile> cf, shared_ptr<VMClass> clz) override;
	shared_ptr<VMClassConstantPool> getClassConstantPool(const wstring& className) override;
	~VMExtensibleMethodArea();

private:
	unordered_map<wstring, shared_ptr<VMClass>> classes;


	unordered_map<wstring, shared_ptr<VMClassConstantPool>> classContantsPoolsMap;

	/*
	���ǰ�stringͬʱ����������ṹ�������ʵ�ֿ��ٲ��ң��������˷��ڴ棬���������ٶȡ�
	*/
	unordered_map<wstring, size_t> stringsMap;
	vector<wstring> stringsVector;
};

class VMMethodAreaFactory {
public:
	static shared_ptr<VMMethodArea> createMethodArea(shared_ptr<Configurations> conf);
};
#endif // __JVM_VM_METHOD_AREA_H__