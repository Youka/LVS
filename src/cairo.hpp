#pragma once

#define CAIRO_WIN32_STATIC_BUILD
#include <cairo/cairo.h>
#include <windows.h>

// Logfont for cairo win32 font
static LOGFONTW cairo_win32_create_logfont(bool bold, bool italic, bool underline, bool strikeout, BYTE charset, const wchar_t *wface){
	// MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/dd145037%28v=vs.85%29.aspx
	LOGFONTW lf = {0};	// Logfont with default values
	lf.lfHeight = 1;	// Size doesn't matter, cairo sets it
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;	// Bold or nomal weight?
	lf.lfItalic = italic;	// Italic?
	lf.lfUnderline = underline;	// Underlined?
	lf.lfStrikeOut = strikeout;	// Strikeouted?
	lf.lfCharSet = charset;	// Which charset?
	lf.lfOutPrecision = OUT_TT_PRECIS;	// TrueType fonts preferred for output precision
	lf.lfQuality = ANTIALIASED_QUALITY;	// Font antialiased
	lf.lfFaceName[31] = L'\0';	// Make sure the fontname string ends with terminating zero
	wcsncpy(lf.lfFaceName, wface, 31);	// Copy until 31 characters of wished font face name to logfont
	return lf;	// Return finished logfont structure
}

// TODO: windows text path creator for cairo
