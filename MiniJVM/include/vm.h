#ifndef __JVM_VM_H__
#define __JVM_VM_H__
#include <string>
#include <memory>
#include "base_type.h"
#include "configurations.h"
#include "vm_method_area.h"
#include "vm_classloader.h"
#include "vm_thread.h"
#include "vm_heap.h"
#include "vm_module.h"

/*
内存管理是只保存一份shared_ptr,因为它会一直存在，所以用weak_ptr给别人用。
*/
struct VM : public std::enable_shared_from_this<VM>
{
private:
	shared_ptr<VMMethodArea> methodArea;
	shared_ptr<Configurations> conf;
	shared_ptr<ClassLoader> bootstrapClassLoader;
	shared_ptr<ClassLoader> appClassLoader;
	shared_ptr<VMHeapPool> heap;
	bool initilized = false;
	/*
	主线程
	*/
	shared_ptr<VMJavaThread> mainThread;

	/*
	GC线程，目前先实现一个GC线程。
	*/
	shared_ptr<VMGCThread> gcThread;

	vector<shared_ptr<VMThread>> allThreads;
	/*Native方法*/
	unordered_map<wstring, void *> nativeMethods;

	/*所有的Module*/
	unordered_map<wstring, shared_ptr<VMModule>> modules;
	// 这个函数做一系列的启动工作，准备环境什么的。
	void startUpVM(shared_ptr<VMJavaThread> executingThread);
	void callInitPhase1(shared_ptr<VMJavaThread> executingThread);
public:
	VM();
	void initVM(shared_ptr<Configurations> conf);
	~VM();
	int run();
	weak_ptr<VMMethodArea> getMethodArea() const { return methodArea; }

	weak_ptr<ClassLoader> getAppClassLoader() const { return appClassLoader; }
	weak_ptr<ClassLoader> boostrapClassLoader() const { return bootstrapClassLoader; }
	weak_ptr< VMHeapPool> getHeapPool() const { return heap; }
	weak_ptr<Configurations> getConf() const { return conf; }
	void* getNativeMethod(const wstring& className, const wstring& signature, const wstring& name) const;
	bool registerNativeMethod(const wstring& className, const wstring& signature, const wstring& name, void * methodAddress);
	bool putModule(const wstring & moduleName, shared_ptr<VMModule> module);
	weak_ptr<VMModule> getModule(const wstring & moduleName) const;

private:
	static wstring makeKey(const wstring& className, const wstring& signature, const wstring& name)
	{
		return className + L"@" + signature + L"@" + name;
	}

	/*这个函数注册所有需要的native methods,因为比较长，所以放在独立的文件里。 vm_native_methods.cpp*/
	int initNativeMethods();
public:
	// static
	static weak_ptr<VM> getVM();
};

/*
这个类是为了写代码方便用的。
*/
class VMHelper {
public:
	static weak_ptr<NullVMHeapObject> getNullVMHeapObject();
	static weak_ptr<IntegerVMHeapObject> getIntegerVMHeapObject(int v);
	static weak_ptr<DoubleVMHeapObject> getDoubleVMHeapObject(double v);
	static weak_ptr<LongVMHeapObject> getLongVMHeapObject(long long v);
	static weak_ptr<FloatVMHeapObject> getFloatVMHeapObject(float v);
	static weak_ptr<InstanceVMHeapObject> getStringVMHeapObject(const wstring & value);
	static weak_ptr<VoidVMHeapObject> getVoidVMHeapObject();
	static weak_ptr<ClassRefVMHeapObject> getClassRefVMHeapObject(weak_ptr<VMClass> clz);
	static weak_ptr<InstanceVMHeapObject> getInstanceVMHeapObject(weak_ptr<VMClass> clz);
	static weak_ptr<ArrayVMHeapObject> createArrayVMHelpObject(weak_ptr<VMClass> subComponent, size_t size);
	static weak_ptr<VMClass> loadClass(const wstring& sig);
	static weak_ptr<VMClass> loadArrayClass(const wstring& sig);
	static std::tuple<wstring, wstring, wstring> getFieldOrMethod(const wstring& className, u2 index);
	static weak_ptr< VMConstantItem> getVMConstantItem(const wstring& className, u2 index);
	static wstring getRefClassName(const wstring& className, u2 index);
	static wstring getConstantString(size_t index);
	static void* getNativeMethod(const wstring& className, const wstring& signature, const wstring& name);
	static weak_ptr<ClassLoader> getBootstrapClassLoader();
};
#endif //__JVM_VM_H__