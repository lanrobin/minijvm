#include "base_type.h"
#include "vm.h"
#include "vm_heap.h"
#include "native/java_lang_System.h"

#include <chrono>

weak_ptr<VMHeapObject> java_lang_System_registerNatives() {
    auto vm = VM::getVM().lock();
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/io/PrintStream;)V", L"setOut0", &java_lang_System_setOut0);
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/io/PrintStream;)V", L"setErr0", &java_lang_System_setErr0);
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/io/InputStream;)V", L"setIn0", &java_lang_System_setIn0);
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/io/InputStream;)V", L"setIn0", &java_lang_System_setIn0);

    vm->registerNativeMethod(L"java/lang/System", L"()J", L"currentTimeMillis", &java_lang_System_currentTimeMillis);
    vm->registerNativeMethod(L"java/lang/System", L"()J", L"nanoTime", &java_lang_System_nanoTime);

    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/lang/Object;ILjava/lang/Object;II)V", L"arraycopy", &java_lang_System_arraycopy);
    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/lang/Object;)I", L"identityHashCode", &java_lang_System_identityHashCode);

    vm->registerNativeMethod(L"java/lang/System", L"(Ljava/lang/String;)Ljava/lang/String;", L"mapLibraryName", &java_lang_System_mapLibraryName);
    return VMHelper::getVoidVMHeapObject();
}
weak_ptr<VMHeapObject> java_lang_System_setIn0(weak_ptr<VMHeapObject> in)
{
    auto clz = VMHelper::loadClass(L"java/lang/System").lock();
    clz->putStaticField(L"Ljava/io/InputStream;", L"in", in);
    return VMHelper::getVoidVMHeapObject();
}
weak_ptr<VMHeapObject> java_lang_System_setOut0(weak_ptr<VMHeapObject> out)
{
    auto clz = VMHelper::loadClass(L"java/lang/System").lock();
    clz->putStaticField(L"Ljava/io/PrintStream;", L"out", out);
    return VMHelper::getVoidVMHeapObject();
}
weak_ptr<VMHeapObject> java_lang_System_setErr0(weak_ptr<VMHeapObject> err)
{
    auto clz = VMHelper::loadClass(L"java/lang/System").lock();
    clz->putStaticField(L"Ljava/io/PrintStream;", L"err", err);
    return VMHelper::getVoidVMHeapObject();
}

weak_ptr<VMHeapObject> java_lang_System_currentTimeMillis()
{
    // https://stackoverflow.com/questions/19555121/how-to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets
    using namespace std::chrono;
    auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return VMHelper::getLongVMHeapObject(ms);
}
weak_ptr<VMHeapObject> java_lang_System_nanoTime()
{
    // https://stackoverflow.com/questions/12937963/get-local-time-in-nanoseconds
    using namespace std::chrono;
    auto ns = duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
    return VMHelper::getLongVMHeapObject(ns);
}

weak_ptr<VMHeapObject> java_lang_System_arraycopy(weak_ptr<VMHeapObject> src, weak_ptr<VMHeapObject>  srcPos,
    weak_ptr<VMHeapObject> dest, weak_ptr<VMHeapObject> destPos,
    weak_ptr<VMHeapObject> length)
{
    throw runtime_error("Not implemented yet.");
}
weak_ptr<VMHeapObject> java_lang_System_identityHashCode(weak_ptr<VMHeapObject> x) {
    throw runtime_error("Not implemented yet.");
}
weak_ptr<VMHeapObject> java_lang_System_mapLibraryName(weak_ptr<VMHeapObject> libname) {
    throw runtime_error("Not implemented yet.");
}