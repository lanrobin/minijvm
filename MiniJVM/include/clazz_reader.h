﻿#ifndef __JVM_CLASS_READER__
#define __JVM_CLASS_READER__

#include <iostream>
#include <vector>
#include <map>
#include "platform.h"
#include "base_type.h"

using namespace std;

/**
这个文件主要是解析class文件，参照  https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html
*/

const u2 MAX_SUPPORTED_CLASS_FILE_VERSION = 58;
const u2 MIN_SUPPORTED_CLASS_FILE_VERSION = 45;



/// field access flag
const u2 FIELD_ACC_PUBLIC = 0x0001; //Declared public; may be accessed from outside its package.
const u2 FIELD_ACC_FINAL = 0x0010; //Declared final; no subclasses allowed.
const u2 FIELD_ACC_SUPER = 0x0020; //Treat superclass methods specially when invoked by the invokespecial instruction.
const u2 FIELD_ACC_INTERFACE = 0x0200; //Is an interface, not a class.
const u2 FIELD_ACC_ABSTRACT = 0x0400; //Declared abstract; must not be instantiated.
const u2 FIELD_ACC_SYNTHETIC = 0x1000; //Declared synthetic; not present in the source code.
const u2 FIELD_ACC_ANNOTATION = 0x2000; //Declared as an annotation type.
const u2 FIELD_ACC_ENUM = 0x4000; //Declared as an enum type.
const u2 FIELD_ACC_MODULE = 0x8000; //Is a module, not a class or interface.

enum ConstantPoolType : u1 {
    Utf8 = 1, // class file version 45.3, since Java:1.0.2
    Integer = 3, // class file version 45.3, since Java:1.0.2
    Float = 4, // class file version 45.3, since Java:1.0.2
    Long = 5, // class file version 45.3, since Java:1.0.2
    Double = 6, // class file version 45.3, since Java:1.0.2
    Class = 7, // class file version 45.3, since Java:1.0.2
    String = 8, // class file version 45.3, since Java:1.0.2
    Fieldref = 9, // class file version 45.3, since Java:1.0.2
    Methodref = 10, // class file version 45.3, since Java:1.0.2
    InterfaceMethodref = 11, // class file version 45.3, since Java:1.0.2
    NameAndType = 12, // class file version 45.3, since Java:1.0.2
    MethodHandle = 15, // class file version 51, since Java:7
    MethodType = 16, // class file version 51, since Java:7
    Dynamic = 17, // class file version 55, since Java:11
    InvokeDynamic = 18, // class file version 51, since Java:7
    Module = 19, // class file version 53, since Java:9
    Package = 20, // class file version 53, since Java:9
};


// https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html#jvms-4.7.6 Table 4.7.16.1-A. Interpretation of tag values as types
enum ElementType : u1 {
    ElementType_Byte = 'B',// value Item:const_value_index, Constant Type:CONSTANT_Integer
    ElementType_Char = 'C',// value Item:const_value_index, Constant Type:CONSTANT_Integer
    ElementType_Double = 'D',// value Item:const_value_index, Constant Type:CONSTANT_Double
    ElementType_Float = 'F',// value Item:const_value_index, Constant Type:CONSTANT_Float
    ElementType_Int = 'I',// value Item:const_value_index, Constant Type:CONSTANT_Integer
    ElementType_Long = 'J',// value Item:const_value_index, Constant Type:CONSTANT_Long
    ElementType_Short = 'S',// value Item:const_value_index, Constant Type:CONSTANT_Integer
    ElementType_Boolean = 'Z',// value Item:const_value_index, Constant Type:CONSTANT_Integer
    ElementType_String = 's',// value Item:const_value_index, Constant Type:CONSTANT_Utf8
    ElementType_Enum_type = 'e',// value Item:enum_const_value, Constant Type:Not applicable
    ElementType_Class = 'c',// value Item:class_info_index, Constant Type:Not applicable
    ElementType_Annotation_type = '@',// value Item:annotation_value, Constant Type:Not applicable
    ElementType_Array_type = '[',// value Item:array_value, Constant Type:Not applicable
};

