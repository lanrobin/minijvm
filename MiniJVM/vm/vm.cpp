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
	spdlog::info("VM created.");
}

VM::~VM() {
	spdlog::info("methodArea use_count:{}", methodArea.use_count());
	spdlog::info("bootstrapClassLoader use_count:{}", bootstrapClassLoader.use_count());
	spdlog::info("appClassLoader use_count:{}", appClassLoader.use_count());
	spdlog::info("conf use_count:{}", conf.use_count());
	methodArea = nullptr;
	appClassLoader = nullptr;
	bootstrapClassLoader = nullptr;
	conf = nullptr;
	spdlog::info("VM gone.");
	//cout << "VM is gone" << endl;
}

int VM::run() {
	spdlog::info("run with config:{}", conf->toString());

	mainThread->startExecute();
	for (auto t = allThreads.begin(); t != allThreads.end(); t++) {
		if (!pthread_equal(mainThread->nativeThread, (*t)->nativeThread)) {
			spdlog::info("waiting for one thread to exit.");
			pthread_join((*t)->nativeThread, NULL);
		}
	}
	spdlog::info("all thread exits.");
	return 0;
}

weak_ptr<VM> VM::getVM() {
	static shared_ptr<VM> VM_INSTANCE = make_shared<VM>();
	return VM_INSTANCE;
}
void VM::initVM(shared_ptr<Configurations> cfs) {
	if (!initilized) {
		spdlog::info("VM initializing.");
		initilized = true;
		conf = cfs;
		methodArea = VMMethodAreaFactory::createMethodArea(conf);
		bootstrapClassLoader = make_shared<BootstrapClassLoader>(conf->getBootStrapClassPath(), std::weak_ptr<ClassLoader>());
		appClassLoader = make_shared<AppClassLoader>(conf->getAppClassPath(), bootstrapClassLoader);

		//其它初始化的代码
		mainThread = make_shared<VMJavaThread>(pthread_self());

		// 设置好开始的参数
		mainThread->setRunningParameters(conf->getTargetClass(), L"main", L"([Ljava/lang/String;)V", true);
		allThreads.push_back(mainThread);

		gcThread = make_shared<VMGCThread>();
		allThreads.push_back(gcThread);
		gcThread->startExecute();
		spdlog::info("VM initialized.");
	}
	else {
		spdlog::warn("VM has already been initialized.");
	}
}