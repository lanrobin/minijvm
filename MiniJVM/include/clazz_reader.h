#ifndef __JVM_CLASS_READER__
#define __JVM_CLASS_READER__

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include "platform.h"
#include "base_type.h"
#include "java_class_common.h"
#include "buffer.h"
#include "log.h"
#include "string_utils.h"

#define DEBUG_READ_CLASS_FILE

/**
这个文件主要是解析class文件，参照  https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html
*/

u4 read_u1_vector(vector<u1>& v, shared_ptr<Buffer> buf, u4 count);
u4 read_u2_vector(vector<u2>& v, shared_ptr<Buffer> buf, u4 count);

template< typename T>
u4 read_vector(vector<shared_ptr<T>>& v, shared_ptr<Buffer> buf, u4 count);


//常量区定义
struct CONSTANT_Info {
    u1 tag;
#ifdef DEBUG_READ_CLASS_FILE
    string typeName;
#endif
    CONSTANT_Info(shared_ptr<Buffer> buf) {
        tag = buf->readu1();
    }
    CONSTANT_Info() {
        tag = 0;
    }
    
    //这个是需要的，否则会报 error C2683: 'dynamic_cast': 'CONSTANT_Info' is not a polymorphic type
    virtual ~CONSTANT_Info() {};
};

struct CONSTANT_Class_info : CONSTANT_Info {
    u2 name_index;
    CONSTANT_Class_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        name_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Class_info";
#endif
    }
};

struct CONSTANT_Fieldref_info : CONSTANT_Info {
    u2 class_index;
    u2 name_and_type_index;

    CONSTANT_Fieldref_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        class_index = buf->readu2();
        name_and_type_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Fieldref_info";
#endif
    }
};

struct CONSTANT_Methodref_info : CONSTANT_Info {
    u2 class_index;
    u2 name_and_type_index;

    CONSTANT_Methodref_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        class_index = buf->readu2();
        name_and_type_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Methodref_info";
#endif
    }
};

struct CONSTANT_InterfaceMethodref_info : CONSTANT_Info {
    u2 class_index;
    u2 name_and_type_index;

    CONSTANT_InterfaceMethodref_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        class_index = buf->readu2();
        name_and_type_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_InterfaceMethodref_info";
#endif
    }
};

struct CONSTANT_String_info : CONSTANT_Info {
    u2 string_index;
    CONSTANT_String_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        string_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_String_info";
#endif
    }
};

struct CONSTANT_Integer_info : CONSTANT_Info {
    u4 bytes;
    CONSTANT_Integer_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        bytes = buf->readu4();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Integer_info";
#endif
    }
};

struct CONSTANT_Float_info : CONSTANT_Info {
    u4 bytes;
    CONSTANT_Float_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        bytes = buf->readu4();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Float_info";
#endif
    }
};

struct  CONSTANT_Long_info : CONSTANT_Info {
    u4 high_bytes;
    u4 low_bytes;

    CONSTANT_Long_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        high_bytes = buf->readu4();
        low_bytes = buf->readu4();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Long_info";
#endif
    }
};

struct  CONSTANT_Double_info : CONSTANT_Info {
    u4 high_bytes;
    u4 low_bytes;

    CONSTANT_Double_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        high_bytes = buf->readu4();
        low_bytes = buf->readu4();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Double_info";
#endif
    }
};

struct CONSTANT_NameAndType_info : CONSTANT_Info {
    u2 name_index;
    u2 descriptor_index;
    CONSTANT_NameAndType_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        name_index = buf->readu2();
        descriptor_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_NameAndType_info";
#endif
    }
};

struct CONSTANT_Utf8_info : CONSTANT_Info {
    u2 length;
    vector<u1> bytes;

    CONSTANT_Utf8_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        length = buf->readu2();
        read_u1_vector(bytes, buf, length);
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Utf8_info";
#endif
    }

    wstring toUTF8String();

private:
    wstring utf8String;
};

struct CONSTANT_MethodHandle_info : CONSTANT_Info {
    u1 reference_kind;
    u2 reference_index;
    CONSTANT_MethodHandle_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        reference_kind = buf->readu1();
        reference_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_MethodHandle_info";
#endif
    }
};

