#include "platform.h"
#include "compressed_file_reader.h"
#include <iostream>
#include <vector>

using namespace std;

CompressedFileReader::CompressedFileReader(string& filePath)
{

}

CompressedFileReader::CompressedFileReader(wstring& filePath)
{

}

bool CompressedFileReader::isValidCompressedFile() const { 
	return false; 
}
vector<wstring> CompressedFileReader::listItems() {
	throw runtime_error("Not implemented.");
}
istream& CompressedFileReader::getItemContent(wstring& itemName) {
	throw runtime_error("Not implemented.");
};

CompressedFileReader::~CompressedFileReader() {};