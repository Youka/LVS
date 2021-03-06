#include "llibs.hpp"
#include "cairo.hpp"
#include "textconv.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include "threads.hpp"	// Multithreading
#include <xmmintrin.h>	// SSE2

// Object names
#define G2D_IMAGE "g2d image"
#define G2D_MATRIX "g2d matrix"
#define G2D_SOURCE "g2d source"
#define G2D_CONTEXT "g2d context"

// Type<->string converters
inline cairo_format_t cairo_format_from_string(const char *format_string){
	if(strcmp(format_string, "RGBA") == 0) return CAIRO_FORMAT_ARGB32;
	else if(strcmp(format_string, "RGB") == 0) return CAIRO_FORMAT_RGB24;
	else if(strcmp(format_string, "ALPHA") == 0) return CAIRO_FORMAT_A8;
	else return CAIRO_FORMAT_INVALID;
}

inline const char* cairo_format_to_string(cairo_format_t format){
	switch(format){
		case CAIRO_FORMAT_ARGB32: return "RGBA";
		case CAIRO_FORMAT_RGB24: return "RGB";
		case CAIRO_FORMAT_A8: return "ALPHA";
		default: return "UNKNOWN";
	}
}

inline cairo_extend_t cairo_extend_from_string(const char *extend_string){
	if(strcmp(extend_string, "NONE") == 0) return CAIRO_EXTEND_NONE;
	else if(strcmp(extend_string, "REPEAT") == 0) return CAIRO_EXTEND_REPEAT;
	else if(strcmp(extend_string, "MIRROR") == 0) return CAIRO_EXTEND_REFLECT;
	else if(strcmp(extend_string, "FLOW") == 0) return CAIRO_EXTEND_PAD;
	else return CAIRO_EXTEND_NONE;
}

inline const char* cairo_extend_to_string(cairo_extend_t extend){
	switch(extend){
		case CAIRO_EXTEND_NONE: return "NONE";
		case CAIRO_EXTEND_REPEAT: return "REPEAT";
		case CAIRO_EXTEND_REFLECT: return "MIRROR";
		case CAIRO_EXTEND_PAD: return "FLOW";
		default: return "UNKNOWN";
	}
}

inline cairo_filter_t cairo_filter_from_string(const char *filter_string){
	if(strcmp(filter_string, "FAST") == 0) return CAIRO_FILTER_FAST;
	else if(strcmp(filter_string, "GOOD") == 0) return CAIRO_FILTER_GOOD;
	else if(strcmp(filter_string, "BEST") == 0) return CAIRO_FILTER_BEST;
	else return CAIRO_FILTER_BEST;
}

inline const char* cairo_filter_to_string(cairo_filter_t filter){
	switch(filter){
		case CAIRO_FILTER_FAST: return "FAST";
		case CAIRO_FILTER_GOOD: return "GOOD";
		case CAIRO_FILTER_BEST: return "BEST";
		default: return "UNKNOWN";
	}
}

inline const char* cairo_pattern_type_to_string(cairo_pattern_type_t type){
	switch(type){
		case CAIRO_PATTERN_TYPE_SOLID: return "COLOR";
		case CAIRO_PATTERN_TYPE_LINEAR: return "LINEAR_GRADIENT";
		case CAIRO_PATTERN_TYPE_RADIAL: return "RADIAL_GRADIENT";
		case CAIRO_PATTERN_TYPE_MESH: return "MESH_GRADIENT";
		case  CAIRO_PATTERN_TYPE_SURFACE: return "IMAGE";
		default: return "UNKNOWN";
	}
}

inline cairo_fill_rule_t cairo_fill_rule_from_string(const char *fill_rule_string){
	if(strcmp(fill_rule_string, "WINDING") == 0) return CAIRO_FILL_RULE_WINDING;
	else if(strcmp(fill_rule_string, "ODD") == 0) return CAIRO_FILL_RULE_EVEN_ODD;
	else return CAIRO_FILL_RULE_WINDING;
}


inline const char* cairo_fill_rule_to_string(cairo_fill_rule_t fill_rule){
	switch(fill_rule){
		case CAIRO_FILL_RULE_WINDING: return "WINDING";
		case CAIRO_FILL_RULE_EVEN_ODD: return "ODD";
		default: return "UNKNOWN";
	}
}

inline cairo_line_cap_t cairo_line_cap_from_string(const char *line_cap_string){
	if(strcmp(line_cap_string, "FLAT") == 0) return CAIRO_LINE_CAP_BUTT;
	else if(strcmp(line_cap_string, "ROUND") == 0) return CAIRO_LINE_CAP_ROUND;
	else if(strcmp(line_cap_string, "SQUARE") == 0) return CAIRO_LINE_CAP_SQUARE;
	else return CAIRO_LINE_CAP_ROUND;
}

inline const char* cairo_line_cap_to_string(cairo_line_cap_t line_cap){
	switch(line_cap){
		case CAIRO_LINE_CAP_BUTT: return "FLAT";
		case CAIRO_LINE_CAP_ROUND: return "ROUND";
		case CAIRO_LINE_CAP_SQUARE: return "SQUARE";
		default: return "UNKNOWN";
	}
}

inline cairo_line_join_t cairo_line_join_from_string(const char *line_join_string){
	if(strcmp(line_join_string, "MITER") == 0) return CAIRO_LINE_JOIN_MITER;
	else if(strcmp(line_join_string, "ROUND") == 0) return CAIRO_LINE_JOIN_ROUND;
	else if(strcmp(line_join_string, "BEVEL") == 0) return CAIRO_LINE_JOIN_BEVEL;
	else return CAIRO_LINE_JOIN_ROUND;
}

inline const char* cairo_line_join_to_string(cairo_line_join_t line_join){
	switch(line_join){
		case CAIRO_LINE_JOIN_MITER: return "MITER";
		case CAIRO_LINE_JOIN_ROUND: return "ROUND";
		case CAIRO_LINE_JOIN_BEVEL: return "BEVEL";
		default: return "UNKNOWN";
	}
}

inline cairo_operator_t cairo_operator_from_string(const char *op_string){
	if(strcmp(op_string, "SOURCE") == 0) return CAIRO_OPERATOR_SOURCE;
	else if(strcmp(op_string, "OVER") == 0) return CAIRO_OPERATOR_OVER;
	else if(strcmp(op_string, "INSIDE") == 0) return CAIRO_OPERATOR_IN;
	else if(strcmp(op_string, "OUTSIDE") == 0) return CAIRO_OPERATOR_OUT;
	else if(strcmp(op_string, "MIX") == 0) return CAIRO_OPERATOR_ATOP;
	else if(strcmp(op_string, "XOR") == 0) return CAIRO_OPERATOR_XOR;
	else if(strcmp(op_string, "ADD") == 0) return CAIRO_OPERATOR_ADD;
	else if(strcmp(op_string, "SATURATE") == 0) return CAIRO_OPERATOR_SATURATE;
	else if(strcmp(op_string, "MULTIPLY") == 0) return CAIRO_OPERATOR_MULTIPLY;
	else if(strcmp(op_string, "DIFFERENCE") == 0) return CAIRO_OPERATOR_DIFFERENCE;
	else return CAIRO_OPERATOR_OVER;
}

