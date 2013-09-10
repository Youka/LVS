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