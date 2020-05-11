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
 根据JVMS的描述，方法区可以分为固定大小的和可扩展的大小的，为了简单实现，我先实现可扩展的
*/
class VMMethodArea {
public:
	virtual shared_ptr<VMClass> put(const wstring& className, shared_ptr<VMClass> clz) = 0;
	virtual shared_ptr<VMClass> get(const wstring& className) = 0;
	virtual bool mark(const wstring& className) = 0;
	virtual bool isClassLoading(const wstring& className) = 0;
	virtual bool classExists(const wstring& className) const = 0;

	/*
	因为JVM规定，所以的字符串必须统一管理，所以在ClassFile里的UTF8 constant都放在这里面。
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
	我们把string同时存放在两个结构体里，用来实现快速查找，这样会浪费内存，但是增加速度。
	*/
	unordered_map<wstring, size_t> stringsMap;
	vector<wstring> stringsVector;
};

class VMMethodAreaFactory {
public:
	static shared_ptr<VMMethodArea> createMethodArea(shared_ptr<Configurations> conf);
};
#endif // __JVM_VM_METHOD_AREA_H__