const u2 METHOD_ACC_PUBLIC = 0x0001; // Declared public; may be accessed from outside its package.
const u2 METHOD_ACC_PRIVATE = 0x0002; // Declared private; accessible only within the defining class and other classes belonging to the same nest (§5.4.4).
const u2 METHOD_ACC_PROTECTED = 0x0004; // Declared protected; may be accessed within subclasses.
const u2 METHOD_ACC_STATIC = 0x0008; // Declared static.
const u2 METHOD_ACC_FINAL = 0x0010; // Declared final; must not be overridden (§5.4.5).
const u2 METHOD_ACC_SYNCHRONIZED = 0x0020; // Declared synchronized; invocation is wrapped by a monitor use.
const u2 METHOD_ACC_BRIDGE = 0x0040; // A bridge method, generated by the compiler.
const u2 METHOD_ACC_VARARGS = 0x0080; // Declared with variable number of arguments.
const u2 METHOD_ACC_NATIVE = 0x0100; // Declared native; implemented in a language other than the Java programming language.
const u2 METHOD_ACC_ABSTRACT = 0x0400; // Declared abstract; no implementation is provided.
const u2 METHOD_ACC_STRICT = 0x0800; // Declared strictfp; floating-point mode is FP-strict.
const u2 METHOD_ACC_SYNTHETIC = 0x1000; // Declared synthetic; not present in the source code.


const u2 NESTED_CLASS_ACC_PUBLIC = 0x0001; // Marked or implicitly public in source.
const u2 NESTED_CLASS_ACC_PRIVATE = 0x0002; // Marked private in source.
const u2 NESTED_CLASS_ACC_PROTECTED = 0x0004; // Marked protected in source.
const u2 NESTED_CLASS_ACC_STATIC = 0x0008; // Marked or implicitly static in source.
const u2 NESTED_CLASS_ACC_FINAL = 0x0010; // Marked or implicitly final in source.
const u2 NESTED_CLASS_ACC_INTERFACE = 0x0200; // Was an interface in source.
const u2 NESTED_CLASS_ACC_ABSTRACT = 0x0400; // Marked or implicitly abstract in source.
const u2 NESTED_CLASS_ACC_SYNTHETIC = 0x1000; // Declared synthetic; not present in the source code.
const u2 NESTED_CLASS_ACC_ANNOTATION = 0x2000; // Declared as an annotation type.
const u2 NESTED_CLASS_ACC_ENUM = 0x4000; // Declared as an enum type.


u1 peaku1(istream& is);
u2 peaku2(istream& is);
u4 peaku4(istream& is);

u1 readu1(istream& is);
u2 readu2(istream& is);
u4 readu4(istream& is);

u4 read_u1_vector(vector<u1>& v, istream& is, u4 count);
u4 read_u2_vector(vector<u2>& v, istream& is, u4 count);

template< typename T>
u4 read_vector(vector<T>& v, istream& is, u4 count);

template< typename T>
u4 read_vector(vector<shared_ptr<T>>& v, istream& is, u4 count);


//常量区定义
struct CONSTANT_Info {
    u1 tag;
    CONSTANT_Info(istream& is) {
        tag = readu1(is);
    }
    CONSTANT_Info() {
        tag = 0;
    }
    
    //这个是需要的，否则会报 error C2683: 'dynamic_cast': 'CONSTANT_Info' is not a polymorphic type
    virtual ~CONSTANT_Info() {};
};

struct CONSTANT_Class_info : CONSTANT_Info {
    u2 name_index;
    CONSTANT_Class_info(istream& is) :CONSTANT_Info(is) {
        name_index = readu2(is);
    }
};

struct CONSTANT_Fieldref_info : CONSTANT_Info {
    u2 class_index;
    u2 name_and_type_index;

    CONSTANT_Fieldref_info(istream& is) :CONSTANT_Info(is) {
        class_index = readu2(is);
        name_and_type_index = readu2(is);
    }
};

struct CONSTANT_Methodref_info : CONSTANT_Info {
    u2 class_index;
    u2 name_and_type_index;

    CONSTANT_Methodref_info(istream& is) :CONSTANT_Info(is) {
        class_index = readu2(is);
        name_and_type_index = readu2(is);
    }
};

struct CONSTANT_InterfaceMethodref_info : CONSTANT_Info {
    u2 class_index;
    u2 name_and_type_index;

    CONSTANT_InterfaceMethodref_info(istream& is) :CONSTANT_Info(is) {
        class_index = readu2(is);
        name_and_type_index = readu2(is);
    }
};

struct CONSTANT_String_info : CONSTANT_Info {
    u2 string_index;
    CONSTANT_String_info(istream& is) :CONSTANT_Info(is) {
        string_index = readu2(is);
    }
};

struct CONSTANT_Integer_info : CONSTANT_Info {
    u4 bytes;
    CONSTANT_Integer_info(istream& is) :CONSTANT_Info(is) {
        bytes = readu4(is);
    }
};

struct CONSTANT_Float_info : CONSTANT_Info {
    u4 bytes;
    CONSTANT_Float_info(istream& is) :CONSTANT_Info(is) {
        bytes = readu4(is);
    }
};

struct  CONSTANT_Long_info : CONSTANT_Info {
    u4 high_bytes;
    u4 low_bytes;

