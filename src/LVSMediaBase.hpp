#pragma once

#include "lua.hpp"

// Base class for LVS video&audio class
class LVSMediaBase{
	protected:
		// Lua environment
		Lua L;
	public:
		// Initialize Lua environment
		LVSMediaBase();
};