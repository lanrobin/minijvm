// MiniJVM.cpp : Defines the entry point for the application.
//
#include<iostream>
#include<fstream>
#include<string>
#include "MiniJVM.h"
#include "clazz_reader.h"

#include "zip/zip.h"

using namespace std;

int main(int argc, char ** argv)
{
	//const string fileName("D:\\github\\lanjvmtest\\main\\target\\classes\\icu\\mianshi\\main\\SynchronizedDemo.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\com\\lan\\tools\\javac\\main\\JavaCompiler.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\com\\lan\\tools\\javac\\code\\Types.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\com\\lan\\tools\\javac\\comp\\Attr.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\module-info.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\lan\\internal\\misc\\VM.class");
	const string targetClassFileName("D:\\github\\java14compiler\\target\\classes\\com\\lan\\tools\\javac\\comp\\Check.class");
	//const string bootstrapJDKPath = "D:\\jdk-14\\jmods\\java.compiler.jmod";
	const string bootstrapJDKPath = "D:\\spring-boot-2.2.6.RELEASE.jar";


	// zip test
	struct zip_t* zip = zip_open(bootstrapJDKPath.c_str(), 0, 'r');
	int i, n = zip_total_entries(zip);
	for (i = 0; i < n; ++i) {
		zip_entry_openbyindex(zip, i);
		{
			const char* name = zip_entry_name(zip);
			cout << "item[" << i << "]:" << name << endl;
			int isdir = zip_entry_isdir(zip);
			unsigned long long size = zip_entry_size(zip);
			unsigned int crc32 = zip_entry_crc32(zip);
		}
		zip_entry_close(zip);
	}
	zip_close(zip);


	std::ifstream clazzFile(targetClassFileName, ios::binary);
	shared_ptr<ClassFile> sd = make_shared<ClassFile>(clazzFile);
	wcout << sd->getCanonicalClassName() << endl;
	return 0;
}