struct CONSTANT_MethodType_info : CONSTANT_Info {
    u2 descriptor_index;

    CONSTANT_MethodType_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        descriptor_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_MethodType_info";
#endif
    }
};

struct CONSTANT_Dynamic_info : CONSTANT_Info {
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
    CONSTANT_Dynamic_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        bootstrap_method_attr_index = buf->readu2();
        name_and_type_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Dynamic_info";
#endif
    }
};

struct CONSTANT_InvokeDynamic_info : CONSTANT_Info {
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
    CONSTANT_InvokeDynamic_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        bootstrap_method_attr_index = buf->readu2();
        name_and_type_index = buf->readu2();
    }
};

struct CONSTANT_Module_info : CONSTANT_Info {
    u2 name_index;
    CONSTANT_Module_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        name_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Module_info";
#endif
    }
};

struct CONSTANT_Package_info : CONSTANT_Info {
    u2 name_index;
    CONSTANT_Package_info(shared_ptr<Buffer> buf) :CONSTANT_Info(buf) {
        name_index = buf->readu2();
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Package_info";
#endif
    }
};

/*
这个类应该是用不到，只是为了Long和Double的类型占位用的。
*/
struct CONSTANT_LargeNumberContinuous_info :CONSTANT_Info {
    CONSTANT_LargeNumberContinuous_info() {
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_LargeNumberContinuous_info";
#endif
    }
};

/*
第一个(Index为0的slot不用，用这个占住。)
*/
struct CONSTANT_Dummy_info :CONSTANT_Info {
    CONSTANT_Dummy_info(){
#ifdef DEBUG_READ_CLASS_FILE
        typeName = "CONSTANT_Dummy_info";
#endif
    }
};
//常量区定义结束。


// Attribute区

struct Attribute_Info {
    u2 attribute_name_index;
    u4 attribute_length;

    //vector<u1> info;
    Attribute_Info(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) :constantPool(cp) {
        attribute_name_index = buf->readu2();
        attribute_length = buf->readu4();
        //read_u1_vector(info, buf, attribute_length);
        //shared_ptr<CONSTANT_Utf8_info> entry = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(cp[attribute_name_index]);
        name = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(constantPool[attribute_name_index])->toUTF8String();
    }

    wstring getAttributeName() const{
        return name;
    }

    // make it virtual.
    virtual ~Attribute_Info() {
        spdlog::info("Attribute_Info:{} gone", w2s(name));
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
shared_ptr<Attribute_Info> readAttributeInfo(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp);

struct ConstantValue_attribute :Attribute_Info {
    u2 constantvalue_index;
    ConstantValue_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp)
    {
        constantvalue_index = buf->readu2();
    }
};

struct Code_attribute : Attribute_Info {

    struct ExceptionTable {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        u2 catch_type;
        ExceptionTable(shared_ptr<Buffer> buf) {
            start_pc = buf->readu2();
            end_pc = buf->readu2();
            handler_pc = buf->readu2();
            catch_type = buf->readu2();
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

    Code_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        max_stack = buf->readu2();
        max_locals = buf->readu2();
        code_length = buf->readu4();
        read_u1_vector(code, buf, code_length);

        // exception table
        exception_table_length = buf->readu2();
        for (u2 i = 0; i < exception_table_length; i++) {
            exception_table.push_back(make_shared<ExceptionTable>(buf));
        }

        // attributes
        attributes_count = buf->readu2();
        for (u2 i = 0; i < attributes_count; i++) {
            attributes.push_back(readAttributeInfo(buf, cp));
        }
    }

