#include "llibs.hpp"
#include <fstream>
#include <iomanip>
using namespace std;

LUA_FUNC_NARG(print, 0)
	// Open log file handle
	ofstream log("log.txt", ios_base::app);
	if(!log)
		return 0;
	// Iterate through arguments
	int n = lua_gettop(L);
	for(int i = 1; i <= n; ++i)
		// Write to standard log file by argument type
		switch(lua_type(L, i)){
			case LUA_TNONE: log << "none\n"; break;
			case LUA_TNIL: log << "nil\n"; break;
			case LUA_TBOOLEAN: log << (lua_toboolean(L,i) ? "true\n" : "false\n"); break;
			case LUA_TNUMBER: log << lua_tonumber(L, i) << '\n'; break;
			case LUA_TSTRING:{
				size_t len;
				const char *str = lua_tolstring(L, i, &len);
				log.write(str, len) << '\n';
			}break;
			case LUA_TLIGHTUSERDATA:
			case LUA_TTABLE:
			case LUA_TFUNCTION:
			case LUA_TUSERDATA:
			case LUA_TTHREAD:
				switch(lua_type(L, i)){
					case LUA_TLIGHTUSERDATA: log << "lightuserdata: "; break;
					case LUA_TTABLE: log << "table: "; break;
					case LUA_TFUNCTION: log << "function: "; break;
					case LUA_TUSERDATA: log << "userdata: "; break;
					case LUA_TTHREAD: log << "thread: "; break;
				}
#ifdef _WIN64
				log << "0x" << setfill('0') << setw(16) << hex << reinterpret_cast<unsigned __int64>(lua_topointer(L, i));
#else
				log << "0x" << setfill('0') << setw(8) << hex << reinterpret_cast<unsigned __int32>(lua_topointer(L, i));
#endif
				log << '\n';
				break;
		}
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