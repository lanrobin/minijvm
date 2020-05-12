#ifndef __JVM_VM_H__
#define __JVM_VM_H__
#include <string>
#include "base_type.h"
#include "configurations.h"
#include "vm_method_area.h"
#include "vm_classloader.h"
#include "vm_thread.h"

/*
�ڴ������ֻ����һ��shared_ptr,��Ϊ����һֱ���ڣ�������weak_ptr�������á�
*/
class VM : public std::enable_shared_from_this<VM> {
private:
	shared_ptr<VMMethodArea> methodArea;
	shared_ptr<Configurations> conf;
	shared_ptr<ClassLoader> bootstrapClassLoader;
	shared_ptr<ClassLoader> appClassLoader;
	bool initilized = false;
	/*
	���߳�
	*/
	shared_ptr<VMJavaThread> mainThread;

	/*GC�̣߳�Ŀǰ��ʵ��һ��GC�̡߳�*/
	shared_ptr<VMGCThread> gcThread;

	vector<shared_ptr<VMThread>> allThreads;
public:
	VM();
	void initVM(shared_ptr<Configurations> conf);
	~VM();
	int run();
	weak_ptr<VMMethodArea> getMethodArea() const { return methodArea; }

	weak_ptr<ClassLoader> getAppClassLoader() const { return appClassLoader; };
	weak_ptr<ClassLoader> boostrapClassLoader() const { return bootstrapClassLoader; };

	weak_ptr< Configurations> getConf() const { return conf; }

public:
	// static 
	static weak_ptr<VM> getVM();
};
#endif //__JVM_VM_H__