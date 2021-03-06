#ifndef __JVM_VM_THREAD_H__
#define __JVM_VM_THREAD_H__

#include "base_type.h"
#include <vector>
#include <memory>
#include "pthread.h"
#include "vm_class.h"
#include "log.h"

//using namespace std;

struct ReferenceVMHeapObject;
struct VMPrimitiveClass;
struct NullVMHeapObject;

// 虚拟机的线程栈，一般有两种实现，一种是固定的另一种是可扩展的。
struct VMThreadStackFrame
{
	VMThreadStackFrame(weak_ptr<VMClassMethod> md, vector<weak_ptr<VMHeapObject>> args);
	~VMThreadStackFrame();
	size_t stackTop;
	size_t localSize;
	size_t stackSize;

	// 局部变量表。
	vector<weak_ptr<VMHeapObject>> locals;

	// 操作栈。
	vector<weak_ptr<VMHeapObject>> stack;

	// 当前栈桢所属的方法。
	weak_ptr<VMClassMethod> method;

	void pushStack(weak_ptr<VMHeapObject> obj);
	weak_ptr<VMHeapObject> peakStack();
	weak_ptr<VMHeapObject> popStack();

	void putLocal(int index, weak_ptr<VMHeapObject> obj);
	weak_ptr<VMHeapObject> getLocal(int index);
};

struct VMThread: public std::enable_shared_from_this<VMThread>
{
public:
	pthread_t nativeThread;
	pthread_attr_t threadAttri;

public:
	virtual void startExecute() = 0;

	weak_ptr<VMThread> getWeakThisPtr() { return shared_from_this(); };

	VMThread(pthread_t pt)
	{
		pthread_attr_init(&threadAttri);
		nativeThread = pt;
	}
	VMThread()
	{
		pthread_attr_init(&threadAttri);
	}
	virtual ~VMThread() { spdlog::info("VMThread gone"); }
};

struct VMJavaThread : public VMThread
{
private:
	vector<shared_ptr<VMThreadStackFrame>> stackFrames;
	weak_ptr<VMClassMethod> startJavaMethod;
	wstring className;							// 要运行的类的名称.
	wstring methodName;							// 要运行的方法名称。
	wstring methodSignature;					// 要运行的方法签名。
	bool needStaticMethod;						// 是不是要运行static的方法。只有在main函数的时候才为true.
	vector<wstring> wstrArgs;						// 给方法的参数，一般只有在main函数的时候才要求，否则都为空。
	shared_ptr<ReferenceVMHeapObject> instance; // 如果不是调用static方法需要一个对象。
												// static field.
public:
	// 表示这个线程的program connter, 如果是native方法调用的时候就是-1, 表示undefined.
	long long pc;
	static const long PC_UNDEFINED = -1L;
public:
	VMJavaThread(pthread_t pt) : VMThread(pt), needStaticMethod(false), pc(PC_UNDEFINED)
	{
	}
	void startExecute() override;
	void setRunningParameters(const wstring &targetClass, const wstring &targetMethod, const wstring &signature, bool isStatic = false)
	{
		className = targetClass;
		methodName = targetMethod;
		methodSignature = signature;
		needStaticMethod = isStatic;
	}
	void setRunningMethodArgs(vector<wstring> &params)
	{
		wstrArgs.insert(wstrArgs.begin(), params.begin(), params.end());
	}
	void setRunningMethod(weak_ptr<VMClassMethod> runningMethod) {
		startJavaMethod = runningMethod;
	}

	void pushStackFrame(shared_ptr<VMThreadStackFrame> f) { stackFrames.push_back(f); }

	weak_ptr< VMThreadStackFrame> popStackFrame() {
		assert(stackFrames.size() > 0);
		auto f = stackFrames.back();
		stackFrames.pop_back();
		return f;
	}

	weak_ptr< VMThreadStackFrame> peakStackFrame() {
		assert(stackFrames.size() > 0);
		auto f = stackFrames.back();
		return f;
	}

	weak_ptr<VMJavaThread> getWeakThisPtr() { return std::dynamic_pointer_cast<VMJavaThread>(shared_from_this()); };
};

struct VMGCThread : public VMThread
{
public:
	VMGCThread();
	~VMGCThread();
	void startExecute() override;

	static void *gc(void *data);

public:
	pthread_cond_t cond;
	pthread_mutex_t mutex;
};

#endif