    ~Code_attribute() {
        exception_table.clear();
        attributes.clear();
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
        verification_type_info(shared_ptr<Buffer> buf) {
            tag = buf->readu1();
        }
        ~verification_type_info() {};
    };
    struct Top_variable_info : verification_type_info {
        Top_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {}
    };
    struct Integer_variable_info : verification_type_info {
        Integer_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {}
    };
    struct Float_variable_info : verification_type_info {
        Float_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {}
    };
    struct Null_variable_info : verification_type_info {
        Null_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {}
    };
    struct UninitializedThis_variable_info : verification_type_info {
        UninitializedThis_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {}
    };
    struct Object_variable_info : verification_type_info {
        u2 cpool_index;
        Object_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {
            cpool_index = buf->readu2();
        }
    };
    struct Uninitialized_variable_info : verification_type_info {
        u2 offset;
        Uninitialized_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {
            offset = buf->readu2();
        }
    };
    struct Long_variable_info : verification_type_info {
        Long_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {}
    };
    struct Double_variable_info : verification_type_info {
        Double_variable_info(shared_ptr<Buffer> buf) : verification_type_info(buf) {}
    };
    /*
    Verification definition end
    */

    struct stack_map_frame {
        u1 frame_type;
        stack_map_frame(shared_ptr<Buffer> buf) {
            frame_type = buf->readu1();
        }
        virtual ~stack_map_frame() {};
    protected:
        void readVerificaitonTypes(vector< shared_ptr<verification_type_info>>& vec, shared_ptr<Buffer> buf, u2 numbers) {
            for (u2 i = 0; i < numbers; i++) {
                vec.push_back(readVerificationTypeInfo(buf));
            }
        }
    };

    struct same_frame :stack_map_frame {
        same_frame(shared_ptr<Buffer> buf) : stack_map_frame(buf) {}
    };
    struct same_locals_1_stack_item_frame :stack_map_frame {
        vector< shared_ptr<verification_type_info>> stack; // 只有一个
        same_locals_1_stack_item_frame(shared_ptr<Buffer> buf) : stack_map_frame(buf) {
            readVerificaitonTypes(stack, buf, 1);
        }
        ~same_locals_1_stack_item_frame() {
            stack.clear();
        }
    };
    struct same_locals_1_stack_item_frame_extended :stack_map_frame {
        u2 offset_delta;
        vector< shared_ptr<verification_type_info>> stack; //只有一个
        same_locals_1_stack_item_frame_extended(shared_ptr<Buffer> buf) : stack_map_frame(buf) {
            offset_delta = buf->readu2();
            readVerificaitonTypes(stack, buf, 1);
        }
        ~same_locals_1_stack_item_frame_extended() {
            stack.clear();
        }
    };
    struct chop_frame :stack_map_frame {
        u2 offset_delta;
        chop_frame(shared_ptr<Buffer> buf) : stack_map_frame(buf) {
            offset_delta = buf->readu2();
        }
    };
    struct same_frame_extended :stack_map_frame {
        u2 offset_delta;
        same_frame_extended(shared_ptr<Buffer> buf) : stack_map_frame(buf) {
            offset_delta = buf->readu2();
        }
    };
    struct append_frame :stack_map_frame {
        vector< shared_ptr<verification_type_info>> locals;
        u2 offset_delta;
        append_frame(shared_ptr<Buffer> buf) : stack_map_frame(buf) {
            offset_delta = buf->readu2();
            u1 locals_count = frame_type - 251;
            for (u1 i = 0; i < locals_count; i++) {
                locals.push_back(readVerificationTypeInfo(buf));
            }
        }

        ~append_frame() {
            locals.clear();
        }
    };

    struct full_frame :stack_map_frame {
        u2 offset_delta;
        u2 number_of_locals;
        vector<shared_ptr<verification_type_info>> locals;
        u2 number_of_stack_items;
        vector<shared_ptr<verification_type_info>> stack;
        full_frame(shared_ptr<Buffer> buf) : stack_map_frame(buf) {
            offset_delta = buf->readu2();
            number_of_locals = buf->readu2();
            readVerificaitonTypes(locals, buf, number_of_locals);
            number_of_stack_items = buf->readu2();
            readVerificaitonTypes(stack, buf, number_of_stack_items);
        }

        ~full_frame() {
            locals.clear();
            stack.clear();
        }
    };

    u2 number_of_entries;
    vector<shared_ptr<stack_map_frame>> entries;

    StackMapTable_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp)
    {
        number_of_entries = buf->readu2();
        for (u2 i = 0; i < number_of_entries; i++) {
            entries.push_back(StackMapTable_attribute::readStackMapFrame(buf));
        }
    }

