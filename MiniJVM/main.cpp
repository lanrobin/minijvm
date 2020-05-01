// MiniJVM.cpp : Defines the entry point for the application.
//
#include<iostream>
#include<fstream>
#include<string>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include"string_utils.h"
#include "clazz_reader.h"
#include "system.h"

#include "zip/zip.h"
#include "compressed_file_reader.h"
#include "vm.h"

#include "pthread.h"

void test();
void testThreads();
void testSystem();

int main(int argc, char ** argv)
{
#ifndef JVM_DEBUG
	if (argc != 3) {
		std::cout << argv[0] << " path/to/bootstrap/modules classfile" << endl;
	}
	string bootStrapModuleFolder(argv[1]);
	string classFileName(argv[2]);
#else
	string bootStrapModuleFolder("D:\\jvm\\modules");
	string classFileName("HelloWorld");
#endif
	//testThreads();
	testSystem();
	shared_ptr<VM> vm = make_shared<VM>(bootStrapModuleFolder, classFileName);
	return vm->run();
}

void test() {
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
	std::wcout << "Read from jar" << bootstrapJDKPath << ", get file:" << fileName << endl;

	auto content = jarReader->getItemContent(fileName);
	wstring winPath(fileName);
	replaceAll(winPath, L"/", L"\\");
	content->dumpToFile(L"d:\\jvm\\" + winPath);
	auto jarClass = make_shared<ClassFile>(content);
	auto buf = Buffer::fromFile(targetClassFileName);
	shared_ptr<ClassFile> sd = make_shared<ClassFile>(buf);
	std::wcout << sd->getCanonicalClassName() << endl;
}


#define NUM_THREADS 5

void* PrintHello(void* threadid) {
	long tid;
	tid = (long)threadid;
	cout << "Hello World! Thread ID, " << tid << endl;
	pthread_exit(NULL);
	return NULL;
}

void testThreads() {
	pthread_t threads[NUM_THREADS];
	int rc;
	int i;

	for (i = 0; i < NUM_THREADS; i++) {
		cout << "main() : creating thread, " << i << endl;
		rc = pthread_create(&threads[i], NULL, PrintHello, (void*)i);

		if (rc) {
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}
	pthread_exit(NULL);
}


void testSystem() {
	auto pwd = std::filesystem::current_path();
	wcout << "currentPath:" << pwd.wstring() << ", is abosulate:" << pwd.is_absolute() << endl;
}