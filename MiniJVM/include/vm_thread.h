#ifndef __JVM_VM_THREAD_H__
#define __JVM_VM_THREAD_H__

#include "base_type.h"
#include <vector>
#include <memory>
#include "pthread.h"
#include "vm_class.h"
#include "log.h"

using namespace std;

// ��������߳�ջ��һ��������ʵ�֣�һ���ǹ̶�����һ���ǿ���չ�ġ�
struct VMThreadStackFrame {
	// �ֲ�������
	vector<u4> localVirableSlots;
	
	// ��ǰջ�������ķ�����
	shared_ptr<VMClassMethod> method;
};

struct VMThread {
public:
	pthread_t nativeThread;
	pthread_attr_t threadAttri;
public:
	virtual void startExecute() = 0;
	VMThread(pthread_t& pt) 
	{ 
		pthread_attr_init(&threadAttri);
		nativeThread = pt;
	};
	VMThread()
	{
		pthread_attr_init(&threadAttri);
	};
	virtual ~VMThread() { spdlog::info("VMThread gone"); };
};

struct VMJavaThread : public VMThread {
private:
	// ��ʾ����̵߳�program connter, �����native�������õ�ʱ�����-1, ��ʾundefined.
	long pc;
	vector<shared_ptr<VMThreadStackFrame>> stackFrames;
	shared_ptr<VMClassMethod> startJavaMethod;
	// static field.
private:
	static const long PC_UNDEFINED = -1L;

public:
	VMJavaThread(shared_ptr<VMClassMethod> method, pthread_t& pt) : VMThread(pt), startJavaMethod(method), pc(PC_UNDEFINED)
	{

	}

	VMJavaThread(pthread_t& pt) : VMThread(pt),pc(PC_UNDEFINED)
	{

	}

	void setJavaMethod(shared_ptr<VMClassMethod> method) { startJavaMethod = method; }
	void startExecute() override;
	
};

struct VMGCThread : public VMThread {
public:
	VMGCThread();
	~VMGCThread();
	void startExecute() override;

	static void* gc(void* data);
public:
	pthread_cond_t cond;
	pthread_mutex_t mutex;
};

#endif