    ~StackMapTable_attribute() {
        entries.clear();
    }
private:
    static shared_ptr<verification_type_info> readVerificationTypeInfo(shared_ptr<Buffer> buf);
    static shared_ptr<stack_map_frame> readStackMapFrame(shared_ptr<Buffer> buf);
};

struct Exceptions_attribute :Attribute_Info {
    u2 number_of_exceptions;
    vector<u2> exception_index_table;
    Exceptions_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        number_of_exceptions = buf->readu2();
        read_u2_vector(exception_index_table, buf, number_of_exceptions);
    }
};

struct InnerClasses_attribute : Attribute_Info {
    struct InnerClass {
        u2 inner_class_info_index;
        u2 outer_class_info_index;
        u2 inner_name_index;
        u2 inner_class_access_flags;
        InnerClass(shared_ptr<Buffer> buf) {
            inner_class_info_index = buf->readu2();
            outer_class_info_index = buf->readu2();
            inner_name_index = buf->readu2();
            inner_class_access_flags = buf->readu2();
        }
    };

    u2 number_of_classes;
    vector<shared_ptr< InnerClass>> classes;

    InnerClasses_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        number_of_classes = buf->readu2();
        read_vector(classes, buf, number_of_classes);
    }

    ~InnerClasses_attribute() {
        classes.clear();
    }
};

struct EnclosingMethod_attribute : Attribute_Info {
    u2 class_index;
    u2 method_index;

    EnclosingMethod_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        class_index = buf->readu2();
        method_index = buf->readu2();
    }
};

struct Synthetic_attribute : Attribute_Info {
    Synthetic_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
    }
};

struct Signature_attribute : Attribute_Info {
    u2 signature_index;
    Signature_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        signature_index = buf->readu2();
    }
};

struct SourceFile_attribute : Attribute_Info {
    u2 sourcefile_index;
    SourceFile_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        sourcefile_index = buf->readu2();
    }
};

struct SourceDebugExtension_attribute : Attribute_Info {
    vector<u1> debug_extension;
    SourceDebugExtension_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        read_u1_vector(debug_extension, buf, attribute_length);
    }
};

struct LineNumberTable_attribute : Attribute_Info {
    struct LineNumberTable {
        u2 start_pc;
        u2 line_number;
        LineNumberTable(shared_ptr<Buffer> buf) {
            start_pc = buf->readu2();
            line_number = buf->readu2();
        }
    };
    u2 line_number_table_length;
    vector<shared_ptr<LineNumberTable>> line_number_table;
    LineNumberTable_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        line_number_table_length = buf->readu2();
        read_vector(line_number_table, buf, line_number_table_length);
    }

    ~LineNumberTable_attribute() {
        line_number_table.clear();
    }
};

struct LocalVariableTable_attribute : Attribute_Info {
    struct LocalVariableTable {
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 descriptor_index;
        u2 index;
        LocalVariableTable(shared_ptr<Buffer> buf)
        {
            start_pc = buf->readu2();
            length = buf->readu2();
            name_index = buf->readu2();
            descriptor_index = buf->readu2();
            index = buf->readu2();
        }
    };
    u2 local_variable_table_length;
    vector<shared_ptr< LocalVariableTable>> local_variable_table;
    LocalVariableTable_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        local_variable_table_length = buf->readu2();
        read_vector(local_variable_table, buf, local_variable_table_length);
    }

    ~LocalVariableTable_attribute() {
        local_variable_table.clear();
    }
};

struct LocalVariableTypeTable_attribute : Attribute_Info {
    struct LocalVariableTypeTable {
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 signature_index;
        u2 index;
        LocalVariableTypeTable(shared_ptr<Buffer> buf)
        {
            start_pc = buf->readu2();
            length = buf->readu2();
            name_index = buf->readu2();
            signature_index = buf->readu2();
            index = buf->readu2();
        }
    };
    u2 local_variable_type_table_length;
    vector<shared_ptr< LocalVariableTypeTable>> local_variable_type_table;
    LocalVariableTypeTable_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>> & cp) : Attribute_Info(buf, cp) {
        local_variable_type_table_length = buf->readu2();
        read_vector(local_variable_type_table, buf, local_variable_type_table_length);
    }

    ~LocalVariableTypeTable_attribute() {
        local_variable_type_table.clear();
    }
};

