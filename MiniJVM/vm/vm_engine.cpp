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
#define printcode(msg, code) spdlog::info("{}:{}", ##msg, code)
#define printcodeW(msg, code) spdlog::warn("{}:{}", ##msg, code)
#else
#define printcode(code)
#endif

/* 这个函数就是主要的执行函数。 */
void VMEngine::execute(weak_ptr<VMJavaThread> thread, shared_ptr<VMThreadStackFrame> f)
{
	auto t = thread.lock();
	t->pushStackFrame(f);
	auto m = f->method.lock();
	auto codes = m->codes;
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
		case Constant_0x12_ldc:
		case Constant_0x13_ldc_w:
		{
			printcode("Sipush int to stack", codes[pc]);
			u1 index = codes[pc + 1];
			if (codes[pc] == Constant_0x13_ldc_w)
			{
				index = (index << 8) | codes[pc + 2];
			}
			auto ci = VMHelper::getVMConstantItem(clz->className(), index).lock();
			switch (ci->type)
			{
			case VMConstantItem::VMConstantItemType::TypeInteger:
			{
				f->pushStack(VMHelper::getIntegerVMHeapObject(std::dynamic_pointer_cast<VMConstantInteger>(ci)->value));
				break;
			}
			case VMConstantItem::VMConstantItemType::TypeFloat:
			{
				f->pushStack(VMHelper::getFloatVMHeapObject(std::dynamic_pointer_cast<VMConstantFloat>(ci)->value));
				break;
			}
			case VMConstantItem::VMConstantItemType::TypeString:
			{
				size_t stringIndex = std::dynamic_pointer_cast<VMConstantStringLiteral>(ci)->literalStringIndex;
				auto str = VMHelper::getConstantString(stringIndex);
				f->pushStack(VMHelper::getStringVMHeapObject(str));
				break;
			}
			case VMConstantItem::VMConstantItemType::TypeClass:
			{
				size_t stringIndex = std::dynamic_pointer_cast<VMConstantStringLiteral>(ci)->literalStringIndex;
				auto refClzName = VMHelper::getConstantString(stringIndex);
				auto refClzInstance = VMHelper::getClassRefVMHeapObject(VMHelper::loadClass(refClzName));
				f->pushStack(refClzInstance);
				break;
			}
			default:
			{
				assert(false);
				spdlog::error("Unsupported constant type:{} for code:{}", ci->type, codes[pc]);
				break;
			}
			}
			break;
		}
		case Load_0x19_aload:
		case Load_0x2a_aload_0:
		case Load_0x2b_aload_1:
		case Load_0x2c_aload_2:
		case Load_0x2d_aload_3:
		{
			int index = codes[pc] - Load_0x2a_aload_0;
			if (codes[pc] == Load_0x19_aload)
			{
				index = codes[pc + 1];
			}
			auto arg = f->getLocal(index);
			f->pushStack(arg);
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
		case Stack_0x59_dup:
		{
			auto top = f->peakStack();
			f->pushStack(top);
			break;
		}
		case Control_0xb1_return:
		{
			printcode("Return int to local", codes[pc]);
			//printcodeW("return from method:", w2s(m->lookupKey()));
			spdlog::info("return from method:{} in class:{}", w2s(m->lookupKey()), w2s(clz->className()));
			t->pc = lastPC;
			t->popStackFrame();
			// 直接返回。
			return;
		}
		case Reference_0xb3_putstatic:
		case Reference_0xb2_getstatic:
		{
			u2 fieldIndex = codes[pc + 1] << 8 | codes[pc + 2];
			auto fieldRef = VMHelper::getFieldOrMethod(clz->className(), fieldIndex);
			auto targetClass = VMHelper::loadClass(std::get<0>(fieldRef)).lock();
			assert(targetClass != nullptr);
			auto fieldSignature = std::get<1>(fieldRef);
			auto fieldName = std::get<2>(fieldRef);
			// 如果类还没有初始化，就先初始化。
			if (targetClass->needInitializing())
			{
				targetClass->initialize(thread);
			}
			if (codes[pc] == Reference_0xb2_getstatic)
			{
				printcode("Get static field to stack", codes[pc]);
				auto field = targetClass->findStaticField(fieldSignature, fieldName);
				f->pushStack(field);
			}
			else
			{
				printcode("Put to static field", codes[pc]);
				auto value = f->popStack();
				targetClass->putStaticField(fieldSignature, fieldName, value);
			}
			break;
		}
		case Reference_0xb6_invokevirtual:
		{
			u2 index = codes[pc + 1] << 8 | codes[pc + 2];
			auto methodRef = VMHelper::getFieldOrMethod(clz->className(), index);
			auto targetClass = VMHelper::loadClass(std::get<0>(methodRef)).lock();
			assert(targetClass != nullptr);
			auto className = std::get<0>(methodRef);
			auto methodSignature = std::get<1>(methodRef);
			auto methodName = std::get<2>(methodRef);
			// 如果类还没有初始化，就先初始化。
			if (!targetClass->needInitializing())
			{
				targetClass->initialize(thread);
			}
			auto method = targetClass->findMethod(methodSignature, methodName).lock();
			assert(!method->isStatic() || !method->isAbstract());
			auto signatures = method->splitSignature();
			// 至少要保证栈里有足够的参数。
			assert(f->stack.size() >= signatures.size());
			/*这里面有一个是函数的返回值 ，但是这里是实例函数，所以就个this参数，
			数字正好，所以就先这么算。*/
			auto argSize = signatures.size();
			vector<weak_ptr<VMHeapObject>> args;
			while (argSize > 0)
			{
				args.push_back(f->popStack());
				argSize --;
			}
			// 参数需要反转。因为参数是 [arg1, [arg2...]]
			std::reverse(args.begin(), args.end());
			if (method->isNative())
			{
				// 如果是本地方法。
				void* func = VMHelper::getNativeMethod(className, methodSignature, methodName);
				if (func == nullptr)
				{
					spdlog::error("NO NATIVE METHOD:{}", w2s(className + L"@" + methodName + methodSignature));
					assert(false);
				}
				auto nativeBackupPc = t->pc;
				t->pc = VMJavaThread::PC_UNDEFINED;
				auto result = invokeNativeMethod(func, args);
				t->pc = nativeBackupPc;
			}
			else
			{
				VMEngine::execute(t, method, args);
			}
			break;
		}
		case Reference_0xb7_invokespecial:
		{
			u2 index = codes[pc + 1] << 8 | codes[pc + 2];
			auto methodRef = VMHelper::getFieldOrMethod(clz->className(), index);
			auto targetClass = VMHelper::loadClass(std::get<0>(methodRef)).lock();
			assert(targetClass != nullptr);
			auto className = std::get<0>(methodRef);
			auto methodSignature = std::get<1>(methodRef);
			auto methodName = std::get<2>(methodRef);
			// 如果类还没有初始化，就先初始化。
			if (!targetClass->needInitializing())
			{
				targetClass->initialize(thread);
			}
			auto method = targetClass->findMethod(methodSignature, methodName).lock();
			if (method == nullptr)
			{
				throw runtime_error("AbstractMethodError:" + w2s(methodName));
			}
			assert(!method->isStatic());
			auto signatures = method->splitSignature();
			// 至少要保证栈里有足够的参数。
			assert(f->stack.size() >= signatures.size());
			/*这里面有一个是函数的返回值 ，但是这里是实例函数，所以就个this参数，
			数字正好，所以就先这么算。*/
			auto argSize = signatures.size();
			vector<weak_ptr<VMHeapObject>> args;
			while (argSize > 0)
			{
				args.push_back(f->popStack());
				argSize--;
			}
			// 参数需要反转。因为参数是 [arg1, [arg2...]]
			std::reverse(args.begin(), args.end());
			if (method->isNative())
			{
				// 如果是本地方法。
				void* func = VMHelper::getNativeMethod(className, methodSignature, methodName);
				if (func == nullptr)
				{
					spdlog::error("NO NATIVE METHOD:{}", w2s(className + L"@" + methodName + methodSignature));
					assert(false);
				}
				auto nativeBackupPc = t->pc;
				t->pc = VMJavaThread::PC_UNDEFINED;
				auto result = invokeNativeMethod(func, args);
				t->pc = nativeBackupPc;
			}
			else
			{
				VMEngine::execute(t, method, args);
			}
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
			if (!targetClass->needInitializing())
			{
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
			while (argSize > 0)
			{
				args.push_back(f->popStack());
				argSize--;
			}
			// 参数需要反转。因为参数是 [arg1, [arg2...]]
			std::reverse(args.begin(), args.end());

			if (method->isNative())
			{
				// 如果是本地方法。
				void *func = VMHelper::getNativeMethod(className, methodSignature, methodName);
				if (func == nullptr)
				{
					spdlog::error("NO NATIVE METHOD:{}", w2s(className + L"@" + methodName + methodSignature));
					assert(false);
				}
				auto nativeBackupPc = t->pc;
				t->pc = VMJavaThread::PC_UNDEFINED;
				auto result = invokeNativeMethod(func, args);
				t->pc = nativeBackupPc;
			}
			else
			{
				VMEngine::execute(t, method, args);
			}
			break;
		}
		case Reference_0xbb_new:
		{
			u2 index = codes[pc + 1] << 8 | codes[pc + 2];
			auto targetClzName = VMHelper::getRefClassName(clz->className(), index);

			auto targetClz = VMHelper::loadClass(targetClzName).lock();
			targetClz->initialize(t);
			auto obj = VMHelper::getInstanceVMHeapObject(targetClz);
			f->pushStack(obj);
			break;
		}
		case Reference_0xbd_anewarray:
		{
			u2 index = codes[pc + 1] << 8 | codes[pc + 2];
			int count = std::dynamic_pointer_cast<IntegerVMHeapObject>(f->popStack().lock())->intValue;
			assert(count >= 0);
			auto targetClzName = VMHelper::getRefClassName(clz->className(), index);

			auto targetClz = VMHelper::loadClass(targetClzName).lock();
			targetClz->initialize(t);
			auto arr = VMHelper::createArrayVMHelpObject(targetClz, count);
			f->pushStack(arr);
			break;
		}
		default:
		{
			spdlog::error("Unhandled code:{}", codes[pc]);
			assert(false);
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
	auto frame = make_shared<VMThreadStackFrame>(method, args);
	execute(thread, frame);
}