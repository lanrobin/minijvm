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


VM::VM(): conf(nullptr){
}

VM::~VM() {
	spdlog::info("VM is gone");
	//cout << "VM is gone" << endl;
}

int VM::run() {
	spdlog::info("run with config:{}", conf->toString().c_str());
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
	}
	else {
		spdlog::warn("VM has already been initialized.");
	}
}