    CONSTANT_Long_info(istream& is) :CONSTANT_Info(is) {
        high_bytes = readu4(is);
        low_bytes = readu4(is);
    }
};

struct  CONSTANT_Double_info : CONSTANT_Info {
    u4 high_bytes;
    u4 low_bytes;

    CONSTANT_Double_info(istream& is) :CONSTANT_Info(is) {
        high_bytes = readu4(is);
        low_bytes = readu4(is);
    }
};

struct CONSTANT_NameAndType_info : CONSTANT_Info {
    u2 name_index;
    u2 descriptor_index;
    CONSTANT_NameAndType_info(istream& is) :CONSTANT_Info(is) {
        name_index = readu2(is);
        descriptor_index = readu2(is);
    }
};

struct CONSTANT_Utf8_info : CONSTANT_Info {
    u2 length;
    vector<u1> bytes;

    CONSTANT_Utf8_info(istream& is) :CONSTANT_Info(is) {
        length = readu2(is);
        read_u1_vector(bytes, is, length);
    }

    wstring toUTF8String();

private:
    wstring utf8String;
};

struct CONSTANT_MethodHandle_info : CONSTANT_Info {
    u1 reference_kind;
    u2 reference_index;
    CONSTANT_MethodHandle_info(istream& is) :CONSTANT_Info(is) {
        reference_kind = readu1(is);
        reference_index = readu2(is);
    }
};

struct CONSTANT_MethodType_info : CONSTANT_Info {
    u2 descriptor_index;

    CONSTANT_MethodType_info(istream& is) :CONSTANT_Info(is) {
        descriptor_index = readu2(is);
    }
};

struct CONSTANT_Dynamic_info : CONSTANT_Info {
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
    CONSTANT_Dynamic_info(istream& is) :CONSTANT_Info(is) {
        bootstrap_method_attr_index = readu2(is);
        name_and_type_index = readu2(is);
    }
};

struct CONSTANT_InvokeDynamic_info : CONSTANT_Info {
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
    CONSTANT_InvokeDynamic_info(istream& is) :CONSTANT_Info(is) {
        bootstrap_method_attr_index = readu2(is);
        name_and_type_index = readu2(is);
    }
};

struct CONSTANT_Module_info : CONSTANT_Info {
    u2 name_index;
    CONSTANT_Module_info(istream& is) :CONSTANT_Info(is) {
        name_index = readu2(is);
    }
};

struct CONSTANT_Package_info : CONSTANT_Info {
    u2 name_index;
    CONSTANT_Package_info(istream& is) :CONSTANT_Info(is) {
        name_index = readu2(is);
    }
};

/*
这个类应该是用不到，只是为了Long和Double的类型占位用的。
*/
struct CONSTANT_LargeNumberContinuous_info :CONSTANT_Info {

};

/*
第一个(Index为0的slot不用，用这个占住。)
*/
struct CONSTANT_Dummy_info :CONSTANT_Info {

};
//常量区定义结束。


// Attribute区

struct Attribute_Info {
    u2 attribute_name_index;
    u4 attribute_length;

    //vector<u1> info;
    Attribute_Info(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) :constantPool(cp) {
        attribute_name_index = readu2(is);
        attribute_length = readu4(is);
        //read_u1_vector(info, is, attribute_length);
        //shared_ptr<CONSTANT_Utf8_info> entry = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(cp[attribute_name_index]);
        name = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(constantPool[attribute_name_index])->toUTF8String();
    }

    wstring getAttributeName() {
        return name;
    }

private:
    wstring name;

protected:
    const vector<shared_ptr<CONSTANT_Info>> & constantPool;
    wstring getConstantPoolUTF8Wstring(u2 index) {
        return std::dynamic_pointer_cast<CONSTANT_Utf8_info>(constantPool[index])->toUTF8String();
    }
};
/*
读取Attribute_Info
*/
shared_ptr<Attribute_Info> readAttributeInfo(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp);

struct ConstantValue_attribute :Attribute_Info {
    u2 constantvalue_index;
    ConstantValue_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp)
    {
        constantvalue_index = readu2(is);
    }
};

struct Code_attribute : Attribute_Info {

    struct ExceptionTable {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        u2 catch_type;
        ExceptionTable(istream& is) {
            start_pc = readu2(is);
            end_pc = readu2(is);
            handler_pc = readu2(is);
            catch_type = readu2(is);
        }
    };

    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    vector<u1> code;
    u2 exception_table_length;
    vector<shared_ptr<ExceptionTable>> exception_table;
    u2 attributes_count;
    vector<shared_ptr<Attribute_Info>> attributes;

