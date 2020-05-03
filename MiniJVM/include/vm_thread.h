#ifndef __JVM_VM_THREAD_H__
#define __JVM_VM_THREAD_H__

#include "base_type.h"
#include <vector>
#include <memory>
#include "pthread.h"
#include "vm_class.h"

using namespace std;

// 虚拟机的线程栈，一般有两种实现，一种是固定的另一种是可扩展的。
struct VMThreadStackFrame {
	// 局部变量表。
	vector<u4> localVirableSlots;
	
	// 当前栈桢所属的方法。
	shared_ptr<VMClassMethod> method;
};


struct VMThread {
private:
	// 表示这个线程的program connter, 如果是native方法调用的时候就是-1, 表示undefined.
	long pc;
	shared_ptr<pthread_t> nativeThread;
	vector<shared_ptr<VMThreadStackFrame>> stackFrames;
	// static field.
private:
	static const long PC_UNDEFINED = -1L;

public:
	VMThread();
	~VMThread();
};
#endif