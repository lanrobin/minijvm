#include "base_type.h"
#include "log.h"
#include "string_utils.h"
#include "vm_class.h"
#include "vm_heap.h"
#include "vm_thread.h"
#include "vm_engine.h"
#include "vm.h"
#include <chrono>
#include <thread>

// 为了让代码看着简单一点，用这宏来替换一下。
#define T weak_ptr<VMHeapObject>

/*这个函数在vm_engine_private.cpp里实现，免得一个文件太大。*/
weak_ptr<VMHeapObject> VMEngine::invokeNativeMethod(void* func, vector<weak_ptr<VMHeapObject>>& args)
{
	if (args.size() == 0) {
		return ((T(*)())func)();
	}
	else if (args.size() == 1) {
		return ((T(*)(T))func)(args[0]);
	}
	else if (args.size() == 2) {
		return ((T(*)(T, T))func)(args[0], args[1]);
	}
	else if (args.size() == 3) {
		return ((T(*)(T, T, T))func)(args[0], args[1], args[3]);
	}
	else if (args.size() == 4) {
		return ((T(*)(T, T, T, T))func)(args[0], args[1], args[2], args[3]);
	}
	else if (args.size() == 5) {
		return ((T(*)(T, T, T, T, T))func)(args[0], args[1], args[2], args[3], args[4]);
	}
	else if (args.size() == 6) {
		return ((T(*)(T, T, T, T, T, T))func)(args[0], args[1], args[2], args[3], args[4], args[5]);
	}
	else {
		throw runtime_error("Please add more number of parameter for native invoke.");
	}
}