    Code_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        max_stack = readu2(is);
        max_locals = readu2(is);
        code_length = readu4(is);
        read_u1_vector(code, is, code_length);

        // exception table
        exception_table_length = readu2(is);
        for (u2 i = 0; i < exception_table_length; i++) {
            exception_table.push_back(make_shared< ExceptionTable>(is));
        }

        // attributes
        attributes_count = readu2(is);
        for (u2 i = 0; i < attributes_count; i++) {
            attributes.push_back(readAttributeInfo(is, cp));
        }
    }
};

struct StackMapTable_attribute : Attribute_Info {

    /*
    Verification definition begin
    */
    enum StackMapTableVerificationItemType : u1 {
        ITEM_Top = 0,
        ITEM_Integer = 1,
        ITEM_Float = 2,
        ITEM_Double = 3,
        ITEM_Long = 4,
        ITEM_Null = 5,
        ITEM_UninitializedThis = 6,
        ITEM_Object = 7,
        ITEM_Uninitialized = 8
    };
    
    struct verification_type_info {
        u1 tag;
        verification_type_info(istream& is) {
            tag = readu1(is);
        }
        ~verification_type_info() {};
    };
    struct Top_variable_info : verification_type_info {
        Top_variable_info(istream& is) : verification_type_info(is) {}
    };
    struct Integer_variable_info : verification_type_info {
        Integer_variable_info(istream& is) : verification_type_info(is) {}
    };
    struct Float_variable_info : verification_type_info {
        Float_variable_info(istream& is) : verification_type_info(is) {}
    };
    struct Null_variable_info : verification_type_info {
        Null_variable_info(istream& is) : verification_type_info(is) {}
    };
    struct UninitializedThis_variable_info : verification_type_info {
        UninitializedThis_variable_info(istream& is) : verification_type_info(is) {}
    };
    struct Object_variable_info : verification_type_info {
        u2 cpool_index;
        Object_variable_info(istream& is) : verification_type_info(is) {
            cpool_index = readu2(is);
        }
    };
    struct Uninitialized_variable_info : verification_type_info {
        u2 offset;
        Uninitialized_variable_info(istream& is) : verification_type_info(is) {
            offset = readu2(is);
        }
    };
    struct Long_variable_info : verification_type_info {
        Long_variable_info(istream& is) : verification_type_info(is) {}
    };
    struct Double_variable_info : verification_type_info {
        Double_variable_info(istream& is) : verification_type_info(is) {}
    };
    /*
    Verification definition end
    */

    struct stack_map_frame {
        u1 frame_type;
        stack_map_frame(istream& is) {
            frame_type = readu1(is);
        }
        virtual ~stack_map_frame() {};
    protected:
        void readVerificaitonTypes(vector< shared_ptr<verification_type_info>>& vec, istream& is, u2 numbers) {
            for (u2 i = 0; i < numbers; i++) {
                vec.push_back(readVerificationTypeInfo(is));
            }
        }
    };

    struct same_frame :stack_map_frame {
        same_frame(istream& is) : stack_map_frame(is) {}
    };
    struct same_locals_1_stack_item_frame :stack_map_frame {
        vector< shared_ptr<verification_type_info>> stack; // 只有一个
        same_locals_1_stack_item_frame(istream& is) : stack_map_frame(is) {
            readVerificaitonTypes(stack, is, 1);
        }
    };
    struct same_locals_1_stack_item_frame_extended :stack_map_frame {
        u2 offset_delta;
        vector< shared_ptr<verification_type_info>> stack; //只有一个
        same_locals_1_stack_item_frame_extended(istream& is) : stack_map_frame(is) {
            offset_delta = readu2(is);
            readVerificaitonTypes(stack, is, 1);
        }
    };
    struct chop_frame :stack_map_frame {
        u2 offset_delta;
        chop_frame(istream& is) : stack_map_frame(is) {
            offset_delta = readu2(is);
        }
    };
    struct same_frame_extended :stack_map_frame {
        u2 offset_delta;
        same_frame_extended(istream& is) : stack_map_frame(is) {
            offset_delta = readu2(is);
        }
    };
    struct append_frame :stack_map_frame {
        vector< shared_ptr<verification_type_info>> locals;
        u2 offset_delta;
        append_frame(istream& is) : stack_map_frame(is) {
            offset_delta = readu2(is);
        }
    };

    struct full_frame :stack_map_frame {
        u2 offset_delta;
        u2 number_of_locals;
        vector<shared_ptr<verification_type_info>> locals;
        u2 number_of_stack_items;
        vector<shared_ptr<verification_type_info>> stack;
        full_frame(istream& is) : stack_map_frame(is) {
            offset_delta = readu2(is);
            number_of_locals = readu2(is);
            readVerificaitonTypes(locals, is, number_of_locals);
            number_of_stack_items = readu2(is);
            readVerificaitonTypes(stack, is, number_of_stack_items);
        }
    };

