#ifndef __JVM_BOOTSTRAP_CLASSLOADER__
#define __JVM_BOOTSTRAP_CLASSLOADER__

#include "base_type.h"
#include "vm_class.h"
#include "buffer.h"

class BootstrapClassLoader {
public:
	shared_ptr<VMClass> loadClass(const wstring& className);
	shared_ptr<VMClass> loadClass(shared_ptr<Buffer> buf);
};
#endif //__JVM_BOOTSTRAP_CLASSLOADER__