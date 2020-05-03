#ifndef __JVM_VM_THREAD_H__
#define __JVM_VM_THREAD_H__

#include "base_type.h"
#include <vector>
#include <memory>
#include "pthread.h"
#include "vm_class.h"

using namespace std;

// ��������߳�ջ��һ��������ʵ�֣�һ���ǹ̶�����һ���ǿ���չ�ġ�
struct VMThreadStackFrame {
	// �ֲ�������
	vector<u4> localVirableSlots;
	
	// ��ǰջ�������ķ�����
	shared_ptr<VMClassMethod> method;
};


struct VMThread {
private:
	// ��ʾ����̵߳�program connter, �����native�������õ�ʱ�����-1, ��ʾundefined.
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