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

using namespace std::chrono_literals;

#define SHOW_CODE

#ifdef SHOW_CODE
#define printcode(msg, code) spdlog::info(msg##":{0:x}", code)
#else
#define printcode(code)
#endif

/*一条指令执行完成后需要向前移动几个，
减1就是后面要跟的参数个数。
0表示这个是动态的，需要根据情况确定。*/
int VMEngine::moveSteps[256] = {
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	2,
	3,
	2,
	3,
	3,
	2,
	2,
	2,
	2,
	2,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	2,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	3,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	2,
	0,
	0,
	1,
	1,
	1,
	1,
	1,
	1,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	5,
	5,
	3,
	2,
	3,
	1,
	1,
	3,
	3,
	1,
	1,
	0,
	4,
	3,
	3,
	5,
	5,
	1,
	1,
	1};

/* 这个函数就是主要的执行函数。 */
void VMEngine::execute(weak_ptr<VMJavaThread> thread, shared_ptr<VMThreadStackFrame> f)
{
	auto t = thread.lock();
	t->pushStackFrame(f);
	auto m = f->method.lock();
	auto codes = m->codes;
	vector<weak_ptr<VMHeapObject>> &stack = f->stack;
	auto lastPC = t->pc;
	long long &pc = t->pc;
	pc = 0;
	while (pc < codes.size())
	{
		int moveForwardSteps = moveSteps[codes[pc]];
		switch (codes[pc])
		{
		case Constant_0x00_nop:
		{
			printcode("No operation", codes[pc]);
			// 啥也不干
			break;
		}
		case Constant_0x01_aconst_null:
		{
			// 把null放到操作数栈顶。
			printcode("Push null to stack", codes[pc]);
			f->pushStack(VMHelper::getNullVMHeapObject());
			break;
		}
		case Constant_0x02_iconst_m1:
		case Constant_0x03_iconst_0:
		case Constant_0x04_iconst_1:
		case Constant_0x05_iconst_2:
		case Constant_0x06_iconst_3:
		case Constant_0x07_iconst_4:
		case Constant_0x08_iconst_5:
		{
			printcode("Push_N int to stack", codes[pc]);
			f->pushStack(VMHelper::getIntegerVMHeapObject(codes[pc] - 0x3));
			break;
		}

		case Store_0x36_istore:
		{
			printcode("Store int to local", codes[pc]);
			int index = codes[pc + 1];
			auto obj = f->popStack();
			f->putLocal(index, obj);
			break;
		}

		case Store_0x3b_istore_0:
		case Store_0x3c_istore_1:
		case Store_0x3d_istore_2:
		case Store_0x3e_istore_3:
		{
			printcode("Store_N int to local", codes[pc]);
			u1 index = Store_0x3e_istore_3 - codes[pc];
			auto obj = f->popStack();
			f->putLocal(index, obj);
			break;
		}
		default:
		{
			printcode("Unhandled code:{}", codes[pc]);
			break;
		}
		}
		pc += moveForwardSteps;
		std::this_thread::sleep_for(2s);
	}
	spdlog::info("running:finished.");
}

void VMEngine::execute(weak_ptr<VMJavaThread> thread, weak_ptr<VMClassMethod> method, vector<weak_ptr<VMHeapObject>> args)
{
}