#ifndef __JVM_VM_H__
#define __JVM_VM_H__
#include <string>
#include "base_type.h"
class VM {
private:
	string bootStrapModulePath;
	string runningClassFile;

public:
	VM(const string& bs, const string c);
	~VM();
	int run();
};
#endif //__JVM_VM_H__