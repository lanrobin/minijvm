#include "base_type.h"
#include "vm_classloader.h"
#include "vm_class.h"
#include "vm_heap.h"
#include "vm.h"
#include "log.h"
#include "string_utils.h"
#include "vm_engine.h"

// 这个是debug的选项，就是在类加载的时候把它的字段和方法对应的方法也resolve了，应该没有必要，可以在执行的时候再resolve.
//#undef RESOLVE_ON_CLASS_LOAD
#define RESOLVE_ON_CLASS_LOAD

VMClass::VMClass(const wstring &name, weak_ptr<ClassLoader> cl)
{
	this->name = name;
	auto packageEnd = name.find_last_of(L"/");
	if (packageEnd != wstring::npos)
	{
		packageName = name.substr(0, packageEnd);
	}
	classLoader = cl;

	pthread_mutexattr_init(&initializeMutexAttr);

	pthread_mutexattr_settype(&initializeMutexAttr, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init(&initializeMutex, &initializeMutexAttr);

	spdlog::info("create class:{} and package:{}", w2s(name), w2s(packageName));
}

VMClass::~VMClass() {
	pthread_mutexattr_destroy(&initializeMutexAttr);
	pthread_mutex_destroy(&initializeMutex);
	spdlog::info("Class:{}, type:{} gone", w2s(name), classType);
}

wstring VMClass::getNextDimensionSignature(const wstring &signature)
{
	assert(signature.size() > 1 && signature.find(L"[") == 0);
	return signature.substr(1);
}

bool VMClass::equals(weak_ptr<VMClass> other) const
{
	if (other.expired())
	{
		return false;
	}
	// 直接比较全名是不是相等就行了。
	return name == other.lock()->className();
}

bool VMClass::isSubClassOf(weak_ptr<VMClass> other) const
{
	if (other.expired())
	{
		return false;
	}
	auto super = this->super;
	while (!super.expired())
	{
		if (super.lock()->equals(other))
		{
			return true;
		}
		super = super.lock()->super;
	}
	return false;
}
bool VMClass::implemented(weak_ptr<VMClass> other) const
{
	if (other.expired() || !other.lock()->isInterface())
	{
		return false;
	}
	for (auto i = interfaces.begin(); i != interfaces.end(); i++)
	{
		if (!(*i).expired() && (*i).lock()->equals(other))
		{
			return true;
		}
	}
	if (!super.expired())
	{
		return super.lock()->implemented(other);
	}
	return false;
}
bool VMClass::hasAccessibilityTo(weak_ptr<VMClass> other) const
{
	if (other.expired())
	{
		return false;
	}
	if (other.lock()->isPublic())
	{
		return true;
	}

	if (other.lock()->isProtected())
	{
		return this->packageName == other.lock()->packageName;
	}
	return false;
}

vector<wstring> VMClass::splitSignatureToElement(const wstring &signature)
{
	throw runtime_error("not implemented yet.");
}

void VMClass::initialize(weak_ptr<VMJavaThread> thread) {
	// 先拿到锁，防止其它线程同时初始化这个类。
	pthread_mutex_lock(&initializeMutex);
	if (state != InitializeState::NotInitialized) {
		pthread_mutex_unlock(&initializeMutex);
		return;
	}
	state = InitializeState::Initializing;

	// 先初始化父类。
	if (!super.expired()) {
		super.lock()->initialize(thread);
	}

	// 初始化接口。
	for (auto i = interfaces.begin(); i != interfaces.end(); i++) {
		if (!(*i).expired()) {
			(*i).lock()->initialize(thread);
		}
	}
	initializeStaticField();

	// 再调用clinit方法。
	auto clinit = findMethod(L"()V", L"<clinit>");
	if (!clinit.expired()) {
		spdlog::info("Found <clinit> for class:{} execute it.", w2s(name));
		vector<weak_ptr<VMHeapObject>> args;
		VMEngine::execute(thread, clinit, args);
	}
	state = InitializeState::Initialized;
	pthread_mutex_unlock(&initializeMutex);
}

void VMClass::initializeStaticField() {
	
	spdlog::info("No initialize static field required for class:{}", w2s(name));
}

weak_ptr<VMClassMethod> VMReferenceClass::findMethod(const wstring &methodSignature, const wstring &name) const
{
	auto key = VMClassMethod::makeLookupKey(methodSignature, name);
	auto m = methods.find(key);
	if (m == methods.end())
	{
		spdlog::warn("No method found for key:{}", w2s(key));
		return std::weak_ptr<VMClassMethod>();
	}
	return m->second;
}

weak_ptr<VMHeapObject> VMReferenceClass::findStaticField(const wstring &methodSignature, const wstring &name) const
{
	auto key = VMClassField::makeLookupKey(methodSignature, name);
	auto value = staticFields.find(key);
	if (value == staticFields.end())
	{
		spdlog::warn("No field found for key:{}", w2s(key));
		return std::weak_ptr<VMHeapObject>();
	}
	return value->second;
}

weak_ptr< VMClassField> VMReferenceClass::findFieldLayout(const wstring& methodSignature, const wstring& name) const {
	auto key = VMClassField::makeLookupKey(methodSignature, name);
	auto fl = allFieldLayout.find(key);
	if (fl != allFieldLayout.end()) {
		return fl->second;
	}
	return std::weak_ptr<VMClassField>();
}

void VMReferenceClass::resolveSymbol()
{

	if (!super.expired())
	{
		super.lock()->resolveSymbol();
	}

	for (auto i = interfaces.begin(); i != interfaces.end(); i++)
	{
		if (!(*i).expired())
		{
			(*i).lock()->resolveSymbol();
		}
	}
	for (auto m = methods.begin(); m != methods.end(); m++)
	{
		m->second->resolveSymbol();
	}

	for (auto f = staticFieldLayout.begin(); f != staticFieldLayout.end(); f++)
	{
		f->second->resolveSymbol();
	}

	for (auto f = instanceFieldLayout.begin(); f != instanceFieldLayout.end(); f++)
	{
		f->second->resolveSymbol();
	}
}

void VMReferenceClass::classLoaded(weak_ptr<VMReferenceClass> other)
{
	if (!other.expired())
	{
		hasAccessibilityTo(other);
	}
}

bool VMLoadableClass::loadClassInfo(shared_ptr<ClassFile> cf)
{
	// signature是 L类名; 的形式。
	signature = L"L" + name + L";";
	accessFlags = cf->access_flags;

	auto superClassName = cf->getClassName(cf->super_class);

	if (superClassName.size() > 0)
	{
		this->super = classLoader.lock()->loadClass(superClassName);
		//this->super->resolveSymbol();
	}
	else
	{
		spdlog::warn("Load java/lang/Object.");
	}

	auto interfaces = cf->interfaces;
	for (auto i = interfaces.begin(); i != interfaces.end(); i++)
	{
		auto interfaceName = cf->getClassName(*i);
		auto thisInterface = classLoader.lock()->loadClass(interfaceName);
		//thisInterface->resolveSymbol();
		this->interfaces.push_back(thisInterface);
	}
	auto methods = cf->methods;
	for (auto m = methods.begin(); m != methods.end(); m++)
	{
		auto cm = make_shared<VMClassMethod>(cf, *m, getSharedPtr());
		//cm->resolveSymbol();
		this->methods[cm->lookupKey()] = cm;
	}

	auto fs = cf->fields;
	for (auto f = fs.begin(); f != fs.end(); f++)
	{
		auto field = make_shared<VMClassField>(cf, *f, getSharedPtr());
		//field->resolveSymbol();
		auto key = field->lookupKey();
		if (field->isStatic())
		{
			// 先查检对应的类是不是存在并且可以访问。
			//field->resolveSymbol(); 先不检查，运行时再检查。
			staticFieldLayout[key] = field;
			// 创建对应存储的空间。不用创建，初始化的才需要创建，否则在找的时候返回Null对象就行了。
			//staticFields[key] = VM::getVM().lock()->getHeapPool().lock()->createVMHeapObject(field->signature, 0);
		}
		else
		{
			instanceFieldLayout[key] = field;
		}
		/*
		if (field->isFinal() && field->signature == L"Ljava/lang/String;") {
			throw runtime_error("String static field need special care.");
		}*/

		allFieldLayout[key] = field;
	}

	state = InitializeState::NotInitialized;
#ifdef RESOLVE_ON_CLASS_LOAD
	// 自己先resolve.
	//resolveSymbol();
#endif
	return true;
}

void VMLoadableClass::initializeStaticField()
{
	spdlog::info("initialize static field for class:{}", w2s(name));
	
	/*
	这里有两种情况：
	1. 所有的 final primitive和String类都要在这里初始化；
	2. 其它的类型的初始化在<cinit>函数里进行，所以这里不用管了。
	*/
	for (auto f = staticFieldLayout.begin(); f != staticFieldLayout.end(); f++) {
		auto fd = (*f).second;
		auto s = fd->signature;
		if (fd->isStatic() && fd->isFinal()) {
			if (s == L"B" || s == L"C" || s == L"I" || s == L"S" || s == L"Z") {
				auto attr = std::dynamic_pointer_cast<VMConstantInteger>(VMHelper::getVMConstantItem(name, fd->initializeAttribute).lock());
				staticFields[fd->lookupKey()] = VMHelper::getIntegerVMHeapObject(attr->value);
			}
			else if (s == L"D") {
				auto attr = std::dynamic_pointer_cast<VMConstantLongAndDouble>(VMHelper::getVMConstantItem(name, fd->initializeAttribute).lock());
				staticFields[fd->lookupKey()] = VMHelper::getDoubleVMHeapObject(attr->doubleValue);
			}
			else if (s == L"J") {
				auto attr = std::dynamic_pointer_cast<VMConstantLongAndDouble>(VMHelper::getVMConstantItem(name, fd->initializeAttribute).lock());
				staticFields[fd->lookupKey()] = VMHelper::getLongVMHeapObject(attr->longValue);
			}
			else if (s == L"F") {
				auto attr = std::dynamic_pointer_cast<VMConstantFloat>(VMHelper::getVMConstantItem(name, fd->initializeAttribute).lock());
				staticFields[fd->lookupKey()] = VMHelper::getFloatVMHeapObject(attr->value);
			}
			else if (s == L"Ljava/lang/String;") {
				auto attr = std::dynamic_pointer_cast<VMConstantStringLiteral>(VMHelper::getVMConstantItem(name, fd->initializeAttribute).lock());
				auto value = VMHelper::getConstantString(attr->literalStringIndex);
				staticFields[fd->lookupKey()] = VMHelper::getStringVMHeapObject(value);
			}
			else {
				spdlog::info("Need not initialize:{}, it will be initiliazied in <cinit>. SKIP....", w2s(fd->lookupKey()));
			}
		}
	}
}
VMClassField::VMClassField(shared_ptr<ClassFile> cf, shared_ptr<Field_Info> fi, weak_ptr<VMClass> owner) : VMClassResolvable(owner)
{
	accessFlags = fi->access_flags;
	name = cf->getUtf8String(fi->name_index);
	signature = cf->getUtf8String(fi->descriptor_index);

	// 我们先处理Deprecated的情况
	for (auto a = fi->attributes.begin(); a != fi->attributes.end(); a++)
	{
		auto attriName = (*a)->getAttributeName();
		if (attriName == L"Deprecated")
		{
			deprecated = true;
		}
		else if (attriName == L"ConstantValue") {
			auto cv = std::dynamic_pointer_cast<ConstantValue_attribute>((*a));
			initializeAttribute = cv->constantvalue_index;
		}
	}
}

vector<wstring> VMClassField::splitSignature()
{
	vector<wstring> elements;
	elements.push_back(signature);
	return elements;
}

VMClassMethod::VMClassMethod(shared_ptr<ClassFile> cf, shared_ptr<Method_Info> mi, weak_ptr<VMClass> owner) : VMClassResolvable(owner)
{
	accessFlags = mi->access_flags;
	name = cf->getUtf8String(mi->name_index);
	signature = cf->getUtf8String(mi->descriptor_index);

	// 处理attribute
	for (auto m = mi->attributes.begin(); m != mi->attributes.end(); m++)
	{
		auto attriName = (*m)->getAttributeName();
		if (attriName == L"Code")
		{
			auto codeAttri = std::dynamic_pointer_cast<Code_attribute>((*m));
			maxStack = codeAttri->max_stack;
			maxLocals = codeAttri->max_locals;
			codes.insert(codes.end(), codeAttri->code.begin(), codeAttri->code.end());
			for (auto et = codeAttri->exception_table.begin(); et != codeAttri->exception_table.end(); et++)
			{
				exceptionTable.push_back(make_shared<VMMethodExceptionTable>(cf, *et));
			}

			// 这里attri还是有其它的attri，暂时不用处理。
			// 有 LineNumberTable， LocalVariableTable， LocalVariableTypeTable， StackMapTable
		}
		else if (attriName == L"Deprecated")
		{
			deprecated = true;
		}
		else if (attriName == L"Exceptions")
		{
			auto exAttri = std::dynamic_pointer_cast<Exceptions_attribute>((*m));
			for (auto e = exAttri->exception_index_table.begin(); e != exAttri->exception_index_table.end(); e++)
			{
				throwExceptions.push_back(cf->getClassName(*e));
			}
		}

		// 其它的先不处理。
	}
}

vector<wstring> VMClassMethod::splitSignature()
{
	if (splittedSignatures.size() == 0)
	{
		wstring sig(signature);
		replaceAll(sig, L"(", L"");
		replaceAll(sig, L")", L"");
		size_t i = 0;
		size_t previous = 0;
		bool inReference = false;
		bool inArray = false;
		while (i < sig.length())
		{
			if (inReference)
			{
				if (sig[i] == L';')
				{
					splittedSignatures.push_back(sig.substr(previous, i - previous + 1));
					inArray = false;
					inReference = false;
					previous = i;
				}
			}
			else if (inArray)
			{
				if (VMPrimitiveClass::isPrimitiveTypeSignature(sig[i]))
				{
					splittedSignatures.push_back(sig.substr(previous, i - previous + 1));
					inArray = false;
					inReference = false;
				}
				else if (sig[i] == L'L')
				{
					inReference = true;
				}
			}
			else if (sig[i] == L'[')
			{
				previous = i;
				inArray = true;
			}
			else if (sig[i] == L'L')
			{
				inReference = true;
				if (!inArray)
				{
					previous = i;
				}
			}
			else
			{
				splittedSignatures.push_back(sig.substr(i, 1));
				previous = i;
			}
			i++;
		}
	}
	return splittedSignatures;
}

void VMClassMethod::resolveSymbol()
{
	VMClassResolvable::resolveSymbol();
	for (auto e = exceptionTable.begin(); e != exceptionTable.end(); e++)
	{
		(*e)->resolveSymbol(ownerClass.lock()->getClassLoader());
	}
}

VMMethodExceptionTable::VMMethodExceptionTable(shared_ptr<ClassFile> cf, shared_ptr<Code_attribute::ExceptionTable> et)
{
	startPC = et->start_pc;
	endPC = et->end_pc;
	handlerPC = et->handler_pc;
	catchType = cf->getClassName(et->catch_type);
}

void VMMethodExceptionTable::resolveSymbol(weak_ptr<ClassLoader> cl)
{
	if (catchType.length() == 0 || cl.expired())
	{
		// any exception type.
		return;
	}
	cl.lock()->loadClass(catchType);
}


void VMClassResolvable::resolveSymbol()
{
	auto symbols = splitSignature();
	for (auto s = symbols.begin(); s != symbols.end(); s++)
	{
		wstring name = *s;
		if (VMPrimitiveClass::isPrimitiveTypeSignature(name))
		{
			spdlog::info("skip to resolve primitive type:{}", w2s(name));
			continue;
		}
		else if (name.find(L"L") == 0)
		{
			// 这是一个类。
			auto cl = ownerClass.lock()->getClassLoader().lock();
			wstring className = name.substr(1, name.length() - 2);
			auto clz = cl->loadClass(className);
			auto ownerClz = ownerClass.lock();
			if (!ownerClz->hasAccessibilityTo(clz))
			{
				throw runtime_error("class:" + w2s(ownerClz->className()) + " has no accessible to " + w2s(clz.lock()->className()));
			}
		}
		else if (name.find(L"[") == 0)
		{
			// 数组, 需要创建数组类。
			auto clz = ownerClass.lock()->getClassLoader().lock()->defineClass(name);
		}
		else
		{
			throw runtime_error("Unknown symbol type:" + w2s(name));
		}
	}
}

bool VMOrdinaryClass::putStaticField(const wstring& signature, const wstring& name, weak_ptr< VMHeapObject> value) {
	// 这里本来应该要验证是不可写，是不是存在的，但是Java编译器已经验证过了，所以这里暂时偷懒不管了。
	auto key = VMClassResolvable::makeLookupKey(signature, name);
	staticFields[key] = value;
	return true;
}

const unordered_map<wchar_t, int> VMPrimitiveClass::PRIMITIVE_TYPES =
{
	{L'B', 0},
	{L'C', 0},
	{L'D', 0},
	{L'F', 0},
	{L'I', 0},
	{L'J', 0},
	{L'S', 0},
	{L'Z', 0},
	{L'V', 1} };

unordered_map<wstring, shared_ptr<VMPrimitiveClass>> VMPrimitiveClass::AllPrimitiveClasses =
	{
		{L"B", make_shared<VMPrimitiveClass>(L"byte")},
		{L"C", make_shared<VMPrimitiveClass>(L"char")},
		{L"D", make_shared<VMPrimitiveClass>(L"double")},
		{L"F", make_shared<VMPrimitiveClass>(L"float")},
		{L"I", make_shared<VMPrimitiveClass>(L"int")},
		{L"J", make_shared<VMPrimitiveClass>(L"long")},
		{L"S", make_shared<VMPrimitiveClass>(L"short")},
		{L"Z", make_shared<VMPrimitiveClass>(L"boolean")},
		{L"V", make_shared<VMPrimitiveClass>(L"void")}
	};

weak_ptr<VMPrimitiveClass> VMPrimitiveClass::getPrimitiveVMClass(const wstring& signature) {
	auto result = std::weak_ptr< VMPrimitiveClass>();
	if (signature.length() != 1) {
		return result;
	}
	auto clz = AllPrimitiveClasses.find(signature);
	if (clz != AllPrimitiveClasses.end()) {
		return clz->second;
	}
	return result;
}
bool VMPrimitiveClass::isPrimitiveTypeSignature(const wstring& signature){
	if (signature.length() != 1) {
		return false;
	}
	return PRIMITIVE_TYPES.find(signature[0]) != PRIMITIVE_TYPES.end();
}

bool VMPrimitiveClass::isPrimitiveTypeSignature(wchar_t c){
	wstring sig(1, c);
	return isPrimitiveTypeSignature(sig);
}