    u2 number_of_entries;
    vector<shared_ptr<stack_map_frame>> entries;

    StackMapTable_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp)
    {
        number_of_entries = readu2(is);
        for (u2 i = 0; i < number_of_entries; i++) {
            entries.push_back(readStackMapFrame(is));
        }
    }
private:
    static shared_ptr<verification_type_info> readVerificationTypeInfo(istream& is);
    static shared_ptr<stack_map_frame> readStackMapFrame(istream& is);
};

struct Exceptions_attribute :Attribute_Info {
    u2 number_of_exceptions;
    vector<u2> exception_index_table;
    Exceptions_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        number_of_exceptions = readu2(is);
        read_u2_vector(exception_index_table, is, number_of_exceptions);
    }
};

struct InnerClasses_attribute : Attribute_Info {
    struct InnerClass {
        u2 inner_class_info_index;
        u2 outer_class_info_index;
        u2 inner_name_index;
        u2 inner_class_access_flags;
        InnerClass(istream& is) {
            inner_class_info_index = readu2(is);
            outer_class_info_index = readu2(is);
            inner_name_index = readu2(is);
            inner_class_access_flags = readu2(is);
        }
    };

    u2 number_of_classes;
    vector<shared_ptr< InnerClass>> classes;

    InnerClasses_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        number_of_classes = readu2(is);
        read_vector(classes, is, number_of_classes);
    }
};

struct EnclosingMethod_attribute : Attribute_Info {
    u2 class_index;
    u2 method_index;

    EnclosingMethod_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        class_index = readu2(is);
        method_index = readu2(is);
    }
};

struct Synthetic_attribute : Attribute_Info {
    Synthetic_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
    }
};

struct Signature_attribute : Attribute_Info {
    u2 signature_index;
    Signature_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        signature_index = readu2(is);
    }
};

struct SourceFile_attribute : Attribute_Info {
    u2 sourcefile_index;
    SourceFile_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        sourcefile_index = readu2(is);
    }
};

struct SourceDebugExtension_attribute : Attribute_Info {
    vector<u1> debug_extension;
    SourceDebugExtension_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        read_u1_vector(debug_extension, is, attribute_length);
    }
};

struct LineNumberTable_attribute : Attribute_Info {
    struct LineNumberTable {
        u2 start_pc;
        u2 line_number;
        LineNumberTable(istream& is) {
            start_pc = readu2(is);
            line_number = readu2(is);
        }
    };
    u2 line_number_table_length;
    vector<shared_ptr<LineNumberTable>> line_number_table;
    LineNumberTable_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        line_number_table_length = readu2(is);
        read_vector(line_number_table, is, line_number_table_length);
    }
};

struct LocalVariableTable_attribute : Attribute_Info {
    struct LocalVariableTable {
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 descriptor_index;
        u2 index;
        LocalVariableTable(istream& is)
        {
            start_pc = readu2(is);
            length = readu2(is);
            name_index = readu2(is);
            descriptor_index = readu2(is);
            index = readu2(is);
        }
    };
    u2 local_variable_table_length;
    vector<shared_ptr< LocalVariableTable>> local_variable_table;
    LocalVariableTable_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        local_variable_table_length = readu2(is);
        read_vector(local_variable_table, is, local_variable_table_length);
    }
};

struct LocalVariableTypeTable_attribute : Attribute_Info {
    struct LocalVariableTypeTable {
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 signature_index;
        u2 index;
        LocalVariableTypeTable(istream& is)
        {
            start_pc = readu2(is);
            length = readu2(is);
            name_index = readu2(is);
            signature_index = readu2(is);
            index = readu2(is);
        }
    };
    u2 local_variable_type_table_length;
    vector<shared_ptr< LocalVariableTypeTable>> local_variable_type_table;
    LocalVariableTypeTable_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(is, cp) {
        local_variable_type_table_length = readu2(is);
        read_vector(local_variable_type_table, is, local_variable_type_table_length);
    }
};

struct Deprecated_attribute : Attribute_Info {
    Deprecated_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
    }
};

struct RuntimeAnnotations_attribute : Attribute_Info {

    // 先在这里申明一下，下面有循环指针引用。
    struct annotation;

    struct element_value {
        u1 tag;
        element_value(istream& is) {
            tag = readu1(is);
        }
        virtual ~element_value() {};
    };

    struct const_element_value : element_value {
        u2 const_value_index;
        const_element_value(istream& is) :element_value(is) {
            const_value_index = readu2(is);
        }
    };

