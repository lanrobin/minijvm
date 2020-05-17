#ifndef __JAVA_BASE_TYPE__
#define __JAVA_BASE_TYPE__

#include <cstdint>
#include <memory>
#include <vector>
#include <iostream>

using std::cout;
using std::wcout;
using std::endl;
using std::ios;
using std::hex;
using std::uppercase;

using std::string;
using std::wstring;

using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::weak_ptr;

using std::runtime_error;

using u1 = std::uint8_t;
using u2 = std::uint16_t;
using u4 = std::uint32_t;

template<typename T>
T popLast(vector<T>& vec) {
	assert(vec.size() > 0);
	T r = vec.back();
	vec.pop_back();
	return r;
}

#endif // !__JAVA_BASE_TYPE__
