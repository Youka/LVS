#include "llibs.hpp"
#include "cairo.hpp"

// Metatable names
#define LVS_IMAGE "LVS_IMAGE"

LUA_FUNC_1ARG(image_gc, 1)
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
	cairo_surface_destroy(surface);
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
	lua_pop(L, 1);
	// Nothing pushed to Lua state
	return 0;
}