struct Deprecated_attribute : Attribute_Info {
    Deprecated_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
    }
};

struct RuntimeAnnotations_attribute : Attribute_Info {

    // 先在这里申明一下，下面有循环指针引用。
    struct annotation;

    struct element_value {
        u1 tag;
        element_value(shared_ptr<Buffer> buf) {
            tag = buf->readu1();
        }
        virtual ~element_value() {};
    };

    struct const_element_value : element_value {
        u2 const_value_index;
        const_element_value(shared_ptr<Buffer> buf) :element_value(buf) {
            const_value_index = buf->readu2();
        }
    };

    struct enum_const_value : element_value
    {
        u2 type_name_index;
        u2 const_name_index;
        enum_const_value(shared_ptr<Buffer> buf) :element_value(buf) {
            type_name_index = buf->readu2();
            const_name_index = buf->readu2();
        }
    };

    struct class_element_value : element_value {
        u2 class_info_index;
        class_element_value(shared_ptr<Buffer> buf) :element_value(buf) {
            class_info_index = buf->readu2();
        }
    };

    struct annotation_element_value : element_value {
        shared_ptr<annotation> annotation_value;
        annotation_element_value(shared_ptr<Buffer> buf) :element_value(buf) {
            annotation_value = make_shared<annotation>(buf);
        }
    };

    struct array_element_value :element_value {
        u2  num_values;
        vector<shared_ptr<element_value>> values;
        array_element_value(shared_ptr<Buffer> buf) :element_value(buf) {
            num_values = buf->readu2();
            for (u2 i = 0; i < num_values; i++) {
                values.push_back(readElementValue(buf));
            }
        }
        ~array_element_value() {
            values.clear();
        }
    };

    struct element_value_pair {
        u2 element_name_index;
        shared_ptr<element_value> value;
        element_value_pair(shared_ptr<Buffer> buf) {
            element_name_index = buf->readu2();
            value = readElementValue(buf);
        }
    };

    struct annotation {
        u2 type_index;
        u2 num_element_value_pairs;
        vector<shared_ptr<element_value_pair>> element_value_pairs;
        annotation(shared_ptr<Buffer> buf) {
            type_index = buf->readu2();
            num_element_value_pairs = buf->readu2();
            read_vector(element_value_pairs, buf, num_element_value_pairs);
        }
        ~annotation() {
            element_value_pairs.clear();
        }
    };

    RuntimeAnnotations_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
    }

protected:
    static shared_ptr<element_value> readElementValue(shared_ptr<Buffer> buf);
};

struct RuntimeVisibleAnnotations_attribute : RuntimeAnnotations_attribute {
    u2 num_annotations;
    vector<shared_ptr<annotation>> annotations;

    RuntimeVisibleAnnotations_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeAnnotations_attribute(buf, cp) {
        num_annotations = buf->readu2();
        read_vector(annotations, buf, num_annotations);
    }

    ~RuntimeVisibleAnnotations_attribute() {
        annotations.clear();
    }
};

struct RuntimeInvisibleAnnotations_attribute : RuntimeVisibleAnnotations_attribute {
    RuntimeInvisibleAnnotations_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeVisibleAnnotations_attribute(buf, cp) {}
};

struct RuntimeVisibleParameterAnnotations_attribute :RuntimeAnnotations_attribute {
    struct parameter_annotation {
        u2         num_annotations;
        vector<shared_ptr<annotation>> annotations;
        parameter_annotation(shared_ptr<Buffer> buf) {
            num_annotations = buf->readu2();
            read_vector(annotations, buf, num_annotations);
        }

        ~parameter_annotation() {
            annotations.clear();
        }
    };
    u1 num_parameters;
    vector<shared_ptr< parameter_annotation>> parameter_annotations;
    RuntimeVisibleParameterAnnotations_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeAnnotations_attribute(buf, cp) {
        num_parameters = buf->readu1();
        read_vector(parameter_annotations, buf, num_parameters);
    }