inline const char* cairo_operator_to_string(cairo_operator_t op){
	switch(op){
		case CAIRO_OPERATOR_SOURCE: return "SOURCE";
		case CAIRO_OPERATOR_OVER: return "OVER";
		case CAIRO_OPERATOR_IN: return "INSIDE";
		case CAIRO_OPERATOR_OUT: return "OUTSIDE";
		case CAIRO_OPERATOR_ATOP: return "MIX";
		case CAIRO_OPERATOR_XOR: return "XOR";
		case CAIRO_OPERATOR_ADD: return "ADD";
		case CAIRO_OPERATOR_SATURATE: return "SATURATE";
		case CAIRO_OPERATOR_MULTIPLY: return "MULTIPLY";
		case CAIRO_OPERATOR_DIFFERENCE: return "DIFFERENCE";
		default: return "UNKNOWN";
	}
}

// G2D LIBRARY
LUA_FUNC_1ARG(create_image, 3)
	// Get parameters
	cairo_format_t format = cairo_format_from_string(luaL_checkstring(L, 1));
	int width = ::ceil(luaL_checknumber(L, 2));
	int height = ::ceil(luaL_checknumber(L, 3));
	// Create image
	cairo_surface_t *surface = cairo_image_surface_create(format, width, height);
	cairo_status_t status = cairo_surface_status(surface);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_surface_destroy(surface);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = surface;
	return 1;
LUA_FUNC_END

cairo_status_t png_stream_reader(void *closure, unsigned char *data, unsigned int length){
	FILE *file = (FILE*)closure;
	if(fread(data, 1, length, file) == length)
		return CAIRO_STATUS_SUCCESS;
	else
		return CAIRO_STATUS_READ_ERROR;
}
LUA_FUNC_1ARG(create_image_from_png, 1)
	// Get parameter
	std::wstring filename = utf8_to_utf16(luaL_checkstring(L, 1));
	// Create image from png
	FILE *file = _wfopen(filename.c_str(), L"rb");
	if(!file)
		luaL_error2(L, "file not found");
	cairo_surface_t *surface = cairo_image_surface_create_from_png_stream(png_stream_reader, file);
	fclose(file);
	cairo_status_t status = cairo_surface_status(surface);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_surface_destroy(surface);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = surface;
	return 1;
LUA_FUNC_END

cairo_status_t png_stream_writer(void *closure, const unsigned char *data, unsigned int length){
	FILE *file = (FILE*)closure;
	if(fwrite(data, 1, length, file) == length)
		return CAIRO_STATUS_SUCCESS;
	else
		return CAIRO_STATUS_WRITE_ERROR;
}
LUA_FUNC_1ARG(create_png_from_image, 2)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	std::wstring filename = utf8_to_utf16(luaL_checkstring(L, 2));
	// Create png from image
	FILE *file = _wfopen(filename.c_str(), L"wb");
	if(!file)
		luaL_error2(L, "couldn't create file");
	cairo_status_t status = cairo_surface_write_to_png_stream(surface, png_stream_writer, file);
	fclose(file);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_2ARG(create_matrix, 0, 6)
	if(lua_gettop(L) == 0){
		// Create matrix
		cairo_matrix_t matrix = {1, 0, 0, 1, 0, 0};
		// Push matrix to Lua
		*lua_createuserdata<cairo_matrix_t>(L, G2D_MATRIX) = matrix;
	}else if(lua_gettop(L) == 6){
		// Create matrix
		cairo_matrix_t matrix = {luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6)};
		// Push matrix to Lua
		*lua_createuserdata<cairo_matrix_t>(L, G2D_MATRIX) = matrix;
	}
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(create_color, 3, 4)
	// Get parameters
	double r = luaL_checknumber(L, 1);
	double g = luaL_checknumber(L, 2);
	double b = luaL_checknumber(L, 3);
	double a = luaL_optnumber(L, 4, 1.0L);
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_rgba(r, g, b, a);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_lgradient, 4)
	// Get parameters
	double x0 = luaL_checknumber(L, 1);
	double y0 = luaL_checknumber(L, 2);
	double x1 = luaL_checknumber(L, 3);
	double y1 = luaL_checknumber(L, 4);
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_linear(x0, y0, x1, y1);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_rgradient, 6)
	// Get parameters
	double cx0 = luaL_checknumber(L, 1);
	double cy0 = luaL_checknumber(L, 2);
	double r0 = luaL_checknumber(L, 3);
	double cx1 = luaL_checknumber(L, 4);
	double cy1 = luaL_checknumber(L, 5);
	double r1 = luaL_checknumber(L, 6);
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_radial(cx0, cy0, r0, cx1, cy1, r1);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_mgradient, 0)
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_mesh();
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_pattern, 1)
	// Get parameter
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(register_font, 1)
	std::wstring fontname = utf8_to_utf16(luaL_checkstring(L, 1));
	lua_pushboolean(L, cairo_win32_register_font(fontname.c_str()));
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(text_extents, 3, 7)
	// Get parameters
	std::wstring text = utf8_to_utf16(luaL_checkstring(L, 1));
	std::wstring face = utf8_to_utf16(luaL_checkstring(L, 2));
	int size = luaL_checknumber(L, 3);
	bool bold = luaL_optboolean(L, 4, false);
	bool italic = luaL_optboolean(L, 5, false);
	bool underline = luaL_optboolean(L, 6, false);
	bool strikeout = luaL_optboolean(L, 7, false);
	// Get text extents
	cairo_win32_text_extents_t extents = cairo_win32_text_extents(text.c_str(), face.c_str(), size, bold, italic, underline, strikeout);
	// Push text extents to Lua
	lua_pushnumber(L, extents.width);
	lua_pushnumber(L, extents.height);
	lua_pushnumber(L, extents.ascent);
	lua_pushnumber(L, extents.descent);
	lua_pushnumber(L, extents.internal_leading);
	lua_pushnumber(L, extents.external_leading);
	return 6;
LUA_FUNC_END

// IMAGE OBJECT
LUA_FUNC_1ARG(image_gc, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	cairo_surface_destroy(surface);
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_width, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	lua_pushnumber(L, cairo_image_surface_get_width(surface));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_height, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	lua_pushnumber(L, cairo_image_surface_get_height(surface));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_format, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	lua_pushstring(L, cairo_format_to_string(cairo_image_surface_get_format(surface)));
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(image_get_data, 1, 5)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	int x0, y0, x1, y1;
	if(lua_gettop(L) == 1)
		x0 = 0, y0 = 0, x1 = cairo_image_surface_get_width(surface), y1 = cairo_image_surface_get_height(surface);
	else{
		x0 = luaL_checknumber(L, 2), y0 = luaL_checknumber(L, 3), x1 = luaL_checknumber(L, 4), y1 = luaL_checknumber(L, 5);
		if(x0 < 0 || y0 < 0 ||
			x1 <= x0 || y1 <= y0 ||
			x1 > cairo_image_surface_get_width(surface) || y1 > cairo_image_surface_get_height(surface))
			luaL_error2(L, "invalid area");
	}
	// Get image data
	cairo_format_t image_format = cairo_image_surface_get_format(surface);
	int image_stride = cairo_image_surface_get_stride(surface);
	cairo_surface_flush(surface);
	register unsigned char *image_data = cairo_image_surface_get_data(surface);
	// Further area data
	int area_width = x1-x0;
	int area_height = y1-y0;
	// Create & return image data table of image area
	switch(image_format){
		case CAIRO_FORMAT_ARGB32:{
			lua_createtable(L, area_width * area_height << 2, 0);
			int table_index = 0;
			unsigned char *row;
			for(int y = y0; y < y1; ++y){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; ++x){
					lua_pushnumber(L, row[2]); lua_rawseti(L, -2, ++table_index);	// R
					lua_pushnumber(L, row[1]); lua_rawseti(L, -2, ++table_index);	// G
					lua_pushnumber(L, row[0]); lua_rawseti(L, -2, ++table_index);	// B
					lua_pushnumber(L, row[3]); lua_rawseti(L, -2, ++table_index);	// A
					row += 4;
				}
			}
			return 1;
		}break;
		case CAIRO_FORMAT_RGB24:{
			lua_createtable(L, area_width * area_height * 3, 0);
			int table_index = 0;
			unsigned char *row;
			for(int y = y0; y < y1; ++y){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; ++x){
					lua_pushnumber(L, row[2]); lua_rawseti(L, -2, ++table_index);	// R
					lua_pushnumber(L, row[1]); lua_rawseti(L, -2, ++table_index);	// G
					lua_pushnumber(L, row[0]); lua_rawseti(L, -2, ++table_index);	// B
					row += 4;
				}
			}
			return 1;
		}break;
		case CAIRO_FORMAT_A8:{
			lua_createtable(L, area_width * area_height, 0);
			int table_index = 0;
			unsigned char *row;
			for(int y = y0; y < y1; ++y){
				row = image_data + y * image_stride + x0;
				for(int x = 0; x < area_width; ++x){
					lua_pushnumber(L, *row++); lua_rawseti(L, -2, ++table_index);	// A
				}
			}
			return 1;
		}break;
	}
