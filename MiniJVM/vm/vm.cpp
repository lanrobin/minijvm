#include "base_type.h"

#include <string>
#include <iostream>
#include <cassert>
#include <memory>
#include "base_type.h"
#include "log.h"
#include "platform.h"
#include "vm.h"
#include "configurations.h"
#include "vm_method_area.h"
#include "vm_classloader.h"
#include "string_utils.h"
#include "vm_heap.h"
#include "vm_engine.h"

VM::VM() : conf(nullptr)
{
	
	spdlog::info("VM created.");
}

VM::~VM()
{
	spdlog::info("methodArea use_count:{}", methodArea.use_count());
	spdlog::info("bootstrapClassLoader use_count:{}", bootstrapClassLoader.use_count());
	spdlog::info("appClassLoader use_count:{}", appClassLoader.use_count());
	spdlog::info("conf use_count:{}", conf.use_count());
	methodArea = nullptr;
	appClassLoader = nullptr;
	bootstrapClassLoader = nullptr;
	conf = nullptr;
	nativeMethods.clear();
	modules.clear();
	spdlog::info("VM gone.");
	//cout << "VM is gone" << endl;
}

int VM::run()
{
	spdlog::info("run with config:{}", conf->toString());

	mainThread->startExecute();
	for (auto t = allThreads.begin(); t != allThreads.end(); t++)
	{
		if (!pthread_equal(mainThread->nativeThread, (*t)->nativeThread))
		{
			spdlog::info("waiting for one thread to exit.");
			pthread_join((*t)->nativeThread, NULL);
		}
	}
	spdlog::info("all thread exits.");
	return 0;
}

weak_ptr<VM> VM::getVM()
{
	static shared_ptr<VM> VM_INSTANCE = make_shared<VM>();
	return VM_INSTANCE;
}
void VM::initVM(shared_ptr<Configurations> cfs)
{
	if (!initilized)
	{
		spdlog::info("VM initializing.");
		initilized = true;
		conf = cfs;
		methodArea = VMMethodAreaFactory::createMethodArea(conf);
		heap = VMHeapPoolFactory::createVMHeapPool(conf);
		bootstrapClassLoader = make_shared<BootstrapClassLoader>(conf->getBootStrapClassPath(), std::weak_ptr<ClassLoader>());
		appClassLoader = make_shared<AppClassLoader>(conf->getAppClassPath(), bootstrapClassLoader);

		//其它初始化的代码
		mainThread = make_shared<VMJavaThread>(pthread_self());

		int count = initNativeMethods();
		spdlog::info("VM initialized, registered {} native methods", count);

		// 初始化VM内部
		startUpVM(mainThread);

		// 设置好开始的参数
		mainThread->setRunningParameters(conf->getTargetClass(), L"main", L"([Ljava/lang/String;)V", true);
		allThreads.push_back(mainThread);

		gcThread = make_shared<VMGCThread>();
		allThreads.push_back(gcThread);
		gcThread->startExecute();
		
	}
	else
	{
		spdlog::warn("VM has already been initialized.");
	}
}

void* VM::getNativeMethod(const wstring& className, const wstring& signature, const wstring& name) const{
	auto key = makeKey(className, signature, name);
	auto m = nativeMethods.find(key);
	if (m != nativeMethods.end()) {
		return m->second;
	}
	else {
		spdlog::warn("No native method registered for {}", w2s(key));
	}
	return nullptr;
}
bool VM::registerNativeMethod(const wstring& className, const wstring& signature, const wstring& name, void* methodAddress) {
	assert(methodAddress != nullptr);
	auto key = makeKey(className, signature, name);
	if (nativeMethods.find(key) != nativeMethods.end()) {
		spdlog::warn("REREGISTERED for {}", w2s(key));
	}
	nativeMethods[key] = methodAddress;
	return true;
}

void VM::startUpVM(shared_ptr<VMJavaThread> executingThread) {
	spdlog::error("Must startup VM First.");
#define INIT(cn, t) bootstrapClassLoader->loadClass(cn).lock()->initialize(t)
	INIT(L"java.lang.String", executingThread);
	INIT(L"java.lang.System", executingThread);
	INIT(L"java.lang.Class", executingThread);
	INIT(L"java.lang.ThreadGroup", executingThread);
	INIT(L"java.lang.Thread", executingThread);
	INIT(L"java.lang.Module", executingThread);
	INIT(L"jdk.internal.misc.UnsafeConstants", executingThread);
	INIT(L"java.lang.reflect.Method", executingThread);
	INIT(L"java.lang.ref.Finalizer", executingThread);

	callInitPhase1(executingThread);

	INIT(L"java.lang.OutOfMemoryError", executingThread);
	INIT(L"java.lang.NullPointerException", executingThread);
	INIT(L"java.lang.ClassCastException", executingThread);
	INIT(L"java.lang.ArrayStoreException", executingThread);
	INIT(L"java.lang.ArithemeticException", executingThread);
	INIT(L"java.lang.StatckOverflowError", executingThread);
	INIT(L"java.lang.IllegalMonitorStateException", executingThread);
	INIT(L"java.lang.IllegalArgumentException", executingThread);
#undef INIT
}

