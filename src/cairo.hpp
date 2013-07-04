#pragma once

// Cairo graphics
#define CAIRO_WIN32_STATIC_BUILD
#include <cairo/cairo.h>

// Windows text extents structure
struct cairo_win32_text_extents_t{
	double width, height, ascent, descent, internal_leading, external_leading;
};
// Text extents in windows
cairo_win32_text_extents_t cairo_win32_text_extents(const wchar_t *text, const wchar_t *face, int size,
													bool bold = false, bool italic = false, bool underline = false, bool strikeout = false,
													unsigned char charset = 1/*DEFAULT_CHARSET*/);

// Text path creator in windows for cairo
void cairo_win32_text_path(cairo_t *ctx, const wchar_t *text, const wchar_t *face, int size,
													bool bold = false, bool italic = false, bool underline = false, bool strikeout = false,
													unsigned char charset = 1/*DEFAULT_CHARSET*/);