    struct enum_const_value : element_value
    {
        u2 type_name_index;
        u2 const_name_index;
        enum_const_value(istream& is) :element_value(is) {
            type_name_index = readu2(is);
            const_name_index = readu2(is);
        }
    };

    struct class_element_value : element_value {
        u2 class_info_index;
        class_element_value(istream& is) :element_value(is) {
            class_info_index = readu2(is);
        }
    };

    struct annotation_element_value : element_value {
        shared_ptr<annotation> annotation_value;
        annotation_element_value(istream& is) :element_value(is) {
            annotation_value = make_shared< annotation>(is);
        }
    };

    struct array_element_value :element_value {
        u2  num_values;
        vector<shared_ptr<element_value>> values;
        array_element_value(istream& is) :element_value(is) {
            num_values = readu2(is);
            for (u2 i = 0; i < num_values; i++) {
                values.push_back(readElementValue(is));
            }
        }
    };

    struct element_value_pair {
        u2 element_name_index;
        shared_ptr<element_value> value;
        element_value_pair(istream& is) {
            element_name_index = readu2(is);
            value = readElementValue(is);
        }
    };
    struct annotation {
        u2 type_index;
        u2 num_element_value_pairs;
        vector<shared_ptr<element_value_pair>> element_value_pairs;
        annotation(istream& is) {
            type_index = readu2(is);
            num_element_value_pairs = readu2(is);
            read_vector(element_value_pairs, is, num_element_value_pairs);
        }
    };

    RuntimeAnnotations_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
    }

protected:
    static shared_ptr<element_value> readElementValue(istream& is);
};

struct RuntimeVisibleAnnotations_attribute : RuntimeAnnotations_attribute {
    u2 num_annotations;
    vector<shared_ptr<annotation>> annotations;

    RuntimeVisibleAnnotations_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeAnnotations_attribute(is, cp) {
        num_annotations = readu2(is);
        read_vector(annotations, is, num_annotations);
    }
};

struct RuntimeInvisibleAnnotations_attribute : RuntimeVisibleAnnotations_attribute {
    RuntimeInvisibleAnnotations_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeVisibleAnnotations_attribute(is, cp) {}
};

struct RuntimeVisibleParameterAnnotations_attribute :RuntimeAnnotations_attribute {
    struct parameter_annotation {
        u2         num_annotations;
        vector<shared_ptr<annotation>> annotations;
        parameter_annotation(istream& is) {
            num_annotations = readu2(is);
            read_vector(annotations, is, num_annotations);
        }
    };
    u1 num_parameters;
    vector<shared_ptr< parameter_annotation>> parameter_annotations;
    RuntimeVisibleParameterAnnotations_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeAnnotations_attribute(is, cp) {
        num_parameters = readu1(is);
        read_vector(parameter_annotations, is, num_parameters);
    }
};

// 4.7.19. The RuntimeInvisibleParameterAnnotations Attribute
struct RuntimeInvisibleParameterAnnotations_attribute :RuntimeVisibleParameterAnnotations_attribute {
    RuntimeInvisibleParameterAnnotations_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeVisibleParameterAnnotations_attribute(is, cp) {}
};

struct RuntimeTypeAnnotations : RuntimeAnnotations_attribute {

    struct type_annotation;

    struct localvar_target_table {
        u2 start_pc;
        u2 length;
        u2 index;
        localvar_target_table(istream& is) {
            start_pc = readu2(is);
            length = readu2(is);
            index = readu2(is);
        }
    };
    struct type_target {
        ~type_target() {};
    };

    struct type_parameter_target :type_target {
        u1 type_parameter_index;
        type_parameter_target(istream& is) {
            type_parameter_index = readu1(is);
        }
    };
    struct supertype_target : type_target {
        u2 supertype_index;
        supertype_target(istream& is) {
            supertype_index = readu2(is);
        }
    };
    struct type_parameter_bound_target : type_target {
        u1 type_parameter_index;
        u1 bound_index;
        type_parameter_bound_target(istream& is) {
            type_parameter_index = readu1(is);
            bound_index = readu1(is);
        }
    };

    struct empty_target :type_target {
        empty_target(istream& is) {}
    };

    struct formal_parameter_target : type_target {
        u1 formal_parameter_index;
        formal_parameter_target(istream& is) {
            formal_parameter_index = readu1(is);
        }
    };

    struct throws_target : type_target {
        u2 throws_type_index;
        throws_target(istream& is) {
            throws_type_index = readu2(is);
        }
    };

    struct localvar_target : type_target {
        u2 table_length;
        vector<shared_ptr< localvar_target_table>> table;
        localvar_target(istream& is) {
            table_length = readu2(is);
            read_vector(table, is, table_length);
        }
    };

