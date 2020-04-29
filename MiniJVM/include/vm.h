#ifndef __JVM_VM_H__
#define __JVM_VM_H__
#include <string>

class VM {
private:
	std::string bootStrapModulePath;
	std::string runningClassFile;

public:
	VM(const std::string& bs, const std::string c);
	~VM();
	int run();
};
#endif //__JVM_VM_H__