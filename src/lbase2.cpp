#include "llibs.hpp"
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
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

struct Point3d{
	double x, y, z;
};
inline unsigned int fac(unsigned int n){
	if(n > 1){
		unsigned int k = 1;
		for(unsigned int i = 2; i <= n; ++i)
			k *= i;
		return k;
	}else
		return 1;
}
inline unsigned int bin_coeff(unsigned int i, unsigned int n){
	return fac(n) / (fac(i) * fac(n - i));
}
inline double bernstein(double pct, unsigned int i, unsigned int n){
	return bin_coeff(i, n) * pow(pct, static_cast<int>(i)) * pow(1 - pct,  static_cast<int>(n - i));
}
LUA_FUNC_1ARG(math_bezier, 2)
	// Check & get arguments
	double pct = luaL_checknumber(L, 1);
	if(pct < 0 || pct > 1)
		luaL_error2(L, "invalid percent value");
	if(!lua_istable(L, 2))
		luaL_error2(L, "second argument must be a table");
	// Extract bezier points from second argument
	vector<Point3d> points;
	size_t points_n = lua_rawlen(L, 2);
	for(size_t i = 1; i <= points_n; ++i){
		lua_rawgeti(L, 2, i);
		lua_rawgeti(L, -1, 1); lua_rawgeti(L, -2, 2); lua_rawgeti(L, -3, 3);
		if(!lua_isnumber(L, -3) || !lua_isnumber(L, -2) || (!lua_isnil(L, -1) && !lua_isnumber(L, -1)))
			luaL_error2(L, "invalid point");
		Point3d point = {lua_tonumber(L, -3), lua_tonumber(L, -2), luaL_optnumber(L, -1, 0)};
		points.push_back(point);
		lua_pop(L, 4);
	}
	// Calculate point on bezier curve
	Point3d point = {0.0, 0.0, 0.0};
	unsigned int n = points_n - 1;
	for(unsigned int i = 0; i <= n; ++i){
		double bern = bernstein(pct, i, n);
		point.x += points[i].x * bern;
		point.y += points[i].y * bern;
		point.z += points[i].z * bern;
	}
	// Return result
	lua_createtable(L, 3, 0);
	lua_pushnumber(L, point.x); lua_rawseti(L, -2, 1);
	lua_pushnumber(L, point.y); lua_rawseti(L, -2, 2);
	lua_pushnumber(L, point.z); lua_rawseti(L, -2, 3);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(math_degree, 2)
	// Check arguments
	if(!lua_istable(L, 1) || !lua_istable(L, 2))
		luaL_error2(L, "two tables expected");

	// TODO

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
	// Register math functions
	LUA_REGISTER_LIB_FUNC(math, bezier)

	// TODO

	// Register string functions

	// TODO

	// Register table functions

	// TODO

	LUA_REGISTER_LIB_FUNC(table, create)
	// Nothing pushed to Lua state
	return 0;
}