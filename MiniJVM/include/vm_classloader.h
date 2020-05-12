#ifndef __JVM_BOOTSTRAP_CLASSLOADER__
#define __JVM_BOOTSTRAP_CLASSLOADER__

#include "base_type.h"
#include "vm_class.h"
#include "buffer.h"

struct VMClass;

class ClassLoader : public std::enable_shared_from_this<ClassLoader> {
public:
	virtual shared_ptr<VMClass> loadClass(const wstring& className) = 0;
	virtual shared_ptr<VMClass> loadClass(shared_ptr<Buffer> buf) = 0;

	shared_ptr<VMClass> defineClass(shared_ptr<Buffer> buf);

	// 这个用来定义数组类。
	shared_ptr<VMClass> defineClass(const wstring& className);

	virtual wstring getClassLoaderName() const = 0;
	ClassLoader(shared_ptr<ClassLoader> parent);

	virtual ~ClassLoader() {};
protected:
	shared_ptr<ClassLoader> parent;

	shared_ptr<ClassLoader> getSharedPtr() { return shared_from_this(); }

	bool classLoaded(const wstring& className);
	shared_ptr<VMClass> getStoredClass(const wstring& className) const;
};

class BootstrapClassLoader : public ClassLoader{
public:
	BootstrapClassLoader(const wstring& bootstrapClassPath, shared_ptr<ClassLoader> parent);
	BootstrapClassLoader(shared_ptr<ClassLoader> parent);
	shared_ptr<VMClass> loadClass(const wstring& className) override;
	shared_ptr<VMClass> loadClass(shared_ptr<Buffer> buf) override;
	wstring getClassLoaderName() const override { return classLoaderName; };
protected:
	virtual wstring getClassRootPath() const { return bootstrapClassPath; };

private:
	wstring bootstrapClassPath;
	wstring classLoaderName;
};

class AppClassLoader : public BootstrapClassLoader {
public:
	AppClassLoader(const wstring& appClassPath, shared_ptr<ClassLoader> parent);
	shared_ptr<VMClass> loadClass(const wstring& className) override;
	shared_ptr<VMClass> loadClass(shared_ptr<Buffer> buf) override;

	wstring getClassRootPath() const override { return appClassRootPath; };

	wstring getClassLoaderName() const override { return classLoaderName; };
private:
	wstring appClassRootPath;
	wstring classLoaderName;
};
#endif //__JVM_BOOTSTRAP_CLASSLOADER__