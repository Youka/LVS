#include "llibs.hpp"
#include "cairo.hpp"
#include "threads.h"	// For image convolution multithreading
#include "textconv.hpp"	// For library text functions
#include <memory>	// For smart pointers
#define M_PI       3.14159265358979323846	// From "math.h"

// Objects names
#define G2D_IMAGE "g2d image"
#define G2D_MATRIX "g2d matrix"
#define G2D_SOURCE "g2d source"
#define G2D_CONTEXT "g2d context"

// Type<->string converters
inline cairo_format_t cairo_format_from_string(const char *format_string){
	if(strcmp(format_string, "RGBA") == 0) return CAIRO_FORMAT_ARGB32;
	else if(strcmp(format_string, "RGB") == 0) return CAIRO_FORMAT_RGB24;
	else if(strcmp(format_string, "ALPHA") == 0) return CAIRO_FORMAT_A8;
	else if(strcmp(format_string, "BINARY") == 0) return CAIRO_FORMAT_A1;
	else return CAIRO_FORMAT_INVALID;
}

inline const char* cairo_format_to_string(cairo_format_t format){
	switch(format){
		case CAIRO_FORMAT_ARGB32: return "RGBA";
		case CAIRO_FORMAT_RGB24: return "RGB";
		case CAIRO_FORMAT_A8: return "ALPHA";
		case CAIRO_FORMAT_A1: return "BINARY";
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
	else return CAIRO_LINE_CAP_BUTT;
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
	else return CAIRO_LINE_JOIN_MITER;
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
	int width = luaL_checknumber(L, 2);
	int height = luaL_checknumber(L, 3);
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

LUA_FUNC_1ARG(create_sub_image, 5)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	int x0 = luaL_checknumber(L, 2);
	int y0 = luaL_checknumber(L, 3);
	int x1 = luaL_checknumber(L, 4);
	int y1 = luaL_checknumber(L, 5);
	if(x0 < 0 || y0 < 0 ||
		x1 <= x0 || y1 <= y0 ||
		x1 > cairo_image_surface_get_width(surface) || y1 > cairo_image_surface_get_height(surface))
		luaL_error2(L, "invalid area");
	// Create sub image
	cairo_surface_t *sub_surface = cairo_image_surface_create(cairo_image_surface_get_format(surface), x1-x0, y1-y0);
	cairo_t *ctx = cairo_create(sub_surface);
	cairo_set_source_surface(ctx, surface, x0, y0);
	cairo_paint(ctx);
	cairo_destroy(ctx);
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = sub_surface;
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
	const char *filename = luaL_checkstring(L, 1);
	// Create image from png
	wchar_t *filenamew = utf8_to_utf16(filename);
	FILE *file = _wfopen(filenamew, L"rb");
	delete[] filenamew;
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
	const char *filename = luaL_checkstring(L, 2);
	// Create png from image
	wchar_t *filenamew = utf8_to_utf16(filename);
	FILE *file = _wfopen(filenamew, L"wb");
	delete[] filenamew;
	if(!file)
		luaL_error2(L, "couldn't create file");
	cairo_status_t status = cairo_surface_write_to_png_stream(surface, png_stream_writer, file);
	fclose(file);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(create_context, 1)
	// Get parameter
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	// Create context
	cairo_t *ctx = cairo_create(surface);
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_destroy(ctx);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push context to Lua
	*lua_createuserdata<cairo_t*>(L, G2D_CONTEXT) = ctx;
	return 1;
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

LUA_FUNC_2ARG(create_source_color, 3, 4)
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

LUA_FUNC_1ARG(create_source_linear_gradient, 4)
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

LUA_FUNC_1ARG(create_source_radial_gradient, 6)
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

LUA_FUNC_1ARG(create_source_mesh_gradient, 0)
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

LUA_FUNC_1ARG(create_source_image, 1)
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

LUA_FUNC_1ARG(image_convolution, 2)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	size_t filter_size;
	float *convolution_filter = luaL_checktable<float>(L, 2, &filter_size);
	std::auto_ptr<float> convolution_filter_obj(convolution_filter);
	lua_getfield(L, 2, "width");
	if(!lua_isnumber(L, -1)) luaL_error2(L, "table needs a valid field 'width'");
	int filter_width = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 2, "height");
	if(!lua_isnumber(L, -1)) luaL_error2(L, "table needs a valid field 'height'");
	int filter_height = lua_tonumber(L, -1);
	lua_pop(L, 1);
	if(filter_width < 1 || !filter_width&0x1 || filter_height < 1 || !filter_height&0x1)
		luaL_error2(L, "table field(s) invalid");
	else if(filter_width * filter_height != filter_size)
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
	float *image_data_copy = new float[image_data_size];
	for(unsigned long int i = 0; i < image_data_size; i++)
		image_data_copy[i] = image_data[i];
	// Threading data
	static Threads<THREAD_DATA> threads(cairo_image_surface_convolution);
	int image_row_step = image_height /  threads.size();
	THREAD_DATA *data;
	for(DWORD i = 0; i < threads.size(); i++){
		// Set current thread data
		data = threads.get(i);
		data->image_width = image_width;
		data->image_height = image_height;
		data->image_stride = image_stride;
		data->image_first_row = i * image_row_step;
		data->image_last_row = i == threads.size() - 1 ? image_height-1 : data->image_first_row + image_row_step-1;
		switch(image_format){
			case CAIRO_FORMAT_ARGB32:
			case CAIRO_FORMAT_RGB24:
				data->image_rgb = true;
				break;
			case CAIRO_FORMAT_A8:
				data->image_rgb = false;
		}
		data->image_src = image_data_copy;
		data->image_dst = image_data;
		data->filter_width = filter_width;
		data->filter_height = filter_height;
		data->filter_kernel = convolution_filter;
	}
	// Apply convolution filter to image in multiple threads
	threads.Run();
	// Free image data copy
	delete[] image_data_copy;
	// Set image data as dirty
	cairo_surface_mark_dirty(surface);
	// Return image
	lua_pushvalue(L, 1);
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(text_extents, 3, 7)
	// Get parameters
	const char *text = luaL_checkstring(L, 1);
	const char *face = luaL_checkstring(L, 2);
	int size = luaL_checknumber(L, 3);
	bool bold = luaL_optboolean(L, 4, false);
	bool italic = luaL_optboolean(L, 5, false);
	bool underline = luaL_optboolean(L, 6, false);
	bool strikeout = luaL_optboolean(L, 7, false);
	// Get text extents
	wchar_t *textw = utf8_to_utf16(text), *facew = utf8_to_utf16(face);
	cairo_win32_text_extents_t extents = cairo_win32_text_extents(textw, facew, size, bold, italic, underline, strikeout);
	delete[] textw; delete[] facew;
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

LUA_FUNC_1ARG(image_get_data, 5)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	int x0 = luaL_checknumber(L, 2);
	int y0 = luaL_checknumber(L, 3);
	int x1 = luaL_checknumber(L, 4);
	int y1 = luaL_checknumber(L, 5);
	if(x0 < 0 || y0 < 0 ||
		x1 <= x0 || y1 <= y0 ||
		x1 > cairo_image_surface_get_width(surface) || y1 > cairo_image_surface_get_height(surface))
		luaL_error2(L, "invalid area");
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
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; x++){
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
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; x++){
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
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + x0;
				for(int x = 0; x < area_width; x++){
					lua_pushnumber(L, *row++); lua_rawseti(L, -2, ++table_index);	// A
				}
			}
			return 1;
		}break;
		case CAIRO_FORMAT_A1:{
			lua_createtable(L, area_width * area_height, 0);
			int table_index = 0;
			unsigned char *row;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride;
				for(int x = x0; x < x1; x++){
					div_t quot_rem = div(x, 8);
					lua_pushnumber(L, row[quot_rem.quot] >> quot_rem.rem & 0x1); lua_rawseti(L, -2, ++table_index);	// A
				}
			}
			return 1;
		}break;
	}
