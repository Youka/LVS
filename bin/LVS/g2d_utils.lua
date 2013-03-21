-- G2D utility library
g2du = {
	-- Create box blur kernel
	create_box_blur_kernel = function(strength)
		if type(strength) ~= "number" or strength < 1 or math.floor(strength) ~= strength then
			error("valid number expected", 2)
		end
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		local kernel_value = 1 / kernel_size
		for i=1, kernel_size do
			kernel[i] = kernel_value
		end
		return kernel
	end,
	-- Create motion blur kernel
	create_motion_blur_kernel = function(strength, angle)
		if type(strength) ~= "number" or strength < 1 or math.floor(strength) ~= strength or
			type(angle) ~= "number" then
			error("valid numbers expected", 2)
		end
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		for i=1, kernel_size do
			kernel[i] = 0
		end
		local kernel_value = 1 / kernel_wide
		local m = math.cos(math.rad(angle)) / math.sin(math.rad(angle))
		m = m == 1/0 and math.huge or m - m % 0.001
		if m > 1 or m < -1 then
			for y = -strength, strength do
				kernel[1 + (-y+strength)*kernel_wide + math.floor(y / m)+strength] = kernel_value
			end
		else
			for x = -strength, strength do
				kernel[1 + (-math.floor(x*m)+strength)*kernel_wide + x+strength] = kernel_value
			end
		end
		return kernel
	end,
	-- Create gaussian blur kernel
	create_gaussian_blur_kernel = function(strength)
		if type(strength) ~= "number" or strength < 1 or math.floor(strength) ~= strength then
			error("valid number expected", 2)
		end
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		local sigma_sqr2 = 2 * strength * strength
		local sigma_sqr2pi = sigma_sqr2 * math.pi
		local x, y
		local sum = 0
		for i=1, kernel_size do
			x, y = (i-1) % kernel_wide - strength, math.floor((i-1) / kernel_wide) - strength
			kernel[i] = math.exp(-(x*x+y*y) / sigma_sqr2) / sigma_sqr2pi
			sum = sum + kernel[i]
		end
		for i=1, kernel_size do
			kernel[i] = kernel[i] / sum
		end
		return kernel
	end,
	-- Create sharpen kernel
	create_sharpen_kernel = function(strength)
		if type(strength) ~= "number" or strength < 1 or math.floor(strength) ~= strength then
			error("valid number expected", 2)
		end
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		for i=1, kernel_size do
			if i == math.ceil(kernel_size/2) then
				kernel[i] = kernel_size
			else
				kernel[i] = -1
			end
		end
		return kernel
	end,
	-- Create edge detection kernel
	create_edge_detect_kernel = function()
		return {
			width = 3, height = 3,
			-1, -1, -1,
			-1, 8, -1,
			-1, -1, -1
		}
	end,
	-- Stock matrices
	identify = g2d.create_matrix(),
	-- Stock color sources
	white = g2d.create_source_color(1, 1, 1),
	black = g2d.create_source_color(0, 0, 0),
	grey = g2d.create_source_color(0.5, 0.5, 0.5),
	dark_grey = g2d.create_source_color(0.25, 0.25, 0.25),
	bright_grey = g2d.create_source_color(0.75, 0.75, 0.75),
	red = g2d.create_source_color(1, 0, 0),
	dark_red = g2d.create_source_color(0.5, 0, 0),
	bright_red = g2d.create_source_color(1, 0.5, 0.5),
	green = g2d.create_source_color(0, 1, 0),
	dark_green = g2d.create_source_color(0, 0.5, 0),
	bright_green = g2d.create_source_color(0.5, 1, 0.5),
	blue = g2d.create_source_color(0, 0, 1),
	dark_blue = g2d.create_source_color(0, 0, 0.5),
	bright_blue = g2d.create_source_color(0.5, 0.5, 1),
	yellow = g2d.create_source_color(1, 1, 0),
	dark_yellow = g2d.create_source_color(0.5, 0.5, 0),
	bright_yellow = g2d.create_source_color(1, 1, 0.5),
	aqua = g2d.create_source_color(0, 1, 1),
	dark_aqua = g2d.create_source_color(0, 0.5, 0.5),
	bright_aqua = g2d.create_source_color(0.5, 1, 1),
	purple = g2d.create_source_color(1, 0, 1),
	dark_purple = g2d.create_source_color(0.5, 0, 0.5),
	bright_purple = g2d.create_source_color(1, 0.5, 1),
	orange = g2d.create_source_color(1, 0.5, 0),
	lime = g2d.create_source_color(0.5, 1, 0),
	spring = g2d.create_source_color(0, 1, 0.5),
	azure = g2d.create_source_color(0, 0.5, 1),
	pink = g2d.create_source_color(1, 0, 0.5),
	violet = g2d.create_source_color(0.5, 0, 1),
	brown = g2d.create_source_color(0.5, 0.25, 0.125),
	sapphire = g2d.create_source_color(0.125, 0.25, 0.5),
	-- Path transformation (points of tiny outline segments)
	path_transform = function(ctx, filter)
		if getmetatable(ctx) ~= "g2d context" or type(filter) ~= "function" then
			error("g2d context and function expected", 2)
		end
		-- Calculate new path
		local new_path, new_path_n = {}, 0
		local success, err = pcall(function()
			local prev_point, prev_move_point
			ctx:path_transform(function(typ, x, y)
				if typ == "move" or (typ == "line" and not prev_point) then
					local x2, y2 = filter(x, y)
					if type(x2) ~= "number" or type(y2) ~= "number" then
						error("filter function must return 2 numbers", 0)
					end
					new_path_n = new_path_n + 1
					new_path[new_path_n] = {typ = typ, x = x2, y = y2}
					prev_point = {x = x, y = y}
					if typ == "move" then
						prev_move_point = prev_point
					end
				elseif typ == "line" then
					local vec = {x = x-prev_point.x, y = y-prev_point.y}
					local len = math.sqrt(vec.x*vec.x + vec.y*vec.y)
					local offset = len % 2
					if offset == 0 then offset = 2 end
					local pct, x2, y2
					for cur_len = offset, len, 2 do
						pct = cur_len / len
						x2, y2 = filter(prev_point.x + pct*vec.x, prev_point.y + pct*vec.y)
						if type(x2) ~= "number" or type(y2) ~= "number" then
							error("filter function must return 2 numbers", 0)
						end
						new_path_n = new_path_n + 1
						new_path[new_path_n] = {typ = typ, x = x2, y = y2}
					end
					prev_point = {x = x, y = y}
				elseif prev_move_point and prev_point then	-- close
					local vec = {x = prev_move_point.x - prev_point.x, y = prev_move_point.y - prev_point.y}
					local len = math.sqrt(vec.x*vec.x + vec.y*vec.y)
					local offset = len % 2
					if offset == 0 then offset = 2 end
					local pct, x2, y2
					for cur_len = offset, len, 2 do
						pct = cur_len / len
						x2, y2 = filter(prev_point.x + pct*vec.x, prev_point.y + pct*vec.y)
						if type(x2) ~= "number" or type(y2) ~= "number" then
							error("filter function must return 2 numbers")
						end
						new_path_n = new_path_n + 1
						new_path[new_path_n] = {typ = "line", x = x2, y = y2}
					end
					new_path_n = new_path_n + 1
					new_path[new_path_n] = {typ = typ}
				end
				return x, y
			end, true)
		end)
		if not success then
			error(err:gsub(".*:%d+: (.*)", "%1", 1), 2)
		end
		-- Replace old with new path
		ctx:path_clear()
		local segment
		for i=1, new_path_n do
			segment = new_path[i]
			if segment.typ == "move" then
				ctx:path_move_to(segment.x, segment.y)
			elseif segment.typ == "line" then
				ctx:path_line_to(segment.x, segment.y)
			else -- segment.typ == "close"
				ctx:path_close()
			end
		end
	end,
	rotate_xy = function(mat, x, y)
		if getmetatable(mat) ~= "g2d matrix" or type(x) ~= "number" or type(y) ~= "number" then
			error("g2d matrix, number and number expected", 2)
		end
		-- Create 3D matrices for x and y rotation

		-- TODO

		-- Convert 3D to 2D matrices and multiply with current matrix

		-- TODO

	end
}