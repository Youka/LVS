#include "llibs.hpp"
#include <fstream>
#include <iomanip>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>

using namespace std;

#define RAD_TO_DEG(x) (x / M_PI * 180.0)
#define DEG_TO_RAD(x) (x / 180.0 * M_PI)

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
	// Get arguments
	vector<double> vec1 = luaL_checktable<double>(L, 1), vec2 = luaL_checktable<double>(L, 2);
	// Calculate degree
	double degree;
	if(vec1.size() == 2 && vec2.size() == 2){
		degree = RAD_TO_DEG(
			acos(
				(vec1[0] * vec2[0] + vec1[1] * vec2[1]) /
				(sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1]) * sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1]))
			)
		);
	}else if(vec1.size() == 3 && vec2.size() == 3){
		degree = RAD_TO_DEG(
			acos(
				(vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2]) /
				(sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1] + vec1[2] * vec1[2]) * sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1] + vec2[2] * vec2[2]))
			)
		);
	}else
		luaL_error2(L, "invalid table size or contents");
	// Return result
	lua_pushnumber(L, (vec1[0] * vec2[1] - vec1[1] * vec2[0]) < 0 ? -degree : degree);
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(math_distance, 2, 3)
	double w = luaL_checknumber(L, 1), h = luaL_checknumber(L, 2);
	if(lua_gettop(L) == 3){
		double d = luaL_checknumber(L, 3);
		lua_pushnumber(L, sqrt(w*w + h*h + d*d));
	}else
		lua_pushnumber(L, sqrt(w*w + h*h));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(math_ellipse, 5)
	double ra = DEG_TO_RAD(luaL_checknumber(L, 5));
	lua_pushnumber(L, luaL_checknumber(L, 1) + luaL_checknumber(L, 3) / 2 * sin(ra));
	lua_pushnumber(L, luaL_checknumber(L, 2) + luaL_checknumber(L, 4) / 2 * cos(ra));
	return 2;
LUA_FUNC_END

LUA_FUNC_1ARG(math_inrange, 3)
	double a = luaL_checknumber(L, 1), c = luaL_checknumber(L, 2), b = luaL_checknumber(L, 3);
	if(b < c && b >= a){
		lua_pushnumber(L, (b-a) / (c-a));
		return 1;
	}
LUA_FUNC_END

LUA_FUNC_1ARG(math_interpolate, 3)
	double a = luaL_checknumber(L, 1);
	lua_pushnumber(L, a + (luaL_checknumber(L, 2) - a) * luaL_checknumber(L, 3));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(math_ortho, 2)
	vector<double> vec1 = luaL_checktable<double>(L, 1), vec2 = luaL_checktable<double>(L, 2);
	if(vec1.size() != 3 || vec2.size() != 3)
		luaL_error2(L, "two 3D vectors expected");
	lua_createtable(L, 3, 0);
	lua_pushnumber(L, vec1[1] * vec2[2] - vec1[2] * vec2[1]	); lua_rawseti(L, -2, 1);
	lua_pushnumber(L, vec1[2] * vec2[0] - vec1[0] * vec2[2]	); lua_rawseti(L, -2, 2);
	lua_pushnumber(L, vec1[0] * vec2[1] - vec1[1] * vec2[0]	); lua_rawseti(L, -2, 3);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(math_randomsteps, 3)
	double start = luaL_checknumber(L, 1), end = luaL_checknumber(L, 2), step = luaL_checknumber(L, 3);
	if(start > end || step <= 0)
		luaL_error2(L, "valid numbers expected");
	lua_pushnumber(L, start + rand() % int((end - start) / step + 1.0) * step);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(math_round, 1)
	lua_pushnumber(L, floor(luaL_checknumber(L, 1) + 0.5));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(math_trim, 3)
	double n = luaL_checknumber(L, 1), start = luaL_checknumber(L, 2), end = luaL_checknumber(L, 3);
	lua_pushnumber(L, n < start ? start : (n > end ? end : n));
	return 1;
LUA_FUNC_END

