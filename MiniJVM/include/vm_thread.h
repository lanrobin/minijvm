#ifndef __JVM_VM_THREAD_H__
#define __JVM_VM_THREAD_H__

#include <vector>
#include <memory>
#include "pthread.h"

using namespace std;

// ��������߳�ջ��һ��������ʵ�֣�һ���ǹ̶�����һ���ǿ���չ�ġ�
class VMThreadStack {

};
class VMThread {
private:
	// ��ʾ����̵߳�program connter, �����native�������õ�ʱ�����-1, ��ʾundefined.
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