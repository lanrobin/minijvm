#include "base_type.h"

#include <string>
#include <iostream>
#include <cassert>
#include "base_type.h"
#include "log.h"
#include "platform.h"
#include "vm.h"
#include "configurations.h"
#include "vm_method_area.h"
#include "vm_classloader.h"
#include "string_utils.h"


VM::VM(): conf(nullptr){
	mainThread = make_shared<VMJavaThread>(pthread_self());
	allThreads.push_back(mainThread);

	gcThread = make_shared<VMGCThread>();
	gcThread->startExecute();
	allThreads.push_back(gcThread);
}

VM::~VM() {
	spdlog::info("VM is gone");
	//cout << "VM is gone" << endl;
}

int VM::run() {
	spdlog::info("run with config:{}", conf->toString());


	shared_ptr<VMClass> mainClass = appClassLoader->loadClass(conf->getTargetClass());
	auto mainMethod = mainClass->findMethod(L"([Ljava/lang/String;)V");
	mainThread->setJavaMethod(mainMethod);
	mainThread->startExecute();
	spdlog::info("run {}.{}", w2s(mainClass->className()), w2s(mainMethod->name));
	for (auto t = allThreads.begin(); t != allThreads.end(); t++) {
		if (!pthread_equal(mainThread->nativeThread, (*t)->nativeThread)) {
			spdlog::info("waiting for one thread to exit.");
			pthread_join((*t)->nativeThread, NULL);
		}
	}
	spdlog::info("all thread exits.");
	return 0;
}

shared_ptr<VM> VM::getVM() {
	static shared_ptr<VM> VM_INSTANCE = make_shared<VM>();
	return VM_INSTANCE;
}
void VM::initVM(shared_ptr<Configurations> cfs) {
	if (!initilized) {
		initilized = true;
		conf = cfs;
		methodArea = VMMethodAreaFactory::createMethodArea(conf);
		bootstrapClassLoader = make_shared<BootstrapClassLoader>(conf->getBootStrapClassPath(), nullptr);
		appClassLoader = make_shared<AppClassLoader>(conf->getAppClassPath(), bootstrapClassLoader);
	}
	else {
		spdlog::warn("VM has already been initialized.");
	}
}