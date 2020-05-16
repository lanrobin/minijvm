#include <vm_thread.h>
#include "log.h"
#include "string_utils.h"
#include "vm.h"
#include "vm_engine.h"
#include "vm_heap.h"
#include <chrono>
#include <thread>


VMThreadStackFrame::VMThreadStackFrame(weak_ptr<VMClassMethod> md, vector<weak_ptr<VMHeapObject>> args)
{
	method = md;
	auto m = method.lock();
	localSize = m->maxLocals;
	stackSize = m->maxStack;
	spdlog::info("create VMThreadStackFrame for method:{}", w2s(m->signature));
	locals.reserve(localSize);
	stack.reserve(stackSize);

	// 把参数都压到locals里去。
	int index = 0;
	for (auto a = args.begin(); a != args.end(); a++) {
		auto arg = (*a).lock();
		locals.push_back(arg);
		index++;
		auto typeClass = arg->typeClass.lock();
		if (typeClass->equals(VMPrimitiveClass::getPrimitiveVMClass(L"D"))
			|| typeClass->equals(VMPrimitiveClass::getPrimitiveVMClass(L"J")))
		{
			// 如果是long或是double,那么一个arg需要占用两个slots.
			locals.push_back(std::weak_ptr<NullVMHeapObject>());
			index++;
		}
	}
	// 先把所有的位置填上，这相才能在指定的位置插入值。
	for (; index < localSize; index++) {
		locals.push_back(std::weak_ptr<NullVMHeapObject>());
	}
}

VMThreadStackFrame::~VMThreadStackFrame() {
	locals.clear();
	stack.clear();
	spdlog::info("destroy VMThreadStackFrame for method:{}", w2s(method.lock()->signature));
}


void VMThreadStackFrame::pushStack(weak_ptr<VMHeapObject> obj) {
	assert(stack.size() < stackSize && !obj.expired());
	stack.push_back(obj);
}
weak_ptr<VMHeapObject> VMThreadStackFrame::peakStack() {
	assert(stack.size() > 0);
	return stack.back();

}
weak_ptr<VMHeapObject> VMThreadStackFrame::popStack() {
	auto s = peakStack();
	stack.pop_back();
	return s;
}

void VMThreadStackFrame::putLocal(int index, weak_ptr<VMHeapObject> obj) {
	assert(index < localSize);
	locals.insert(locals.begin() + index - 1, obj);
}
weak_ptr<VMHeapObject> VMThreadStackFrame::getLocal(int index) {
	assert(index < localSize);
	return locals[index];
}

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
		method = startJavaMethod.lock();
		if (method->isStatic() != needStaticMethod)
		{
			throw runtime_error("Method staticness is incorrect.");
		}
	}
	else {
		method = startJavaMethod.lock();
	}
	vector<weak_ptr<VMHeapObject>> args;
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
	using namespace std::chrono_literals;
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
		std::this_thread::sleep_for(4s);
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