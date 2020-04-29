#ifndef __JVM_BUFFER_READER__
#define __JVM_BUFFER_READER__
#include "base_type.h"
#include "platform.h"
#include <iostream>

class Buffer {
private:
	u1* buffer;
	size_t buffer_size;
	size_t read_pos;
public:
	Buffer(istream& is);
	Buffer(const char* buf, size_t offset, size_t count);

	u1 readu1();
	u2 readu2();
	u4 readu4();

	u1 peaku1();
	u2 peaku2();
	u4 peaku4();

	size_t rewind(size_t count);

	size_t size() const;
	size_t pos() const;

	~Buffer();
private:
	void init(const char* buf, size_t offset, size_t count);

};
#endif __JVM_BUFFER_READER__