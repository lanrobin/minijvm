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

	auto packageEnd = name.find_last_of(L"/");
	if (packageEnd != wstring::npos) {
		packageName = name.substr(0, packageEnd);
	}

	spdlog::info("create class:{}", w2s(name));
	classLoader = cl;

	auto superClassName = cf->getClassName(cf->super_class);

	if (superClassName.size() > 0) {
		this->super = cl->loadClass(superClassName);
		//this->super->resolveSymbol();
	}
	else {
		spdlog::warn("Load java/lang/Object.");
	}

	auto interfaces = cf->interfaces;
	for (auto i = interfaces.begin(); i != interfaces.end(); i++) {
		auto interfaceName = cf->getClassName(*i);
		auto thisInterface = cl->loadClass(interfaceName);
		//thisInterface->resolveSymbol();
		this->interfaces.push_back(thisInterface);
	}

	auto methods = cf->methods;
	for (auto m = methods.begin(); m != methods.end(); m++) {
		auto cm = make_shared< VMClassMethod>(cf, *m);
		//cm->resolveSymbol();
		this->methods[cm->lookupKey()] = cm;
	}

	auto fs = cf->fields;
	for (auto f = fs.begin(); f != fs.end(); f++) {
		auto field = make_shared<VMClassField>(cf, *f);
		//field->resolveSymbol();
		auto key = field->lookupKey();
		if (field->isStatic()) {
			
			classStaticFieldLayout[key] = field;
			// 创建对应存储的空间。
			classStaticFields[key] = VMHeapPool::createVMHeapObject(field->signature);
		}
		else {
			classInstanceFieldLayout[key] = field;
		}
	}
}


VMClass::VMClass(const wstring& signature) {
	name = signature;
	spdlog::info("create class:{}", w2s(name));
}

wstring VMClass::getNextDimensionSignature(const wstring& signature) {
	assert(signature.size() > 1 && signature.find(L"[") == 0);
	return signature.substr(1);
}


bool VMClass::equals(shared_ptr<VMClass> other) const {
	if (other == nullptr) {
		return false;
	}
	// 直接比较全名是不是相等就行了。
	return name == super->className();
}

bool VMClass::isSubClassOf(shared_ptr<VMClass> other) const {
	if (other == nullptr) {
		return false;
	}
	auto super = this->super;
	while (super != nullptr) {
		if (super->equals(other)) {
			return true;
		}
		super = super->super;
	}
	return false;
}
bool VMClass::implemented(shared_ptr<VMClass> other) const {
	if (other == nullptr || !other->isInterface()) {
		return false;
	}
	for (auto i = interfaces.begin(); i != interfaces.end(); i++) {
		if ((*i)->equals(other)) {
			return true;
		}
	}
	if (super != nullptr) {
		return super->implemented(other);
	}
	return false;
}
bool VMClass::hasAccessibilityTo(shared_ptr<VMClass> other) const {
	if (other == nullptr) {
		return false;
	}
	if (other->isPublic()) {
		return true;
	}

	if (other->isProtected()) {
		return this->packageName == other->packageName;
	}
	return false;
}

vector<wstring> VMClass::splitSignatureToElement(const wstring& signature) {
	throw runtime_error("not implemented yet.");
}

shared_ptr<VMClassMethod> VMClass::findMethod(const wstring& methodSignature, const wstring & name) const {
	auto key = VMClassMethod::makeLookupKey(methodSignature, name);
	auto m = methods.find(key);
	if (m == methods.end()) {
		spdlog::warn("No method found for key:{}", w2s(key));
		return nullptr;
	}
	return m->second;
}

shared_ptr<VMHeapObject> VMClass::findStaticField(const wstring& methodSignature, const wstring& name) const {
	auto key = VMClassField::makeLookupKey(methodSignature, name);
	auto value = classStaticFields.find(key);
	if (value == classStaticFields.end()) {
		spdlog::warn("No field found for key:{}", w2s(key));
		return nullptr;
	}
	return value->second;
}


void VMClass::resolveSymbol() {
	for(auto m = methods.begin(); m != methods.end(); m++) {
		m->second->resolveSymbol(getSharedPtr());
	}

	for (auto f = classStaticFieldLayout.begin(); f != classStaticFieldLayout.end(); f++) {
		f->second->resolveSymbol(getSharedPtr());
	}

	for (auto f = classInstanceFieldLayout.begin(); f != classInstanceFieldLayout.end(); f++) {
		f->second->resolveSymbol(getSharedPtr());
	}
}
/*
这个常量是在类被加载的时候的占位，如果没有会出现循环调用。
*/
shared_ptr<VMClass> VMClass::LOADING_VMCLASS = make_shared<VMClass>(L"ClassBeingLoadPlaceHolder");

VMOrdinaryClass::VMOrdinaryClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl) :VMClass(cf, cl) {
	classType = VMClass::ClassType::ClassTypeOrdinaryClass;
}

VMInterfaceClass::VMInterfaceClass(shared_ptr<ClassFile> cf, shared_ptr<ClassLoader> cl) : VMClass(cf, cl) {
	classType = VMClass::ClassType::ClassTypeInterface;
}

