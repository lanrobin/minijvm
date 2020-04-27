#include <iostream>
#include<fstream>
#include <iomanip>
#include "base_type.h"
#include "platform.h"
#include "clazz_reader.h"


#define DEBUG_READ_FILE_IN_BYTE

u1 peaku1(std::istream& is)
{
	u1 v1 =  is.peek();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "peak at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	return v1;
}


u2 peaku2(std::istream& is) {
	u1 v1 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "peak at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	u1 v2 = is.peek();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "peak at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v2 << endl;
#endif
	is.unget();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "rewind at:0x" << is.tellg() << endl;
#endif
#ifdef __JVM_LITTLE_ENDIAN__
	return ((v1 << 8) | v2) & 0xFFFF;
#else // __LITTLE_ENDIAN__
	return ((v2 << 8) | v1) & 0xFFFF;
#endif // __LITTLE_ENDIAN__
}


u4 peaku4(std::istream& is) {
	u1 v1 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "peak at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	u1 v2 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "peak at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v2 << endl;
#endif
	u1 v3 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "peak at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v3 << endl;
#endif
	u4 v4 = is.peek();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "peak at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v4 << endl;
#endif
	is.unget();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "rewind at:0x" << is.tellg() << endl;
#endif
	is.unget();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "rewind at:0x" << is.tellg() << endl;
#endif
	is.unget();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "rewind at:0x" << is.tellg() << endl;
#endif
#ifdef __JVM_LITTLE_ENDIAN__
	return ((v1 << 24) | (v2 << 16) | (v3 << 8) | v4) & 0xFFFFFFFF;
#else // __LITTLE_ENDIAN__
	return ((v4 << 24) | (v3 << 16) | (v2 << 8) | v1) & 0xFFFFFFFF;
#endif // __LITTLE_ENDIAN__
}

u1 readu1(std::istream& is) {
	u1 value = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)value << endl;
#endif
	return value;
}

u2 readu2(std::istream& is) {
	u1 v1 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	u1 v2 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v2 << endl;
#endif
#ifdef __JVM_LITTLE_ENDIAN__
	return ((v1 << 8) | v2) & 0xFFFF;
#else // __LITTLE_ENDIAN__
	return ((v2 << 8) | v1) & 0xFFFF;
#endif // __LITTLE_ENDIAN__
}


u4 readu4(std::istream& is) {
	u1 v1 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	u1 v2 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v2 << endl;
#endif
	u1 v3 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v3 << endl;
#endif
	u4 v4 = is.get();
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v4 << endl;
#endif
#ifdef __JVM_LITTLE_ENDIAN__
	return ((v1 << 24) | (v2 << 16) | (v3 << 8) | v4) & 0xFFFFFFFF;
#else // __LITTLE_ENDIAN__
	return ((v4 << 24) | (v3 << 16) | (v2 << 8) | v1) & 0xFFFFFFFF;
#endif // __LITTLE_ENDIAN__
}

u4 read_u1_vector(vector<u1>& v, istream& is, u4 count) {
	// 这里应该可能换更好的方法。
	for( u4 it = 0; it < count; it++) {
		u1 value = readu1(is);
		v.push_back(value);
	}
	return count;
}

u4 read_u2_vector(vector<u2>& v, istream& is, u4 count) {
	// 这里应该可能换更好的方法。
	for (u4 it = 0; it < count; it++) {
		u2 value = readu2(is);
		v.push_back(value);
	}
	return count;
}
template< typename T>
u4 read_vector(vector<shared_ptr<T>>& v, istream& is, u4 count) {
	for (u4 it = 0; it < count; it++) {
		v.push_back(make_shared<T>(is));
	}
	return count;
}

