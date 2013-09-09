--[[
UTF16 -> UTF8
--------------
U-00000000 EU-0000007F: 	0xxxxxxx
U-00000080 EU-000007FF: 	110xxxxx 10xxxxxx
U-00000800 EU-0000FFFF: 	1110xxxx 10xxxxxx 10xxxxxx
U-00010000 EU-001FFFFF: 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
U-00200000 EU-03FFFFFF: 	111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
U-04000000 EU-7FFFFFFF: 	1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
]]
function string.ucharrange(s, i)
	if type(s) ~= "string" or type(i) ~= "number" or i < 1 or i > s:len() then
		error("string and valid number expected", 2)
	end
	local byte = s:byte(i)
	return not byte and 0 or
			byte < 192 and 1 or
			byte < 224 and 2 or
			byte < 240 and 3 or
			byte < 248 and 4 or
			byte < 252 and 5 or
			6
end

function string.uchars(s)
	if type(s) ~= "string" then
		error("string expected", 2)
	end
	local char_i, s_pos = 0, 1
	return function()
		if s_pos > s:len() then
			return nil
		end
		local cur_pos = s_pos
		s_pos = s_pos + s:ucharrange(s_pos)
		char_i = char_i + 1
		return char_i, s:sub(cur_pos, s_pos-1)
	end
end

function string.ulen(s)
	if type(s) ~= "string" then
		error("string expected", 2)
	end
	local n = 0
	for _ in s:uchars() do
		n = n + 1
	end
	return n
end

function table.copy(old_t)
	if type(old_t) ~= "table" then
		error("table expected", 2)
	end
	local new_t = {}
	for index, value in pairs(old_t) do
		if type(value) == "table" then
		    new_t[index] = table.copy(value)
		else
            new_t[index] = value
		end
	end
	return new_t
end

function table.distributor(t)
	if type(t) ~= "table" or #t < 1 then
		error("table expected (not empty)", 2)
	end
	local func_t = {}
	func_t.__index = func_t
	local index = 1
	function func_t:get()
		if index > #self then index = 1 end
		local val = self[index]
		index = index + 1
		return val
	end
	return setmetatable(t, func_t)
end

function table.tostring(t)
	if type(t) ~= "table" then
		error("table expected", 2)
	end
	local result, result_n = {tostring(t)}, 1
	local function table_print(t, space)
		for i, v in pairs(t) do
			result_n = result_n + 1
			result[result_n] = string.format("%s[%s] = %s", space,
															type(i) == "string" and string.format("%q", i) or tostring(i),
															type(v) == "string" and string.format("%q", v) or tostring(v))
			if type(v) == "table" then
				table_print(v, space .. "\t")
			end
		end
	end
	table_print(t, "\t")
	return table.concat(result, "\n")
end