/*
UTF16 -> UTF8
----------------------------------
U-00000000 -> U-0000007F: 		0xxxxxxx
U-00000080 -> U-000007FF: 		110xxxxx 10xxxxxx
U-00000800 -> U-0000FFFF: 		1110xxxx 10xxxxxx 10xxxxxx
U-00010000 -> U-001FFFFF: 		11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
U-00200000 -> U-03FFFFFF: 		111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
U-04000000 -> U-7FFFFFFF: 		1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
inline unsigned char uchar_width(unsigned char c){
	if(c < 192)
		return 1;
	else if(c < 224)
		return 2;
	else if(c < 240)
		return 3;
	else if(c < 248)
		return 4;
	else if(c < 252)
		return 5;
	else
		return 6;
}
LUA_FUNC_1ARG(string_ucharrange, 2)
	const char *text = luaL_checkstring(L, 1);
	int i = luaL_checknumber(L, 2);
	if(i < 1 || i > strlen(text))
		luaL_error2(L, "invalid character index");
	lua_pushnumber(L, uchar_width(text[i-1]));
	return 1;
LUA_FUNC_END

int uchar_iterator(lua_State *L){
	// Get upvalues
	const char *text = lua_tostring(L, lua_upvalueindex(1));
	unsigned int text_len = lua_tonumber(L, lua_upvalueindex(2));
	unsigned int text_pos = lua_tonumber(L, lua_upvalueindex(3));
	unsigned int char_index = lua_tonumber(L, lua_upvalueindex(4));
	// Check iterator abortion
	if(text_pos >= text_len)
		return 0;
	// Update upvalues
	unsigned int text_pos_new = text_pos + uchar_width(text[text_pos]);
	lua_pushnumber(L, text_pos_new);
	lua_replace(L, lua_upvalueindex(3));
	lua_pushnumber(L, ++char_index);
	lua_replace(L, lua_upvalueindex(4));
	// Return result
	lua_pushnumber(L, char_index);
	lua_pushlstring(L, text + text_pos, text_pos_new >= text_len ? text_len - text_pos : text_pos_new - text_pos);
	return 2;
}
LUA_FUNC_1ARG(string_uchars, 1)
	lua_pushnumber(L, strlen(luaL_checkstring(L, 1)));	// string + its length
	lua_pushnumber(L, 0);	// string byte position
	lua_pushnumber(L, 0);	// string character index (Lua version)
	lua_pushcclosure(L, uchar_iterator, 4);
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(string_ulen, 1)
	const char *text = luaL_checkstring(L, 1);
	unsigned int n = 0;
	for(const char *text_end = text + strlen(text); text < text_end; text += uchar_width(*text))
		++n;
	lua_pushnumber(L, n);
	return 1;
LUA_FUNC_END

void table_copy_deep(lua_State *L, unsigned int depth){
	// New table
	lua_newtable(L);
	// Iterate through old table
	lua_pushnil(L);
	while(lua_next(L, -3)){
		// Prepare next iteration pass
		lua_pushvalue(L, -2);
		lua_insert(L, -2);
		// Copy elements from old to new table
		if(lua_istable(L,-1) && depth > 1)
			table_copy_deep(L, depth-1);
		lua_rawset(L, -4);
	}
	// Remove old table
	lua_remove(L, -2);
}
LUA_FUNC_2ARG(table_copy, 1, 2)
	if(!lua_istable(L,1))
		luaL_typeerror(L, 1, "table");
	if(lua_gettop(L) == 1)
		table_copy_deep(L, 10);
	else{
		int depth = luaL_checknumber(L, 2);
		if(depth < 1)
			luaL_error2(L, "depth must be larger than zero");
		lua_pop(L, 1);
		table_copy_deep(L, depth);
	}
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(table_create, 2)
	int arraysize = luaL_checknumber(L, 1), mapsize = luaL_checknumber(L, 2);
	if(arraysize >= 0 && mapsize >= 0){
		lua_createtable(L, arraysize, mapsize);
		return 1;
	}else
		luaL_error2(L, "sizes have to be bigger-equal zero");
LUA_FUNC_END

int distro_func(lua_State *L){
	// Update upvalue
	unsigned int i = lua_tonumber(L, lua_upvalueindex(2));
	lua_pushnumber(L, i >= lua_rawlen(L, lua_upvalueindex(1)) ? 1 : i+1);
	lua_replace(L, lua_upvalueindex(2));
	// Return result
	lua_rawgeti(L, lua_upvalueindex(1), i);
	return 1;
}
LUA_FUNC_1ARG(table_distributor, 1)
	if(!lua_istable(L,1))
		luaL_typeerror(L, 1, "table");
	if(lua_rawlen(L, 1) < 1)
		luaL_error2(L, "table size must be larger zero");
	lua_pushnumber(L, 1);	// table index
	lua_pushcclosure(L, distro_func, 2);
	return 1;
LUA_FUNC_END

void table_tostring_process(lua_State *L, stringstream &stream, string prespace){
	// Iterate through table
	lua_pushnil(L);
	while(lua_next(L, -2)){
		// Add entry to buffer
		stream << prespace << '[' << '] = ';	// "%s[%s] = %s"

		#pragma message("WARNING: Implementation missing!")

		// Process tables recursively
		if(lua_istable(L, -1))
			table_tostring_process(L, stream, prespace + '\t');
		// Prepare next iteration pass
		lua_pop(L, 1);
	}
	// Remove table
	lua_remove(L, -1);
}
LUA_FUNC_1ARG(table_tostring, 1)
	if(!lua_istable(L,1))
		luaL_typeerror(L, 1, "table");
	stringstream stream;
	table_tostring_process(L, stream, "\t");
	string buf = stream.str();
	lua_pushlstring(L, buf.data(), buf.size());
	return 1;
LUA_FUNC_END

int luaopen_base2(lua_State *L){
	// Register global function 'print'
	LUA_REGISTER_FUNC(print)
	// Register math functions
	LUA_REGISTER_LIB_FUNC(math, bezier)
	LUA_REGISTER_LIB_FUNC(math, degree)
	LUA_REGISTER_LIB_FUNC(math, distance)
	LUA_REGISTER_LIB_FUNC(math, ellipse)
	LUA_REGISTER_LIB_FUNC(math, inrange)
	LUA_REGISTER_LIB_FUNC(math, interpolate)
	LUA_REGISTER_LIB_FUNC(math, ortho)
	LUA_REGISTER_LIB_FUNC(math, randomsteps)
	LUA_REGISTER_LIB_FUNC(math, round)
	LUA_REGISTER_LIB_FUNC(math, trim)
	// Register string functions
	LUA_REGISTER_LIB_FUNC(string, ucharrange)
	LUA_REGISTER_LIB_FUNC(string, uchars)
	LUA_REGISTER_LIB_FUNC(string, ulen)
	// Register table functions
	LUA_REGISTER_LIB_FUNC(table, copy)
	LUA_REGISTER_LIB_FUNC(table, create)
	LUA_REGISTER_LIB_FUNC(table, distributor)
	LUA_REGISTER_LIB_FUNC(table, tostring)
	// Nothing pushed to Lua state
	return 0;
}