    ~RuntimeVisibleParameterAnnotations_attribute() {
        parameter_annotations.clear();
    }
};

// 4.7.19. The RuntimeInVisibleParameterAnnotations Attribute
struct RuntimeInvisibleParameterAnnotations_attribute :RuntimeVisibleParameterAnnotations_attribute {
    RuntimeInvisibleParameterAnnotations_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeVisibleParameterAnnotations_attribute(buf, cp) {}
};

struct RuntimeTypeAnnotations : RuntimeAnnotations_attribute {

    struct type_annotation;

    struct localvar_target_table {
        u2 start_pc;
        u2 length;
        u2 index;
        localvar_target_table(shared_ptr<Buffer> buf) {
            start_pc = buf->readu2();
            length = buf->readu2();
            index = buf->readu2();
        }
    };
    struct type_target {
        ~type_target() {};
    };

    struct type_parameter_target :type_target {
        u1 type_parameter_index;
        type_parameter_target(shared_ptr<Buffer> buf) {
            type_parameter_index = buf->readu1();
        }
    };
    struct supertype_target : type_target {
        u2 supertype_index;
        supertype_target(shared_ptr<Buffer> buf) {
            supertype_index = buf->readu2();
        }
    };
    struct type_parameter_bound_target : type_target {
        u1 type_parameter_index;
        u1 bound_index;
        type_parameter_bound_target(shared_ptr<Buffer> buf) {
            type_parameter_index = buf->readu1();
            bound_index = buf->readu1();
        }
    };

    struct empty_target :type_target {
        empty_target(shared_ptr<Buffer> buf) {}
    };

    struct formal_parameter_target : type_target {
        u1 formal_parameter_index;
        formal_parameter_target(shared_ptr<Buffer> buf) {
            formal_parameter_index = buf->readu1();
        }
    };

    struct throws_target : type_target {
        u2 throws_type_index;
        throws_target(shared_ptr<Buffer> buf) {
            throws_type_index = buf->readu2();
        }
    };

    struct localvar_target : type_target {
        u2 table_length;
        vector<shared_ptr< localvar_target_table>> table;
        localvar_target(shared_ptr<Buffer> buf) {
            table_length = buf->readu2();
            read_vector(table, buf, table_length);
        }

        ~localvar_target() {
            table.clear();
        }
    };

    struct catch_target : type_target {
        u2 exception_table_index;
        catch_target(shared_ptr<Buffer> buf) {
            exception_table_index = buf->readu2();
        }
    };

    struct offset_target : type_target {
        u2 offset;
        offset_target(shared_ptr<Buffer> buf) {
            offset = buf->readu2();
        }
    };

    struct type_argument_target : type_target {
        u2 offset;
        u1 type_argument_index;
        type_argument_target(shared_ptr<Buffer> buf) {
            offset = buf->readu2();
            type_argument_index = buf->readu1();
        }
    };
    struct path_struct {
        u1 type_path_kind;
        u1 type_argument_index;
        path_struct(shared_ptr<Buffer> buf) {
            type_path_kind = buf->readu1();
            type_argument_index = buf->readu1();
        }
    };
    struct type_path {
        u1 path_length;
        vector<shared_ptr< path_struct>> path;
        type_path(shared_ptr<Buffer> buf) {
            path_length = buf->readu1();
            read_vector(path, buf, path_length);
        }

        ~type_path() {
            path.clear();
        }
    };

    struct type_annotation {
        u1 target_type;
        shared_ptr<type_target> target_info;
        shared_ptr<type_path> target_path;
        u2        type_index;
        u2        num_element_value_pairs;
        vector<shared_ptr<element_value_pair>> element_value_pairs;
        type_annotation(shared_ptr<Buffer> buf) {
            target_type = buf->readu1();
            target_info = readTypeTarget(buf, target_type);
            target_path = make_shared< type_path>(buf);
            type_index = buf->readu2();
            num_element_value_pairs = buf->readu2();
            read_vector(element_value_pairs, buf, num_element_value_pairs);
        }

