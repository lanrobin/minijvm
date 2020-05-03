#include "base_type.h"

#include <string>
#include <iostream>
#include <cassert>
#include "platform.h"
#include "vm.h"
#include "configurations.h"
#include "vm_method_area.h"

static shared_ptr<VM> VM_INSTANCE = nullptr;

VM::VM(shared_ptr<Configurations> conf): conf(conf){
	methodArea = VMMethodAreaFactory::createMethodArea(conf);
}

VM::~VM() {
	cout << "VM is gone" << endl;
}

int VM::run() {
	cout << "run with config:"<<endl << conf->toString()<< endl;
	return 0;
}

shared_ptr<VM> VM::getVM() {
	return VM_INSTANCE;
}
void VM::initVM(shared_ptr<Configurations> conf) {
	assert(VM_INSTANCE == nullptr);
	VM_INSTANCE = shared_ptr<VM>(new VM(conf));
	
}