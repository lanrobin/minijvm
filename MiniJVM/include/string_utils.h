#ifndef __JVM_STRING_UTILS_H__
#define __JVM_STRING_UTILS_H__

#include "base_type.h"
#include <string>

wstring stringToWstring(const string& t_str);

wstring charsToWstring(const char* t_str);

string wstringToString(const wstring& wstr);


/// copied from https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void replaceAll(string& str, const string& from, const string& to);
void replaceAll(wstring& str, const wstring& from, const wstring& to);
#endif //__JVM_STRING_UTILS_H__