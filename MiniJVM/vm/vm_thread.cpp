#include <vm_thread.h>
#include "log.h"
#include "string_utils.h"
#include "vm.h"
#include "vm_engine.h"
#include <chrono>
#include <thread>

void VMJavaThread::startExecute()
{
	shared_ptr<VMClassMethod> method = nullptr;
	if (startJavaMethod.expired()) {

		// 如果是main函数,那就得自己找出来。
		auto appClassLoader = VM::getVM().lock()->getAppClassLoader().lock();

		shared_ptr<VMClass> mainClass = appClassLoader->loadClass(className).lock();
		startJavaMethod = mainClass->findMethod(methodSignature, methodName);
		if (startJavaMethod.expired())
		{
			throw runtime_error("No start method found:" + w2s(methodName));
		}
		auto method = startJavaMethod.lock();
		if (method->isStatic() != needStaticMethod)
		{
			throw runtime_error("Method staticness is incorrect.");
		}
	}
	else {
		method = startJavaMethod.lock();
	}

	auto stackFrame = make_shared<VMThreadStackFrame>(startJavaMethod, args);
	spdlog::info("startExecute:{} with signature:{}", w2s(method->name), w2s(method->signature));
	VMEngine::execute(getWeakThisPtr(), stackFrame);
	spdlog::info("endExecute:{} with signature:{}", w2s(method->name), w2s(method->signature));
}

VMGCThread::VMGCThread()
{
	cond = PTHREAD_COND_INITIALIZER;
	mutex = PTHREAD_MUTEX_INITIALIZER;
	auto err = pthread_create(&nativeThread, NULL, &VMGCThread::gc, this);
	spdlog::info("VMGCThread::VMGCThread, create gc thread returns:{}", err);
}

void VMGCThread::startExecute()
{
	// 开始。
	pthread_cond_signal(&cond);
	spdlog::info("Fire gc.");
}

void *VMGCThread::gc(void *ptr)
{
	spdlog::info("GC started but wait for chance.");
	auto thiz = reinterpret_cast<VMGCThread *>(ptr);
	static struct timespec timeout = {0, 0};
	int exitCount = 0;
	while (exitCount < 10)
	{
		timeout.tv_sec = time(NULL) + 4;
		pthread_mutex_lock(&thiz->mutex);
		spdlog::info("GC waiting");
		pthread_cond_timedwait(&thiz->cond, &thiz->mutex, &timeout);
		//std::this_thread::sleep_for(4s);
		spdlog::info("GC active:{}", exitCount);
		pthread_mutex_unlock(&thiz->mutex);
		exitCount++;
	}
	//pthread_exit(&thiz->nativeThread);
	cout << "GC exit" << endl;
	return nullptr;
}

VMGCThread::~VMGCThread()
{
	pthread_exit(&nativeThread);
	pthread_attr_destroy(&threadAttri);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}