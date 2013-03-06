#pragma once

#include "lua.hpp"

// Base class for LVS video&audio class
class LVSMediaBase{
	private:
		// Lua environment
		Lua L;
	public:
		// Initialize Lua environment
		LVSMediaBase();
};