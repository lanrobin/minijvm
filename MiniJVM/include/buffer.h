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
	void resetReadPos();

	void dumpToFile(const string& path);
	void dumpToFile(const wstring& path);

	~Buffer();
private:
	void init(const char* buf, size_t offset, size_t count);

public:
	static shared_ptr<Buffer> fromFile(const string& filePath);
	static shared_ptr<Buffer> fromFile(const wstring& filePath);
};
#endif __JVM_BUFFER_READER__