    struct catch_target : type_target {
        u2 exception_table_index;
        catch_target(istream& is) {
            exception_table_index = readu2(is);
        }
    };

    struct offset_target : type_target {
        u2 offset;
        offset_target(istream& is) {
            offset = readu2(is);
        }
    };

    struct type_argument_target : type_target {
        u2 offset;
        u1 type_argument_index;
        type_argument_target(istream& is) {
            offset = readu2(is);
            type_argument_index = readu1(is);
        }
    };
    struct path_struct {
        u1 type_path_kind;
        u1 type_argument_index;
        path_struct(istream& is) {
            type_path_kind = readu1(is);
            type_argument_index = readu1(is);
        }
    };
    struct type_path {
        u1 path_length;
        vector<shared_ptr< path_struct>> path;
        type_path(istream& is) {
            path_length = readu1(is);
            read_vector(path, is, path_length);
        }
    };

    struct type_annotation {
        u1 target_type;
        shared_ptr<type_target> target_info;
        shared_ptr<type_path> target_path;
        u2        type_index;
        u2        num_element_value_pairs;
        vector<shared_ptr<element_value_pair>> element_value_pairs;
        type_annotation(istream& is) {
            target_type = readu1(is);
            target_info = readTypeTarget(is, target_type);
            target_path = make_shared< type_path>(is);
            type_index = readu2(is);
            num_element_value_pairs = readu2(is);
            read_vector(element_value_pairs, is, num_element_value_pairs);
        }
    };
    RuntimeTypeAnnotations(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeAnnotations_attribute(is, cp) {}
protected:
    static shared_ptr<type_target> readTypeTarget(istream& is, u1 target_type);
};

struct RuntimeVisibleTypeAnnotations_attribute : RuntimeTypeAnnotations {
    u2 num_annotations;
    vector <shared_ptr< type_annotation>> annotations;
    RuntimeVisibleTypeAnnotations_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeTypeAnnotations(is, cp) {
        num_annotations = readu2(is);
        read_vector(annotations, is, num_annotations);
    }
};

struct RuntimeInvisibleTypeAnnotations_attribute : RuntimeVisibleTypeAnnotations_attribute {
    RuntimeInvisibleTypeAnnotations_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeVisibleTypeAnnotations_attribute(is, cp) {}
};

struct AnnotationDefault_attribute : RuntimeTypeAnnotations {
    shared_ptr< element_value> default_value;
    AnnotationDefault_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeTypeAnnotations(is, cp) {
        default_value = readElementValue(is);
    }
};

struct BootstrapMethods_attribute : Attribute_Info {
    struct bootstrap_method {
        u2 bootstrap_method_ref;
        u2 num_bootstrap_arguments;
        vector<u2> bootstrap_arguments;
        bootstrap_method(istream& is) {
            bootstrap_method_ref = readu2(is);
            num_bootstrap_arguments = readu2(is);
            read_u2_vector(bootstrap_arguments, is, num_bootstrap_arguments);
        }
    };
    u2 num_bootstrap_methods;

    vector<shared_ptr<bootstrap_method>>bootstrap_methods;

    BootstrapMethods_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
        num_bootstrap_methods = readu2(is);
        read_vector(bootstrap_methods, is, num_bootstrap_methods);
    }
};

struct MethodParameters_attribute : Attribute_Info {
    struct parameter {
        u2 name_index;
        u2 access_flags;
        parameter(istream& is) {
            name_index = readu2(is);
            access_flags = readu2(is);
        }
    };
    u1 parameters_count;
    vector<shared_ptr< parameter>> parameters;
    MethodParameters_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
        parameters_count = readu1(is);
        read_vector(parameters, is, parameters_count);
    }
};

struct Module_attribute : Attribute_Info {

    struct require {
        u2 requires_index;
        u2 requires_flags;
        u2 requires_version_index;
        require(istream& is) {
            requires_index = readu2(is);
            requires_flags = readu2(is);
            requires_version_index = readu2(is);
        }
    };

    // since export is keyword in c++, so add _
    struct _export {
        u2 exports_index;
        u2 exports_flags;
        u2 exports_to_count;
        vector<u2> exports_to_index;
        _export(istream& is) {
            exports_index = readu2(is);
            exports_flags = readu2(is);
            exports_to_count = readu2(is);
            read_u2_vector(exports_to_index, is, exports_to_count);
        }
    };

