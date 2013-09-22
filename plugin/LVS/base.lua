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