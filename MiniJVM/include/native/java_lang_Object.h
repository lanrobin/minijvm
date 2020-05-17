#ifndef __java_lang_Object_h__
#define __java_lang_Object_h__
#include "../vm_class.h"

weak_ptr<VMHeapObject> java_lang_Object_getClass(weak_ptr<VMHeapObject>);
weak_ptr<VMHeapObject> java_lang_Object_clone(weak_ptr<VMHeapObject>);
weak_ptr<VMHeapObject> java_lang_Object_notify(weak_ptr<VMHeapObject>);
weak_ptr<VMHeapObject> java_lang_Object_notifyAll(weak_ptr<VMHeapObject>);
weak_ptr<VMHeapObject> java_lang_Object_wait(weak_ptr<VMHeapObject>, weak_ptr<VMHeapObject>);
int java_lang_Object_runRegisterNatives();
#endif //__java_lang_Object_h__