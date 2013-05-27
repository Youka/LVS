#pragma once

// Cairo graphics
#define CAIRO_WIN32_STATIC_BUILD
#include <cairo/cairo.h>
// Windows API
#include <windows.h>

// Windows text extents structure
struct cairo_win32_text_extents_t{
	double width, height, ascent, descent, internal_leading, external_leading;
};
// Text extents in windows
static cairo_win32_text_extents_t cairo_win32_text_extents(const wchar_t *text, const wchar_t *face, int size,
													bool bold = false, bool italic = false, bool underline = false, bool strikeout = false,
													BYTE charset = DEFAULT_CHARSET){
	// Create windows context
	HDC dc = CreateCompatibleDC(NULL);
	SetMapMode(dc, MM_TEXT);	// Map for pixels
	// Create windows font (MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/dd145037%28v=vs.85%29.aspx)
	LOGFONTW lf = {0};	// Logfont with default values
	lf.lfHeight = size << 6;	// Font size (64-fold upscaled)
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;	// Bold or nomal weight?
	lf.lfItalic = italic;	// Italic?
	lf.lfUnderline = underline;	// Underlined?
	lf.lfStrikeOut = strikeout;	// Strikeouted?
	lf.lfCharSet = charset;	// Which charset?
	lf.lfOutPrecision = OUT_TT_PRECIS;	// TrueType fonts preferred for output precision
	lf.lfQuality = ANTIALIASED_QUALITY;	// Font antialiased
	lf.lfFaceName[31] = L'\0';	// Make sure the fontname string ends with terminating zero
	wcsncpy(lf.lfFaceName, face, 31);	// Copy until 31 characters of wished font face name to logfont
	HFONT font = CreateFontIndirect(&lf);
	SelectObject(dc, font);
	// Get font metrics
	TEXTMETRICW metrics = {0};
	GetTextMetricsW(dc, &metrics);
	// Get text extents
	SIZE sz = {0};
	LONG max_width = 0, height = 0;
	wchar_t *start = const_cast<wchar_t*>(text), *end = NULL;
	do{
		end = wcschr(start, L'\n');
		if(end){
			GetTextExtentPoint32W(dc, start, end-start, &sz);
			height += metrics.tmExternalLeading;
			start = end + 1;
		}else
			GetTextExtentPoint32W(dc, start, wcslen(start), &sz);
		height += sz.cy;
		max_width = max(max_width, sz.cx);
	}while(end);
	// Free windows objects
	DeleteObject(font);
	DeleteDC(dc);
	// Return text extents (64-fold downscaled)
	cairo_win32_text_extents_t text_extents = {
		static_cast<double>(max_width) / 64, static_cast<double>(height) / 64,
		static_cast<double>(metrics.tmAscent) / 64, static_cast<double>(metrics.tmDescent) / 64,
		static_cast<double>(metrics.tmInternalLeading) / 64, static_cast<double>(metrics.tmExternalLeading) / 64};
	return text_extents;
}

// Text path creator in windows for cairo
static void cairo_win32_text_path(cairo_t *ctx, const wchar_t *text, const wchar_t *face, int size,
													bool bold = false, bool italic = false, bool underline = false, bool strikeout = false,
													BYTE charset = DEFAULT_CHARSET){
	// Create windows context
	HDC dc = CreateCompatibleDC(NULL);
	SetMapMode(dc, MM_TEXT);	// Map for pixels
	SetBkMode(dc, TRANSPARENT);	// Draw text directly, not the stencil
	// Create windows font (MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/dd145037%28v=vs.85%29.aspx)
	LOGFONTW lf = {0};	// Logfont with default values
	lf.lfHeight = size << 6;	// Font size (64-fold upscaled)
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;	// Bold or nomal weight?
	lf.lfItalic = italic;	// Italic?
	lf.lfUnderline = underline;	// Underlined?
	lf.lfStrikeOut = strikeout;	// Strikeouted?
	lf.lfCharSet = charset;	// Which charset?
	lf.lfOutPrecision = OUT_TT_PRECIS;	// TrueType fonts preferred for output precision
	lf.lfQuality = ANTIALIASED_QUALITY;	// Font antialiased
	lf.lfFaceName[31] = L'\0';	// Make sure the fontname string ends with terminating zero
	wcsncpy(lf.lfFaceName, face, 31);	// Copy until 31 characters of wished font face name to logfont
	HFONT font = CreateFontIndirect(&lf);
	SelectObject(dc, font);
	// Draw text path to context
	BeginPath(dc);
	TEXTMETRICW metrics = {0};
	GetTextMetricsW(dc, &metrics);
	int y = 0;
	wchar_t *start = const_cast<wchar_t*>(text), *end = NULL;
	do{
		end = wcschr(start, L'\n');
		if(end){
			ExtTextOutW(dc, 0, y, 0x0, NULL, start, end-start, NULL);
			y += metrics.tmHeight + metrics.tmExternalLeading;
			start = end + 1;
		}else
			ExtTextOutW(dc, 0, y, 0x0, NULL, start, wcslen(start), NULL);
	}while(end);
	EndPath(dc);
	// Get context path
	POINT *points;
	BYTE *types;
	int points_n = GetPath(dc, NULL, NULL, 0);
	if(points_n > 0){
		points = new POINT[points_n];
		types = new BYTE[points_n];
		GetPath(dc, points, types, points_n);
	}
	// Free windows objects
	DeleteObject(font);
	DeleteDC(dc);
	// Convert windows path (64-fold downscaled) to cairo path
	POINT last_point = {-1,-1};
	int point_i = 0;
	while(point_i < points_n)
		switch(types[point_i]){
			case PT_MOVETO:
				cairo_close_path(ctx);
				cairo_move_to(ctx, static_cast<double>(points[point_i].x) / 64, static_cast<double>(points[point_i].y) / 64);
				last_point = points[point_i];
				point_i++;
				break;
			case PT_LINETO:
			case PT_LINETO | PT_CLOSEFIGURE:
				if(!(points[point_i].x == last_point.x && points[point_i].y == last_point.y))
					cairo_line_to(ctx, static_cast<double>(points[point_i].x) / 64, static_cast<double>(points[point_i].y) / 64);
				last_point = points[point_i];
				point_i++;
				break;
			case PT_BEZIERTO:
			case PT_BEZIERTO | PT_CLOSEFIGURE:
				if(!(points[point_i].x == last_point.x && points[point_i].y == last_point.y))
					points[point_i] = points[point_i+1];
				cairo_curve_to(ctx,
											static_cast<double>(points[point_i].x) / 64, static_cast<double>(points[point_i].y) / 64,
											static_cast<double>(points[point_i+1].x) / 64, static_cast<double>(points[point_i+1].y) / 64,
											static_cast<double>(points[point_i+2].x) / 64, static_cast<double>(points[point_i+2].y) / 64);
				last_point = points[point_i+2];
				point_i += 3;
				break;
		}
	cairo_close_path(ctx);
	// Free path resources
	if(points_n > 0){
		delete[] points;
		delete[] types;
	}
}
