#include "base_type.h"
#include "vm_classloader.h"
#include "vm_class.h"
#include "vm_heap.h"
#include "log.h"
#include "string_utils.h"

// �����debug��ѡ�����������ص�ʱ��������ֶκͷ�����Ӧ�ķ���Ҳresolve�ˣ�Ӧ��û�б�Ҫ��������ִ�е�ʱ����resolve.
//#undef RESOLVE_ON_CLASS_LOAD
#define RESOLVE_ON_CLASS_LOAD


VMClass::VMClass(const wstring& signature, shared_ptr<ClassLoader> cl) {
	name = signature;
	auto packageEnd = name.find_last_of(L"/");
	if (packageEnd != wstring::npos) {
		packageName = name.substr(0, packageEnd);
	}
	classLoader = cl;
	spdlog::info("create class:{} and package:{}", w2s(name), w2s(packageName));
}

wstring VMClass::getNextDimensionSignature(const wstring& signature) {
	assert(signature.size() > 1 && signature.find(L"[") == 0);
	return signature.substr(1);
}


bool VMClass::equals(shared_ptr<VMClass> other) const {
	if (other == nullptr) {
		return false;
	}
	// ֱ�ӱȽ�ȫ���ǲ�����Ⱦ����ˡ�
	return name == other->className();
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

shared_ptr<VMClassMethod> VMReferenceClass::findMethod(const wstring& methodSignature, const wstring & name) const {
	auto key = VMClassMethod::makeLookupKey(methodSignature, name);
	auto m = methods.find(key);
	if (m == methods.end()) {
		spdlog::warn("No method found for key:{}", w2s(key));
		return nullptr;
	}
	return m->second;
}

shared_ptr<VMHeapObject> VMReferenceClass::findStaticField(const wstring& methodSignature, const wstring& name) const {
	auto key = VMClassField::makeLookupKey(methodSignature, name);
	auto value = staticFields.find(key);
	if (value == staticFields.end()) {
		spdlog::warn("No field found for key:{}", w2s(key));
		return nullptr;
	}
	return value->second;
}


void VMReferenceClass::resolveSymbol() {

	if (super != nullptr) {
		super->resolveSymbol();
	}

	for (auto i = interfaces.begin(); i != interfaces.end(); i++) {
		(*i)->resolveSymbol();
	}
	for(auto m = methods.begin(); m != methods.end(); m++) {
		m->second->resolveSymbol();
	}

	for (auto f = staticFieldLayout.begin(); f != staticFieldLayout.end(); f++) {
		f->second->resolveSymbol();
	}

	for (auto f = instanceFieldLayout.begin(); f != instanceFieldLayout.end(); f++) {
		f->second->resolveSymbol();
	}

}

void VMReferenceClass::classLoaded(shared_ptr< VMReferenceClass> other) {
	if (other != nullptr) {
		hasAccessibilityTo(other);
	}
}

bool VMLoadableClass::loadClassInfo(shared_ptr<ClassFile> cf) {

	accessFlags = cf->access_flags;

	auto superClassName = cf->getClassName(cf->super_class);

	if (superClassName.size() > 0) {
		this->super = classLoader->loadClass(superClassName);
		//this->super->resolveSymbol();
	}
	else {
		spdlog::warn("Load java/lang/Object.");
	}

	auto interfaces = cf->interfaces;
	for (auto i = interfaces.begin(); i != interfaces.end(); i++) {
		auto interfaceName = cf->getClassName(*i);
		auto thisInterface = classLoader->loadClass(interfaceName);
		//thisInterface->resolveSymbol();
		this->interfaces.push_back(thisInterface);
	}
	bool hasCInitMethod = false;
	auto methods = cf->methods;
	for (auto m = methods.begin(); m != methods.end(); m++) {
		auto cm = make_shared< VMClassMethod>(cf, *m, getSharedPtr());

		// ���������� <cinit>����������Ҫ����ʼ�������û�����ùܡ�
		if (cm->signature == L"<cinit>") {
			hasCInitMethod = true;
		}
		//cm->resolveSymbol();
		this->methods[cm->lookupKey()] = cm;
	}

	auto fs = cf->fields;
	for (auto f = fs.begin(); f != fs.end(); f++) {
		auto field = make_shared<VMClassField>(cf, *f, getSharedPtr());
		//field->resolveSymbol();
		auto key = field->lookupKey();
		if (field->isStatic()) {

			staticFieldLayout[key] = field;
			// ������Ӧ�洢�Ŀռ䡣
			//staticFields[key] = VMHeapPool::createVMHeapObject(field->signature);
		}
		else {
			instanceFieldLayout[key] = field;
		}
	}
	
	state = hasCInitMethod? InitializeState::NotInitialized: InitializeState::NotInitialized;
#ifdef RESOLVE_ON_CLASS_LOAD
	// �Լ���resolve.
	resolveSymbol();
#endif 
	return true;
}


VMClassField::VMClassField(shared_ptr< ClassFile> cf, shared_ptr<Field_Info> fi, shared_ptr<VMClass> owner):VMClassResolvable(owner){
	accessFlags = fi->access_flags;
	name = cf->getUtf8String(fi->name_index);
	signature = cf->getUtf8String(fi->descriptor_index);

	// �����ȴ���Deprecated�����
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

VMClassMethod::VMClassMethod(shared_ptr< ClassFile> cf, shared_ptr<Method_Info> mi, shared_ptr<VMClass> owner) :VMClassResolvable(owner) {
	accessFlags = mi->access_flags;
	name = cf->getUtf8String(mi->name_index);
	signature = cf->getUtf8String(mi->descriptor_index);

	// ����attribute
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

			// ����attri������������attri����ʱ���ô���
			// �� LineNumberTable�� LocalVariableTable�� LocalVariableTypeTable�� StackMapTable
		}else if (attriName == L"Deprecated") {
			deprecated = true;
		}
		else if (attriName == L"Exceptions") {
			auto exAttri = std::dynamic_pointer_cast<Exceptions_attribute>((*m));
			for (auto e = exAttri->exception_index_table.begin(); e != exAttri->exception_index_table.end(); e++) {
				throwExceptions.push_back(cf->getClassName(*e));
			}
		}

		// �������Ȳ�����
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

void VMClassMethod::resolveSymbol() {
	VMClassResolvable::resolveSymbol();
	for (auto e = exceptionTable.begin(); e != exceptionTable.end(); e++) {
		(*e)->resolveSymbol(ownerClass->getClassLoader());
	}
}

VMMethodExceptionTable::VMMethodExceptionTable(shared_ptr< ClassFile> cf, shared_ptr<Code_attribute::ExceptionTable> et)
{
	startPC = et->start_pc;
	endPC = et->end_pc;
	handlerPC = et->handler_pc;
	catchType = cf->getClassName(et->catch_type);
}

void VMMethodExceptionTable::resolveSymbol(shared_ptr<ClassLoader> cl) {
	if (catchType.length() == 0) {
		// any exception type.
		return;
	}
	cl->loadClass(catchType);
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

void VMClassResolvable::resolveSymbol() {
	auto symbols = splitSignature();
	for (auto s = symbols.begin(); s != symbols.end(); s++) {
		wstring name = *s;
		if (PRIMITIVE_TYPES.find(name[0]) != PRIMITIVE_TYPES.end()) {
			spdlog::info("skip to resolve primitive type:{}", w2s(name));
			continue;
		}
		else if (name.find(L"L") == 0) {
			// ����һ���ࡣ
			wstring className = name.substr(1, name.length() - 2);
			auto clz = ownerClass->getClassLoader()->loadClass(className);
			if (!ownerClass->hasAccessibilityTo(clz)) {
				throw runtime_error("class:" + w2s(ownerClass->className()) + " has no accessible to " + w2s(clz->className()));
			}
		}
		else if (name.find(L"[") == 0) {
			// ����, ��Ҫ���������ࡣ
			auto clz = ownerClass->getClassLoader()->defineClass(name);
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