        ~type_annotation() {
            element_value_pairs.clear();
        }
    };
    RuntimeTypeAnnotations(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeAnnotations_attribute(buf, cp) {}
protected:
    static shared_ptr<type_target> readTypeTarget(shared_ptr<Buffer> buf, u1 target_type);
};

struct RuntimeVisibleTypeAnnotations_attribute : RuntimeTypeAnnotations {
    u2 num_annotations;
    vector <shared_ptr< type_annotation>> annotations;
    RuntimeVisibleTypeAnnotations_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeTypeAnnotations(buf, cp) {
        num_annotations = buf->readu2();
        read_vector(annotations, buf, num_annotations);
    }

    ~RuntimeVisibleTypeAnnotations_attribute() {
        annotations.clear();
    }
};

struct RuntimeInvisibleTypeAnnotations_attribute : RuntimeVisibleTypeAnnotations_attribute {
    RuntimeInvisibleTypeAnnotations_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeVisibleTypeAnnotations_attribute(buf, cp) {}
};

struct AnnotationDefault_attribute : RuntimeTypeAnnotations {
    shared_ptr< element_value> default_value;
    AnnotationDefault_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : RuntimeTypeAnnotations(buf, cp) {
        default_value = readElementValue(buf);
    }
};

struct BootstrapMethods_attribute : Attribute_Info {
    struct bootstrap_method {
        u2 bootstrap_method_ref;
        u2 num_bootstrap_arguments;
        vector<u2> bootstrap_arguments;
        bootstrap_method(shared_ptr<Buffer> buf) {
            bootstrap_method_ref = buf->readu2();
            num_bootstrap_arguments = buf->readu2();
            read_u2_vector(bootstrap_arguments, buf, num_bootstrap_arguments);
        }
    };
    u2 num_bootstrap_methods;

    vector<shared_ptr<bootstrap_method>>bootstrap_methods;

    BootstrapMethods_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
        num_bootstrap_methods = buf->readu2();
        read_vector(bootstrap_methods, buf, num_bootstrap_methods);
    }

    ~BootstrapMethods_attribute() {
        bootstrap_methods.clear();
    }
};

struct MethodParameters_attribute : Attribute_Info {
    struct parameter {
        u2 name_index;
        u2 access_flags;
        parameter(shared_ptr<Buffer> buf) {
            name_index = buf->readu2();
            access_flags = buf->readu2();
        }
    };
    u1 parameters_count;
    vector<shared_ptr< parameter>> parameters;
    MethodParameters_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
        parameters_count = buf->readu1();
        read_vector(parameters, buf, parameters_count);
    }

    ~MethodParameters_attribute() {
        parameters.clear();
    }
};

struct Module_attribute : Attribute_Info {

    struct require {
        u2 requires_index;
        u2 requires_flags;
        u2 requires_version_index;
        require(shared_ptr<Buffer> buf) {
            requires_index = buf->readu2();
            requires_flags = buf->readu2();
            requires_version_index = buf->readu2();
        }
    };

    // since export buf keyword in c++, so add _
    struct _export {
        u2 exports_index;
        u2 exports_flags;
        u2 exports_to_count;
        vector<u2> exports_to_index;
        _export(shared_ptr<Buffer> buf) {
            exports_index = buf->readu2();
            exports_flags = buf->readu2();
            exports_to_count = buf->readu2();
            read_u2_vector(exports_to_index, buf, exports_to_count);
        }
    };

    struct open {
        u2 opens_index;
        u2 opens_flags;
        u2 opens_to_count;
        vector<u2> opens_to_index;
        open(shared_ptr<Buffer> buf) {
            opens_index = buf->readu2();
            opens_flags = buf->readu2();
            opens_to_count = buf->readu2();
            read_u2_vector(opens_to_index, buf, opens_to_count);
        }
    };
    struct provide {
        u2 provides_index;
        u2 provides_with_count;
        vector<u2> provides_with_index;
        provide(shared_ptr<Buffer> buf) {
            provides_index = buf->readu2();
            provides_with_count = buf->readu2();
            read_u2_vector(provides_with_index, buf, provides_with_count);
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

    Module_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
        module_name_index = buf->readu2();
        module_flags = buf->readu2();
        module_version_index = buf->readu2();

        // requires.
        requires_count = buf->readu2();
        read_vector(requires, buf, requires_count);

        // exportes
        exports_count = buf->readu2();
        read_vector(exports, buf, exports_count);

        // opens
        opens_count = buf->readu2();
        read_vector(opens, buf, opens_count);

        // uses
        uses_count = buf->readu2();
        read_u2_vector(uses_index, buf, uses_count);

        // provides
        provides_count = buf->readu2();
        read_vector(provides, buf, provides_count);
    }