LUA_FUNC_END

LUA_FUNC_2ARG(image_set_data, 2, 6)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	int x0, y0, x1, y1;
	std::vector<unsigned char> new_data_vec;
	if(lua_gettop(L) == 2){
		x0 = 0, y0 = 0, x1 = cairo_image_surface_get_width(surface), y1 = cairo_image_surface_get_height(surface);
		new_data_vec = luaL_checktable<unsigned char>(L, 2);
	}else{
		x0 = luaL_checknumber(L, 2), y0 = luaL_checknumber(L, 3), x1 = luaL_checknumber(L, 4), y1 = luaL_checknumber(L, 5);
		if(x0 < 0 || y0 < 0 ||
			x1 <= x0 || y1 <= y0 ||
			x1 > cairo_image_surface_get_width(surface) || y1 > cairo_image_surface_get_height(surface))
			luaL_error2(L, "invalid area");
		new_data_vec = luaL_checktable<unsigned char>(L, 6);
	}
	size_t new_data_size = new_data_vec.size();
	unsigned char *new_data = new_data_size > 0 ? &new_data_vec[0] : 0;
	// Get image data
	cairo_format_t image_format = cairo_image_surface_get_format(surface);
	int image_stride = cairo_image_surface_get_stride(surface);
	cairo_surface_flush(surface);
	register unsigned char *image_data = cairo_image_surface_get_data(surface);
	// Further area data
	int area_width = x1-x0;
	int area_height = y1-y0;
	// Set image data table to image area
	switch(image_format){
		case CAIRO_FORMAT_ARGB32:{
			if(area_width * area_height << 2 != new_data_size)
				luaL_error2(L, "wrong table size");
			unsigned char *row;
			for(int y = y0; y < y1; ++y){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; ++x){
					row[2] = *new_data++;	// R
					row[1] = *new_data++;	// G
					row[0] = *new_data++;	// B
					row[3] = *new_data++;	// A
					row += 4;
				}
			}
		}break;
		case CAIRO_FORMAT_RGB24:{
			if(area_width * area_height * 3 != new_data_size)
				luaL_error2(L, "wrong table size");
			unsigned char *row;
			for(int y = y0; y < y1; ++y){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; ++x){
					row[2] = *new_data++;	// R
					row[1] = *new_data++;	// G
					row[0] = *new_data++;	// B
					row += 4;
				}
			}
		}break;
		case CAIRO_FORMAT_A8:{
			if(area_width * area_height != new_data_size)
				luaL_error2(L, "wrong table size");
			unsigned char *row;
			for(int y = y0; y < y1; ++y){
				row = image_data + y * image_stride + x0;
				for(int x = 0; x < area_width; ++x)
					*row++ = *new_data++;	// A
			}
		}break;
	}
	// Set image data as dirty
	cairo_surface_mark_dirty_rectangle(surface, x0, y0, area_width, area_height);
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_context, 1)
	// Get parameter
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	// Create context
	cairo_t *ctx = cairo_create(surface);
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_destroy(ctx);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Set defaults
	cairo_set_line_join(ctx, CAIRO_LINE_JOIN_ROUND);
	cairo_set_line_cap(ctx, CAIRO_LINE_CAP_ROUND);
	// Push context to Lua
	*lua_createuserdata<cairo_t*>(L, G2D_CONTEXT) = ctx;
	return 1;
LUA_FUNC_END

// Thread data for function below
struct THREAD_DATA_COLOR_TRANSFORM{
	// Images
	int image_width, image_height, image_stride, image_voffset, image_vstride;
	float *image_src;
	unsigned char *image_dst;
	// Matrix
	float *matrix;
};
// Thread function for cairo image surface (ARGB32+RGB24) color transformation
static DWORD WINAPI cairo_image_surface_color_transform(void *userdata){
	THREAD_DATA_COLOR_TRANSFORM *thread_data = reinterpret_cast<THREAD_DATA_COLOR_TRANSFORM*>(userdata);
	// Create SSE2 matrix columns
	__m128 mat_c0 = _mm_loadu_ps(thread_data->matrix);
	__m128 mat_c1 = _mm_loadu_ps(thread_data->matrix + 4);
	__m128 mat_c2 = _mm_loadu_ps(thread_data->matrix + 8);
	__m128 mat_c3 = _mm_loadu_ps(thread_data->matrix + 12);
	// Process pixels
	register float *row_src; register unsigned char *row_dst;
	__declspec(align(16)) float dst_buf[4];
	for(register int y = thread_data->image_voffset; y < thread_data->image_height; y += thread_data->image_vstride){
		row_src = thread_data->image_src + y * thread_data->image_stride;
		row_dst = thread_data->image_dst + y * thread_data->image_stride;
		for(register int x = 0; x < thread_data->image_width; ++x){
			_mm_store_ps(
				dst_buf,
				_mm_max_ps(
					_mm_min_ps(
						_mm_add_ps(
							_mm_add_ps(
								_mm_mul_ps(_mm_set_ps1(row_src[2]), mat_c0),	// R
								_mm_mul_ps(_mm_set_ps1(row_src[1]), mat_c1)		// G
							),
							_mm_add_ps(
								_mm_mul_ps(_mm_set_ps1(row_src[0]), mat_c2),	// B
								_mm_mul_ps(_mm_set_ps1(row_src[3]), mat_c3)		// A
							)
						),
						_mm_set_ps1(255)
					),
					_mm_setzero_ps()
				)
			);
			row_dst[0] = dst_buf[2];
			row_dst[1] = dst_buf[1];
			row_dst[2] = dst_buf[0];
			row_dst[3] = dst_buf[3];
			row_src += 4;
			row_dst += 4;
		}
	}
	return 0;
}
LUA_FUNC_1ARG(image_color_transform, 2)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	std::vector<float> matrix = luaL_checktable<float>(L, 2);
	if(matrix.size() != 16)
		luaL_error2(L, "table size must be 16");
	// Get image data
	cairo_format_t image_format = cairo_image_surface_get_format(surface);
	if(image_format != CAIRO_FORMAT_ARGB32 && image_format != CAIRO_FORMAT_RGB24)
		luaL_error2(L, "colorspace must be RGBA or RGB");
	int image_width = cairo_image_surface_get_width(surface);
	int image_height = cairo_image_surface_get_height(surface);
	int image_stride = cairo_image_surface_get_stride(surface);
	cairo_surface_flush(surface);
	unsigned char *image_data = cairo_image_surface_get_data(surface);
	// Image data copy (use copy as source, original as destination)
	unsigned long image_data_size = image_height * image_stride;
	std::vector<float> image_data_copy(image_data_size);
	for(unsigned long int i = 0; i < image_data_size; ++i)
		image_data_copy[i] = image_data[i];
	// Threading data
	static Threads<THREAD_DATA_COLOR_TRANSFORM> threads(cairo_image_surface_color_transform);
	static const DWORD passes =  threads.size();
	for(DWORD i = 0; i < passes; ++i){
		// Set current thread data
		THREAD_DATA_COLOR_TRANSFORM data = {image_width, image_height, image_stride, i, passes, &image_data_copy[0], image_data, &matrix[0]};
		*threads.get(i) = data;
	}
	// Apply convolution filter to image in multiple threads
	threads.Run();
	// Set image data as dirty
	cairo_surface_mark_dirty(surface);
	// Return image
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

