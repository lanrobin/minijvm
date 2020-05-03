#include "vm_method_area.h"

shared_ptr<VMClass> VMExtensibleMethodArea::put(const wstring& className, shared_ptr<VMClass> clz) {
	auto existing = classes.find(className);
	if (existing != classes.end()) {
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
	return nullptr;
}

VMExtensibleMethodArea::~VMExtensibleMethodArea(){

}

shared_ptr<VMMethodArea> VMMethodAreaFactory::createMethodArea(shared_ptr<Configurations> conf)
{
	if (conf->isExtensibleMethodArea()) {
		return make_shared< VMExtensibleMethodArea>();
	}
}