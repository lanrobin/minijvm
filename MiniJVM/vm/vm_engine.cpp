#include "base_type.h"
#include "log.h"
#include "string_utils.h"
#include "vm_class.h"
#include "vm_heap.h"
#include "vm_thread.h"
#include "vm_engine.h"
#include <chrono>

/*这个函数就是主要的执行函数。*/
void VMEngine::execute(weak_ptr< VMThread> thread, weak_ptr<VMThreadStackFrame> frame) 
{
	int count = 0;
	while (count < 15)
	{
		spdlog::info("running:{}", count);
		std::this_thread::sleep_for(2s);
		count++;
	}
}

void VMEngine::execute(weak_ptr<VMThread> thread, weak_ptr<VMClassMethod> method, vector<weak_ptr<VMHeapObject>> args) {

}