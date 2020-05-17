#include "base_type.h"
#include "pthread.h"
#include "vm_heap.h"
#include "vm.h"
#include "native/java_lang_Object.h"

weak_ptr<VMHeapObject> java_lang_Object_getClass(weak_ptr<VMHeapObject> thiz) {
	return VMHelper::getClassRefVMHeapObject(thiz.lock()->typeClass);
}
weak_ptr<VMHeapObject> java_lang_Object_clone(weak_ptr<VMHeapObject> thiz){
	auto thizClass = thiz.lock()->typeClass.lock();
	auto clonableClzz = VMHelper::loadClass(L"Ljava/lang/Cloneable;");
	if (!thizClass->implemented(clonableClzz))
	{
		throw runtime_error("object doesn't implemented java.lang.Cloneable.");
	}
	throw runtime_error("object doesn't implemented java.lang.Cloneable.");
}
weak_ptr<VMHeapObject> java_lang_Object_notify(weak_ptr<VMHeapObject> thiz) {
	return VMHelper::getVoidVMHeapObject();
}
weak_ptr<VMHeapObject> java_lang_Object_notifyAll(weak_ptr<VMHeapObject> thiz) {
	return VMHelper::getVoidVMHeapObject();
}
weak_ptr<VMHeapObject> java_lang_Object_wait(weak_ptr<VMHeapObject> thiz, weak_ptr<VMHeapObject> milliseconds) {
	return VMHelper::getVoidVMHeapObject();
}
int java_lang_Object_runRegisterNatives() {
	int count = 0;
	auto vm = VM::getVM().lock();
	vm->registerNativeMethod(L"java/lang/Object", L"()Ljava/lang/Class;", L"getClass", &java_lang_Object_getClass);
	count++;

	vm->registerNativeMethod(L"java/lang/Object", L"()java/lang/Object", L"clone", &java_lang_Object_clone);
	count++;

	vm->registerNativeMethod(L"java/lang/Object", L"()V", L"notify", &java_lang_Object_notify);
	count++;

	vm->registerNativeMethod(L"java/lang/Object", L"()V", L"notifyAll", &java_lang_Object_notifyAll);
	count++;

	vm->registerNativeMethod(L"java/lang/Object", L"(J)V", L"wait", &java_lang_Object_wait);
	count++;
	return count;
}