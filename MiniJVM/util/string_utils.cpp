#include "base_type.h"
#include "string_utils.h"

#include <iostream>       // cout, hex
#include <string>         // string, u32string
#include <locale>         // wstring_convert
#include <codecvt>        // codecvt_utf8
#include <cstdint>        // uint_least32_t
#include <cstdio>
#include <cstring>

#include <cwchar>

using std::codecvt_utf8;
using std::wstring_convert;

wstring stringToWstring(const string& t_str)
{
    //setup converter
    typedef codecvt_utf8<wchar_t> convert_type;
    wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}

wstring charsToWstring(const char* t_str)
{
    //setup converter
    typedef codecvt_utf8<wchar_t> convert_type;
    wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}

string wstringToString(const wstring& wstr) {
    mbstate_t state;
    memset(&state, 0, sizeof state);
    auto pwstr = wstr.c_str();
    size_t len = 1 + wcsrtombs(NULL, &pwstr, 0, &state);
    char * mbstr = new char[len];
    wcsrtombs(mbstr, &pwstr, len, &state);
    string result(mbstr, 0, len);
    delete[] mbstr;
    return result;
}


/// copied from https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void replaceAll(string& str, const string& from, const string& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void replaceAll(wstring& str, const wstring& from, const wstring& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != wstring::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}