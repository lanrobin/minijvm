﻿// MiniJVM.cpp : Defines the entry point for the application.
//
#include<iostream>
#include<fstream>
#include<string>
#include"string_utils.h"
#include "clazz_reader.h"

#include "zip/zip.h"
#include "compressed_file_reader.h"

using namespace std;

int main(int argc, char ** argv)
{
	//const string fileName("D:\\github\\lanjvmtest\\main\\target\\classes\\icu\\mianshi\\main\\SynchronizedDemo.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\com\\lan\\tools\\javac\\main\\JavaCompiler.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\com\\lan\\tools\\javac\\code\\Types.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\com\\lan\\tools\\javac\\comp\\Attr.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\module-info.class");
	//const string fileName("D:\\github\\java14compiler\\target\\classes\\lan\\internal\\misc\\VM.class");
	//const string targetClassFileName("D:\\github\\java14compiler\\target\\classes\\com\\lan\\tools\\javac\\comp\\Check.class");
	const string targetClassFileName("D:\\jvm\\modules\\java.base\\classes\\java\\lang\\ClassLoader.class");
	//const string bootstrapJDKPath = "D:\\jdk-14\\jmods\\java.compiler.jmod.skip";
	const wstring bootstrapJDKPath = L"C:\\Users\\rolan\\.m2\\repository\\org\\apache\\commons\\commons-lang3\\3.1\\commons-lang3-3.1.jar";


	shared_ptr<CompressedFileReader> jarReader = make_shared<CompressedFileReader>(bootstrapJDKPath);
	auto fileName = jarReader->listItems()[5];
	wcout << "Read from jar" << bootstrapJDKPath << ", get file:" << fileName << endl;

	auto content = jarReader->getItemContent(fileName);
	wstring winPath(fileName);
	replaceAll(winPath, L"/", L"\\");
	content->dumpToFile(L"d:\\jvm\\" + winPath);
	auto jarClass = make_shared<ClassFile>(content);
	auto buf = Buffer::fromFile(targetClassFileName);
	shared_ptr<ClassFile> sd = make_shared<ClassFile>(buf);
	wcout << sd->getCanonicalClassName() << endl;
	return 0;
}