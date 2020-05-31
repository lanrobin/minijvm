#include "base_type.h"
#include "vm_heap.h"
#include "vm_class.h"
#include "string_utils.h"
#include "vm.h"

ArrayVMHeapObject::ArrayVMHeapObject(weak_ptr<VMClass> typeClz, size_t size) :ReferenceVMHeapObject(typeClz), maxsize(size) {
	elements.reserve(size);
	auto thisClzName = typeClz.lock()->getClassSignature();
	assert(thisClzName.length() > 0 && thisClzName[0] == L'[');
	componentType = VMHelper::loadClass(thisClzName.substr(1, thisClzName.length() - 1));
}
weak_ptr<IntegerVMHeapObject> VMHeapPool::createIntegerVMHeapObject(int defaultValue) {
	shared_ptr<IntegerVMHeapObject> obj = nullptr;
	auto clz = VMPrimitiveClass::getPrimitiveVMClass(L"I");
	assert(!clz.expired());
	obj = make_shared<IntegerVMHeapObject>(defaultValue, clz);
	storeObject(obj);
	return obj;
}
weak_ptr<FloatVMHeapObject> VMHeapPool::createFloatVMHeapObject(float defaultValue) {
	shared_ptr<FloatVMHeapObject> obj = make_shared<FloatVMHeapObject>(defaultValue);
	storeObject(obj);
	return obj;
}
weak_ptr<LongVMHeapObject> VMHeapPool::createLongVMHeapObject(long long defaultValue) {
	shared_ptr<LongVMHeapObject> obj = make_shared<LongVMHeapObject>(defaultValue);
	storeObject(obj);
	return obj;
}
weak_ptr<DoubleVMHeapObject> VMHeapPool::createDoubleVMHeapObject(double defaultValue) {
	shared_ptr<DoubleVMHeapObject> obj = make_shared<DoubleVMHeapObject>(defaultValue);
	storeObject(obj);
	return obj;
}
weak_ptr<VMHeapObject> VMHeapPool::createStringVMHeapObject(const wstring& defaultValue) {
	auto clz = VMHelper::loadClass(L"java/lang/String");
	shared_ptr<VMHeapObject> obj = make_shared<InstanceVMHeapObject>(clz);
	storeObject(obj);
	return obj;

}
weak_ptr<VMHeapObject> VMHeapPool::createVMHeapObject(const wstring& s) {
	shared_ptr<VMHeapObject> obj = nullptr;
	if (s[0] == L'L' && s[s.length() - 1] == L';') {
		// 普通的类
		auto clz = VMHelper::loadClass(s);
		obj = make_shared<InstanceVMHeapObject>(clz);
		storeObject(obj);
	}
	else {
		throw runtime_error("Incorrect object. signature:" + w2s(s));
	}
	return obj;
}
weak_ptr<ArrayVMHeapObject> VMHeapPool::createArrayVMHeapObject(const wstring& s, size_t demension) {
	shared_ptr<ArrayVMHeapObject> obj = nullptr;
	auto thizClassName = L"[" + s;
	auto clz = VMHelper::loadArrayClass(thizClassName);
	obj = make_shared<ArrayVMHeapObject>(clz, demension);
	storeObject(obj);
	return obj;
}
weak_ptr<ClassRefVMHeapObject> VMHeapPool::createClassRefVMHeapObject(weak_ptr<VMClass> clz) {
	assert(!clz.expired());
	auto obj = make_shared<ClassRefVMHeapObject>(clz);
	storeObject(obj);
	return obj;
}

weak_ptr<NullVMHeapObject> FixSizeVMHeapPool::getNullVMHeapObject() const {

	// 第一个元素就是null.
	return std::dynamic_pointer_cast<NullVMHeapObject>(objects[0]);
}

weak_ptr<VoidVMHeapObject> FixSizeVMHeapPool::getVoidVMHeapObject() const {
	// 第二个元素就是null.
	return std::dynamic_pointer_cast<VoidVMHeapObject>(objects[0]);
}

void FixSizeVMHeapPool::storeObject(shared_ptr< VMHeapObject> obj) {
	if (obj == nullptr) {
		spdlog::warn("Try to store nullptr into heap area.");
		return;
	}
	objects.push_back(obj);
}

weak_ptr< VMHeapObject> InstanceVMHeapObject::getField(const wstring& signature, const wstring& name) const {
	if (isInterface) {
		throw runtime_error("No field operation for Java interface.");
	}
	// 直接找自己的，如果没有就返回空,数据验证在put那里验证。
	auto key = makeLookupKey(signature, name);
	auto f = fields.find(key);
	if (f != fields.end()) {
		return f->second;
	}
	return std::weak_ptr<VMHeapObject>();
}
weak_ptr< VMHeapObject> InstanceVMHeapObject::getStaticField(const wstring& signature, const wstring& name) const {
	auto clz = typeClass.lock();
	return clz->findStaticField(signature, name);
}

void InstanceVMHeapObject::putField(const wstring& signature, const wstring& name, weak_ptr<VMHeapObject> value)
{
	// 如果field已经存在，直接写。
	/*
	其实如果不存在，要去验证一下是不是有这个字段，如果没有不让写，这里就先简单处理，其实Java编译器已经帮我们处理过了。
	*/
	auto key = makeLookupKey(signature, name);
	fields[key] = value;
}
void InstanceVMHeapObject::putStaticField(const wstring& signature, const wstring& name, weak_ptr<VMHeapObject> value)
{
	typeClass.lock()->putStaticField(signature, name, value);
}

void ArrayVMHeapObject::putArray(size_t index, weak_ptr<VMHeapObject> value) {
	if (index < 0 || index >= maxsize) {
		throw runtime_error("Should throw IndexOutOfBoundException.");
	}
	elements[index] = value;
}


weak_ptr<VMHeapObject> ArrayVMHeapObject::getArray(size_t index) const {
	if (index < 0 || index >= maxsize) {
		throw runtime_error("Should throw IndexOutOfBoundException.");
	}
	return elements[index];
}

shared_ptr<VMHeapPool> VMHeapPoolFactory::createVMHeapPool(weak_ptr<Configurations> conf) {
	auto cf = conf.lock();
	if (cf->useFixHeap()) {
		return make_shared< FixSizeVMHeapPool>(cf->maxHeapSize());
	}
	else {
		throw runtime_error("Exensible heap not impemented yet.");
	}
}