// Thread data for function below
struct THREAD_DATA_CONVOLUTION{
	// Images
	int image_width, image_height, image_stride, image_voffset, image_vstride;
	cairo_format_t image_format;
	float *image_src;
	unsigned char *image_dst;
	// Filter
	int filter_width, filter_height;
	float *filter_kernel;
};
// Thread function for cairo image surface (ARGB32+RGB24+A8) convolution
static DWORD WINAPI cairo_image_surface_convolution(void *userdata){
	THREAD_DATA_CONVOLUTION *thread_data = reinterpret_cast<THREAD_DATA_CONVOLUTION*>(userdata);
	// RGB(A) or A8?
	if(thread_data->image_format == CAIRO_FORMAT_ARGB32 || thread_data->image_format == CAIRO_FORMAT_RGB24){
		// Storages for pixel processing
		unsigned char *row_dst;
		int image_x, image_y;
		__declspec(align(16)) float dst_buf[4];
		// Iterate through source image pixels
		for(register int y = thread_data->image_voffset; y < thread_data->image_height; y += thread_data->image_vstride){
			row_dst = thread_data->image_dst + y * thread_data->image_stride;
			for(register int x = 0; x < thread_data->image_width; ++x){
				// Accumulate pixels by filter rule
				__m128 accum = _mm_setzero_ps();
				for(int yy = 0; yy < thread_data->filter_height; ++yy){
					image_y = y - (thread_data->filter_height >> 1) + yy;
					if(image_y < 0 || image_y >= thread_data->image_height)
						continue;
					for(int xx = 0; xx < thread_data->filter_width; ++xx){
						image_x = x - (thread_data->filter_width >> 1) + xx;
						if(image_x < 0 || image_x >= thread_data->image_width)
							continue;
						accum = _mm_add_ps(
							accum,
							_mm_mul_ps(
								_mm_loadu_ps(thread_data->image_src + image_y * thread_data->image_stride + (image_x << 2)),
								_mm_set_ps1(thread_data->filter_kernel[yy * thread_data->filter_width + xx])
							)
						);
					}
				}
				// Trim and set accumulator to destination image pixel
				_mm_store_ps(
					dst_buf,
					_mm_max_ps(
						_mm_setzero_ps(),
						_mm_min_ps(
							_mm_set_ps1(255),
							accum
						)
					)
				);
				row_dst[0] = dst_buf[0];
				row_dst[1] = dst_buf[1];
				row_dst[2] = dst_buf[2];
				row_dst[3] = dst_buf[3];
				row_dst += 4;
			}
		}
	}else{	// CAIRO_FORMAT_A8
		// Storages for pixel processing
		unsigned char *row_dst;
		float accum;
		int image_x, image_y;
		// Iterate through source image pixels
		for(register int y = thread_data->image_voffset; y < thread_data->image_height; y += thread_data->image_vstride){
			row_dst = thread_data->image_dst + y * thread_data->image_stride;
			for(register int x = 0; x < thread_data->image_width; ++x){
				// Accumulate pixels by filter rule
				accum = 0;
				for(int yy = 0; yy < thread_data->filter_height; ++yy){
					image_y = y - (thread_data->filter_height >> 1) + yy;
					if(image_y < 0 || image_y >= thread_data->image_height)
						continue;
					for(int xx = 0; xx < thread_data->filter_width; ++xx){
						image_x = x - (thread_data->filter_width >> 1) + xx;
						if(image_x < 0 || image_x >= thread_data->image_width)
							continue;
						accum += thread_data->image_src[image_y * thread_data->image_stride + image_x] * thread_data->filter_kernel[yy * thread_data->filter_width + xx];
					}
				}
				// Set accumulator to destination image pixel
				*row_dst++ = accum > 255.0f ? 255.0f : (accum < 0.0f ? 0.0f : accum);
			}
		}
	}
	return 0;
}
LUA_FUNC_1ARG(image_convolute, 2)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	std::vector<float> convolution_filter = luaL_checktable<float>(L, 2);
	lua_getfield(L, 2, "width"); if(!lua_isnumber(L, -1)) luaL_error2(L, "table needs a valid field 'width'");
	int filter_width = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 2, "height"); if(!lua_isnumber(L, -1)) luaL_error2(L, "table needs a valid field 'height'");
	int filter_height = lua_tonumber(L, -1);
	lua_pop(L, 1);
	if(filter_width < 1 || !filter_width&0x1 || filter_height < 1 || !filter_height&0x1)
		luaL_error2(L, "table field(s) invalid");
	else if(filter_width * filter_height != convolution_filter.size())
		luaL_error2(L, "table fields and elements number don't fit");
	// Get image data
	cairo_format_t image_format = cairo_image_surface_get_format(surface);
	int image_width = cairo_image_surface_get_width(surface);
	int image_height = cairo_image_surface_get_height(surface);
	int image_stride = cairo_image_surface_get_stride(surface);
	cairo_surface_flush(surface);
	unsigned char *image_data = cairo_image_surface_get_data(surface);
	// Image data copy (use copy as source, original as destination)
	unsigned long image_data_size = image_height * image_stride;
	std::vector<float> image_data_copy(image_data_size);
	for(unsigned long int i = 0; i < image_data_size; ++i)
		image_data_copy[i] = image_data[i];
	// Threading data
	static Threads<THREAD_DATA_CONVOLUTION> threads(cairo_image_surface_convolution);
	static const DWORD passes =  threads.size();
	for(DWORD i = 0; i < passes; ++i){
		// Set current thread data
		THREAD_DATA_CONVOLUTION data = {image_width, image_height, image_stride, i, passes, image_format, &image_data_copy[0], image_data, filter_width, filter_height, &convolution_filter[0]};
		*threads.get(i) = data;
	}
	// Apply convolution filter to image in multiple threads
	threads.Run();
	// Set image data as dirty
	cairo_surface_mark_dirty(surface);
	// Return image
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(image_invert, 1)
	// Get parameter
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	// Create context
	cairo_t *ctx = cairo_create(surface);
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_destroy(ctx);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Invert
	cairo_set_operator(ctx, CAIRO_OPERATOR_DIFFERENCE);
	cairo_set_source_rgba(ctx, 1, 1, 1, 1);
	cairo_paint(ctx);
	// Destroy context
	cairo_destroy(ctx);
	// Return image
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END


