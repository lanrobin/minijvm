#ifndef __JVM_PLATFORM_H__
#define __JVM_PLATFORM_H__

#define JVM_DEBUG

#ifdef _WIN32
#include <Windows.h>
#if REG_DWORD == REG_DWORD_LITTLE_ENDIAN
#define __JVM_LITTLE_ENDIAN__
#endif
#else //_WIN32
#include <endian.h>

#ifdef __LITTLE_ENDIAN
#define __JVM_LITTLE_ENDIAN__
#endif // __LITTLE_ENDIAN
#endif

#endif // __JVM_PLATFORM_H__