LUA_FUNC_END

LUA_FUNC_1ARG(image_set_data, 6)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	int x0 = luaL_checknumber(L, 2);
	int y0 = luaL_checknumber(L, 3);
	int x1 = luaL_checknumber(L, 4);
	int y1 = luaL_checknumber(L, 5);
	if(x0 < 0 || y0 < 0 ||
		x1 <= x0 || y1 <= y0 ||
		x1 > cairo_image_surface_get_width(surface) || y1 > cairo_image_surface_get_height(surface))
		luaL_error2(L, "invalid area");
	size_t new_data_size;
	unsigned char *new_data = luaL_checktable<unsigned char>(L, 6, &new_data_size);
	std::auto_ptr<unsigned char> new_data_obj(new_data);
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
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; x++){
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
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; x++){
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
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + x0;
				for(int x = 0; x < area_width; x++)
					*row++ = *new_data++;	// A
			}
		}break;
		case CAIRO_FORMAT_A1:{
			if(area_width * area_height != new_data_size)
				luaL_error2(L, "wrong table size");
			unsigned char *row, dst, mask, src;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride;
				for(int x = x0; x < x1; x++){
					div_t quot_rem = div(x, 8);
					mask = 0x1 << quot_rem.rem;
					src = *new_data++;
					dst = row[quot_rem.quot] & mask;
					if(src && !dst)
						row[quot_rem.quot] |= mask;	// A
					else if(!src && dst)
						row[quot_rem.quot] &= ~mask;	// A
				}
			}
		}break;
	}
	// Set image data as dirty
	cairo_surface_mark_dirty_rectangle(surface, x0, y0, area_width, area_height);
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

