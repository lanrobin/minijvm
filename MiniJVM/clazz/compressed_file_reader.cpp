#include "platform.h"
#include "compressed_file_reader.h"
#include "zip/zip.h"
#include "base_type.h"
#include "string_utils.h"
#include <istream>
#include <vector>
#include <string>
#include <sstream>

CompressedFileReader::CompressedFileReader(const string& filePath): zip(nullptr), isValidZipFile(false)
{
	init(filePath.c_str());
}

CompressedFileReader::CompressedFileReader(const wstring& filePath):zip(nullptr), isValidZipFile(false)
{
	string path = wstringToString(filePath);
	init(path.c_str());
}

void CompressedFileReader::init(const char* file) {
	zip = zip_open(file, 0, 'r');
	int i, n = zip_total_entries(zip);
	for (i = 0; i < n; ++i) {
		zip_entry_openbyindex(zip, i);
		{
			const char* name = zip_entry_name(zip);
			int isdir = zip_entry_isdir(zip);
			if (isdir == 0) {
				entries.push_back(charsToWstring(name));
			}
		}
		zip_entry_close(zip);
	}
}

bool CompressedFileReader::isValidCompressedFile() const { 
	return false; 
}
vector<wstring> CompressedFileReader::listItems() const{
	return entries;
}
shared_ptr<Buffer> CompressedFileReader::getItemContent(wstring& itemName) {
	char* buf = NULL;
	size_t buf_size = 0;
	ssize_t readSize = 0;
	// 这个应该需要同步，否则zip不知道读的是哪个？
	if (zip) {
		string str = wstringToString(itemName);
		zip_entry_open(zip, str.c_str());
		readSize = zip_entry_read(zip, (void **)&buf, &buf_size);
		zip_entry_close(zip);
	}
	auto buffer = make_shared<Buffer>(buf, 0, readSize);
	if (buf != NULL) {
		free(buf);
	}
	return buffer;
};

CompressedFileReader::~CompressedFileReader()
{
	if (zip != NULL) {
		zip_close(zip);
	}
	zip = NULL;
};