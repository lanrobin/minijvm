#ifndef __JVM_VM_THREAD_H__
#define __JVM_VM_THREAD_H__

#include <vector>
#include <memory>
#include "pthread.h"

using namespace std;

// 虚拟机的线程栈，一般有两种实现，一种是固定的另一种是可扩展的。
class VMThreadStack {

};
class VMThread {
private:
	// 表示这个线程的program connter, 如果是native方法调用的时候就是-1, 表示undefined.
	long pc;
	VMThreadStack* stack;
	pthread_t* nativeThread;
	vector<shared_ptr<VMThreadStack>> stacks;
	// static field.
private:
	static const long PC_UNDEFINED = -1L;

public:
	VMThread();
};
#endif