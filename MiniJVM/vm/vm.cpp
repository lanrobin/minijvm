#include "base_type.h"

#include <string>
#include <iostream>

#include "platform.h"
#include "vm.h"

VM::VM(const string& bs, const string c) {
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