#include "base_type.h"

#include <string>
#include <iostream>
#include <cassert>
#include "platform.h"
#include "vm.h"

static shared_ptr<VM> VM_INSTANCE = nullptr;

VM::VM(const string& bs, const string &c) {
	bootStrapModulePath = bs;
	runningClassFile = c;
}

VM::~VM() {
	cout << "VM is gone" << endl;
}

int VM::run() {
	cout << "run vm with bs:" << bootStrapModulePath << ", classFile:" << runningClassFile << endl;
	return 0;
}

shared_ptr<VM> VM::getVM() {
	return VM_INSTANCE;
}
void VM::initVM(const string& bs, const string& c) {
	assert(VM_INSTANCE == nullptr);
	VM_INSTANCE = shared_ptr<VM>(new VM(bs, c));
	
}