#ifndef __JVM_STRINGS_H__
#define __JVM_STRINGS_H__

#include <codecvt>

std::wstring stringToWstring(const std::string& t_str)
{
    //setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}

std::wstring charsToWstring(const char * t_str)
{
    //setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}

std::string wstringToString(const std::wstring& wstr) {
    mbstate_t state;
    memset(&state, 0, sizeof state);
    auto pwstr = wstr.c_str();
    auto len = wstr.length() * 4 + 1;
    char* mbstr = new char[len];
    size_t convertedSize;
    wcsrtombs_s(&convertedSize, mbstr, len, &pwstr, wstr.length(), &state);
    string result(mbstr, 0, convertedSize);
    delete[] mbstr;
    return result;
}
#endif //__JVM_STRINGS_H__