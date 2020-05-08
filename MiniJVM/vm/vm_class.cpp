#include "base_type.h"
#include "vm_classloader.h"
#include "vm_class.h"

VMClass::VMClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl) {

	accessFlags = cf->access_flags;

	// 正常的ClassName,如果是数组为内存生成的，不在这里生成。
	name = cf->getCanonicalClassName();

}
