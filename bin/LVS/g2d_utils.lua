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
		m = m - m % 0.001
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
	
		-- TODO
		
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
				kernel[i] = -1
			else
				kernel[i] = kernel_size
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
	-- Create emboss kernel
	create_emboss_kernel = function(strength, angle)
		if type(strength) ~= "number" or strength < 1 or math.floor(strength) ~= strength or
			type(angle) ~= "number" then
			error("valid numbers expected", 2)
		end
		
		-- TODO
	
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
	-- Path transformation (in tiny segments)
	path_transform = function(ctx, filter)
		
		-- TODO
	
	end
}