    ~Module_attribute() {
        requires.clear();
        exports.clear();
        opens.clear();
        provides.clear();
    }
};

struct ModulePackages_attribute : Attribute_Info {
    u2 package_count;
    vector<u2> package_index;
    ModulePackages_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
        package_count = buf->readu2();
        read_u2_vector(package_index, buf, package_count);
    }
};

struct ModuleMainClass_attribute : Attribute_Info {
    u2 main_class_index;
    ModuleMainClass_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
        main_class_index = buf->readu2();
    }
};

struct NestHost_attribute : Attribute_Info {
    u2 host_class_index;
    NestHost_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
        host_class_index = buf->readu2();
    }
};

struct NestMembers_attribute : Attribute_Info {
    u2 number_of_classes;
    vector<u2> classes;
    NestMembers_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
        number_of_classes = buf->readu2();
        read_u2_vector(classes, buf, number_of_classes);
    }
};

struct Unknown_attribute :Attribute_Info {
    vector<u1> info;
    Unknown_attribute(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) : Attribute_Info(buf, cp) {
        read_u1_vector(info, buf, attribute_length);
    }
};

//字段区定义
struct Field_Info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    vector<shared_ptr<Attribute_Info>> attributes;
    Field_Info(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) {
        access_flags = buf->readu2();
        name_index = buf->readu2();
        descriptor_index = buf->readu2();
        attributes_count = buf->readu2();
        for (int i = 0; i < attributes_count; i++) {
            attributes.push_back(readAttributeInfo(buf, cp));
        }
    }

    ~Field_Info() {
        attributes.clear();
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
    Method_Info(shared_ptr<Buffer> buf, const vector<shared_ptr<CONSTANT_Info>>& cp) {
        access_flags = buf->readu2();
        name_index = buf->readu2();
        methodName = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(cp[name_index])->toUTF8String();
        descriptor_index = buf->readu2();
        attributes_count = buf->readu2();
        for (int i = 0; i < attributes_count; i++) {
            //attributes.push_back(make_shared<Attribute_Info>(buf, cp));
            attributes.push_back(readAttributeInfo(buf, cp));
        }
        //methodName = std::dynamic_pointer_cast<CONSTANT_Utf8_info>(cp[name_index])->toUTF8String();
    }

    ~Method_Info() {
        attributes.clear();
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

    ClassFile(shared_ptr<Buffer> buf);
    /*ClassFile(wstring& filepath);
    ClassFile(string& filepath);
    ClassFile(ClassFile&& cf) = default;
    ClassFile(ClassFile& cf) = default;*/
    ~ClassFile();

    bool isJavaClassFile() const;
    wstring getCanonicalClassName() const;
    bool isSupportedClassFile() const { return isSupportedVersion; }

    bool isInterface() const{ return ((access_flags & CLASS_ACC_INTERFACE) == CLASS_ACC_INTERFACE); }
    wstring getClassName(u2 index);
    wstring getUtf8String(u2 index);
    std::pair<wstring, wstring> getNameAndType(u2 index);
    shared_ptr<Attribute_Info> getAttributeByName(const wstring& name) const;
    // these are private members.
private:
    bool parsed = false;
    bool validJavaFile = true;
    bool isSupportedVersion = true;
    wstring canonicalName;

    // method
    bool parse(shared_ptr<Buffer> buf);
    void extract();
    void readConstantPools(shared_ptr<Buffer> buf);
    void readInterfaces(shared_ptr<Buffer> buf);
    void readFieldInfos(shared_ptr<Buffer> buf);
    void readMethodInfos(shared_ptr<Buffer> buf);
    void readAttributes(shared_ptr<Buffer> buf);
};


#endif // !__JAVA_CLASS__