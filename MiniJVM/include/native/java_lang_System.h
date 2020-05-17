#ifndef __java_lang_System_h__
#define __java_lang_System_h__
#include "../vm_heap.h"
weak_ptr<VMHeapObject> java_lang_System_registerNatives();
weak_ptr<VMHeapObject> java_lang_System_setIn0(weak_ptr<VMHeapObject> in);
weak_ptr<VMHeapObject> java_lang_System_setOut0(weak_ptr<VMHeapObject> out);
weak_ptr<VMHeapObject> java_lang_System_setErr0(weak_ptr<VMHeapObject> err);

weak_ptr<VMHeapObject> java_lang_System_currentTimeMillis();
weak_ptr<VMHeapObject> java_lang_System_nanoTime();

weak_ptr<VMHeapObject> java_lang_System_arraycopy(weak_ptr<VMHeapObject> src, weak_ptr<VMHeapObject>  srcPos,
    weak_ptr<VMHeapObject> dest, weak_ptr<VMHeapObject> destPos,
    weak_ptr<VMHeapObject> length);
weak_ptr<VMHeapObject> java_lang_System_identityHashCode(weak_ptr<VMHeapObject> x);
weak_ptr<VMHeapObject> java_lang_System_mapLibraryName(weak_ptr<VMHeapObject> libname);
#endif 