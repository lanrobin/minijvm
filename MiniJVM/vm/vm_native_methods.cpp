#include "vm.h"
#include "native/java_lang_System.h"
#include "native/java_lang_Object.h"
/*
�����������ע�᷽ʽ��
һ����ͨ�� clinit����registerNatives��ע�ᣬͨ�� R2��ʵ�֡�
��һ����ֱ��ע�ᡣͨ��RUN��ע�ᡣ
*/
#define R2(className, prefix) registerNativeMethod(className, L"()V", L"registerNatives", (void *)&##prefix##_registerNatives); count ++
#define RUN(prefix) count += prefix##_runRegisterNatives(); 

int VM::initNativeMethods() {
	int count = 0;
	R2(L"java/lang/System", java_lang_System);
	RUN(java_lang_Object)
	return count;
}