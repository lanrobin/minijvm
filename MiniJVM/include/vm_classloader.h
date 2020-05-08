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
	virtual wstring getClassLoaderName() const = 0;

	virtual ~ClassLoader() {};
protected:
	shared_ptr<ClassLoader> parent;

	shared_ptr<ClassLoader> getSharedPtr() { return shared_from_this(); }

	bool classLoaded(const wstring& className);
	shared_ptr<VMClass> storeClass(shared_ptr<VMClass> clz);
	shared_ptr<VMClass> getStoredClass(const wstring& className) const;
};

class BootstrapClassLoader : public ClassLoader{
public:
	BootstrapClassLoader(const wstring& bootstrapClassPath);
	shared_ptr<VMClass> loadClass(const wstring& className) override;
	shared_ptr<VMClass> loadClass(shared_ptr<Buffer> buf) override;
	wstring getClassLoaderName() const override;

private:
	wstring bootstrapClassPath;
	wstring classLoaderName;
};
#endif //__JVM_BOOTSTRAP_CLASSLOADER__