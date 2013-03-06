#pragma once

#include <lua.hpp>

// Lua instance
class Lua{
	public:
		// Allocate Lua instance and open all standard libraries
		Lua(){
			this->L = luaL_newstate();
			luaL_openlibs(this->L);
		}
		// Free Lua instance
		~Lua(){
			lua_close(this->L);
		}
		// Access to Lua instance
		operator lua_State*(){return this->L;}
	private:
		lua_State *L;
};

// Lua debug functions
static void luaL_error2(lua_State *L, const char *msg = 0){
	if(!msg){
		lua_Debug ar;
		lua_getstack(L, 0, &ar);
		lua_getinfo(L, "n", &ar);
		luaL_error(L, "Bad arguments to function '%s'!", ar.name);
	}else
		luaL_error(L, msg);
}

static void luaL_typerror(lua_State *L, int narg, const char *expected){
	const char *msg = lua_pushfstring(L, "%s expected, got %s", expected, luaL_typename(L, narg));
	luaL_argerror(L, narg, msg);
}

// Lua boolean functions
static int luaL_checkboolean(lua_State *L, int i){
	if(!lua_isboolean(L,i))
		luaL_typerror(L,i,"boolean");
	return lua_toboolean(L,i);
}

static int luaL_optboolean(lua_State *L, int i, int d){
	if(lua_type(L, i) < 1)
		return d;
	else
		return luaL_checkboolean(L, i);
}

// Lua userdata functions
static void *luaL_checkuserdata(lua_State *L, int i, const char* type){
	void *ud = luaL_checkudata(L, i, type);
	luaL_argcheck(L, ud, i, "wrong userdata");
	return ud;
}

template <class T> static T *lua_createuserdata(lua_State *L, const char* meta_name){
		T *ud = reinterpret_cast<T*>(lua_newuserdata(L, sizeof(T)));
		luaL_newmetatable(L, meta_name);
		lua_setmetatable(L, -2);
		return ud;
}

// Lua table functions
template <class T> static T *luaL_checktable(lua_State *L, int i){
	if(!lua_istable(L,i))
		luaL_typerror(L,i,"table");
	size_t len = lua_objlen(L,i);

	if(len > 0){
		T *table = new T[len];
		for(int ii = 1; ii <= len; ii++){
			lua_rawgeti(L, i, ii);
			luaL_argcheck(L, lua_isnumber(L,-1), i, reinterpret_cast<char*>("invalid table"));
			table[ii-1] = lua_tonumber(L, -1);
			lua_pop(L,1);
		}
		return table;
	}else
		return 0;
}

template <class T> static void lua_pushtable(lua_State *L, T *t, unsigned long int len){
	lua_createtable(L, len, 0);
	for (unsigned int i = 0; i < len; i++){
		lua_pushnumber(L, t[i]);
		lua_rawseti(L, -2, i+1);
	}
}

// Lua macros
#define LUA_REGISTER_FUNC(func) lua_register(L, #func, l_##func)
#define LUA_REGISTER_VAR(var) lua_pushnumber(L, var); lua_setglobal(L, #var)
#define LUA_FUNC(func) int l_##func(lua_State *L)
#define LUA_FUNC_1ARG(func, param_n) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) == param_n){
#define LUA_FUNC_2ARG(func, param_n1, param_n2) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) == param_n1 || lua_gettop(L) == param_n2){
#define LUA_FUNC_3ARG(func, param_n1, param_n2, param_n3) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) == param_n1 || lua_gettop(L) == param_n2 || lua_gettop(L) == param_n3){
#define LUA_FUNC_NARG(func, param_n) \
int l_ ## func (lua_State *L){ \
	if(lua_gettop(L) != param_n){
#define LUA_FUNC_END \
	}else \
		luaL_error2(L); \
	return 0; \
}