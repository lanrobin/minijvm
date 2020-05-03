#include "vm_method_area.h"
#include "log.h"
#include "string_utils.h"

shared_ptr<VMClass> VMExtensibleMethodArea::put(const wstring& className, shared_ptr<VMClass> clz) {
	auto existing = classes.find(className);
	if (existing != classes.end()) {
		spdlog::info("class:{}  existed in MethodArea", w2s(className));
		return existing->second;
	}
	classes[className] = clz;
	return clz;
}

shared_ptr<VMClass> VMExtensibleMethodArea::get(const wstring& className) {
	auto existing = classes.find(className);
	if (existing != classes.end()) {
		return existing->second;
	}
	spdlog::info("class: {} doesn't exist in MethodArea", w2s(className));
	return nullptr;
}

std::pair<size_t, shared_ptr<VMConstantItem>> VMExtensibleMethodArea::putConstant(const wstring& t)
{
	throw runtime_error("Not implemented yet.");
};
std::pair<size_t, shared_ptr<VMConstantItem>> VMExtensibleMethodArea::putConstantDouble(u4 high, u4 low) 
{
	throw runtime_error("Not implemented yet.");
};
std::pair<size_t, shared_ptr<VMConstantItem>> VMExtensibleMethodArea::putConstantLong(u4 high, u4 low)
{
	throw runtime_error("Not implemented yet.");
};
std::pair<size_t, shared_ptr<VMConstantItem>> VMExtensibleMethodArea::putConstant(u4 v) {
	throw runtime_error("Not implemented yet.");
};
std::pair<size_t, shared_ptr<VMConstantItem>> VMExtensibleMethodArea::putConstant(u2 v) {
	throw runtime_error("Not implemented yet.");
};
std::pair<size_t, shared_ptr<VMConstantItem>> VMExtensibleMethodArea::putConstant(u1 v) {
	throw runtime_error("Not implemented yet.");
};
std::pair<size_t, shared_ptr<VMConstantItem>> VMExtensibleMethodArea::putConstantChar(u2 v) {
	throw runtime_error("Not implemented yet.");
};
shared_ptr<VMConstantItem> VMExtensibleMethodArea::getConstant(size_t index) {
	throw runtime_error("Not implemented yet.");
};

VMExtensibleMethodArea::~VMExtensibleMethodArea(){
	spdlog::info("VMExtensibleMethodArea gone");
}

shared_ptr<VMMethodArea> VMMethodAreaFactory::createMethodArea(shared_ptr<Configurations> conf)
{
	if (conf->isExtensibleMethodArea()) {
		return make_shared< VMExtensibleMethodArea>();
	}
	return nullptr;
}