LUA_FUNC_1ARG(matrix_multiply, 7)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_t a = {luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6), luaL_checknumber(L, 7)};
	cairo_matrix_t b = *matrix;
	cairo_matrix_multiply(matrix, &a, &b);
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
	// Create image copy
	cairo_surface_t *new_surface = cairo_image_surface_create(cairo_image_surface_get_format(surface), cairo_image_surface_get_width(surface), cairo_image_surface_get_height(surface));
	cairo_surface_flush(surface); cairo_surface_flush(new_surface);
	unsigned char *surface_data = cairo_image_surface_get_data(surface), *new_surface_data = cairo_image_surface_get_data(new_surface);
	memcpy(new_surface_data, surface_data, cairo_image_surface_get_height(surface) * cairo_image_surface_get_stride(surface));
	cairo_surface_mark_dirty(new_surface);
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = new_surface;
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
		double *dashes = new double[dash_n];
		double offset;
		cairo_get_dash(ctx, dashes, &offset);
		lua_pushnumber(L, offset);
		lua_pushtable(L, dashes, dash_n);
		delete[] dashes;
	}
	return 2;
LUA_FUNC_END

LUA_FUNC_1ARG(context_set_dash, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double offset = luaL_checknumber(L, 2);
	unsigned int dash_n;
	double *dashes = luaL_checktable<double>(L, 3, &dash_n);
	cairo_set_dash(ctx, dashes, dash_n, offset);
	if(dashes)
		delete[] dashes;
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
	for(int rect_i = 0; rect_i < rect_list->num_rectangles; rect_i++){
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

LUA_FUNC_2ARG(context_path_add_text, 4, 8)
	// Get parameters
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	const char *text = luaL_checkstring(L, 2);
	const char *face = luaL_checkstring(L, 3);
	int size = luaL_checknumber(L, 4);
	bool bold = luaL_optboolean(L, 5, false);
	bool italic = luaL_optboolean(L, 6, false);
	bool underline = luaL_optboolean(L, 7, false);
	bool strikeout = luaL_optboolean(L, 8, false);
	// Set text path
	wchar_t *textw = utf8_to_utf16(text), *facew = utf8_to_utf16(face);
	cairo_win32_text_path(ctx, textw, facew, size, bold, italic, underline, strikeout);
	delete[] textw; delete[] facew;
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_close, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_close_path(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_clear, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_new_path(ctx);
LUA_FUNC_END

LUA_FUNC_2ARG(context_path_transform, 2, 3)
	// Get parameters
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	if(lua_istable(L, 2))
		luaL_typerror(L, 2, "function");
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
				for(char i = 1; i <= 3; i++){
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

LUA_FUNC_1ARG(context_path_fill, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_fill_preserve(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_fill_bounding, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double x1, y1, x2, y2;
	cairo_fill_extents(ctx, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_fill_contains, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushboolean(L, cairo_in_fill(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_stroke, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_stroke_preserve(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_stroke_bounding, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	double x1, y1, x2, y2;
	cairo_stroke_extents(ctx, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
LUA_FUNC_END

LUA_FUNC_1ARG(context_path_stroke_contains, 3)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	lua_pushboolean(L, cairo_in_stroke(ctx, luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(context_paint, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_paint(ctx);
LUA_FUNC_END

LUA_FUNC_1ARG(context_masked_paint, 2)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 2, G2D_SOURCE));
	cairo_mask(ctx, pattern);
LUA_FUNC_END

// Register
int luaopen_g2d(lua_State *L){
	// Create 'g2d' table
	lua_newtable(L);
	// Register 'g2d' functions to table
	const luaL_Reg g2d_lib[] = {
		"create_image", l_create_image,
		"create_sub_image", l_create_sub_image,
		"create_image_from_png", l_create_image_from_png,
		"create_png_from_image", l_create_png_from_image,
		"create_context", l_create_context,
		"create_matrix", l_create_matrix,
		"create_source_color", l_create_source_color,
		"create_source_linear_gradient", l_create_source_linear_gradient,
		"create_source_radial_gradient", l_create_source_radial_gradient,
		"create_source_mesh_gradient", l_create_source_mesh_gradient,
		"create_source_image", l_create_source_image,
		"image_convolution", l_image_convolution,
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
	lua_pushcfunction(L, l_context_path_transform); lua_setfield(L, -2, "path_transform");
	lua_pushcfunction(L, l_context_path_bounding); lua_setfield(L, -2, "path_bounding");
	lua_pushcfunction(L, l_context_path_fill); lua_setfield(L, -2, "path_fill");
	lua_pushcfunction(L, l_context_path_fill_bounding); lua_setfield(L, -2, "path_fill_bounding");
	lua_pushcfunction(L, l_context_path_fill_contains); lua_setfield(L, -2, "path_fill_contains");
	lua_pushcfunction(L, l_context_path_stroke); lua_setfield(L, -2, "path_stroke");
	lua_pushcfunction(L, l_context_path_stroke_bounding); lua_setfield(L, -2, "path_stroke_bounding");
	lua_pushcfunction(L, l_context_path_stroke_contains); lua_setfield(L, -2, "path_stroke_contains");
	lua_pushcfunction(L, l_context_paint); lua_setfield(L, -2, "paint");
	lua_pushcfunction(L, l_context_masked_paint); lua_setfield(L, -2, "masked_paint");
	lua_pop(L, 1);
	// Nothing pushed to Lua state
	return 0;
}