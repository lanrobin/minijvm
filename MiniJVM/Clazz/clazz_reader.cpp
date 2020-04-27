#include <iostream>
#include <iomanip>
#include "base_type.h"
#include "platform.h"
#include "clazz_reader.h"

#define DEBUG_READ_CLASS_FILE

u1 peaku1(std::istream& is)
{
	return is.peek();
}


u2 peaku2(std::istream& is) {
	u1 v1 = is.get();
	u1 v2 = is.peek();
	is.unget();
#ifdef __JVM_LITTLE_ENDIAN__
	return ((v1 << 8) | v2) & 0xFFFF;
#else // __LITTLE_ENDIAN__
	return ((v2 << 8) | v1) & 0xFFFF;
#endif // __LITTLE_ENDIAN__
}


u4 peaku4(std::istream& is) {
	u1 v1 = is.get();
	u1 v2 = is.get();
	u1 v3 = is.get();
	u4 v4 = is.peek();
	is.unget();
	is.unget();
	is.unget();
#ifdef __JVM_LITTLE_ENDIAN__
	return ((v1 << 24) | (v2 << 16) | (v3 << 8) | v4) & 0xFFFFFFFF;
#else // __LITTLE_ENDIAN__
	return ((v4 << 24) | (v3 << 16) | (v2 << 8) | v1) & 0xFFFFFFFF;
#endif // __LITTLE_ENDIAN__
}

u1 readu1(std::istream& is) {
	u1 value = is.get();
#ifdef DEBUG_READ_CLASS_FILE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)value << endl;
#endif
	return value;
}

u2 readu2(std::istream& is) {
	u1 v1 = is.get();
#ifdef DEBUG_READ_CLASS_FILE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	u1 v2 = is.get();
#ifdef DEBUG_READ_CLASS_FILE
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
#ifdef DEBUG_READ_CLASS_FILE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	u1 v2 = is.get();
#ifdef DEBUG_READ_CLASS_FILE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v2 << endl;
#endif
	u1 v3 = is.get();
#ifdef DEBUG_READ_CLASS_FILE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v3 << endl;
#endif
	u4 v4 = is.get();
#ifdef DEBUG_READ_CLASS_FILE
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
	throw runtime_error("Not implemented yet.");
}

#ifdef __JVM_LITTLE_ENDIAN__
const u4 MAGIC_NUMBER = 0xCAFEBABE;
#else
const u4 MAGIC_NUMBER = 0xBEBAFECA;
#endif

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
	magic = readu4(is);
	minor_version = readu2(is);
	major_version = readu2(is);
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
}

bool ClassFile::checkClassFile() {
	return true;
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
		attributes.push_back(make_shared<Attribute_Info>(is, constant_pool));
	}
}