#include "llibs.hpp"
#include "cstdlib"
#include "cstring"

LUA_FUNC_NARG(print, 0)
	// Number of arguments
	int n = lua_gettop(L);
	// Iterate through arguments
	for(int i = 1; i <= n; i++)
		// Write to standard output stream by argument type
		switch(lua_type(L, i)){
			case LUA_TNONE:
				fwrite("None\n", 1, 5, stdout);
				break;
			case LUA_TNIL:
				fwrite("Nil\n", 1, 4, stdout);
				break;
			case LUA_TBOOLEAN:
				if(lua_toboolean(L,i))
					fwrite("True\n", 1, 5, stdout);
				else
					fwrite("False\n", 1, 6, stdout);
				break;
			case LUA_TNUMBER:{
				char buf[32];
				_snprintf(buf, sizeof(buf), "%f", lua_tonumber(L, i));
				fwrite(buf, 1, strlen(buf), stdout);
				fputc('\n', stdout);
				break;
			 }
			case LUA_TSTRING:{
				size_t len;
				const char *str = lua_tolstring(L, i, &len);
				fwrite(str, 1, len, stdout);
				fputc('\n', stdout);
				break;
			}
			case LUA_TLIGHTUSERDATA:
			case LUA_TTABLE:
			case LUA_TFUNCTION:
			case LUA_TUSERDATA:
			case LUA_TTHREAD:{
#ifdef _WIN64
				unsigned __int64 addr = reinterpret_cast<unsigned __int64>(lua_topointer(L, i));
				char buf[21];
				_ui64toa(addr, buf, 10);
#else
				unsigned __int32 addr = reinterpret_cast<unsigned __int32>(lua_topointer(L, i));
				char buf[11];
				ultoa(addr, buf, 10);
#endif
				switch(lua_type(L, i)){
					case LUA_TLIGHTUSERDATA:
						fwrite("Lightuserdata: ", 1, 15, stdout);
						break;
					case LUA_TTABLE:
						fwrite("Table: ", 1, 7, stdout);
						break;
					case LUA_TFUNCTION:
						fwrite("Function: ", 1, 10, stdout);
						break;
					case LUA_TUSERDATA:
						fwrite("Userdata: ", 1, 10, stdout);
						break;
					case LUA_TTHREAD:
						fwrite("Thread: ", 1, 8, stdout);
						break;
				}
				fwrite(buf, 1, strlen(buf), stdout);
				fputc('\n', stdout);
				break;
			}
		}
	// Flush output stream
	fflush(stdout);
LUA_FUNC_END

LUA_FUNC_1ARG(table_create, 2)
	double arraysize = luaL_checknumber(L, 1), mapsize = luaL_checknumber(L, 2);
	if(arraysize >= 0 && mapsize >= 0){
		lua_createtable(L, arraysize, mapsize);
		return 1;
	}else
		luaL_error2(L, "sizes have to be bigger-equal zero");
LUA_FUNC_END

int luaopen_base2(lua_State *L){
	// Register global function 'print'
	LUA_REGISTER_FUNC(print)
	// Register function 'create' to library 'table'
	LUA_REGISTER_LIB_FUNC(table, create)
	// Nothing pushed to Lua state
	return 0;
}