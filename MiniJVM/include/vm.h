#ifndef __JVM_VM_H__
#define __JVM_VM_H__
#include <string>
#include "base_type.h"
class VM : public std::enable_shared_from_this<VM> {
private:
	string bootStrapModulePath;
	string runningClassFile;

	VM(const string& bs, const string &c);
public:
	~VM();
	int run();

public:
	// static 
	static shared_ptr<VM> getVM();
	static void initVM(const string& bs, const string & c);
};
#endif //__JVM_VM_H__