    struct open {
        u2 opens_index;
        u2 opens_flags;
        u2 opens_to_count;
        vector<u2> opens_to_index;
        open(istream& is) {
            opens_index = readu2(is);
            opens_flags = readu2(is);
            opens_to_count = readu2(is);
            read_u2_vector(opens_to_index, is, opens_to_count);
        }
    };
    struct provide {
        u2 provides_index;
        u2 provides_with_count;
        vector<u2> provides_with_index;
        provide(istream& is) {
            provides_index = readu2(is);
            provides_with_count = readu2(is);
            read_u2_vector(provides_with_index, is, provides_with_count);
        }
    };

    u2 module_name_index;
    u2 module_flags;
    u2 module_version_index;

    u2 requires_count;
    vector<shared_ptr< require>> requires;

    u2 exports_count;
    vector<shared_ptr<_export>> exports;

    u2 opens_count;
    vector<shared_ptr<open>> opens;

    u2 uses_count;
    vector<u2> uses_index;

    u2 provides_count;
    vector<shared_ptr<provide>> provides;

    Module_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
        module_name_index = readu2(is);
        module_flags = readu2(is);
        module_version_index = readu2(is);

        // requires.
        requires_count = readu2(is);
        read_vector(requires, is, requires_count);

        // exportes
        exports_count = readu2(is);
        read_vector(exports, is, exports_count);

        // opens
        opens_count = readu2(is);
        read_vector(opens, is, opens_count);

        // uses
        uses_count = readu2(is);
        read_u2_vector(uses_index, is, uses_count);

        // provides
        provides_count = readu2(is);
        read_vector(provides, is, provides_count);
    }
};

struct ModulePackages_attribute : Attribute_Info {
    u2 package_count;
    vector<u2> package_index;
    ModulePackages_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
        package_count = readu2(is);
        read_u2_vector(package_index, is, package_count);
    }
};

struct ModuleMainClass_attribute : Attribute_Info {
    u2 main_class_index;
    ModuleMainClass_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
        main_class_index = readu2(is);
    }
};

struct NestHost_attribute : Attribute_Info {
    u2 host_class_index;
    NestHost_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
        host_class_index = readu2(is);
    }
};

struct NestMembers_attribute : Attribute_Info {
    u2 number_of_classes;
    vector<u2> classes;
    NestMembers_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
        number_of_classes = readu2(is);
        read_u2_vector(classes, is, number_of_classes);
    }
};

struct Unknown_attribute :Attribute_Info {
    vector<u1> info;
    Unknown_attribute(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(is, cp) {
        read_u1_vector(info, is, attribute_length);
    }
};

//字段区定义
struct Field_Info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    vector<shared_ptr<Attribute_Info>> attributes;
    Field_Info(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) {
        access_flags = readu2(is);
        name_index = readu2(is);
        descriptor_index = readu2(is);
        attributes_count = readu2(is);
        for (int i = 0; i < attributes_count; i++) {
            attributes.push_back(readAttributeInfo(is, cp));
        }
    }
};
//字段区定义结束


//方法区

struct Method_Info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    vector<shared_ptr<Attribute_Info>> attributes;
    Method_Info(istream& is, const vector<shared_ptr<CONSTANT_Info>>& cp) {
        access_flags = readu2(is);
        name_index = readu2(is);
        methodName = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(cp[name_index])->toUTF8String();
        descriptor_index = readu2(is);
        attributes_count = readu2(is);
        for (int i = 0; i < attributes_count; i++) {
            //attributes.push_back(make_shared<Attribute_Info>(is, cp));
            attributes.push_back(readAttributeInfo(is, cp));
        }
        //methodName = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(cp[name_index])->toUTF8String();
    }

    wstring getMethodName() const {
        return methodName;
    }
private:
    wstring methodName;
};

//方法区结束

/*
定义一个.class文件的结构体。
*/
struct ClassFile {
    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count;
    vector<shared_ptr<CONSTANT_Info>> constant_pool;
    u2 access_flags;
    u2 this_class;
    u2 super_class;
    u2 interfaces_count;
    vector<u2> interfaces;
    u2 fields_count;
    vector<shared_ptr<Field_Info>> fields;
    u2 methods_count;
    vector<shared_ptr<Method_Info>> methods;
    u2 attributes_count;
    vector<shared_ptr<Attribute_Info>> attributes;

    ClassFile(istream& is);
    ClassFile(wstring& filepath);
    ClassFile(string& filepath);

    bool isJavaClassFile() const;
    wstring getCanonicalClassName() const;

    // these are private members.
private:
    bool parsed = false;
    bool validJavaFile = true;
    bool isSupportedVersion = true;
    wstring canonicalName;

    // method
    bool parse(istream& stream);
    void extract();
    void readConstantPools(istream& is);
    void readInterfaces(istream& is);
    void readFieldInfos(istream& is);
    void readMethodInfos(istream& is);
    void readAttributes(istream& is);
};


#endif // !__JAVA_CLASS__