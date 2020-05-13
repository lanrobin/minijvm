#include <iostream>
#include <fstream>
#include <iomanip>
#include "base_type.h"
#include "platform.h"
#include "buffer.h"
#include "clazz_reader.h"
#include "log.h"
#include "string_utils.h"

#undef DEBUG_READ_FILE_IN_BYTE

u4 read_u1_vector(vector<u1> &v, shared_ptr<Buffer> buf, u4 count)
{
	// 这里应该可能换更好的方法。
	for (u4 it = 0; it < count; it++)
	{
		u1 value = buf->readu1();
		v.push_back(value);
	}
	return count;
}

u4 read_u2_vector(vector<u2> &v, shared_ptr<Buffer> buf, u4 count)
{
	// 这里应该可能换更好的方法。
	for (u4 it = 0; it < count; it++)
	{
		u2 value = buf->readu2();
		v.push_back(value);
	}
	return count;
}
template <typename T>
u4 read_vector(vector<shared_ptr<T>> &v, shared_ptr<Buffer> buf, u4 count)
{
	for (u4 it = 0; it < count; it++)
	{
		v.push_back(make_shared<T>(buf));
	}
	return count;
}

shared_ptr<Attribute_Info> readAttributeInfo(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> &cp)
{
	u2 attribute_name_index = buf->peaku2();
	auto utf8 = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(cp[attribute_name_index]);
	wstring attributeName = utf8->toUTF8String();

	// order by https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.7 Table 4.7-A. Predefined class file attributes (by section)
	if (attributeName == L"ConstantValue")
	{
		return make_shared<ConstantValue_attribute>(buf, cp);
	}
	else if (attributeName == L"Code")
	{
		return make_shared<Code_attribute>(buf, cp);
	}
	else if (attributeName == L"StackMapTable")
	{
		return make_shared<StackMapTable_attribute>(buf, cp);
	}
	else if (attributeName == L"Exceptions")
	{
		return make_shared<Exceptions_attribute>(buf, cp);
	}
	else if (attributeName == L"InnerClasses")
	{
		return make_shared<InnerClasses_attribute>(buf, cp);
	}
	else if (attributeName == L"EnclosingMethod")
	{
		return make_shared<EnclosingMethod_attribute>(buf, cp);
	}
	else if (attributeName == L"Synthetic")
	{
		return make_shared<Synthetic_attribute>(buf, cp);
	}
	else if (attributeName == L"Signature")
	{
		return make_shared<Signature_attribute>(buf, cp);
	}
	else if (attributeName == L"SourceFile")
	{
		return make_shared<SourceFile_attribute>(buf, cp);
	}
	else if (attributeName == L"SourceDebugExtension")
	{
		return make_shared<SourceDebugExtension_attribute>(buf, cp);
	}
	else if (attributeName == L"LineNumberTable")
	{
		return make_shared<LineNumberTable_attribute>(buf, cp);
	}
	else if (attributeName == L"LocalVariableTable")
	{
		return make_shared<LocalVariableTable_attribute>(buf, cp);
	}
	else if (attributeName == L"LocalVariableTypeTable")
	{
		return make_shared<LocalVariableTypeTable_attribute>(buf, cp);
	}
	else if (attributeName == L"Deprecated")
	{
		return make_shared<Deprecated_attribute>(buf, cp);
	}
	else if (attributeName == L"RuntimeVisibleAnnotations")
	{
		return make_shared<RuntimeVisibleAnnotations_attribute>(buf, cp);
	}
	else if (attributeName == L"RuntimeInvisibleAnnotations")
	{
		return make_shared<RuntimeInvisibleAnnotations_attribute>(buf, cp);
	}
	else if (attributeName == L"RuntimeVisibleParameterAnnotations")
	{
		return make_shared<RuntimeVisibleParameterAnnotations_attribute>(buf, cp);
	}
	else if (attributeName == L"RuntimeInvisibleParameterAnnotations")
	{
		return make_shared<RuntimeInvisibleParameterAnnotations_attribute>(buf, cp);
	}
	else if (attributeName == L"RuntimeVisibleTypeAnnotations")
	{
		return make_shared<RuntimeVisibleTypeAnnotations_attribute>(buf, cp);
	}
	else if (attributeName == L"RuntimeInvisibleTypeAnnotations")
	{
		return make_shared<RuntimeInvisibleTypeAnnotations_attribute>(buf, cp);
	}
	else if (attributeName == L"AnnotationDefault")
	{
		return make_shared<AnnotationDefault_attribute>(buf, cp);
	}
	else if (attributeName == L"BootstrapMethods")
	{
		return make_shared<BootstrapMethods_attribute>(buf, cp);
	}
	else if (attributeName == L"MethodParameters")
	{
		return make_shared<MethodParameters_attribute>(buf, cp);
	}
	else if (attributeName == L"Module")
	{
		return make_shared<Module_attribute>(buf, cp);
	}
	else if (attributeName == L"ModulePackages")
	{
		return make_shared<ModulePackages_attribute>(buf, cp);
	}
	else if (attributeName == L"ModuleMainClass")
	{
		return make_shared<ModuleMainClass_attribute>(buf, cp);
	}
	else if (attributeName == L"NestHost")
	{
		return make_shared<NestHost_attribute>(buf, cp);
	}
	else if (attributeName == L"NestMembers")
	{
		return make_shared<NestMembers_attribute>(buf, cp);
	}
	else
	{
		cout << "Unsupported attribute:" << attributeName.c_str() << ", skip it." << endl;
		return make_shared<Unknown_attribute>(buf, cp);
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
shared_ptr<RuntimeAnnotations_attribute::element_value> RuntimeAnnotations_attribute::readElementValue(shared_ptr<Buffer> buf)
{
	u1 tag = buf->peaku1();
	switch (tag)
	{
	case ElementType::ElementType_Byte:
	case ElementType::ElementType_Char:
	case ElementType::ElementType_Double:
	case ElementType::ElementType_Float:
	case ElementType::ElementType_Int:
	case ElementType::ElementType_Long:
	case ElementType::ElementType_Short:
	case ElementType::ElementType_Boolean:
	case ElementType::ElementType_String:
		return make_shared<RuntimeAnnotations_attribute::const_element_value>(buf);
	case ElementType::ElementType_Enum_type:
		return make_shared<RuntimeAnnotations_attribute::enum_const_value>(buf);
	case ElementType::ElementType_Class:
		return make_shared<RuntimeAnnotations_attribute::class_element_value>(buf);
	case ElementType::ElementType_Annotation_type:
		return make_shared<RuntimeAnnotations_attribute::annotation_element_value>(buf);
	case ElementType::ElementType_Array_type:
		return make_shared<RuntimeAnnotations_attribute::array_element_value>(buf);
	default:
		throw runtime_error("Uknown element tag:" + tag);
	}
}

// https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.4.1 Table 4.7.20-A. Interpretation of target_type values (Part 1)
shared_ptr<RuntimeTypeAnnotations::type_target> RuntimeTypeAnnotations::readTypeTarget(shared_ptr<Buffer> buf, u1 target_type)
{
	switch (target_type)
	{
	case 0x00:
	case 0x01:
		return make_shared<RuntimeTypeAnnotations::type_parameter_target>(buf);
	case 0x10:
		return make_shared<RuntimeTypeAnnotations::supertype_target>(buf);
	case 0x11:
	case 0x12:
		return make_shared<RuntimeTypeAnnotations::type_parameter_bound_target>(buf);
	case 0x13:
	case 0x14:
	case 0x15:
		return make_shared<RuntimeTypeAnnotations::empty_target>(buf);
	case 0x16:
		return make_shared<RuntimeTypeAnnotations::formal_parameter_target>(buf);
	case 0x17:
		return make_shared<RuntimeTypeAnnotations::throws_target>(buf);
	case 0x40:
	case 0x41:
		return make_shared<RuntimeTypeAnnotations::localvar_target>(buf);
	case 0x42:
		return make_shared<RuntimeTypeAnnotations::catch_target>(buf);
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
		return make_shared<RuntimeTypeAnnotations::offset_target>(buf);
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x4B:
		return make_shared<RuntimeTypeAnnotations::type_argument_target>(buf);
	default:
		throw runtime_error("Unsupported type target:" + target_type);
	}
}
wstring CONSTANT_Utf8_info::toUTF8String()
{
	if (!utf8String.empty())
	{
		return utf8String;
	}
	vector<u2> buf;
	/*
	* 标准在这里 https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.4.7
	*/
	for (auto it = bytes.begin(); it != bytes.end();)
	{
		u2 x = *it++;
		if (x <= 0x7F)
		{
			buf.push_back(x);
		}
		else
		{
			u2 y = *it++;
			if ((x & 0xE0) == 0xC0 && (y & 0xC0) == 0x80)
			{
				buf.push_back(((x & 0x1f) << 6) + (y & 0x3f));
			}
			else
			{
				u2 z = *it++;
				if ((x & 0xF0) == 0xE0 && (y & 0xC0) == 0x80 && (z & 0xC0) == 0x80)
				{
					buf.push_back(((x & 0xf) << 12) + ((y & 0x3f) << 6) + (z & 0x3f));
				}
				else
				{
					u2 u = x;
					u2 v = y;
					u2 w = z;
					x = *it++;
					y = *it++;
					z = *it++;
					if ((u == 0xED) && (v & 0xF0) == 0xA0 && (w & 0xC0) == 0x80 &&
						(x == 0xED) && (y & 0xF0) == 0xB0 && (z & 0xC0) == 0x80)
					{
						buf.push_back(0x10000 + ((v & 0x0f) << 16) + ((w & 0x3f) << 10) +
									  ((y & 0x0f) << 6) + (z & 0x3f));
					}
					else
					{
						throw runtime_error("CodePoint error.");
					}
				}
			}
		}
	}
	utf8String = wstring(buf.begin(), buf.end());
	return utf8String;
}

shared_ptr<StackMapTable_attribute::verification_type_info> StackMapTable_attribute::readVerificationTypeInfo(shared_ptr<Buffer> buf)
{
	u1 type = buf->peaku1();
	switch (type)
	{
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Top:
		return make_shared<StackMapTable_attribute::Top_variable_info>(buf);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Integer:
		return make_shared<StackMapTable_attribute::Integer_variable_info>(buf);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Float:
		return make_shared<StackMapTable_attribute::Float_variable_info>(buf);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Double:
		return make_shared<StackMapTable_attribute::Double_variable_info>(buf);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Long:
		return make_shared<StackMapTable_attribute::Long_variable_info>(buf);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_UninitializedThis:
		return make_shared<StackMapTable_attribute::UninitializedThis_variable_info>(buf);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Object:
		return make_shared<StackMapTable_attribute::Object_variable_info>(buf);
	case StackMapTable_attribute::StackMapTableVerificationItemType::ITEM_Uninitialized:
		return make_shared<StackMapTable_attribute::Uninitialized_variable_info>(buf);
	default:
		throw runtime_error("Unknow verification_type_info type:" + type);
	}
}

shared_ptr<StackMapTable_attribute::stack_map_frame> StackMapTable_attribute::readStackMapFrame(shared_ptr<Buffer> buf)
{
	u1 type = buf->peaku1();
	if (type <= 63)
	{
		return make_shared<StackMapTable_attribute::same_frame>(buf);
	}
	else if (type <= 127)
	{
		return make_shared<StackMapTable_attribute::same_locals_1_stack_item_frame>(buf);
	}
	else if (type <= 246)
	{
		throw runtime_error("Unsupported frame type:" + type);
	}
	else if (type == 247)
	{
		return make_shared<StackMapTable_attribute::same_locals_1_stack_item_frame_extended>(buf);
	}
	else if (type <= 250)
	{
		return make_shared<StackMapTable_attribute::chop_frame>(buf);
	}
	else if (type == 251)
	{
		return make_shared<StackMapTable_attribute::same_frame_extended>(buf);
	}
	else if (type <= 254)
	{
		return make_shared<StackMapTable_attribute::append_frame>(buf);
	}
	else
	{
		return make_shared<StackMapTable_attribute::full_frame>(buf);
	}
}

ClassFile::ClassFile(shared_ptr<Buffer> buf)
{
	parse(buf);
}
/*
ClassFile::ClassFile(wstring& filePath) {
	std::ifstream filestream(filePath.c_str(), ios::binary);
	parse(make_shared<Buffer>(filestream));
}

ClassFile::ClassFile(string& filePath) {
	std::ifstream filestream(filePath, ios::binary);
	parse(make_shared<Buffer>(filestream));
}*/

bool ClassFile::isJavaClassFile() const
{
	return validJavaFile;
}

wstring ClassFile::getCanonicalClassName() const
{
	return canonicalName;
}
bool ClassFile::parse(shared_ptr<Buffer> buf)
{
	// rewind to begin and clear the state.
	buf->resetReadPos();
	magic = buf->readu4();
	if (magic != MAGIC_NUMBER)
	{
		validJavaFile = false;
		return false;
	}
	minor_version = buf->readu2();
	major_version = buf->readu2();

	if (major_version < MIN_SUPPORTED_CLASS_FILE_VERSION || major_version > MAX_SUPPORTED_CLASS_FILE_VERSION)
	{
		isSupportedVersion = false;
		return false;
	}

	constant_pool_count = buf->readu2();
	readConstantPools(buf);
	access_flags = buf->readu2();
	this_class = buf->readu2();
	super_class = buf->readu2();
	interfaces_count = buf->readu2();
	readInterfaces(buf);
	fields_count = buf->readu2();
	readFieldInfos(buf);
	methods_count = buf->readu2();
	readMethodInfos(buf);
	attributes_count = buf->readu2();
	readAttributes(buf);
	parsed = true;

	// extract some properties
	extract();
	return true;
}

void ClassFile::extract()
{
	if (!parsed)
	{
		return;
	}
	auto thisClazz = std::dynamic_pointer_cast<CONSTANT_Class_info>(constant_pool[this_class]);
	canonicalName = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(constant_pool[thisClazz->name_index])->toUTF8String();
}
void ClassFile::readConstantPools(shared_ptr<Buffer> buf)
{
	// index是从1开始的。
	constant_pool.push_back(make_shared<CONSTANT_Dummy_info>());
	for (int i = 1; i < constant_pool_count; i++)
	{
		u1 type = buf->peaku1();
		switch (type)
		{
		case ConstantPoolType::Utf8:
			constant_pool.push_back(make_shared<CONSTANT_Utf8_info>(buf));
			break;
		case ConstantPoolType::Integer:
			constant_pool.push_back(make_shared<CONSTANT_Integer_info>(buf));
			break;
		case ConstantPoolType::Float:
			constant_pool.push_back(make_shared<CONSTANT_Float_info>(buf));
			break;
		case ConstantPoolType::Long:
			constant_pool.push_back(make_shared<CONSTANT_Long_info>(buf));
			// 这里要加一个，因为一个Long需要占用两个slot, 具体参见 https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.4.5
			constant_pool.push_back(make_shared<CONSTANT_LargeNumberContinuous_info>());
			i++;
			break;
		case ConstantPoolType::Double:
			constant_pool.push_back(make_shared<CONSTANT_Double_info>(buf));
			// 这里要加一个，因为一个Long需要占用两个slot, 具体参见 https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.4.5
			constant_pool.push_back(make_shared<CONSTANT_LargeNumberContinuous_info>());
			i++;
			break;
		case ConstantPoolType::Class:
			constant_pool.push_back(make_shared<CONSTANT_Class_info>(buf));
			break;
		case ConstantPoolType::String:
			constant_pool.push_back(make_shared<CONSTANT_String_info>(buf));
			break;
		case ConstantPoolType::Fieldref:
			constant_pool.push_back(make_shared<CONSTANT_Fieldref_info>(buf));
			break;
		case ConstantPoolType::Methodref:
			constant_pool.push_back(make_shared<CONSTANT_Methodref_info>(buf));
			break;
		case ConstantPoolType::InterfaceMethodref:
			constant_pool.push_back(make_shared<CONSTANT_InterfaceMethodref_info>(buf));
			break;
		case ConstantPoolType::NameAndType:
			constant_pool.push_back(make_shared<CONSTANT_NameAndType_info>(buf));
			break;
		case ConstantPoolType::MethodHandle:
			constant_pool.push_back(make_shared<CONSTANT_MethodHandle_info>(buf));
			break;
		case ConstantPoolType::MethodType:
			constant_pool.push_back(make_shared<CONSTANT_MethodType_info>(buf));
			break;
		case ConstantPoolType::Dynamic:
			constant_pool.push_back(make_shared<CONSTANT_Dynamic_info>(buf));
			break;
		case ConstantPoolType::InvokeDynamic:
			constant_pool.push_back(make_shared<CONSTANT_InvokeDynamic_info>(buf));
			break;
		case ConstantPoolType::Module:
			constant_pool.push_back(make_shared<CONSTANT_Module_info>(buf));
			break;
		case ConstantPoolType::Package:
			constant_pool.push_back(make_shared<CONSTANT_Package_info>(buf));
			break;
		default:
			throw std::runtime_error("Unsupported constant type:" + type);
			break;
		}
	}
}

void ClassFile::readInterfaces(shared_ptr<Buffer> buf)
{
	for (auto it = 0; it < interfaces_count; it++)
	{
		interfaces.push_back(buf->readu2());
	}
}

void ClassFile::readFieldInfos(shared_ptr<Buffer> buf)
{
	for (int i = 0; i < fields_count; i++)
	{
		fields.push_back(make_shared<Field_Info>(buf, constant_pool));
	}
}

void ClassFile::readMethodInfos(shared_ptr<Buffer> buf)
{
	for (int i = 0; i < methods_count; i++)
	{
		methods.push_back(make_shared<Method_Info>(buf, constant_pool));
	}
}

void ClassFile::readAttributes(shared_ptr<Buffer> buf)
{
	for (int i = 0; i < attributes_count; i++)
	{
		attributes.push_back(readAttributeInfo(buf, constant_pool));
	}
}

wstring ClassFile::getClassName(u2 index)
{
	if (index < 1 || index > constant_pool_count)
	{
		return wstring();
	}
	// 正常的ClassName,如果是数组为内存生成的，不在这里生成。
	auto classAttri = std::dynamic_pointer_cast<CONSTANT_Class_info>(constant_pool[index]);
	return getUtf8String(classAttri->name_index);
}

wstring ClassFile::getUtf8String(u2 index)
{
	if (index < 1 || index > constant_pool_count)
	{
		return wstring();
	}
	return std::dynamic_pointer_cast<CONSTANT_Utf8_info>(constant_pool[index])->toUTF8String();
}

std::pair<wstring, wstring> ClassFile::getNameAndType(u2 index)
{
	wstring name;
	wstring type;
	if (index > 0 && index < constant_pool_count)
	{
		auto nat = std::dynamic_pointer_cast<CONSTANT_NameAndType_info>(constant_pool[index]);
		name = getUtf8String(nat->name_index);
		type = getUtf8String(nat->descriptor_index);
	}
	return std::make_pair(name, type);
}
/*
先这么实现，应该用unordered_map来加速。
*/
shared_ptr<Attribute_Info> ClassFile::getAttributeByName(const wstring &name) const
{
	for (auto a = attributes.begin(); a != attributes.end(); a++)
	{
		if ((*a)->getAttributeName() == name)
		{
			return *a;
		}
	}
	return nullptr;
}

ClassFile::~ClassFile()
{
	constant_pool.clear();
	methods.clear();
	attributes.clear();
	spdlog::info("ClassFile:{} gone", w2s(canonicalName));
}