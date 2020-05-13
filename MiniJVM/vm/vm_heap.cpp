#include "vm_heap.h"
#include "vm_class.h"
#include "string_utils.h"
#include "vm.h"

template<typename T>
weak_ptr<VMHeapObject> VMHeapPool::createVMHeapObject(weak_ptr<VMClass> clz, T& param)
{
	if (clz.expired()) {
		throw runtime_error("Template class doesnot exist any more, Could not create instance for it.");
	}
	shared_ptr<VMClass> c = clz.lock();
	shared_ptr< VMHeapObject> obj = nullptr;
	wstring s = c->getClassSignature();
	if (s.length < 1) {
		throw runtime_error("Unable to create heap object of empty signature.");
	}
	else if (s.length == 1) {
		if (VMPrimitiveClass::isPrimitiveTypeSignature(s)) {
			if (s == L"B" || s == L"C" || s == L"I" || s == L"S" || s == L"Z") {
				obj = make_shared<IntegerVMHeapObject>(param, clz);
			}
			else if (s == L"F") {
				obj = make_shared<FloatVMHeapObject>(param, clz);
			}
			else if (s == L"D") {
				obj = make_shared<DoubleVMHeapObject>(param, clz);
			}
			else if (s == L"J") {
				obj = make_shared<LongVMHeapObject>(param, clz);
			}
			else if (s == L"V") {
				obj = make_shared<VoidVMHeapObject>(param, clz);
			}
			else {
				assert(false);
			}
		}
		else {
			throw runtime_error("Unsupported primitive type:" + w2s(s));
		}
	}
	else if (s[0] == L'L') {
		// ��ͨ����
		obj = make_shared< ClassVMHeapObject>(clz);
	}
	else if (s[0] == L"[") {
		//����
		obj = make_shared<ArrayVMHeapObject>(clz, param);
	}
	else {
		throw runtime_error("Unable to create heap object of signature:" + w2s(s));
	}
	storeObject(obj);
	return obj;
}

weak_ptr<VMHeapObject> FixSizeVMHeapPool::getNullVMHeapObject() const {

	// ��һ��Ԫ�ؾ���null.
	return objects[0];
}

void FixSizeVMHeapPool::storeObject(shared_ptr< VMHeapObject> obj) {
	if (obj == nullptr) {
		spdlog::warn("Try to store nullptr into heap area.");
		return;
	}
	objects.push_back(obj);
}

weak_ptr< VMHeapObject> ClassVMHeapObject::getField(const wstring& signature, const wstring& name) const {
	if (isInterface) {
		throw runtime_error("No field operation for Java interface.");
	}
	// ֱ�����Լ��ģ����û�оͷ��ؿ�,������֤��put������֤��
	auto key = makeLookupKey(signature, name);
	auto f = fields.find(key);
	if (f != fields.end()) {
		return f->second;
	}
	return std::weak_ptr<VMHeapObject>();
};
weak_ptr< VMHeapObject> ClassVMHeapObject::getStaticField(const wstring& signature, const wstring& name) const {
	auto clz = typeClass.lock();
	return clz->findStaticField(signature, name);
};

void ClassVMHeapObject::putField(const wstring& signature, const wstring& name, weak_ptr<VMHeapObject> value)
{
	// ���field�Ѿ����ڣ�ֱ��д��
	/*
	��ʵ��������ڣ�Ҫȥ��֤һ���ǲ���������ֶΣ����û�в���д��������ȼ򵥴�����ʵJava�������Ѿ������Ǵ�����ˡ�
	*/
	auto key = makeLookupKey(signature, name);
	fields[key] = value;
};
void ClassVMHeapObject::putStaticField(const wstring& signature, const wstring& name, weak_ptr<VMHeapObject> value)
{
	typeClass.lock()->putStaticField(signature, name, value);
};

void ArrayVMHeapObject::putArray(size_t index, weak_ptr<VMHeapObject> value) {
	if (index < 0 || index >= maxsize) {
		throw runtime_error("Should throw IndexOutOfBoundException.");
	}
	elements[index] = value;
};


weak_ptr<VMHeapObject> ArrayVMHeapObject::getArray(size_t index) const {
	if (index < 0 || index >= maxsize) {
		throw runtime_error("Should throw IndexOutOfBoundException.");
	}
	return elements[index];
};

shared_ptr<VMHeapPool> VMHeapPoolFactory::createVMHeapPool(weak_ptr<Configurations> conf) {
	auto cf = conf.lock();
	if (cf->useFixHeap()) {
		return make_shared< FixSizeVMHeapPool>(cf->maxHeapSize());
	}
	else {
		throw runtime_error("Exensible heap not impemented yet.");
	}
}