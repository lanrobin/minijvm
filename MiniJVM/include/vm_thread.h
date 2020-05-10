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
	wstring className; // Ҫ���е��������.
	wstring methodName; // Ҫ���еķ������ơ�
	wstring methodSignature; // Ҫ���еķ���ǩ����
	bool needStaticMethod; // �ǲ���Ҫ����static�ķ�����ֻ����main������ʱ���Ϊtrue.
	vector<wstring> args; // �������Ĳ�����һ��ֻ����main������ʱ���Ҫ�󣬷���Ϊ�ա�
	shared_ptr<ReferenceVMHeapObject> instance; // ������ǵ���static������Ҫһ������
	// static field.
private:
	static const long PC_UNDEFINED = -1L;

public:
	VMJavaThread(pthread_t& pt) : VMThread(pt),pc(PC_UNDEFINED), needStaticMethod(false)
	{

	}
	void startExecute() override;
	void setRunningParameters(const wstring& targetClass, const wstring& targetMethod, const wstring& signature, bool isStatic = false)
	{
		className = targetClass;
		methodName = targetMethod;
		methodSignature = signature;
		needStaticMethod = isStatic;
	}
	void setRunningMethodArgs(vector<wstring>& params) {
		args.insert(args.begin(), params.begin(), params.end());
	}
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