void VM::callInitPhase1(shared_ptr<VMJavaThread> executingThread)
{
	spdlog::info("callInitPhase1.");
	auto sysClz = bootstrapClassLoader->loadClass(L"java.lang.System").lock();
	auto method = sysClz->findMethod(L"()V", L"initPhase1").lock();
	assert(method != nullptr && method->isStatic());
	vector<weak_ptr<VMHeapObject>> args;
	VMEngine::execute(executingThread, method, args);
}

bool VM::putModule(const wstring & moduleName, shared_ptr<VMModule> module)
{
	auto existing = modules.find(moduleName);
	if (existing == modules.end()) {
		assert(module != nullptr);
		modules[moduleName] = module;
		return true;
	}
	spdlog::warn("Module:{} already exists.", w2s(moduleName));
	return false;
}
weak_ptr<VMModule> VM::getModule(const wstring & moduleName) const {
	auto existing = modules.find(moduleName);
	if (existing != modules.end()) {
		return existing->second;
	}
	return std::weak_ptr<VMModule>();
}
weak_ptr<NullVMHeapObject> VMHelper::getNullVMHeapObject() {
	return VM::getVM().lock()->getHeapPool().lock()->getNullVMHeapObject();
}

weak_ptr<IntegerVMHeapObject> VMHelper::getIntegerVMHeapObject(int v) {
	auto obj = VM::getVM().lock()->getHeapPool().lock()->createIntegerVMHeapObject(v);
	return obj;
}

weak_ptr<DoubleVMHeapObject> VMHelper::getDoubleVMHeapObject(double v) {
	auto obj = VM::getVM().lock()->getHeapPool().lock()->createDoubleVMHeapObject(v);
	return obj;
}
weak_ptr<LongVMHeapObject> VMHelper::getLongVMHeapObject(long long v) {
	auto obj = VM::getVM().lock()->getHeapPool().lock()->createLongVMHeapObject(v);
	return obj;
}
 weak_ptr<FloatVMHeapObject> VMHelper::getFloatVMHeapObject(float v) {
	auto obj = VM::getVM().lock()->getHeapPool().lock()->createFloatVMHeapObject(v);
	return obj;
}
 weak_ptr<InstanceVMHeapObject> VMHelper::getStringVMHeapObject(const wstring& v) {
	 auto obj = VM::getVM().lock()->getHeapPool().lock()->createStringVMHeapObject(v);
	 return std::dynamic_pointer_cast<InstanceVMHeapObject>(obj.lock());
}

 weak_ptr<VoidVMHeapObject> VMHelper::getVoidVMHeapObject() {
	 return VM::getVM().lock()->getHeapPool().lock()->getVoidVMHeapObject();
 }
 weak_ptr<ClassRefVMHeapObject> VMHelper::getClassRefVMHeapObject(weak_ptr<VMClass> clz) {
	 return VM::getVM().lock()->getHeapPool().lock()->createClassRefVMHeapObject(clz);
}

 weak_ptr<InstanceVMHeapObject> VMHelper::getInstanceVMHeapObject(weak_ptr<VMClass> clz) {
	 return std::dynamic_pointer_cast<InstanceVMHeapObject>(VM::getVM().lock()->getHeapPool().lock()->createVMHeapObject(clz.lock()->getClassSignature()).lock());
 }

 weak_ptr<ArrayVMHeapObject> VMHelper::createArrayVMHelpObject(weak_ptr<VMClass> subComponent, size_t size)
 {
	 return VM::getVM().lock()->getHeapPool().lock()->createArrayVMHeapObject(subComponent.lock()->getClassSignature(), size);
 }

weak_ptr<VMClass> VMHelper::loadClass(const wstring& sig) {
	return VM::getVM().lock()->getAppClassLoader().lock()->loadClass(sig);
}

weak_ptr<VMClass> VMHelper::loadArrayClass(const wstring& sig) {
	return VM::getVM().lock()->boostrapClassLoader().lock()->defineClass(sig);
}

std::tuple<wstring, wstring, wstring> VMHelper::getFieldOrMethod(const wstring& className, u2 index) {
	return VM::getVM().lock()->getMethodArea().lock()->getFieldOrMethod(className, index);
}

weak_ptr< VMConstantItem> VMHelper::getVMConstantItem(const wstring& className, u2 index) {
	auto cp = VM::getVM().lock()->getMethodArea().lock()->getClassConstantPool(className);
	return cp->getVMConstantItem(index);
}

wstring VMHelper::getRefClassName(const wstring& className, u2 index) {
	auto lp = std::dynamic_pointer_cast<VMConstantStringLiteral>(getVMConstantItem(className, index).lock());
	return getConstantString(lp->literalStringIndex);
}

wstring VMHelper::getConstantString(size_t index)
{
	auto str = VM::getVM().lock()->getMethodArea().lock()->getConstantString(index);
	return str;
}

void* VMHelper::getNativeMethod(const wstring& className, const wstring& signature, const wstring& name) {
	return VM::getVM().lock()->getNativeMethod(className, signature, name);
}

weak_ptr<ClassLoader> VMHelper::getBootstrapClassLoader()
{
	return VM::getVM().lock()->boostrapClassLoader();
}