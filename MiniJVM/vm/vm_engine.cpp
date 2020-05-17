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
	auto clz = m->ownerClass.lock();
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
		case Constant_0x11_sipush:
		{
			printcode("Sipush int to stack", codes[pc]);
			int value = codes[pc + 1] << 8 | codes[pc + 2];
			f->pushStack(VMHelper::getIntegerVMHeapObject(value));
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
		case Reference_0xb2_getstatic:
		{
			u2 fieldIndex = codes[pc + 1] << 8 | codes[pc + 2];
			auto fieldRef = VMHelper::getFieldOrMethod(clz->className(), fieldIndex);
			auto targetClass = VMHelper::loadClass(std::get<0>(fieldRef)).lock();
			assert(targetClass != nullptr);
			auto fieldSignature = std::get<1>(fieldRef);
			auto fieldName = std::get<2>(fieldRef);
			// 如果类还没有初始化，就先初始化。
			if (targetClass->needInitializing()) {
				targetClass->initialize(thread);
			}

			auto field = targetClass->findStaticField(fieldSignature, fieldName);
			f->pushStack(field);
			break;
		}
		case Reference_0xb3_putstatic:
		{
			assert(false);
			break;
		}
		case Reference_0xb8_invokestatic:
		{
			u2 index = codes[pc + 1] << 8 | codes[pc + 2];
			auto methodRef = VMHelper::getFieldOrMethod(clz->className(), index);
			auto targetClass = VMHelper::loadClass(std::get<0>(methodRef)).lock();
			assert(targetClass != nullptr);
			auto className = std::get<0>(methodRef);
			auto methodSignature = std::get<1>(methodRef);
			auto methodName = std::get<2>(methodRef);
			// 如果类还没有初始化，就先初始化。
			if (!targetClass->needInitializing()) {
				targetClass->initialize(thread);
			}
			auto method = targetClass->findMethod(methodSignature, methodName).lock();
			assert(method->isStatic());
			// 把函数的签名拆分，可以从这里得到参数和返回值。
			// signatures.size()肯定是大于0的，最后一个
			// 就是函数的返回值，之前的都是参数。
			auto signatures = method->splitSignature();
			// 至少要保证栈里有足够的参数。
			assert(f->stack.size() >= signatures.size() - 1);
			auto argSize = signatures.size() - 1;
			vector<weak_ptr<VMHeapObject>> args;
			while (argSize > 0) {
				args.push_back(f->popStack());
			}
			// 参数需要反转。因为参数是 [arg1, [arg2...]]
			std::reverse(args.begin(), args.end());

			if (method->isNative()) {
				// 如果是本地方法。
				void* f = VMHelper::getNativeMethod(className, methodSignature, methodName);
				if (f == nullptr) {
					spdlog::error("NO NATIVE METHOD:{}", w2s(className + L"@" + methodName + methodSignature));
					assert(false);
				}
				auto nativeBackupPc = t->pc;
				t->pc = VMJavaThread::PC_UNDEFINED;
				auto result = invokeNativeMethod(f, args);
				t->pc = nativeBackupPc;
			}
			else {
				VMEngine::execute(t, method, args);
			}
			break;
		}
		default:
		{
			printcode("Unhandled code:{}", codes[pc]);
			break;
		}
		}
		pc += moveForwardSteps;
		//std::this_thread::sleep_for(2s);
	}
	t->pc = lastPC;
	spdlog::info("running:finished restore pc to :{}", lastPC);
	t->popStackFrame();
	spdlog::info("running:finished.");
}

void VMEngine::execute(weak_ptr<VMJavaThread> thread, weak_ptr<VMClassMethod> method, vector<weak_ptr<VMHeapObject>> args)
{
	auto frame = make_shared< VMThreadStackFrame>(method, args);
	execute(thread, frame);
}