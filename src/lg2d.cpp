#include "llibs.hpp"
#include "cairo.hpp"

// Objects names
#define G2D_IMAGE "G2D_IMAGE"
#define G2D_MATRIX "G2D_MATRIX"
#define G2D_SOURCE "G2D_SOURCE"
#define G2D_CONTEXT "G2D_CONTEXT"

// Converters
cairo_format_t cairo_format_from_string(const char *format_string){
	if(strcmp(format_string, "ARGB") == 0)
		return CAIRO_FORMAT_ARGB32;
	else if(strcmp(format_string, "RGB") == 0)
		return CAIRO_FORMAT_RGB24;
	else if(strcmp(format_string, "ALPHA") == 0)
		return CAIRO_FORMAT_A8;
	else if(strcmp(format_string, "BINARY") == 0)
		return CAIRO_FORMAT_A1;
	else
		return CAIRO_FORMAT_INVALID;
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

LUA_FUNC_1ARG(create_image_from_png, 1)
	// Get parameter
	const char *filename = luaL_checkstring(L, 1);
	// Create image

	// TODO: add utf-8 support

	cairo_surface_t *surface = cairo_image_surface_create_from_png(filename);
	cairo_status_t status = cairo_surface_status(surface);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_surface_destroy(surface);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = surface;
	return 1;
LUA_FUNC_END

// IMAGE OBJECT
LUA_FUNC_1ARG(image_gc, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	cairo_surface_destroy(surface);
LUA_FUNC_END

// MATRIX OBJECT

// SOURCE OBJECT
LUA_FUNC_1ARG(source_gc, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_destroy(pattern);
LUA_FUNC_END

// CONTEXT OBJECT
LUA_FUNC_1ARG(context_gc, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_destroy(ctx);
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
		0, 0
	};
	luaL_setfuncs(L, g2d_lib, 0);
	// Set table to  global name 'g2d'
	lua_setglobal(L, "g2d");
	// Define image object methods
	luaL_newmetatable(L, G2D_IMAGE);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_image_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
	// Define matrix object methods
	luaL_newmetatable(L, G2D_MATRIX);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	// Define source object methods
	luaL_newmetatable(L, G2D_SOURCE);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_source_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
	// Define context object methods
	luaL_newmetatable(L, G2D_CONTEXT);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_context_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
	// Nothing pushed to Lua state
	return 0;
}