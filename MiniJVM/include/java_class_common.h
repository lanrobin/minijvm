#ifndef __JAVA_CLASS_COMMON_H__
#define __JAVA_CLASS_COMMON_H__
#include "base_type.h"

const u2 MAX_SUPPORTED_CLASS_FILE_VERSION = 58;
const u2 MIN_SUPPORTED_CLASS_FILE_VERSION = 45;

/// class file access flag
const u2 CLASS_ACC_PUBLIC = 0x0001; //Declared public; may be accessed from outside its package.
const u2 CLASS_ACC_FINAL = 0x0010; //Declared final; no subclasses allowed.
const u2 CLASS_ACC_SUPER = 0x0020; //Treat superclass methods specially when invoked by the invokespecial instruction.
const u2 CLASS_ACC_INTERFACE = 0x0200; //Is an interface, not a class.
const u2 CLASS_ACC_ABSTRACT = 0x0400; //Declared abstract; must not be instantiated.
const u2 CLASS_ACC_SYNTHETIC = 0x1000; //Declared synthetic; not present in the source code.
const u2 CLASS_ACC_ANNOTATION = 0x2000; //Declared as an annotation type.
const u2 CLASS_ACC_ENUM = 0x4000; //Declared as an enum type.
const u2 CLASS_ACC_MODULE = 0x8000; //Is a module, not a class or interface.


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


//field.
const u2 FIELD_ACC_PUBLIC = 0x0001; // Declared public; may be accessed from outside its package.
const u2 FIELD_ACC_PRIVATE = 0x0002; // Declared private; accessible only within the defining class and other classes belonging to the same nest (§5.4.4).
const u2 FIELD_ACC_PROTECTED = 0x0004; // Declared protected; may be accessed within subclasses.
const u2 FIELD_ACC_STATIC = 0x0008; // Declared static.
const u2 FIELD_ACC_FINAL = 0x0010; // Declared final; never directly assigned to after object construction (JLS §17.5).
const u2 FIELD_ACC_VOLATILE = 0x0040; // Declared volatile; cannot be cached.
const u2 FIELD_ACC_TRANSIENT = 0x0080; // Declared transient; not written or read by a persistent object manager.
const u2 FIELD_ACC_SYNTHETIC = 0x1000; // Declared synthetic; not present in the source code.
const u2 FIELD_ACC_ENUM = 0x4000; // Declared as an element of an enum.

#endif // __JAVA_CLASS_COMMON_H__