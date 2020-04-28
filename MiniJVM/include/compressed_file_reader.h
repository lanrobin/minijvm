#ifndef __JVM_JMOD_OR_JAR_FILE_READER__
#define __JVM_JMOD_OR_JAR_FILE_READER__
#include <iostream>
#include <vector>
using namespace std;

class CompressedFileReader {
public:
	CompressedFileReader(string& filePath);
	CompressedFileReader(wstring& filePath);
	bool isValidCompressedFile() const;
	vector<wstring> listItems();
	istream& getItemContent(wstring& itemName);

	~CompressedFileReader();
};
#endif //__JVM_JMOD_OR_JAR_FILE_READER__