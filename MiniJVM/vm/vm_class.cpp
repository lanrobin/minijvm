#include "base_type.h"
#include "vm_classloader.h"
#include "vm_class.h"
#include "vm_heap.h"
#include "log.h"
#include "string_utils.h"

VMClass::VMClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl) {

	accessFlags = cf->access_flags;

	// 正常的ClassName,如果是数组为内存生成的，不在这里生成。
	name = cf->getCanonicalClassName();

	classLoader = cl;

	auto superClassName = cf->getClassName(cf->super_class);

	if (superClassName.size() > 0) {
		this->super = cl->loadClass(superClassName);
	}
	else {
		spdlog::warn("Load java/lang/Object.");
	}

	auto interfaces = cf->interfaces;
	for (auto i = interfaces.begin(); i != interfaces.end(); i++) {
		auto interfaceName = cf->getClassName(*i);
		this->interfaces.push_back(cl->loadClass(interfaceName));
	}

	auto methods = cf->methods;
	for (auto m = methods.begin(); m != methods.end(); m++) {
		auto cm = make_shared< VMClassMethod>(cf, *m);
		this->methods[cm->signature] = cm;
	}

	auto fs = cf->fields;
	for (auto f = fs.begin(); f != fs.end(); f++) {
		auto field = make_shared<VMClassField>(cf, *f);
		if (field->isStatic()) {
			classStaticFieldLayout[field->signature] = field;
			// 创建对应存储的空间。
			classStaticFields[field->signature] = VMHeapPool::createVMHeapObject(field->signature);
		}
		else {
			classInstanceFieldLayout[field->signature] = field;
		}
	}
}

VMClass::VMClass(const wstring& signature) {
	name = signature;
}

wstring VMClass::getNextDimensionSignature(const wstring& signature) {
	assert(signature.size() > 1 && signature.find(L"[") == 0);
	return signature.substr(1);
}

vector<wstring> VMClass::splitSignatureToElement(const wstring& signature) {
	throw runtime_error("not implemented yet.");
}

shared_ptr<VMClassMethod> VMClass::findMethod(const wstring& methodSignature) const {
	auto m = methods.find(methodSignature);
	if (m == methods.end()) {
		throw runtime_error("No method found for signature:" + w2s(methodSignature));
	}
	return m->second;
}


VMOrdinaryClass::VMOrdinaryClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl) :VMClass(cf, cl) {
	classType = VMClass::ClassType::ClassTypeOrdinaryClass;
}

VMInterfaceClass::VMInterfaceClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl) : VMClass(cf, cl) {
	classType = VMClass::ClassType::ClassTypeInterface;
}

VMArrayClass::VMArrayClass(const wstring& componentSignature, u4 l) : VMClass(componentSignature), lenghth(l) {
	classType = VMClass::ClassType::ClassTypeInterface;
	lenghth = l;
}


VMClassField::VMClassField(shared_ptr< ClassFile> cf, shared_ptr<Field_Info> fi) {
	accessFlags = fi->access_flags;
	name = cf->getUtf8String(fi->name_index);
	signature = cf->getUtf8String(fi->descriptor_index);

	// 我们先处理Deprecated的情况
	for (auto a = fi->attributes.begin(); a != fi->attributes.end(); a++) {
		auto attriName = (*a)->getAttributeName();
		if (attriName == L"Deprecated") {
			deprecated = true;
		}
	}
}


VMClassMethod::VMClassMethod(shared_ptr< ClassFile> cf, shared_ptr<Method_Info> mi) {
	accessFlags = mi->access_flags;
	name = cf->getUtf8String(mi->name_index);
	signature = cf->getUtf8String(mi->descriptor_index);

	// 处理attribute
	for (auto m = mi->attributes.begin(); m != mi->attributes.end(); m++) {
		auto attriName = (*m)->getAttributeName();
		if (attriName == L"Code") {
			auto codeAttri = std::dynamic_pointer_cast<Code_attribute>((*m));
			maxStack = codeAttri->max_stack;
			maxLocals = codeAttri->max_locals;
			codes.insert(codes.end(), codeAttri->code.begin(), codeAttri->code.end());
			for (auto et = codeAttri->exception_table.begin(); et != codeAttri->exception_table.end(); et++) {
				exceptionTable.push_back(make_shared<VMMethodExceptionTable>(cf, *et));
			}

			// 这里attri还是有其它的attri，暂时不用处理。
			// 有 LineNumberTable， LocalVariableTable， LocalVariableTypeTable， StackMapTable
		}else if (attriName == L"Deprecated") {
			deprecated = true;
		}
		else if (attriName == L"Exceptions") {
			auto exAttri = std::dynamic_pointer_cast<Exceptions_attribute>((*m));
			for (auto e = exAttri->exception_index_table.begin(); e != exAttri->exception_index_table.end(); e++) {
				throwExceptions.push_back(cf->getClassName(*e));
			}
		}

		// 其它的先不处理。
	}
}

VMMethodExceptionTable::VMMethodExceptionTable(shared_ptr< ClassFile> cf, shared_ptr<Code_attribute::ExceptionTable> et)
{
	startPC = et->start_pc;
	endPC = et->end_pc;
	handlerPC = et->handler_pc;
	catchType = cf->getUtf8String(et->catch_type);
}