#ifndef __JVM_VM_H__
#define __JVM_VM_H__
#include <string>
#include "base_type.h"
#include "configurations.h"
#include "vm_method_area.h"

class VM : public std::enable_shared_from_this<VM> {
private:
	shared_ptr<VMMethodArea> methodArea;
	shared_ptr<Configurations> conf;
	bool initilized = false;

public:
	VM();
	void initVM(shared_ptr<Configurations> conf);
	~VM();
	int run();
	shared_ptr<VMMethodArea> getMethodArea() const { return methodArea; }

public:
	// static 
	static shared_ptr<VM> getVM();
};
#endif //__JVM_VM_H__