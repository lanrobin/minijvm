#include <vm_thread.h>
#include "log.h"
#include "string_utils.h"
#include <chrono>
#include <thread>


void VMJavaThread::startExecute() {
	assert(startJavaMethod != nullptr);
	spdlog::info("startExecute:{} with signature:{}", w2s(startJavaMethod->name), w2s(startJavaMethod->signature));
	int count = 0;
	while (count < 15) {
		spdlog::info("running:{}", count);
		std::this_thread::sleep_for(2s);
		count++;
	}
}

VMGCThread::VMGCThread() {
	cond = PTHREAD_COND_INITIALIZER;
	mutex = PTHREAD_MUTEX_INITIALIZER;
	auto err = pthread_create(&nativeThread, NULL, &VMGCThread::gc, this);
	spdlog::info("VMGCThread::VMGCThread, create gc thread returns:{}", err);
}

void VMGCThread::startExecute() {
	// ¿ªÊ¼¡£
	pthread_cond_signal(&cond);
	spdlog::info("Fire gc.");
}

void* VMGCThread::gc(void* ptr) {
	spdlog::info("GC started but wait for chance.");
	auto thiz = reinterpret_cast<VMGCThread*>(ptr);
	static struct timespec timeout = { 0, 0 };
	int exitCount = 0;
	while (exitCount < 10) {
		timeout.tv_sec = time(NULL) + 4;
		pthread_mutex_lock(&thiz->mutex);
		spdlog::info("GC waiting");
		pthread_cond_timedwait(&thiz->cond, &thiz->mutex, &timeout);
		std::this_thread::sleep_for(4s);
		spdlog::info("GC active:{}", exitCount);
		pthread_mutex_unlock(&thiz->mutex);
		exitCount++;
	}
	//pthread_exit(&thiz->nativeThread);
	cout << "GC exit" << endl;
	return nullptr;
}

VMGCThread::~VMGCThread() {
	pthread_exit(&nativeThread);
	pthread_attr_destroy(&threadAttri);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}