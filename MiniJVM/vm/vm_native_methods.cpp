#include "vm.h"
#include "native/java_lang_System.h"
#include "native/java_lang_Object.h"
#include "native/java_lang_Class.h"
/*
这里会有两种注册方式：
一种是通过 clinit调用registerNatives来注册，通过 R2来实现。
别一种是直接注册。通过RUN来注册。
*/
#define CAST_METHOD(m) (void *)&m 
#define R2(className, prefix) registerNativeMethod(className, L"()V", L"registerNatives", CAST_METHOD(prefix##_registerNatives)); count ++
#define RUN(prefix) count += prefix##_runRegisterNatives(); 

int VM::initNativeMethods() {
	int count = 0;
	R2(L"java/lang/System", java_lang_System);
	R2(L"java/lang/Class", java_lang_Class);
	RUN(java_lang_Object)
		return count;
}