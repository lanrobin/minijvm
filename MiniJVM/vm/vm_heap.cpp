#include "vm_heap.h"
#include "vm_class.h"

weak_ptr<VMHeapObject> FixSizeVMHeapPool::createVMHeapObject(const wstring &signature)
{
	throw runtime_error("Not implemented yet.");
}

shared_ptr<VMHeapPool> VMHeapPoolFactory::createVMHeapPool(weak_ptr<Configurations> conf) {
	auto cf = conf.lock();
	if (cf->useFixHeap()) {
		return make_shared< FixSizeVMHeapPool>(cf->maxHeapSize());
	}
	else {
		throw runtime_error("Exensible heap not impemented yet.");
	}
}