shared_ptr<Attribute_Info> readAttributeInfo(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp)
{
	u2 attribute_name_index = peaku2(is);
	auto utf8 = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(cp[attribute_name_index]);
	wstring attributeName = utf8->toUTF8String();

	// order by https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.7 Table 4.7-A. Predefined class file attributes (by section)
	if (attributeName == L"ConstantValue"){
		return make_shared<ConstantValue_attribute>(is, cp);
	}
	else if (attributeName == L"Code") {
		return make_shared<Code_attribute>(is, cp);
	}
	else if (attributeName == L"StackMapTable") {
		return make_shared<StackMapTable_attribute>(is, cp);
	}
	else if (attributeName == L"Exceptions") {
		return make_shared<Exceptions_attribute>(is, cp);
	}
	else if (attributeName == L"InnerClasses") {
		return make_shared<InnerClasses_attribute>(is, cp);
	}
	else if (attributeName == L"EnclosingMethod") {
		return make_shared<EnclosingMethod_attribute>(is, cp);
	}
	else if (attributeName == L"Synthetic") {
		return make_shared<Synthetic_attribute>(is, cp);
	}
	else if (attributeName == L"Signature") {
		return make_shared<Signature_attribute>(is, cp);
	}
	else if (attributeName == L"SourceFile") {
		return make_shared<SourceFile_attribute>(is, cp);
	}
	else if (attributeName == L"SourceDebugExtension") {
		return make_shared<SourceDebugExtension_attribute>(is, cp);
	}
	else if (attributeName == L"LineNumberTable") {
		return make_shared<LineNumberTable_attribute>(is, cp);
	}
	else if (attributeName == L"LocalVariableTable") {
		return make_shared<LocalVariableTable_attribute>(is, cp);
	}
	else if (attributeName == L"LocalVariableTypeTable") {
		return make_shared<LocalVariableTypeTable_attribute>(is, cp);
	}
	else if (attributeName == L"Deprecated") {
		return make_shared<Deprecated_attribute>(is, cp);
	}
	else if (attributeName == L"RuntimeVisibleAnnotations") {
		return make_shared<RuntimeVisibleAnnotations_attribute>(is, cp);
	}
	else if (attributeName == L"RuntimeInvisibleAnnotations") {
		return make_shared<RuntimeInvisibleAnnotations_attribute>(is, cp);
	}
	else if (attributeName == L"RuntimeVisibleParameterAnnotations") {
		return make_shared<RuntimeVisibleParameterAnnotations_attribute>(is, cp);
	}
	else if (attributeName == L"RuntimeInvisibleParameterAnnotations") {
		return make_shared<RuntimeInvisibleParameterAnnotations_attribute>(is, cp);
	}
	else if (attributeName == L"RuntimeVisibleTypeAnnotations") {
		return make_shared<RuntimeVisibleTypeAnnotations_attribute>(is, cp);
	}
	else if (attributeName == L"RuntimeInvisibleTypeAnnotations") {
		return make_shared<RuntimeInvisibleTypeAnnotations_attribute>(is, cp);
	}
	else if (attributeName == L"AnnotationDefault") {
		return make_shared<AnnotationDefault_attribute>(is, cp);
	}
	else if (attributeName == L"BootstrapMethods") {
		return make_shared<BootstrapMethods_attribute>(is, cp);
	}
	else if (attributeName == L"MethodParameters") {
		return make_shared<MethodParameters_attribute>(is, cp);
	}
	else if (attributeName == L"Module") {
		return make_shared<Module_attribute>(is, cp);
	}
	else if (attributeName == L"ModulePackages") {
		return make_shared<ModulePackages_attribute>(is, cp);
	}
	else if (attributeName == L"ModuleMainClass") {
		return make_shared<ModuleMainClass_attribute>(is, cp);
	}
	else if (attributeName == L"NestHost") {
		return make_shared<NestHost_attribute>(is, cp);
	}
	else if (attributeName == L"NestMembers") {
		return make_shared<NestMembers_attribute>(is, cp);
	}
	else {
		cout << "Unsupported attribute:" << attributeName.c_str() << ", skip it." << endl;
		return make_shared<Unknown_attribute>(is, cp);
	}
}

#ifdef __JVM_LITTLE_ENDIAN__
const u4 MAGIC_NUMBER = 0xCAFEBABE;
#else
const u4 MAGIC_NUMBER = 0xBEBAFECA;
#endif

/*
https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.7.16
4.7.16.1. The element_value structure
*/
shared_ptr<RuntimeAnnotations_attribute::element_value> RuntimeAnnotations_attribute::readElementValue(istream& is) {
	u1 tag = peaku1(is);
	switch (tag) {
	case ElementType::ElementType_Byte:
	case ElementType::ElementType_Char:
	case ElementType::ElementType_Double:
	case ElementType::ElementType_Float:
	case ElementType::ElementType_Int:
	case ElementType::ElementType_Long:
	case ElementType::ElementType_Short:
	case ElementType::ElementType_Boolean:
	case ElementType::ElementType_String:
		return make_shared<RuntimeAnnotations_attribute::const_element_value>(is);
	case ElementType::ElementType_Enum_type:
		return make_shared< RuntimeAnnotations_attribute::enum_const_value>(is);
	case ElementType::ElementType_Class:
		return make_shared< RuntimeAnnotations_attribute::class_element_value>(is);
	case ElementType::ElementType_Annotation_type:
		return make_shared< RuntimeAnnotations_attribute::annotation_element_value>(is);
	case ElementType::ElementType_Array_type:
		return make_shared< RuntimeAnnotations_attribute::array_element_value>(is);
	default:
		throw runtime_error("Uknown element tag:" + tag);
	}
}