// MATRIX OBJECT
LUA_FUNC_1ARG(matrix_get_data, 1)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	lua_pushnumber(L, matrix->xx);
	lua_pushnumber(L, matrix->yx);
	lua_pushnumber(L, matrix->xy);
	lua_pushnumber(L, matrix->yy);
	lua_pushnumber(L, matrix->x0);
	lua_pushnumber(L, matrix->y0);
	return 6;
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_set_data, 7)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	matrix->xx = luaL_checknumber(L, 2);
	matrix->yx = luaL_checknumber(L, 3);
	matrix->xy = luaL_checknumber(L, 4);
	matrix->yy = luaL_checknumber(L, 5);
	matrix->x0 = luaL_checknumber(L, 6);
	matrix->y0 = luaL_checknumber(L, 7);
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_identity, 1)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_init_identity(matrix);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_invert, 1)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_status_t status = cairo_matrix_invert(matrix);
	if(status != CAIRO_STATUS_SUCCESS)
				luaL_error2(L, cairo_status_to_string(status));
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(matrix_multiply, 2, 7)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	if(lua_gettop(L) == 2){
		cairo_matrix_t *a = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 2, G2D_MATRIX));
		cairo_matrix_t b = *matrix;
		cairo_matrix_multiply(matrix, a, &b);
	}else{
		cairo_matrix_t a = {luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6), luaL_checknumber(L, 7)};
		cairo_matrix_t b = *matrix;
		cairo_matrix_multiply(matrix, &a, &b);
	}
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_translate, 3)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_translate(matrix, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_scale, 3)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_scale(matrix, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_rotate, 2)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_rotate(matrix, luaL_checknumber(L, 2) / 180.0L * M_PI);
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_shear, 3)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_t a = {1, luaL_checknumber(L, 3), luaL_checknumber(L, 2), 1, 0, 0};
	cairo_matrix_t b = *matrix;
	cairo_matrix_multiply(matrix, &a, &b);
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_transform, 3)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_matrix_transform_point(matrix, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
LUA_FUNC_END

// SOURCE OBJECT
LUA_FUNC_1ARG(source_gc, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_destroy(pattern);
LUA_FUNC_END

LUA_FUNC_2ARG(source_add_color, 5, 6)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_add_color_stop_rgba(pattern, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_optnumber(L, 6, 1.0L));
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(source_get_color, 1, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	// Get color of source color
	if(lua_gettop(L) == 1){
		double r, g, b, a;
		cairo_status_t status = cairo_pattern_get_rgba(pattern, &r, &g, &b, &a);
		if(status != CAIRO_STATUS_SUCCESS)
			luaL_error2(L, cairo_status_to_string(status));
		lua_pushnumber(L, r);
		lua_pushnumber(L, g);
		lua_pushnumber(L, b);
		lua_pushnumber(L, a);
		return 4;
	// Get color of source linear gradient or source radial gradient
	}else{
		double pct, r, g, b, a;
		cairo_status_t status = cairo_pattern_get_color_stop_rgba(pattern, luaL_checknumber(L, 2), &pct, &r, &g, &b, &a);
		if(status == CAIRO_STATUS_INVALID_INDEX)
			return 0;
		else if(status != CAIRO_STATUS_SUCCESS)
			luaL_error2(L, cairo_status_to_string(status));
		lua_pushnumber(L, pct);
		lua_pushnumber(L, r);
		lua_pushnumber(L, g);
		lua_pushnumber(L, b);
		lua_pushnumber(L, a);
		return 5;
	}
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_points, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	double x0, y0, x1, y1;
	cairo_status_t status = cairo_pattern_get_linear_points(pattern, &x0, &y0, &x1, &y1);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	lua_pushnumber(L, x0);
	lua_pushnumber(L, y0);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	return 4;
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_circles, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	double x0, y0, r0, x1, y1, r1;
	cairo_status_t status = cairo_pattern_get_radial_circles(pattern, &x0, &y0, &r0, &x1, &y1, &r1);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	lua_pushnumber(L, x0);
	lua_pushnumber(L, y0);
	lua_pushnumber(L, r0);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, r1);
	return 6;
LUA_FUNC_END

LUA_FUNC_2ARG(source_add_mesh, 19, 25)
	// Get parameters
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	double x0 = luaL_checknumber(L, 2), y0 = luaL_checknumber(L, 3), r0 = luaL_checknumber(L, 4), g0 = luaL_checknumber(L, 5), b0 = luaL_checknumber(L, 6), a0 = luaL_checknumber(L, 7),
	x1 = luaL_checknumber(L, 8), y1 = luaL_checknumber(L, 9), r1 = luaL_checknumber(L, 10), g1 = luaL_checknumber(L, 11), b1 = luaL_checknumber(L, 12), a1 = luaL_checknumber(L, 13),
	x2 = luaL_checknumber(L, 14), y2 = luaL_checknumber(L, 15), r2 = luaL_checknumber(L, 16), g2 = luaL_checknumber(L, 17), b2 = luaL_checknumber(L, 18), a2 = luaL_checknumber(L, 19),
	x3 = luaL_optnumber(L, 20, x0), y3 = luaL_optnumber(L, 21, y0), r3 = luaL_optnumber(L, 22, r0), g3 = luaL_optnumber(L, 23, g0), b3 = luaL_optnumber(L, 24, b0), a3 = luaL_optnumber(L, 25, a0);
	// Add patch to pattern
	cairo_mesh_pattern_begin_patch(pattern);
	cairo_mesh_pattern_move_to(pattern, x0, y0);
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 0, r0, g0, b0, a0);
	cairo_mesh_pattern_line_to(pattern, x1, y1);
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 1, r1, g1, b1, a1);
	cairo_mesh_pattern_line_to(pattern, x2, y2);
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 2, r2, g2, b2, a2);
	cairo_mesh_pattern_line_to(pattern, x3, y3);
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 3, r3, g3, b3, a3);
	cairo_mesh_pattern_end_patch(pattern);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_mesh, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	unsigned int index = luaL_checknumber(L, 2);
	// Get mesh points
	cairo_path_t *path = cairo_mesh_pattern_get_path(pattern, index);
	if(path->status == CAIRO_STATUS_INVALID_INDEX)
		return 0;
	else if(path->status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(path->status));
	// Get mesh colors
	double r0, g0, b0, a0, r1, g1, b1, a1, r2, g2, b2, a2, r3, g3, b3, a3;
	cairo_mesh_pattern_get_corner_color_rgba(pattern, index, 0, &r0, &g0, &b0, &a0);
	cairo_mesh_pattern_get_corner_color_rgba(pattern, index, 1, &r1, &g1, &b1, &a1);
	cairo_mesh_pattern_get_corner_color_rgba(pattern, index, 2, &r2, &g2, &b2, &a2);
	cairo_mesh_pattern_get_corner_color_rgba(pattern, index, 3, &r3, &g3, &b3, &a3);
	// Push points and colors to Lua
	lua_pushnumber(L, path->data[1].point.x);
	lua_pushnumber(L, path->data[1].point.y);
	lua_pushnumber(L, r0);
	lua_pushnumber(L, g0);
	lua_pushnumber(L, b0);
	lua_pushnumber(L, a0);
	lua_pushnumber(L, path->data[5].point.x);
	lua_pushnumber(L, path->data[5].point.y);
	lua_pushnumber(L, r1);
	lua_pushnumber(L, g1);
	lua_pushnumber(L, b1);
	lua_pushnumber(L, a1);
	lua_pushnumber(L, path->data[9].point.x);
	lua_pushnumber(L, path->data[9].point.y);
	lua_pushnumber(L, r2);
	lua_pushnumber(L, g2);
	lua_pushnumber(L, b2);
	lua_pushnumber(L, a2);
	lua_pushnumber(L, path->data[13].point.x);
	lua_pushnumber(L, path->data[13].point.y);
	lua_pushnumber(L, r3);
	lua_pushnumber(L, g3);
	lua_pushnumber(L, b3);
	lua_pushnumber(L, a3);
	return 24;
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_image, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_surface_t *surface;
	cairo_status_t status = cairo_pattern_get_surface(pattern, &surface);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = cairo_surface_reference(surface);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_extender, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	lua_pushstring(L, cairo_extend_to_string(cairo_pattern_get_extend(pattern)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_set_extender, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_set_extend(pattern, cairo_extend_from_string(luaL_checkstring(L, 2)));
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_filter, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	lua_pushstring(L, cairo_filter_to_string(cairo_pattern_get_filter(pattern)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_set_filter, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_set_filter(pattern, cairo_filter_from_string(luaL_checkstring(L, 2)));
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_matrix, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	// Get matrix
	cairo_matrix_t matrix;
	cairo_pattern_get_matrix(pattern, &matrix);
	// Invert matrix for pattern space to user space (see 'cairo_pattern_set_matrix' description)
	cairo_status_t status = cairo_matrix_invert(&matrix);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	// Push matrix to Lua
	*lua_createuserdata<cairo_matrix_t>(L, G2D_MATRIX) = matrix;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_set_matrix, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 2, G2D_MATRIX));
	// Create inverted matrix for pattern space to user space (see 'cairo_pattern_set_matrix' description)
	cairo_matrix_t inverted_matrix = *matrix;
	cairo_status_t status = cairo_matrix_invert(&inverted_matrix);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	// Set matrix to pattern
	cairo_pattern_set_matrix(pattern, &inverted_matrix);
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_type, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	lua_pushstring(L, cairo_pattern_type_to_string(cairo_pattern_get_type(pattern)));
	return 1;
LUA_FUNC_END

// CONTEXT OBJECT
LUA_FUNC_1ARG(context_gc, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_destroy(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_matrix, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_matrix_t matrix;
	cairo_get_matrix(ctx, &matrix);
	*lua_createuserdata<cairo_matrix_t>(L, G2D_MATRIX) = matrix;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_matrix, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 2, G2D_MATRIX));
	cairo_set_matrix(ctx, matrix);
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_source, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_pattern_t *pattern = cairo_get_source(ctx);
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = cairo_pattern_reference(pattern);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_source, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 2, G2D_SOURCE));
	cairo_set_source(ctx, pattern);
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(context_push_state, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_save(ctx);
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(context_pop_state, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_restore(ctx);
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_antialiasing, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushboolean(L, cairo_get_antialias(ctx) == CAIRO_ANTIALIAS_NONE ? false : true);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_antialiasing, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_set_antialias(ctx, luaL_checkboolean(L, 2) ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_fill_rule, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushstring(L, cairo_fill_rule_to_string(cairo_get_fill_rule(ctx)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_fill_rule, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_set_fill_rule(ctx, cairo_fill_rule_from_string(luaL_checkstring(L, 2)));
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_dash, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	int dash_n = cairo_get_dash_count(ctx);
	if(dash_n == 0){
		lua_pushnumber(L, 0);
		lua_newtable(L);
	}else{
		std::vector<double> dashes(dash_n);
		double offset;
		cairo_get_dash(ctx, &dashes[0], &offset);
		lua_pushnumber(L, offset);
		lua_pushtable(L, dashes);
	}
	return 2;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_dash, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double offset = luaL_checknumber(L, 2);
	std::vector<double> dashes = luaL_checktable<double>(L, 3);
	cairo_set_dash(ctx, dashes.size() > 0 ? &dashes[0] : 0, dashes.size(), offset);
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_line_cap, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushstring(L, cairo_line_cap_to_string(cairo_get_line_cap(ctx)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_line_cap, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_set_line_cap(ctx, cairo_line_cap_from_string(luaL_checkstring(L, 2)));
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_line_join, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushstring(L, cairo_line_join_to_string(cairo_get_line_join(ctx)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_line_join, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_set_line_join(ctx, cairo_line_join_from_string(luaL_checkstring(L, 2)));
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_miter_limit, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushnumber(L, cairo_get_miter_limit(ctx));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_miter_limit, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_set_miter_limit(ctx, luaL_checknumber(L, 2));
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_line_width, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushnumber(L, cairo_get_line_width(ctx));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_line_width, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_set_line_width(ctx, luaL_checknumber(L, 2));
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(context_get_composition, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushstring(L, cairo_operator_to_string(cairo_get_operator(ctx)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_composition, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_set_operator(ctx, cairo_operator_from_string(luaL_checkstring(L, 2)));
LUA_FUNC_END

LUA_FUNC_1ARG(context_clip_from_path, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_clip_preserve(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_clip_clear, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_reset_clip(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_clip_get_rectangles, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	// Get clip rectangles
	cairo_rectangle_list_t *rect_list = cairo_copy_clip_rectangle_list(ctx);
	cairo_status_t status = rect_list->status;
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_rectangle_list_destroy(rect_list);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push rectangles to Lua
	lua_createtable(L, rect_list->num_rectangles, 0);
	for(int rect_i = 0; rect_i < rect_list->num_rectangles; ++rect_i){
		lua_createtable(L, 0, 4);
		lua_pushnumber(L, rect_list->rectangles[rect_i].x); lua_setfield(L, -2, "x");
		lua_pushnumber(L, rect_list->rectangles[rect_i].y); lua_setfield(L, -2, "y");
		lua_pushnumber(L, rect_list->rectangles[rect_i].width); lua_setfield(L, -2, "width");
		lua_pushnumber(L, rect_list->rectangles[rect_i].height); lua_setfield(L, -2, "height");
		lua_rawseti(L, -2, rect_i+1);
	}
	cairo_rectangle_list_destroy(rect_list);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_clip_bounding, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double x1, y1, x2, y2;
	cairo_clip_extents(ctx, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
LUA_FUNC_END

LUA_FUNC_1ARG(context_clip_contains, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushboolean(L, cairo_in_clip(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_move_to, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_move_to(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_line_to, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_line_to(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_curve_to, 7)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_curve_to(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6), luaL_checknumber(L, 7));
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_add_rectangle, 5)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_rectangle(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5));
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_add_arc, 6)
	// Get parameters
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double cx = luaL_checknumber(L, 2);
	double cy = luaL_checknumber(L, 3);
	double radius = luaL_checknumber(L, 4);
	double angle1 = luaL_checknumber(L, 5) / 180.0L * M_PI;
	double angle2 = luaL_checknumber(L, 6) / 180.0L * M_PI;
	// Set arc path
	if(angle2 > angle1)
		cairo_arc(ctx, cx, cy, radius, angle1, angle2);
	else if(angle2 < angle1)
		cairo_arc_negative(ctx, cx, cy, radius, angle1, angle2);
LUA_FUNC_END

LUA_FUNC_2ARG(context_path_add_text, 6, 10)
	// Get needed parameters
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	std::wstring text = utf8_to_utf16(luaL_checkstring(L, 4));
	std::wstring face = utf8_to_utf16(luaL_checkstring(L, 5));
	int size = luaL_checknumber(L, 6);
	// Get optional parameters
	bool bold = luaL_optboolean(L, 7, false);
	bool italic = luaL_optboolean(L, 8, false);
	bool underline = luaL_optboolean(L, 9, false);
	bool strikeout = luaL_optboolean(L, 10, false);
	// Set text path
	cairo_translate(ctx, x, y);
	cairo_win32_text_path(ctx, text.c_str(), face.c_str(), size, bold, italic, underline, strikeout);
	cairo_translate(ctx, -x, -y);
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_close, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_close_path(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_clear, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_new_path(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_transfer, 2)
	// Get parameters
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_t *ctx2 = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 2, G2D_CONTEXT));
	// Get path from source context
	cairo_path_t *path = cairo_copy_path(ctx);
	cairo_status_t status = path->status;
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_path_destroy(path);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Append path to destination context
	cairo_append_path(ctx2, path);
	cairo_path_destroy(path);
LUA_FUNC_END

LUA_FUNC_2ARG(context_path_transform, 2, 3)
	// Get parameters
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	if(!lua_isfunction(L, 2))
		luaL_typeerror(L, 2, "function");
	bool is_flat = luaL_optboolean(L, 3, false);
	// Get path
	cairo_path_t *path = is_flat ? cairo_copy_path_flat(ctx) : cairo_copy_path(ctx);
	cairo_status_t status = path->status;
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_path_destroy(path);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Transform path points
	cairo_path_data_t *data;
	for(int data_i = 0; data_i < path->num_data; data_i += path->data[data_i].header.length){
		data = path->data + data_i;
		switch(data[0].header.type){
			case CAIRO_PATH_MOVE_TO:
			case CAIRO_PATH_LINE_TO:
				lua_pushvalue(L, 2);
				lua_pushstring(L, data[0].header.type == CAIRO_PATH_MOVE_TO ? "move" : "line");
				lua_pushnumber(L, data[1].point.x);
				lua_pushnumber(L, data[1].point.y);
				if(lua_pcall(L, 3, 2, 0)){
					cairo_path_destroy(path);
					luaL_error2(L, lua_tostring(L, -1));
				}
				if(!lua_isnumber(L, -2) || !lua_isnumber(L, -1)){
					cairo_path_destroy(path);
					luaL_error2(L, "path transformation function must return 2 numbers");
				}
				data[1].point.x = lua_tonumber(L, -2);
				data[1].point.y = lua_tonumber(L, -1);
				break;
			case CAIRO_PATH_CURVE_TO:
				for(char i = 1; i <= 3; ++i){
					lua_pushvalue(L, 2);
					lua_pushstring(L, "curve");
					lua_pushnumber(L, data[i].point.x);
					lua_pushnumber(L, data[i].point.y);
					if(lua_pcall(L, 3, 2, 0)){
						cairo_path_destroy(path);
						luaL_error2(L, lua_tostring(L, -1));
					}
					if(!lua_isnumber(L, -2) || !lua_isnumber(L, -1)){
						cairo_path_destroy(path);
						luaL_error2(L, "path transformation function must return 2 numbers");
					}
					data[i].point.x = lua_tonumber(L, -2);
					data[i].point.y = lua_tonumber(L, -1);
				}
				break;
			case CAIRO_PATH_CLOSE_PATH:
				lua_pushvalue(L, 2);
				lua_pushstring(L, "close");
				if(lua_pcall(L, 1, 0, 0)){
					cairo_path_destroy(path);
					luaL_error2(L, lua_tostring(L, -1));
				}
				break;
		}
	}
	// Set path
	cairo_new_path(ctx);
	cairo_append_path(ctx, path);
	cairo_path_destroy(path);
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_bounding, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double x1, y1, x2, y2;
	cairo_path_extents(ctx, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
LUA_FUNC_END

LUA_FUNC_1ARG(context_fill, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_fill_preserve(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_fill_bounding, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double x1, y1, x2, y2;
	cairo_fill_extents(ctx, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
LUA_FUNC_END

LUA_FUNC_1ARG(context_fill_contains, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushboolean(L, cairo_in_fill(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_stroke, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_stroke_preserve(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_stroke_bounding, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double x1, y1, x2, y2;
	cairo_stroke_extents(ctx, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
LUA_FUNC_END

LUA_FUNC_1ARG(context_stroke_contains, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushboolean(L, cairo_in_stroke(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_paint, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_paint(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_masked_paint, 2)
	// Get context
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	// Check for parameter
	if(lua_type(L, 2) == LUA_TNUMBER)
		cairo_paint_with_alpha(ctx, lua_tonumber(L, 2));
	else{
		cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_testudata(L, 2, G2D_SOURCE));
		if(pattern)
			cairo_mask(ctx, pattern);
		else
			luaL_error2(L, "number or g2d source expected");
	}
LUA_FUNC_END

// Register
int luaopen_g2d(lua_State *L){
	// Create 'g2d' table
	lua_newtable(L);
	// Register 'g2d' functions to table
	const luaL_Reg g2d_lib[] = {
		"create_image", l_create_image,
		"create_image_from_png", l_create_image_from_png,
		"create_png_from_image", l_create_png_from_image,
		"create_matrix", l_create_matrix,
		"create_color", l_create_color,
		"create_lgradient", l_create_lgradient,
		"create_rgradient", l_create_rgradient,
		"create_mgradient", l_create_mgradient,
		"create_pattern", l_create_pattern,
		"register_font", l_register_font,
		"text_extents", l_text_extents,
		0, 0
	};
	luaL_setfuncs(L, g2d_lib, 0);
	// Set table to  global name 'g2d'
	lua_setglobal(L, "g2d");
	// Define image object methods
	luaL_newmetatable(L, G2D_IMAGE);
	lua_pushstring(L, G2D_IMAGE); lua_setfield(L, -2, "__metatable");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_image_gc); lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, l_image_get_width); lua_setfield(L, -2, "get_width");
	lua_pushcfunction(L, l_image_get_height); lua_setfield(L, -2, "get_height");
	lua_pushcfunction(L, l_image_get_format); lua_setfield(L, -2, "get_format");
	lua_pushcfunction(L, l_image_get_data); lua_setfield(L, -2, "get_data");
	lua_pushcfunction(L, l_image_set_data); lua_setfield(L, -2, "set_data");
	lua_pushcfunction(L, l_image_get_context); lua_setfield(L, -2, "get_context");
	lua_pushcfunction(L, l_image_color_transform); lua_setfield(L, -2, "color_transform");
	lua_pushcfunction(L, l_image_convolute); lua_setfield(L, -2, "convolute");
	lua_pushcfunction(L, l_image_invert); lua_setfield(L, -2, "invert");
	lua_pop(L, 1);
	// Define matrix object methods
	luaL_newmetatable(L, G2D_MATRIX);
	lua_pushstring(L, G2D_MATRIX); lua_setfield(L, -2, "__metatable");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_matrix_get_data); lua_setfield(L, -2, "get_data");
	lua_pushcfunction(L, l_matrix_set_data); lua_setfield(L, -2, "set_data");
	lua_pushcfunction(L, l_matrix_identity); lua_setfield(L, -2, "identity");
	lua_pushcfunction(L, l_matrix_invert); lua_setfield(L, -2, "invert");
	lua_pushcfunction(L, l_matrix_multiply); lua_setfield(L, -2, "multiply");
	lua_pushcfunction(L, l_matrix_translate); lua_setfield(L, -2, "translate");
	lua_pushcfunction(L, l_matrix_scale); lua_setfield(L, -2, "scale");
	lua_pushcfunction(L, l_matrix_rotate); lua_setfield(L, -2, "rotate");
	lua_pushcfunction(L, l_matrix_shear); lua_setfield(L, -2, "shear");
	lua_pushcfunction(L, l_matrix_transform); lua_setfield(L, -2, "transform");
	lua_pop(L, 1);
	// Define source object methods
	luaL_newmetatable(L, G2D_SOURCE);
	lua_pushstring(L, G2D_SOURCE); lua_setfield(L, -2, "__metatable");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_source_gc); lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, l_source_add_color); lua_setfield(L, -2, "add_color");
	lua_pushcfunction(L, l_source_get_color); lua_setfield(L, -2, "get_color");
	lua_pushcfunction(L, l_source_get_points); lua_setfield(L, -2, "get_points");
	lua_pushcfunction(L, l_source_get_circles); lua_setfield(L, -2, "get_circles");
	lua_pushcfunction(L, l_source_add_mesh); lua_setfield(L, -2, "add_mesh");
	lua_pushcfunction(L, l_source_get_mesh); lua_setfield(L, -2, "get_mesh");
	lua_pushcfunction(L, l_source_get_image); lua_setfield(L, -2, "get_image");
	lua_pushcfunction(L, l_source_get_extender); lua_setfield(L, -2, "get_extender");
	lua_pushcfunction(L, l_source_set_extender); lua_setfield(L, -2, "set_extender");
	lua_pushcfunction(L, l_source_get_filter); lua_setfield(L, -2, "get_filter");
	lua_pushcfunction(L, l_source_set_filter); lua_setfield(L, -2, "set_filter");
	lua_pushcfunction(L, l_source_get_matrix); lua_setfield(L, -2, "get_matrix");
	lua_pushcfunction(L, l_source_set_matrix); lua_setfield(L, -2, "set_matrix");
	lua_pushcfunction(L, l_source_get_type); lua_setfield(L, -2, "get_type");
	lua_pop(L, 1);
	// Define context object methods
	luaL_newmetatable(L, G2D_CONTEXT);
	lua_pushstring(L, G2D_CONTEXT); lua_setfield(L, -2, "__metatable");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_context_gc); lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, l_context_get_matrix); lua_setfield(L, -2, "get_matrix");
	lua_pushcfunction(L, l_context_set_matrix); lua_setfield(L, -2, "set_matrix");
	lua_pushcfunction(L, l_context_get_source); lua_setfield(L, -2, "get_source");
	lua_pushcfunction(L, l_context_set_source); lua_setfield(L, -2, "set_source");
	lua_pushcfunction(L, l_context_push_state); lua_setfield(L, -2, "push_state");
	lua_pushcfunction(L, l_context_pop_state); lua_setfield(L, -2, "pop_state");
	lua_pushcfunction(L, l_context_get_antialiasing); lua_setfield(L, -2, "get_antialiasing");
	lua_pushcfunction(L, l_context_set_antialiasing); lua_setfield(L, -2, "set_antialiasing");
	lua_pushcfunction(L, l_context_get_fill_rule); lua_setfield(L, -2, "get_fill_rule");
	lua_pushcfunction(L, l_context_set_fill_rule); lua_setfield(L, -2, "set_fill_rule");
	lua_pushcfunction(L, l_context_get_dash); lua_setfield(L, -2, "get_dash");
	lua_pushcfunction(L, l_context_set_dash); lua_setfield(L, -2, "set_dash");
	lua_pushcfunction(L, l_context_get_line_cap); lua_setfield(L, -2, "get_line_cap");
	lua_pushcfunction(L, l_context_set_line_cap); lua_setfield(L, -2, "set_line_cap");
	lua_pushcfunction(L, l_context_get_line_join); lua_setfield(L, -2, "get_line_join");
	lua_pushcfunction(L, l_context_set_line_join); lua_setfield(L, -2, "set_line_join");
	lua_pushcfunction(L, l_context_get_miter_limit); lua_setfield(L, -2, "get_miter_limit");
	lua_pushcfunction(L, l_context_set_miter_limit); lua_setfield(L, -2, "set_miter_limit");
	lua_pushcfunction(L, l_context_get_line_width); lua_setfield(L, -2, "get_line_width");
	lua_pushcfunction(L, l_context_set_line_width); lua_setfield(L, -2, "set_line_width");
	lua_pushcfunction(L, l_context_get_composition); lua_setfield(L, -2, "get_composition");
	lua_pushcfunction(L, l_context_set_composition); lua_setfield(L, -2, "set_composition");
	lua_pushcfunction(L, l_context_clip_from_path); lua_setfield(L, -2, "clip_from_path");
	lua_pushcfunction(L, l_context_clip_clear); lua_setfield(L, -2, "clip_clear");
	lua_pushcfunction(L, l_context_clip_get_rectangles); lua_setfield(L, -2, "clip_get_rectangles");
	lua_pushcfunction(L, l_context_clip_bounding); lua_setfield(L, -2, "clip_bounding");
	lua_pushcfunction(L, l_context_clip_contains); lua_setfield(L, -2, "clip_contains");
	lua_pushcfunction(L, l_context_path_move_to); lua_setfield(L, -2, "path_move_to");
	lua_pushcfunction(L, l_context_path_line_to); lua_setfield(L, -2, "path_line_to");
	lua_pushcfunction(L, l_context_path_curve_to); lua_setfield(L, -2, "path_curve_to");
	lua_pushcfunction(L, l_context_path_add_rectangle); lua_setfield(L, -2, "path_add_rectangle");
	lua_pushcfunction(L, l_context_path_add_arc); lua_setfield(L, -2, "path_add_arc");
	lua_pushcfunction(L, l_context_path_add_text); lua_setfield(L, -2, "path_add_text");
	lua_pushcfunction(L, l_context_path_close); lua_setfield(L, -2, "path_close");
	lua_pushcfunction(L, l_context_path_clear); lua_setfield(L, -2, "path_clear");
	lua_pushcfunction(L, l_context_path_transfer); lua_setfield(L, -2, "path_transfer");
	lua_pushcfunction(L, l_context_path_transform); lua_setfield(L, -2, "path_transform");
	lua_pushcfunction(L, l_context_path_bounding); lua_setfield(L, -2, "path_bounding");
	lua_pushcfunction(L, l_context_fill); lua_setfield(L, -2, "fill");
	lua_pushcfunction(L, l_context_fill_bounding); lua_setfield(L, -2, "fill_bounding");
	lua_pushcfunction(L, l_context_fill_contains); lua_setfield(L, -2, "fill_contains");
	lua_pushcfunction(L, l_context_stroke); lua_setfield(L, -2, "stroke");
	lua_pushcfunction(L, l_context_stroke_bounding); lua_setfield(L, -2, "stroke_bounding");
	lua_pushcfunction(L, l_context_stroke_contains); lua_setfield(L, -2, "stroke_contains");
	lua_pushcfunction(L, l_context_paint); lua_setfield(L, -2, "paint");
	lua_pushcfunction(L, l_context_masked_paint); lua_setfield(L, -2, "masked_paint");
	lua_pop(L, 1);
	// Nothing pushed to Lua state
	return 0;
}