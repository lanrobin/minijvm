#include <string>
#include <iostream>

#include "platform.h"
#include "vm.h"

VM::VM(const std::string& bs, const std::string c) {
	bootStrapModulePath = bs;
	runningClassFile = c;
}

VM::~VM() {
	std::cout << "VM is gone" << std::endl;
}

int VM::run() {
	std::cout << "run vm with bs:" << bootStrapModulePath << ", classFile:" << runningClassFile << std::endl;
	return 0;
}