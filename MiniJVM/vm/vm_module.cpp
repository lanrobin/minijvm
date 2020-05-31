#include "vm_classloader.h"
#include "vm_class.h"
#include "vm_module.h"
#include "log.h"

const wstring & VMModule::UNNAMED_MODULE = L"unnamed_module";

VMModule::VMModule(const wstring& mname, weak_ptr<ClassLoader> cl)
{
	name = mname;
	classLoader = cl;
}

VMModule::~VMModule() {
	spdlog::info("module:{} is gone", w2s(name));
}