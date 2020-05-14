#ifndef __JVM_BOOTSTRAP_CLASSLOADER__
#define __JVM_BOOTSTRAP_CLASSLOADER__

#include "base_type.h"
#include "vm_class.h"
#include "buffer.h"

struct VMClass;

class ClassLoader : public std::enable_shared_from_this<ClassLoader> {
public:
	virtual weak_ptr<VMClass> loadClass(const wstring& className) = 0;
	virtual weak_ptr<VMClass> loadClass(shared_ptr<Buffer> buf) = 0;

	weak_ptr<VMClass> defineClass(shared_ptr<Buffer> buf);
	// 这个用来定义数组类。
	weak_ptr<VMClass> defineClass(const wstring& className);

	virtual wstring getClassLoaderName() const = 0;
	ClassLoader(weak_ptr<ClassLoader> parent);

	virtual ~ClassLoader() {};
protected:
	weak_ptr<ClassLoader> parent;

	weak_ptr<ClassLoader> getSharedPtr() { return shared_from_this(); }

	bool classLoaded(const wstring& className);
	weak_ptr<VMClass> getStoredClass(const wstring& className) const;
	shared_ptr<VMClass> defineArrayClass(const wstring& className);
};

class BootstrapClassLoader : public ClassLoader{
public:
	BootstrapClassLoader(const wstring& bootstrapClassPath, weak_ptr<ClassLoader> parent);
	BootstrapClassLoader(weak_ptr<ClassLoader> parent);
	weak_ptr<VMClass> loadClass(const wstring& className) override;
	weak_ptr<VMClass> loadClass(shared_ptr<Buffer> buf) override;
	wstring getClassLoaderName() const override { return classLoaderName; }
protected:
	virtual wstring getClassRootPath() const { return bootstrapClassPath; }

private:
	wstring bootstrapClassPath;
	wstring classLoaderName;
};

class AppClassLoader : public BootstrapClassLoader {
public:
	AppClassLoader(const wstring& appClassPath, weak_ptr<ClassLoader> parent);
	weak_ptr<VMClass> loadClass(const wstring& className) override;
	weak_ptr<VMClass> loadClass(shared_ptr<Buffer> buf) override;

	wstring getClassRootPath() const override { return appClassRootPath; }

	wstring getClassLoaderName() const override { return classLoaderName; }
private:
	wstring appClassRootPath;
	wstring classLoaderName;
};
#endif //__JVM_BOOTSTRAP_CLASSLOADER__