VMArrayClass::VMArrayClass(const wstring& componentSignature) : VMClass(componentSignature){
	classType = VMClass::ClassType::ClassTypeArrayClass;
}


VMClassField::VMClassField(shared_ptr< ClassFile> cf, shared_ptr<Field_Info> fi){
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

vector<wstring> VMClassField::splitSignature(){
	vector<wstring> elements;
	elements.push_back(signature);
	return elements;
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

vector<wstring> VMClassMethod::splitSignature() {
	wstring sig(signature);
	replaceAll(sig, L"(", L"");
	replaceAll(sig, L")", L"");
	vector<wstring> elements;
	int i = 0;
	int previous = 0;
	bool inReference = false;
	bool inArray = false;
	while (i < sig.length()) {
		if (inReference) {
			if (sig[i] == L';') {
				elements.push_back(sig.substr(previous, i - previous + 1));
				inArray = false;
				inReference = false;
				previous = i;
			}
		}
		else if (inArray) {
			if (PRIMITIVE_TYPES.find(sig[i]) != PRIMITIVE_TYPES.end()) {
				elements.push_back(sig.substr(previous, i - previous + 1));
				inArray = false;
				inReference = false;
			}
			else if (sig[i] == L'L') {
				inReference = true;
			}
		}
		else if (sig[i] == L'[') {
			previous = i;
			inArray = true;
		}
		else if (sig[i] == L'L') {
			inReference = true;
			if (!inArray) {
				previous = i;
			}
		}
		else {
			elements.push_back(sig.substr(i, 1));
			previous = i;
		}
		i++;
	}
	return elements;
}

VMMethodExceptionTable::VMMethodExceptionTable(shared_ptr< ClassFile> cf, shared_ptr<Code_attribute::ExceptionTable> et)
{
	startPC = et->start_pc;
	endPC = et->end_pc;
	handlerPC = et->handler_pc;
	catchType = cf->getUtf8String(et->catch_type);
}

const unordered_map<wchar_t, int> VMClassResolvable::PRIMITIVE_TYPES =
{
	{L'B', 0},
	{L'C', 0},
	{L'D', 0},
	{L'F', 0},
	{L'I', 0},
	{L'J', 0},
	{L'S', 0},
	{L'Z', 0},
	{L'V', 1}
};

void VMClassResolvable::resolveSymbol(shared_ptr<VMClass> owner) {
	ownerClass = owner;
	auto symbols = splitSignature();
	for (auto s = symbols.begin(); s != symbols.end(); s++) {
		wstring name = *s;
		if (PRIMITIVE_TYPES.find(name[0]) != PRIMITIVE_TYPES.end()) {
			spdlog::info("skip to resolve primitive type:{}", w2s(name));
			continue;
		}
		else if (name.find(L"L") == 0) {
			// 这是一个类。
			wstring className = name.substr(1, name.length() - 2);
			auto clz = ownerClass->getClassLoader()->loadClass(className);
			if (!ownerClass->hasAccessibilityTo(clz)) {
				throw runtime_error("class:" + w2s(ownerClass->className()) + " has no accessible to " + w2s(clz->className()));
			}
		}
		else if (name.find(L"[") == 0) {
			// 数组
			auto clz = resolveArray(name);
		}
		else {
			throw runtime_error("Unknown symbol type:" + w2s(name));
		}
	}
}

unordered_map<wstring, shared_ptr< VMPrimitiveClass>> VMPrimitiveClass::AllPrimitiveClasses =
{
	{L"B", make_shared< VMPrimitiveClass >(L"byte")},
	{L"C", make_shared< VMPrimitiveClass >(L"char")},
	{L"D", make_shared< VMPrimitiveClass >(L"double")},
	{L"F", make_shared< VMPrimitiveClass >(L"float")},
	{L"I", make_shared< VMPrimitiveClass >(L"int")},
	{L"J", make_shared< VMPrimitiveClass >(L"long")},
	{L"S", make_shared< VMPrimitiveClass >(L"short")},
	{L"Z", make_shared< VMPrimitiveClass >(L"boolean")},
	{L"V", make_shared< VMPrimitiveClass >(L"void")}
};

shared_ptr<VMClass> VMClassResolvable::resolveArray(const wstring& sym) {
	if (sym.length() == 0) {
		throw runtime_error("Error in resolverArray, left sym is empty.");
	}
	shared_ptr<VMClass> componentType = nullptr;
	if (sym[0] == L'[') {
		// 如果还是数组，则递归
		wstring subName = sym.substr(1, sym.length() - 1);
		auto subComponentType = resolveArray(subName);
		componentType = make_shared< VMArrayClass>(subName);
	}
	else if (sym[0] == L'L') {
		// 如果是正常的引用类型。
		wstring className = sym.substr(1, sym.length() - 2);
		componentType = ownerClass->getClassLoader()->loadClass(className);
	}
	else {
		// 到了这里应该只剩下一个字符了。
		assert(sym.length() == 1);
		if (PRIMITIVE_TYPES.find(sym[0]) == PRIMITIVE_TYPES.end()) {
			throw runtime_error("Unsupported type:" + w2s(sym));
		}
		componentType = VMPrimitiveClass::AllPrimitiveClasses.find(sym)->second;
	}
	return componentType;
}