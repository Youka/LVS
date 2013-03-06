#pragma once

#include <windows.h>

// Convert UTF-16 text to UTF-8 text (returns heap memory)
inline char* utf16_to_utf8(const wchar_t *wc){
    int c_len = WideCharToMultiByte(CP_UTF8, 0, wc, wcslen(wc)+1, 0, 0, 0, 0);
    char *c = new char[c_len];
    WideCharToMultiByte(CP_UTF8, 0, wc, wcslen(wc)+1, c, c_len, 0, 0);
    return c;
}

// Convert UTF-8 text to UTF-16 text (returns heap memory)
inline wchar_t* utf8_to_utf16(const char *c){
    int w_len = MultiByteToWideChar(CP_UTF8, 0, c, strlen(c)+1, 0, 0);
    wchar_t *wc = new wchar_t[w_len];
    MultiByteToWideChar(CP_UTF8, 0, c, strlen(c)+1, wc, w_len);
    return wc;
}
