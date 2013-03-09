#include "llibs.hpp"
#include "cairo.hpp"

// Metatable names
#define LVS_IMAGE "LVS_IMAGE"
#define LVS_CONTEXT "LVS_CONTEXT"
#define LVS_MATRIX "LVS_MATRIX"
#define LVS_SOURCE "LVS_SOURCE"

LUA_FUNC_1ARG(image_gc, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, LVS_IMAGE));
	cairo_surface_destroy(surface);
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_header, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, LVS_IMAGE));
	lua_pushnumber(L, cairo_image_surface_get_width(surface));
	lua_pushnumber(L, cairo_image_surface_get_height(surface));
	switch(cairo_image_surface_get_format(surface)){
		case CAIRO_FORMAT_ARGB32: lua_pushstring(L, "RGBA"); break;
		case  CAIRO_FORMAT_RGB24: lua_pushstring(L, "RGB"); break;
		case CAIRO_FORMAT_A8: lua_pushstring(L, "ALPHA"); break;
		case CAIRO_FORMAT_A1: lua_pushstring(L, "BINARY"); break;
		default: lua_pushstring(L, "INVALID");
	}
	return 3;
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_data, 5)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, LVS_IMAGE));

	// TEST START
	cairo_t *ctx = cairo_create(surface);
	cairo_translate(ctx, 320, 50);
	const wchar_t text[] = {L'H', L'e', L'l', L'l', L'o', L' ', L'w', L'o', L'r', L'l', L'd', L'!', 0x3042, 0};
	cairo_win32_text_path(ctx, text, L"Comic Sans MS", 60, true, true, true, true);
	cairo_set_source_rgba(ctx, 1, 1, 0.5, 0.7);
	cairo_fill(ctx);
	cairo_destroy(ctx);
	// TEST END

LUA_FUNC_END

int luaopen_g2d(lua_State *L){
	// Register 'g2d' library
	const luaL_Reg g2d_lib[] = {
		0, 0
	};
	luaL_setfuncs(L, g2d_lib, 0);
	// Register 'g2d' objects methods
	luaL_newmetatable(L, LVS_IMAGE);
	lua_pushcfunction(L, l_image_gc); lua_setfield(L, -2, "__gc");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_image_get_header); lua_setfield(L, -2, "get_header");
	lua_pushcfunction(L, l_image_get_data); lua_setfield(L, -2, "get_data");
	lua_pop(L, 1);
	// Nothing pushed to Lua state
	return 0;
}