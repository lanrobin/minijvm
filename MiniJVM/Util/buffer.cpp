#include "buffer.h"
#include "cassert"

Buffer::Buffer(istream& is): buffer(nullptr), read_pos(0), buffer_size(0) {
	is.seekg(0, is.end);
	auto len = is.tellg();
	is.seekg(0, is.beg);
	char* data = new char[len];
	assert(data != nullptr);
	is.read(data, len);
	init(data, 0, len);
	delete[] data;
}

Buffer::Buffer(const char* data, size_t offset, size_t count) : buffer(nullptr), read_pos(0), buffer_size(0) {
	if (data != nullptr)
	{
		init(data, offset, count);
	}
}

void Buffer::init(const char* data, size_t offset, size_t count) {
	buffer_size = count;
	read_pos = 0;
	buffer = new u1[buffer_size];
	memcpy(buffer, data + offset, count);
}

size_t Buffer::size() const {
	return buffer_size;
}

size_t Buffer::pos() const {
	return read_pos;
}

size_t Buffer::rewind(size_t count) {
	assert(count >= 0 && count <= read_pos);
	read_pos -= count;
	return read_pos;
}

u1 Buffer::readu1()
{
	assert(buffer_size > read_pos);
	u1 value = *(buffer + read_pos++);
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)value << endl;
#endif
	return value;
}
u2 Buffer::readu2() {
	assert(buffer_size > read_pos + 2);
	u1 v1 = *(buffer + read_pos++);
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	u1 v2 = *(buffer + read_pos++);
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v2 << endl;
#endif
#ifdef __JVM_LITTLE_ENDIAN__
	return ((v1 << 8) | v2) & 0xFFFF;
#else // __LITTLE_ENDIAN__
	return ((v2 << 8) | v1) & 0xFFFF;
#endif // __LITTLE_ENDIAN__
}
u4 Buffer::readu4()
{
	assert(buffer_size > read_pos + 4);
	u1 v1 = *(buffer + read_pos++);
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v1 << endl;
#endif
	u1 v2 = *(buffer + read_pos++);
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v2 << endl;
#endif
	u1 v3 = *(buffer + read_pos++);
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v3 << endl;
#endif
	u4 v4 = *(buffer + read_pos++);
#ifdef DEBUG_READ_FILE_IN_BYTE
	cout << "read at:0x" << is.tellg() << " = " << setiosflags(ios::uppercase) << hex << "0x" << (int)v4 << endl;
#endif
#ifdef __JVM_LITTLE_ENDIAN__
	return ((v1 << 24) | (v2 << 16) | (v3 << 8) | v4) & 0xFFFFFFFF;
#else // __LITTLE_ENDIAN__
	return ((v4 << 24) | (v3 << 16) | (v2 << 8) | v1) & 0xFFFFFFFF;
#endif // __LITTLE_ENDIAN__
}

u1 Buffer::peaku1()
{
	u1 v = readu1();
	rewind(1);
	return v;
}
u2 Buffer::peaku2()
{
	u2 v = readu2();
	rewind(2);
	return v;
}
u4 Buffer::peaku4() {
	u4 v = readu4();
	rewind(4);
	return v;
}

Buffer::~Buffer() {
	if (buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
	buffer_size = read_pos = 0;
}