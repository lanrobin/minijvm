// MiniJVM.cpp : Defines the entry point for the application.
//
#include<iostream>
#include<fstream>
#include<string>
#include "MiniJVM.h"
#include "clazz_reader.h"

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
	const string bootstrapJDKPath("D:\\jdk-14");
	std::ifstream clazzFile(targetClassFileName, ios::binary);
	shared_ptr<ClassFile> sd = make_shared<ClassFile>(clazzFile);
	wcout << sd->getCanonicalClassName() << endl;
	return 0;
}
