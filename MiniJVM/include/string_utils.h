#ifndef __JVM_STRING_UTILS_H__
#define __JVM_STRING_UTILS_H__

#include <string>

std::wstring stringToWstring(const std::string& t_str);

std::wstring charsToWstring(const char* t_str);

std::string wstringToString(const std::wstring& wstr);


/// copied from https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void replaceAll(std::string& str, const std::string& from, const std::string& to);
void replaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to);
#endif //__JVM_STRING_UTILS_H__