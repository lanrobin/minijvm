// MiniJVM.cpp : Defines the entry point for the application.
//
#include<iostream>
#include<fstream>
#include<string>
#include "MiniJVM.h"
#include "clazz_reader.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;
	const string fileName("D:\\code\\lanjvmtest\\main\\target\\classes\\icu\\mianshi\\main\\SynchronizedDemo.class");
	std::ifstream clazzFile(fileName);
	shared_ptr<ClassFile> sd = make_shared<ClassFile>(clazzFile);
	return 0;
}
