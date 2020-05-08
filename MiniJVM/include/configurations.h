#ifndef __JVM_VM_CONFIGURATIONS_H__
#define __JVM_VM_CONFIGURATIONS_H__
#include "base_type.h"

class Configurations {
public:
	Configurations(const wstring& configFilePath, int argc, char** argv);

	bool isExtensibleMethodArea() const {
		return true;
	}

	wstring getBootStrapClassPath() const {
		return bootStrapClassPath;
	}

	wstring getAppClassPath() const {
		return appClassPath;
	}

	wstring getTargetClass() const {
		return targetClass;
	}

	string toString();
private:
	wstring bootStrapClassPath;
	wstring appClassPath;
	wstring targetClass;
};

#endif //__JVM_VM_CONFIGURATIONS_H__
