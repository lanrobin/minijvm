#include "base_type.h"
#include "vm_classloader.h"
#include "vm_class.h"

VMClass::VMClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl) {

	accessFlags = cf->access_flags;

	// ������ClassName,���������Ϊ�ڴ����ɵģ������������ɡ�
	name = cf->getCanonicalClassName();

}
