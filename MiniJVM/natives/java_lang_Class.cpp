#include "base_type.h"
#include "vm.h"
#include "vm_heap.h"
#include "native/java_lang_Class.h"

#include <chrono>

weak_ptr<VMHeapObject> java_lang_Class_registerNatives() {
    auto vm = VM::getVM().lock();
    /*
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/io/PrintStream;)V", L"setOut0", (void*)&java_lang_System_setOut0);
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/io/PrintStream;)V", L"setErr0", (void*)&java_lang_System_setErr0);
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/io/InputStream;)V", L"setIn0", (void*)&java_lang_System_setIn0);
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/io/InputStream;)V", L"setIn0", (void*)&java_lang_System_setIn0);

    vm->registerNativeMethod(L"java/lang/System", L"()J", L"currentTimeMillis", (void*)&java_lang_System_currentTimeMillis);
    vm->registerNativeMethod(L"java/lang/System", L"()J", L"nanoTime", (void*)&java_lang_System_nanoTime);

    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/lang/Object;ILjava/lang/Object;II)V", L"arraycopy", (void*)&java_lang_System_arraycopy);
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/lang/Object;)I", L"identityHashCode", (void*)&java_lang_System_identityHashCode);

    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/lang/String;)Ljava/lang/String;", L"mapLibraryName", (void*)&java_lang_System_mapLibraryName);
    */
    return VMHelper::getVoidVMHeapObject();
}