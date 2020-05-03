#ifndef __JVM_VM_CONFIGURATIONS_H__
#define __JVM_VM_CONFIGURATIONS_H__
#include "base_type.h"

class Configurations {
public:
	Configurations(const wstring& configFilePath, int argc, char** argv);

	bool isExtensibleMethodArea() const {
		return true;
	}

	string toString();
};

#endif //__JVM_VM_CONFIGURATIONS_H__
