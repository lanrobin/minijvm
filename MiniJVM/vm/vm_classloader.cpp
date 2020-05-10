#include "vm_classloader.h"
#include "string_utils.h"
#include "log.h"
#include "vm.h"
#include "vm_method_area.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>

ClassLoader::ClassLoader(shared_ptr<ClassLoader> p) : parent(p) {}


bool ClassLoader::classLoaded(const wstring& className) {
	//check if class has been loaded.
	auto ma = VM::getVM()->getMethodArea();
	return ma->classExists(className);
}

shared_ptr<VMClass> ClassLoader::storeClass(shared_ptr<VMClass> clz) {
	assert(clz != nullptr);

	if (classLoaded(clz->className())) {
		return clz;
	}
	auto ma = VM::getVM()->getMethodArea();
	return ma->put(clz->className(), clz);
}

shared_ptr<VMClass> ClassLoader::getStoredClass(const wstring& className) const{
	auto ma = VM::getVM()->getMethodArea();
	return ma->get(className);
}


shared_ptr<VMClass> ClassLoader::defineClass(shared_ptr<Buffer> buf) {
	auto cf = make_shared<ClassFile>(buf);
	if (cf != nullptr) {

		if (classLoaded(cf->getCanonicalClassName())) {
			spdlog::info("Class:{} had been loaded.", w2s(cf->getCanonicalClassName()));
			return getStoredClass(cf->getCanonicalClassName());
		}

		if (!cf->isJavaClassFile()) {
			throw runtime_error("Is not supported class file:" + w2s(buf->getMappingFile()));
		}
		if (!cf->isSupportedClassFile()) {
			throw runtime_error("Unsupported class file version:" + std::to_string(cf->major_version) + "." + std::to_string(cf->minor_version));
		}
		shared_ptr<VMClass> clz = nullptr;
		if (cf->isInterface()) {
			clz = make_shared<VMInterfaceClass>(cf, getSharedPtr());
		}
		else {
			clz = make_shared<VMOrdinaryClass>(cf, getSharedPtr());
		}

		// 把常量放到常量池中
		VM::getVM()->getMethodArea()->putClassConstantPool(cf, clz);

		return clz;
	}
	spdlog::error("Cannot laod class:{}", w2s(buf->getMappingFile()));

	return nullptr;
}
BootstrapClassLoader::BootstrapClassLoader(const wstring& cp, shared_ptr<ClassLoader> p):
	ClassLoader(p), bootstrapClassPath(cp), classLoaderName(L"bootstrapClassLoader") {

}

BootstrapClassLoader::BootstrapClassLoader(shared_ptr<ClassLoader> p) :
	ClassLoader(p), classLoaderName(L"bootstrapClassLoader") {

}

shared_ptr<VMClass> BootstrapClassLoader::loadClass(const wstring& className) {
	wstring canonicalClassPath(className);
	// replace . with /
	replaceAll(canonicalClassPath, L".", L"/");

	if (classLoaded(canonicalClassPath)) {
		spdlog::info("Class:{} had been loaded.", w2s(canonicalClassPath));
		return getStoredClass(canonicalClassPath);
	}

	std::filesystem::path clazz(getClassRootPath() + L"/" + canonicalClassPath + L".class");

	spdlog::info("Tried to load class {} from BootstrapClassLoader", w2s(clazz.wstring()));

	if (std::filesystem::is_regular_file(clazz) && std::filesystem::exists(clazz)) {
		auto buffer = Buffer::fromFile(clazz.wstring());
		return loadClass(buffer);
	}
	spdlog::error("Cannot laod class:{}", w2s(clazz.wstring()));
	return nullptr;
}

shared_ptr<VMClass> BootstrapClassLoader::loadClass(shared_ptr<Buffer> buf) {
	return defineClass(buf);
}

AppClassLoader::AppClassLoader(const wstring& appClassPath, shared_ptr<ClassLoader> p):
	BootstrapClassLoader(p),
	appClassRootPath(appClassPath),
	classLoaderName(L"AppClassLoader"){

}

shared_ptr<VMClass> AppClassLoader::loadClass(const wstring& className) {

	auto parentReadClass = parent->loadClass(className);
	if (parentReadClass != nullptr) {
		spdlog::info("Class:{} read from parent classloader", w2s(className));
		return parentReadClass;
	}

	wstring canonicalClassPath(className);
	// replace . with /
	replaceAll(canonicalClassPath, L".", L"/");

	if (classLoaded(canonicalClassPath)) {
		spdlog::info("Class:{} had been loaded.", w2s(canonicalClassPath));
		return getStoredClass(canonicalClassPath);
	}

	std::filesystem::path clazz(getClassRootPath() + L"/" + canonicalClassPath + L".class");

	spdlog::info("Tried to load class {} from BootstrapClassLoader", w2s(clazz.wstring()));

	if (std::filesystem::is_regular_file(clazz) && std::filesystem::exists(clazz)) {
		auto buffer = Buffer::fromFile(clazz.wstring());
		return loadClass(buffer);
	}
	spdlog::error("Cannot laod class:{}", w2s(clazz.wstring()));
	return nullptr;
}

shared_ptr<VMClass> AppClassLoader::loadClass(shared_ptr<Buffer> buf) {
	auto parentReadClass = parent->loadClass(buf);
	if (parentReadClass != nullptr) {
		spdlog::info("Class:{} read from parent classloader", w2s(buf->getMappingFile()));
		return parentReadClass;
	}
	return defineClass(buf);
}