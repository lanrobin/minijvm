#ifndef __JVM_ZIP_FILE_READER__
#define __JVM_ZIP_FILE_READER__
#include <iostream>
#include <vector>
#include "zip/zip.h"
using namespace std;

class CompressedFileReader {
public:
	CompressedFileReader(const string& filePath);
	CompressedFileReader(const wstring& filePath);
	bool isValidCompressedFile() const;
	vector<wstring> listItems() const;
	vector<char> getItemContent(wstring& itemName);

	~CompressedFileReader();
private:
	void init(const char* file);
private:
	struct zip_t* zip;
	vector<wstring> entries;
	wstring fileName;
	bool isValidZipFile;
};
#endif //__JVM_ZIP_FILE_READER__