shared_ptr<RuntimeTypeAnnotations::type_target> RuntimeTypeAnnotations::readTypeTarget(istream& is, u1 target_type) {
	throw runtime_error("Not implemented yet.");
}
wstring CONSTANT_Utf8_info::toUTF8String() {
	if (!utf8String.empty()) {
		return utf8String;
	}
	vector<u2> buf;
	/*
	* 标准在这里 https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.4.7
	*/
	for (auto it = bytes.begin(); it != bytes.end();) {
		u2 x = *it++;
		if ((x & 0x80) == 0) {
			buf.push_back(x);
		}
		else {
			u2 y = *it++;
			if ((x & 0xC0) == 0xC0 && (y & 0x80) == 0x80) {
				buf.push_back(((x & 0x1f) << 6) + (y & 0x3f));
			}
			else {
				u2 z = *it++;
				if ((x & 0xC0) == 0xC0 && (y & 0x80) == 0x80 && (z & 0x80) == 0x80) {
					buf.push_back(((x & 0xf) << 12) + ((y & 0x3f) << 6) + (z & 0x3f));
				}
				else {
					u2 u = x;
					u2 v = y;
					u2 w = z;
					x = *it++;
					y = *it++;
					z = *it++;
					if ((u == 0xED) && (v & 0xA0) == 0xA0 && (w & 0x80) == 0x80 &&
						(x == 0xED) && (y & 0xB0) == 0xB0 && (z & 0x80) == 0x80) {
						buf.push_back(0x10000 + ((v & 0x0f) << 16) + ((w & 0x3f) << 10) +
							((y & 0x0f) << 6) + (z & 0x3f));
					}
					else {
						throw runtime_error("CodePoint error.");
					}
				}
			}
		}
	}
	utf8String = wstring(buf.begin(), buf.end());
	return utf8String;
}

shared_ptr<StackMapTable_attribute::verification_type_info> StackMapTable_attribute::readVerificationTypeInfo(istream& is) {
	u1 type = peaku1(is);
	switch (type) {
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Top:
		return make_shared< StackMapTable_attribute::Top_variable_info>(is);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Integer:
		return make_shared< StackMapTable_attribute::Integer_variable_info>(is);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Float:
		return make_shared< StackMapTable_attribute::Float_variable_info>(is);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Double:
		return make_shared< StackMapTable_attribute::Double_variable_info>(is);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Long:
		return make_shared< StackMapTable_attribute::Long_variable_info>(is);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_UninitializedThis:
		return make_shared< StackMapTable_attribute::UninitializedThis_variable_info>(is);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Object:
		return make_shared< StackMapTable_attribute::Object_variable_info>(is);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Uninitialized:
		return make_shared< StackMapTable_attribute::Uninitialized_variable_info>(is);
	default:
		throw runtime_error("Unknow verification_type_info type:" + type);
	}
}

shared_ptr<StackMapTable_attribute::stack_map_frame> StackMapTable_attribute::readStackMapFrame(istream& is) {
	u1 type = peaku1(is);
	if (type <= 63) {
		return make_shared< StackMapTable_attribute::same_frame>(is);
	}
	else if (type <= 127) {
		return make_shared< StackMapTable_attribute::same_locals_1_stack_item_frame>(is);
	}
	else if (type <= 246) {
		throw runtime_error("Unsupported frame type:" + type);
	}
	else if (type == 247) {
		return make_shared< StackMapTable_attribute::same_locals_1_stack_item_frame_extended>(is);
	}
	else if (type <=250) {
		return make_shared< StackMapTable_attribute::chop_frame>(is);
	}
	else if (type == 251) {
		return make_shared< StackMapTable_attribute::same_frame_extended>(is);
	}
	else if (type <= 254) {
		return make_shared< StackMapTable_attribute::append_frame>(is);
	}
	else {
		return make_shared< StackMapTable_attribute::full_frame>(is);
	}
}

ClassFile::ClassFile(istream& is) {
	parse(is);
}

ClassFile::ClassFile(wstring& filePath) {
	std::ifstream filestream(filePath.c_str(), ios::binary);
	parse(filestream);
}

ClassFile::ClassFile(string& filePath) {
	std::ifstream filestream(filePath, ios::binary);
	parse(filestream);
}

bool ClassFile::isJavaClassFile() const{
	return validJavaFile;
}

