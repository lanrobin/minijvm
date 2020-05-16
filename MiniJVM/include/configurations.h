#ifndef __JVM_VM_CONFIGURATIONS_H__
#define __JVM_VM_CONFIGURATIONS_H__
#include "base_type.h"

struct Configurations {
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

	bool useFixHeap() const {
		return true;
	}

	long long maxHeapSize() const {

		// 先设置为2G
		return 2 * 1024 * 1024 * 1024LL;
	}

	string toString();
private:
	wstring bootStrapClassPath;
	wstring appClassPath;
	wstring targetClass;
};

#endif //__JVM_VM_CONFIGURATIONS_H__
