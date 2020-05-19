#ifndef __JVM_VM_ENGINE_H__
#define __JVM_VM_ENGINE_H__

#include "base_type.h"
enum Opcode : u1 {
	Constant_0x00_nop = 0,
	Constant_0x01_aconst_null = 1,
	Constant_0x02_iconst_m1 = 2,
	Constant_0x03_iconst_0 = 3,
	Constant_0x04_iconst_1 = 4,
	Constant_0x05_iconst_2 = 5,
	Constant_0x06_iconst_3 = 6,
	Constant_0x07_iconst_4 = 7,
	Constant_0x08_iconst_5 = 8,
	Constant_0x09_lconst_0 = 9,
	Constant_0x0a_lconst_1 = 10,
	Constant_0x0b_fconst_0 = 11,
	Constant_0x0c_fconst_1 = 12,
	Constant_0x0d_fconst_2 = 13,
	Constant_0x0e_dconst_0 = 14,
	Constant_0x0f_dconst_1 = 15,
	Constant_0x10_bipush = 16,
	Constant_0x11_sipush = 17,
	Constant_0x12_ldc = 18,
	Constant_0x13_ldc_w = 19,
	Constant_0x14_ldc2_w = 20,
	Load_0x15_iload = 21,
	Load_0x16_lload = 22,
	Load_0x17_fload = 23,
	Load_0x18_dload = 24,
	Load_0x19_aload = 25,
	Load_0x1a_iload_0 = 26,
	Load_0x1b_iload_1 = 27,
	Load_0x1c_iload_2 = 28,
	Load_0x1d_iload_3 = 29,
	Load_0x1e_lload_0 = 30,
	Load_0x1f_lload_1 = 31,
	Load_0x20_lload_2 = 32,
	Load_0x21_lload_3 = 33,
	Load_0x22_fload_0 = 34,
	Load_0x23_fload_1 = 35,
	Load_0x24_fload_2 = 36,
	Load_0x25_fload_3 = 37,
	Load_0x26_dload_0 = 38,
	Load_0x27_dload_1 = 39,
	Load_0x28_dload_2 = 40,
	Load_0x29_dload_3 = 41,
	Load_0x2a_aload_0 = 42,
	Load_0x2b_aload_1 = 43,
	Load_0x2c_aload_2 = 44,
	Load_0x2d_aload_3 = 45,
	Load_0x2e_iaload = 46,
	Load_0x2f_laload = 47,
	Load_0x30_faload = 48,
	Load_0x31_daload = 49,
	Load_0x32_aaload = 50,
	Load_0x33_baload = 51,
	Load_0x34_caload = 52,
	Load_0x35_saload = 53,
	Store_0x36_istore = 54,
	Store_0x37_lstore = 55,
	Store_0x38_fstore = 56,
	Store_0x39_dstore = 57,
	Store_0x3a_astore = 58,
	Store_0x3b_istore_0 = 59,
	Store_0x3c_istore_1 = 60,
	Store_0x3d_istore_2 = 61,
	Store_0x3e_istore_3 = 62,
	Store_0x3f_lstore_0 = 63,
	Store_0x40_lstore_1 = 64,
	Store_0x41_lstore_2 = 65,
	Store_0x42_lstore_3 = 66,
	Store_0x43_fstore_0 = 67,
	Store_0x44_fstore_1 = 68,
	Store_0x45_fstore_2 = 69,
	Store_0x46_fstore_3 = 70,
	Store_0x47_dstore_0 = 71,
	Store_0x48_dstore_1 = 72,
	Store_0x49_dstore_2 = 73,
	Store_0x4a_dstore_3 = 74,
	Store_0x4b_astore_0 = 75,
	Store_0x4c_astore_1 = 76,
	Store_0x4d_astore_2 = 77,
	Store_0x4e_astore_3 = 78,
	Store_0x4f_iastore = 79,
	Store_0x50_lastore = 80,
	Store_0x51_fastore = 81,
	Store_0x52_dastore = 82,
	Store_0x53_aastore = 83,
	Store_0x54_bastore = 84,
	Store_0x55_castore = 85,
	Store_0x56_sastore = 86,
	Stack_0x57_pop = 87,
	Stack_0x58_pop2 = 88,
	Stack_0x59_dup = 89,
	Stack_0x5a_dup_x1 = 90,
	Stack_0x5b_dup_x2 = 91,
	Stack_0x5c_dup2 = 92,
	Stack_0x5d_dup2_x1 = 93,
	Stack_0x5e_dup2_x2 = 94,
	Stack_0x5f_swap = 95,
	Math_0x60_iadd = 96,
	Math_0x61_ladd = 97,
	Math_0x62_fadd = 98,
	Math_0x63_dadd = 99,
	Math_0x64_isub = 100,
	Math_0x65_lsub = 101,
	Math_0x66_fsub = 102,
	Math_0x67_dsub = 103,
	Math_0x68_imul = 104,
	Math_0x69_lmul = 105,
	Math_0x6a_fmul = 106,
	Math_0x6b_dmul = 107,
	Math_0x6c_idiv = 108,
	Math_0x6d_ldiv = 109,
	Math_0x6e_fdiv = 110,
	Math_0x6f_ddiv = 111,
	Math_0x70_irem = 112,
	Math_0x71_lrem = 113,
	Math_0x72_frem = 114,
	Math_0x73_drem = 115,
	Math_0x74_ineg = 116,
	Math_0x75_lneg = 117,
	Math_0x76_fneg = 118,
	Math_0x77_dneg = 119,
	Math_0x78_ishl = 120,
	Math_0x79_lshl = 121,
	Math_0x7a_ishr = 122,
	Math_0x7b_lshr = 123,
	Math_0x7c_iushr = 124,
	Math_0x7d_lushr = 125,
	Math_0x7e_iand = 126,
	Math_0x7f_land = 127,
	Math_0x80_ior = 128,
	Math_0x81_lor = 129,
	Math_0x82_ixor = 130,
	Math_0x83_lxor = 131,
	Math_0x84_iinc = 132,
	Conversion_0x85_i2l = 133,
	Conversion_0x86_i2f = 134,
	Conversion_0x87_i2d = 135,
	Conversion_0x88_l2i = 136,
	Conversion_0x89_l2f = 137,
	Conversion_0x8a_l2d = 138,
	Conversion_0x8b_f2i = 139,
	Conversion_0x8c_f2l = 140,
	Conversion_0x8d_f2d = 141,
	Conversion_0x8e_d2i = 142,
	Conversion_0x8f_d2l = 143,
	Conversion_0x90_d2f = 144,
	Conversion_0x91_i2b = 145,
	Conversion_0x92_i2c = 146,
	Conversion_0x93_i2s = 147,
	Comparison_0x94_lcmp = 148,
	Comparison_0x95_fcmpl = 149,
	Comparison_0x96_fcmpg = 150,
	Comparison_0x97_dcmpl = 151,
	Comparison_0x98_dcmpg = 152,
	Comparison_0x99_ifeq = 153,
	Comparison_0x9a_ifne = 154,
	Comparison_0x9b_iflt = 155,
	Comparison_0x9c_ifge = 156,
	Comparison_0x9d_ifgt = 157,
	Comparison_0x9e_ifle = 158,
	Comparison_0x9f_if_icmpeq = 159,
	Comparison_0xa0_if_icmpne = 160,
	Comparison_0xa1_if_icmplt = 161,
	Comparison_0xa2_if_icmpge = 162,
	Comparison_0xa3_if_icmpgt = 163,
	Comparison_0xa4_if_icmple = 164,
	Comparison_0xa5_if_acmpeq = 165,
	Comparison_0xa6_if_acmpne = 166,
	Control_0xa7_goto = 167,
	Control_0xa8_jsr = 168,
	Control_0xa9_ret = 169,
	Control_0xaa_tableswitch = 170,
	Control_0xab_lookupswitch = 171,
	Control_0xac_ireturn = 172,
	Control_0xad_lreturn = 173,
	Control_0xae_freturn = 174,
	Control_0xaf_dreturn = 175,
	Control_0xb0_areturn = 176,
	Control_0xb1_return = 177,
	Reference_0xb2_getstatic = 178,
	Reference_0xb3_putstatic = 179,
	Reference_0xb4_getfield = 180,
	Reference_0xb5_putfield = 181,
	Reference_0xb6_invokevirtual = 182,
	Reference_0xb7_invokespecial = 183,
	Reference_0xb8_invokestatic = 184,
	Reference_0xb9_invokeinterface = 185,
	Reference_0xba_invokedynamic = 186,
	Reference_0xbb_new = 187,
	Reference_0xbc_newarray = 188,
	Reference_0xbd_anewarray = 189,
	Reference_0xbe_arraylength = 190,
	Reference_0xbf_athrow = 191,
	Reference_0xc0_checkcast = 192,
	Reference_0xc1_instanceof = 193,
	Reference_0xc2_monitorenter = 194,
	Reference_0xc3_monitorexit = 195,
	Extended_0xc4_wide = 196,
	Extended_0xc5_multianewarray = 197,
	Extended_0xc6_ifnull = 198,
	Extended_0xc7_ifnonnull = 199,
	Extended_0xc8_goto_w = 200,
	Extended_0xc9_jsr_w = 201,
	Reserved_0xca_breakpoint = 202,
	Reserved_0xfe_impdep1 = 254,
	Reserved_0xff_impdep2 = 255,
};
struct VMClassMethod;
struct VMJavaThread;
struct VMThreadStackFrame;
struct VMHeapObject;

class VMEngine {
private:
	static int moveSteps[256];
public:
	void static execute(weak_ptr<VMJavaThread> thread, shared_ptr<VMThreadStackFrame> frame);
	void static execute(weak_ptr<VMJavaThread> thread, weak_ptr<VMClassMethod> method, vector<weak_ptr<VMHeapObject>> args);

private:
	/*这个函数在vm_engine_private.cpp里实现，免得一个文件太大。*/
	static weak_ptr<VMHeapObject> invokeNativeMethod(void* func, vector<weak_ptr<VMHeapObject>>& args);
};
#endif //__JVM_VM_ENGINE_H__