#ifndef __JVM_VM_MODULE_H__
#define __JVM_VM_MODULE_H__
#include "base_type.h"
#include <unordered_map>
#include <unordered_set>

using std::unordered_map;
using std::unordered_set;

const u2 MODULE_ACC_OPEN = 0x0020;
const u2 MODULE_ACC_SYNTHETIC = 0x0020;
const u2 MODULE_ACC_MANDATED = 0x0020;

const u2 MODULE_REQUIRE_ACC_TRANSITIVE = 0x0020;
const u2 MODULE_REQUIRE_ACC_STATIC_PHASE = 0x0040;
const u2 MODULE_REQUIRE_ACC_SYNTHETIC = 0x0100;
const u2 MODULE_REQUIRE_ACC_MANDATED = 0x8000;

const u2 MODULE_EXPORT_ACC_SYNTHETIC = 0x0100;
const u2 MODULE_EXPORT_ACC_MANDATED = 0x8000;

class ClassLoader;
struct VMClass;

struct VMModuleElementCommon {
	wstring name;
	u2 flags;
};
struct VMModuleRequire: VMModuleElementCommon {
	wstring requiredVersion;
};

struct VMModuleExport : VMModuleElementCommon {
	unordered_set<wstring> to;
};

struct VMModuleOpen : VMModuleElementCommon {
	unordered_set<wstring> to;
};

struct VMModuleProvide : VMModuleElementCommon {
	unordered_set<wstring> with;
};

class VMModule : public std::enable_shared_from_this<VMModule> {
public:
	wstring name;
	u2 flags;
	weak_ptr<ClassLoader> classLoader;
	wstring mainClassName;
	wstring version;
	// 这个Module包含的所有的包和类
	//unordered_map<wstring, weak_ptr<VMClass>> packages;
	unordered_set<wstring> packages;
	unordered_map<wstring, shared_ptr<VMModuleRequire>> requires;
	unordered_map<wstring, shared_ptr<VMModuleExport>> exports;
	unordered_map<wstring, shared_ptr<VMModuleOpen>> opens;
	unordered_set<wstring> uses;
	unordered_map<wstring, shared_ptr<VMModuleProvide>> provides;

public:
	VMModule(const wstring& mname, weak_ptr<ClassLoader> classLoader);
	virtual ~VMModule();
	bool isUnnamedModule() const { return VMModule::UNNAMED_MODULE == name; }

public:
	static const wstring & UNNAMED_MODULE;
};
#endif // __JVM_VM_MODULE_H__