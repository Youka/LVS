-- G2D utility library
g2du = {
	-- Stock matrices
	identity = g2d.create_matrix(),
	flip_h = g2d.create_matrix(-1,0,0,1,0,0),
	flip_v = g2d.create_matrix(1,0,0,-1,0,0),
	-- Stock color sources
	white = g2d.create_color(1, 1, 1),
	black = g2d.create_color(0, 0, 0),
	grey = g2d.create_color(0.5, 0.5, 0.5),
	dark_grey = g2d.create_color(0.25, 0.25, 0.25),
	bright_grey = g2d.create_color(0.75, 0.75, 0.75),
	red = g2d.create_color(1, 0, 0),
	dark_red = g2d.create_color(0.5, 0, 0),
	bright_red = g2d.create_color(1, 0.5, 0.5),
	green = g2d.create_color(0, 1, 0),
	dark_green = g2d.create_color(0, 0.5, 0),
	bright_green = g2d.create_color(0.5, 1, 0.5),
	blue = g2d.create_color(0, 0, 1),
	dark_blue = g2d.create_color(0, 0, 0.5),
	bright_blue = g2d.create_color(0.5, 0.5, 1),
	yellow = g2d.create_color(1, 1, 0),
	dark_yellow = g2d.create_color(0.5, 0.5, 0),
	bright_yellow = g2d.create_color(1, 1, 0.5),
	aqua = g2d.create_color(0, 1, 1),
	dark_aqua = g2d.create_color(0, 0.5, 0.5),
	bright_aqua = g2d.create_color(0.5, 1, 1),
	purple = g2d.create_color(1, 0, 1),
	dark_purple = g2d.create_color(0.5, 0, 0.5),
	bright_purple = g2d.create_color(1, 0.5, 1),
	orange = g2d.create_color(1, 0.5, 0),
	lime = g2d.create_color(0.5, 1, 0),
	spring = g2d.create_color(0, 1, 0.5),
	azure = g2d.create_color(0, 0.5, 1),
	pink = g2d.create_color(1, 0, 0.5),
	violet = g2d.create_color(0.5, 0, 1),
	brown = g2d.create_color(0.5, 0.25, 0.125),
	sapphire = g2d.create_color(0.125, 0.25, 0.5),
	transparent = g2d.create_color(0, 0, 0, 0),
	-- Create color-converted image
	create_converted_image = function(image, color_type)
		-- Check parameters type
		if getmetatable(image) ~= "g2d image" or type(color_type) ~= "string" then
			error("g2d image and string expected", 2)
		end
		-- Check color type
		if color_type ~= "RGBA" and color_type ~= "RGB" and color_type ~= "ALPHA" and color_type ~= "BINARY" then
			error("valid color type expected", 2)
		end
		-- Create new image
		local new_image = g2d.create_image(color_type, image:get_width(), image:get_height())
		-- Draw old image on new one
		local ctx = new_image:get_context()
		ctx:set_composition("SOURCE")
		ctx:set_source(g2d.create_pattern(image))
		ctx:paint()
		-- Return new image
		return new_image
	end,
	-- Create scaled image
	create_scaled_image = function(image, w, h)
		-- Check parameters type
		if getmetatable(image) ~= "g2d image" or type(w) ~= "number" or type(h) ~= "number" then
			error("g2d image, number and number expected", 2)
		end
		-- Check image dimension
		w, h = math.floor(w), math.floor(h)
		if w <= 0 or h <= 0 then
			error("invalid image dimension", 2)
		end
		-- Create new image
		local new_image = g2d.create_image(image:get_format(), w, h)
		-- Draw old image on new one
		local ctx = new_image:get_context()
		local src = g2d.create_pattern(image)
		src:set_matrix(g2d.create_matrix(w / image:get_width(), 0, 0, h / image:get_height(), 0, 0))
		src:set_filter("BEST")
		ctx:set_composition("SOURCE")
		ctx:set_source(src)
		ctx:paint()
		-- Return new image
		return new_image
	end,
	-- Create sub-image
	create_sub_image = function(image, x0, y0, x1, y1)
		-- Check parameters type
		if getmetatable(image) ~= "g2d image" or type(x0) ~= "number" or type(y0) ~= "number" or type(x1) ~= "number" or type(y1) ~= "number" then
			error("g2d image and 4 numbers expected", 2)
		end
		-- Check image area
		x0, y0, x1, y1 = math.floor(x0), math.floor(y0), math.ceil(x1), math.ceil(y1)
		if x0 < 0 or y0 < 0 or x1 > image:get_width() or y1 > image:get_height() or x0 >= x1 or y0 >= y1 then
			error("invalid image area", 2)
		end
		-- Create new image
		local new_image = g2d.create_image(image:get_format(), x1 - x0, y1 - y0)
		-- Draw old image on new one
		local ctx = new_image:get_context()
		ctx:set_composition("SOURCE")
		ctx:set_matrix(g2d.create_matrix(1,0,0,1,-x0,-y0))
		ctx:set_source(g2d.create_pattern(image))
		ctx:paint()
		-- Return new image
		return new_image
	end,
	-- Extract image pixels
	get_pixels = function(image)
		if getmetatable(image) ~= "g2d image" then
			error("g2d image expected", 2)
		end
		-- Get image data
		local w, h = image:get_width(),image:get_height()
		local color_type = image:get_format()
		local data = image:get_data(0,0,w,h)
		-- Extract pixels by color type
		if color_type == "RGBA" then
			-- Count pixels number
			local n = 0
			for i=1, w*h*4, 4 do
				if data[i+3] > 0 then
					n = n + 1
				end
			end
			-- Save & return pixels
			local pixels, pixels_n = table.create(n,0), 0
			local pixel_i
			for i=1, w*h*4, 4 do
				if data[i+3] > 0 then
					pixel_i = (i-1) / 4
					pixels_n = pixels_n + 1
					pixels[pixels_n] = {r = data[i] / 255, g = data[i+1] / 255, b = data[i+2] / 255, a = data[i+3] / 255, x = pixel_i % w, y = math.floor(pixel_i / w)}
				end
			end
			return pixels
		elseif color_type == "RGB" then
			-- Count pixels number
			local n = 0
			for i=1, w*h*3, 3 do
				if not (data[i] == 0 and data[i+1] == 0 and data[i+2] == 0) then
					n = n + 1
				end
			end
			-- Save & return pixels
			local pixels, pixels_n = table.create(n,0), 0
			local pixel_i
			for i=1, w*h*3, 3 do
				if not (data[i] == 0 and data[i+1] == 0 and data[i+2] == 0) then
					pixel_i = (i-1) / 3
					pixels_n = pixels_n + 1
					pixels[pixels_n] = {r = data[i] / 255, g = data[i+1] / 255, b = data[i+2] / 255, x = pixel_i % w, y = math.floor(pixel_i / w)}
				end
			end
			return pixels
		else	-- "ALPHA" or "BINARY"
			-- Count pixels number
			local n = 0
			for i=1, w*h do
				if data[i] > 0 then
					n = n + 1
				end
			end
			-- Save & return pixels
			local pixels, pixels_n = table.create(n,0), 0
			for i=1, w*h do
				if data[i] > 0 then
					pixels_n = pixels_n + 1
					pixels[pixels_n] = {a = data[i] / 255, x = (i-1) % w, y = math.floor((i-1) / w)}
				end
			end
			return pixels
		end
	end,
	-- Create box blur kernel
	create_box_blur_kernel = function(strength)
		-- Check parameter
		if type(strength) ~= "number" or strength < 1 then
			error("valid number expected", 2)
		end
		strength = math.floor(strength)
		-- Create kernel table
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		-- Fill kernel
		local kernel_value = 1 / kernel_size
		for i=1, kernel_size do
			kernel[i] = kernel_value
		end
		return kernel
	end,
	-- Create motion blur kernel
	create_motion_blur_kernel = function(strength, angle)
		-- Check parameters
		if type(strength) ~= "number" or strength < 1 or type(angle) ~= "number" then
			error("valid 2 numbers expected", 2)
		end
		strength = math.floor(strength)
		-- Create kernel table
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		-- Fill kernel
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
		-- Check parameter
		if type(strength) ~= "number" or strength < 1 then
			error("valid number expected", 2)
		end
		strength = math.floor(strength)
		-- Create kernel table
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		-- Fill kernel
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
		-- Check parameter
		if type(strength) ~= "number" or strength < 1 then
			error("valid number expected", 2)
		end
		strength = math.floor(strength)
		-- Create kernel table
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		-- Fill kernel
		local mid_i = math.ceil(kernel_size/2)
		for i=1, kernel_size do
			if i == mid_i then
				kernel[i] = kernel_size
			else
				kernel[i] = -1
			end
		end
		return kernel
	end,
	-- Create edge detection kernel
	create_edge_detect_kernel = function(strength)
		-- Check parameter
		if type(strength) ~= "number" or strength < 1 then
			error("valid number expected", 2)
		end
		strength = math.floor(strength)
		-- Create kernel table
		local kernel_wide = 1 + 2*strength
		local kernel_size = kernel_wide * kernel_wide
		local kernel = table.create(kernel_size, 2)
		kernel.width = kernel_wide
		kernel.height = kernel_wide
		-- Fill kernel
		local mid_i = math.ceil(kernel_size/2)
		for i=1, kernel_size do
			if i == mid_i then
				kernel[i] = kernel_size-1
			else
				kernel[i] = -1
			end
		end
		return kernel
	end,
	-- Matrix 3D manipulation (rotate x+y)
	rotate_xy = function(mat, x, y)
		if getmetatable(mat) ~= "g2d matrix" or type(x) ~= "number" or type(y) ~= "number" then
			error("g2d matrix, number and number expected", 2)
		end
		-- Convert angles to radians
		x, y = math.rad(x), math.rad(y)
		-- Apply xy-rotation to matrix
		return mat:multiply(math.cos(y), 0, math.sin(x)*math.sin(y), math.cos(x), 0, 0)
	end,
	-- Matrix 3D manipulation (rotate y+x)
	rotate_yx = function(mat, x, y)
		if getmetatable(mat) ~= "g2d matrix" or type(x) ~= "number" or type(y) ~= "number" then
			error("g2d matrix, number and number expected", 2)
		end
		-- Convert angles to radians
		x, y = math.rad(x), math.rad(y)
		-- Apply yx-rotation to matrix
		return mat:multiply(math.cos(y), -math.sin(x) * -math.sin(y), 0, math.cos(x), 0, 0)
	end,
	-- Path transformation (points of tiny outline segments)
	path_transform = function(ctx, filter)
		if getmetatable(ctx) ~= "g2d context" or type(filter) ~= "function" then
			error("g2d context and function expected", 2)
		end
		-- Calculate new path
		local new_path, new_path_n = {}, 0
		local success, err = pcall(function()
			-- Previous path points storage
			local prev_point, prev_move_point
			-- Iterate through flatten path
			ctx:path_transform(function(typ, x, y)
				-- Move: filter point and save him
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
				-- Line: split line, filter points and save last
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
				-- Close: same like line but by closing path
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
		-- Error by path iteration?
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
	end
}