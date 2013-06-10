function math.bezier(pct, p)
	if type(pct) ~= "number" or type(p) ~= "table" then
		error("number and table expected", 2)
	elseif pct < 0 or pct > 1 or #p < 2 then
		error("invalid arguments", 2)
	end
	for i, v in ipairs(p) do
		if type(v[1]) ~= "number" or type(v[2]) ~= "number" or (type(v[3]) ~= "number" and type(v[3]) ~= "nil") then
			error("invalid table content", 2)
		end
	end
	--Factorial
	local function fac(n)
		local k = 1
		if n > 1 then
			for i=2, n do
				k = k * i
			end
		end
		return k
	end
	--Binomial coefficient
	local function bin(i, n)
		return fac(n) / (fac(i) * fac(n-i))
	end
	--Bernstein polynom
	local function bernstein(pct, i, n)
		return bin(i, n) * pct^i * (1 - pct)^(n - i)
	end
	--Calculate coordinate
	local point = {0, 0, 0}
	local n = #p - 1
	for i=0, n do
		local bern = bernstein(pct, i, n)
		point[1] = point[1] + p[i+1][1] * bern
		point[2] = point[2] + p[i+1][2] * bern
		point[3] = point[3] + (p[i+1][3] or 0) * bern
	end
	return point
end

function math.degree(vec1, vec2)
	if type(vec1) ~= "table" or type(vec2) ~= "table" then
		error("table and table expected", 2)
	end
	if type(vec1[1]) == "number" and type(vec1[2]) == "number" and
		type(vec2[1]) == "number" and type(vec2[2]) == "number" then
		local degree
		if type(vec1[3]) == "number" and type(vec2[3]) == "number" then
			-- 3D variant
			degree = math.deg(
				math.acos(
					(vec1[1] * vec2[1] + vec1[2] * vec2[2] + vec1[3] * vec2[3]) /
					(math.distance(vec1[1], vec1[2], vec1[3]) * math.distance(vec2[1], vec2[2], vec2[3]))
				)
			)
		else
			-- 2D variant
			degree = math.deg(
				math.acos(
					(vec1[1] * vec2[1] + vec1[2] * vec2[2]) /
					(math.distance(vec1[1], vec1[2]) * math.distance(vec2[1], vec2[2]))
				)
			)
		end
		-- Return result
		return	(vec1[1]*vec2[2] - vec1[2]*vec2[1]) < 0 and -degree or degree
	else
		error("invalid table content", 2)
	end
end

function math.distance(w, h, d)
	if type(w) ~= "number" or type(h) ~= "number" or (type(d) ~= "number" and type(d) ~= "nil") then
		error("number, number and optional number expected", 2)
	end
	if d then
		return math.sqrt(w^2 + h^2 + d^2)
	else
		return math.sqrt(w^2 + h^2)
	end
end

function math.ellipse(x, y, w, h, a)
	if type(x) ~= "number" or
	type(y) ~= "number" or
	type(w) ~= "number" or
	type(h) ~= "number" or
	type(a) ~= "number" then
		error("number, number, number, number and number expected", 2)
	end
	local ra = math.rad(a)
	return x + w/2 * math.sin(ra), y + h/2 * math.cos(ra)
end

function math.inrange(a, c, b)
	if type(a) ~= "number" or type(c) ~= "number" or type(b) ~= "number" then
		error("number, number and number expected", 2)
	end
	if b <= c and b >= a then
		return (b-a) / (c-a)
	end
end

function math.interpolate(a, b, pct)
	if type(a) ~= "number" or type(b) ~= "number" or type(pct) ~= "number" then
		error("number, number and number expected", 2)
	end
	return a + (b-a) * pct
end

function math.ortho(vec1, vec2)
	if type(vec1) ~= "table" or type(vec2) ~= "table" then
		error("table and table expected", 2)
	elseif type(vec1[1]) ~= "number" or type(vec1[2]) ~= "number" or type(vec1[3]) ~= "number" or
			type(vec2[1]) ~= "number" or type(vec2[2]) ~= "number" or type(vec2[3]) ~= "number" then
		error("invalid table content", 2)
	end
	return {
		vec1[2] * vec2[3] - vec1[3] * vec2[2],
		vec1[3] * vec2[1] - vec1[1] * vec2[3],
		vec1[1] * vec2[2] - vec1[2] * vec2[1]
	}
end

function math.randomsteps(start, ends, step)
	if type(start) ~= "number" or
	type(ends) ~= "number" or
	type(step) ~= "number" or
	start > ends or
	step <= 0 then
		error("valid number, number and number expected", 2)
	end
	return start + math.random(0, math.floor((ends - start) / step)) * step
end

function math.round(num)
	if type(num) ~= "number" then
		error("number expected", 2)
	end
	return math.floor(num + 0.5)
end

function math.trim(num, starts, ends)
	if type(starts) ~= "number" or
	type(ends) ~= "number" or
	type(num) ~= "number" then
		error("number, number and number expected", 2)
	end
	return (num < starts and starts) or (num > ends and ends) or num
end

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