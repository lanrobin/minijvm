#ifndef __JVM_VM_H__
#define __JVM_VM_H__
#include <string>
#include "base_type.h"
#include "configurations.h"
#include "vm_method_area.h"
#include "vm_classloader.h"
#include "vm_thread.h"

class VM : public std::enable_shared_from_this<VM> {
private:
	shared_ptr<VMMethodArea> methodArea;
	shared_ptr<Configurations> conf;
	shared_ptr<ClassLoader> bootstrapClassLoader;
	shared_ptr<ClassLoader> appClassLoader;
	bool initilized = false;
	/*
	主线程
	*/
	shared_ptr<VMJavaThread> mainThread;

	/*GC线程，目前先实现一个GC线程。*/
	shared_ptr<VMGCThread> gcThread;

	vector<shared_ptr<VMThread>> allThreads;
public:
	VM();
	void initVM(shared_ptr<Configurations> conf);
	~VM();
	int run();
	shared_ptr<VMMethodArea> getMethodArea() const { return methodArea; }

	shared_ptr<ClassLoader> getAppClassLoader() const { return appClassLoader; };

public:
	// static 
	static shared_ptr<VM> getVM();
};
#endif //__JVM_VM_H__