wstring ClassFile::getCanonicalClassName() const {
	return canonicalName;
}
bool ClassFile::parse(istream& is) {
	// rewind to begin and clear the state.
	is.seekg(0, is.beg);
	magic = readu4(is);
	if (magic != MAGIC_NUMBER) {
		validJavaFile = false;
		return false;
	}
	minor_version = readu2(is);
	major_version = readu2(is);

	if (major_version < MIN_SUPPORTED_CLASS_FILE_VERSION || major_version > MAX_SUPPORTED_CLASS_FILE_VERSION) {
		isSupportedVersion = false;
		return false;
	}

	constant_pool_count = readu2(is);
	readConstantPools(is);
	access_flags = readu2(is);
	this_class = readu2(is);
	super_class = readu2(is);
	interfaces_count = readu2(is);
	readInterfaces(is);
	fields_count = readu2(is);
	readFieldInfos(is);
	methods_count = readu2(is);
	readMethodInfos(is);
	attributes_count = readu2(is);
	readAttributes(is);
	parsed = true;

	// extract some properties
	extract();
	return true;
}

void ClassFile::extract() {
	if (!parsed) {
		return;
	}
	auto thisClazz = std::dynamic_pointer_cast<CONSTANT_Class_info>(constant_pool[this_class]);
	canonicalName = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(constant_pool[thisClazz->name_index])->toUTF8String();
}
void ClassFile::readConstantPools(istream& is) {
	// index是从1开始的。
	constant_pool.push_back(make_shared< CONSTANT_Dummy_info>());
	for (int i = 1; i < constant_pool_count; i++) {
		u1 type = peaku1(is);
		switch (type) {
		case ConstantPoolType::Utf8:
			constant_pool.push_back(make_shared< CONSTANT_Utf8_info>(is));
			break;
		case ConstantPoolType::Integer:
			constant_pool.push_back(make_shared< CONSTANT_Integer_info>(is));
			break;
		case ConstantPoolType::Float:
			constant_pool.push_back(make_shared< CONSTANT_Float_info>(is));
			break;
		case ConstantPoolType::Long:
			constant_pool.push_back(make_shared< CONSTANT_Long_info>(is));
			// 这里要加一个，因为一个Long需要占用两个slot, 具体参见 https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.4.5
			constant_pool.push_back(make_shared< CONSTANT_LargeNumberContinuous_info>());
			i++;
			break;
		case ConstantPoolType::Double:
			constant_pool.push_back(make_shared< CONSTANT_Double_info>(is));
			// 这里要加一个，因为一个Long需要占用两个slot, 具体参见 https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.4.5
			constant_pool.push_back(make_shared< CONSTANT_LargeNumberContinuous_info>());
			i++;
			break;
		case ConstantPoolType::Class:
			constant_pool.push_back(make_shared< CONSTANT_Class_info>(is));
			break;
		case ConstantPoolType::String:
			constant_pool.push_back(make_shared< CONSTANT_String_info>(is));
			break;
		case ConstantPoolType::Fieldref:
			constant_pool.push_back(make_shared< CONSTANT_Fieldref_info>(is));
			break;
		case ConstantPoolType::Methodref:
			constant_pool.push_back(make_shared< CONSTANT_Methodref_info>(is));
			break;
		case ConstantPoolType::InterfaceMethodref:
			constant_pool.push_back(make_shared< CONSTANT_InterfaceMethodref_info>(is));
			break;
		case ConstantPoolType::NameAndType:
			constant_pool.push_back(make_shared< CONSTANT_NameAndType_info>(is));
			break;
		case ConstantPoolType::MethodHandle:
			constant_pool.push_back(make_shared< CONSTANT_MethodHandle_info>(is));
			break;
		case ConstantPoolType::MethodType:
			constant_pool.push_back(make_shared< CONSTANT_MethodType_info>(is));
			break;
		case ConstantPoolType::Dynamic:
			constant_pool.push_back(make_shared< CONSTANT_Dynamic_info>(is));
			break;
		case ConstantPoolType::InvokeDynamic:
			constant_pool.push_back(make_shared< CONSTANT_InvokeDynamic_info>(is));
			break;
		case ConstantPoolType::Module:
			constant_pool.push_back(make_shared< CONSTANT_Module_info>(is));
			break;
		case ConstantPoolType::Package:
			constant_pool.push_back(make_shared< CONSTANT_Package_info>(is));
			break;
		default:
			throw std::runtime_error("Unsupported constant type:" + type);
			break;
		}
	}
}

void ClassFile::readInterfaces(istream& is)
{
	for (auto it = 0; it < interfaces_count; it++) {
		interfaces.push_back(readu2(is));
	}
}

void ClassFile::readFieldInfos(istream& is)
{
	for (int i = 0; i < fields_count; i++) {
		fields.push_back(make_shared<Field_Info>(is, constant_pool));
	}
}

void ClassFile::readMethodInfos(istream& is)
{
	for (int i = 0; i < methods_count; i++) {
		methods.push_back(make_shared<Method_Info>(is, constant_pool));
	}
}

void ClassFile::readAttributes(istream& is)
{
	for (int i = 0; i < attributes_count; i++) {
		attributes.push_back(readAttributeInfo(is, constant_pool));
	}
}