-- Create blurred white point source
local source
do
	-- Create image with enough space
	local image = g2d.create_image("RGBA", 120, 120)
	-- Draw white point
	local ctx = g2d.create_context(image)
	ctx:path_add_arc(60,60,50,0,360)
	ctx:set_source(g2du.white)
	ctx:path_fill()
	-- Blur image/point
	g2d.image_convolution(image, g2du.create_gaussian_blur_kernel(10))
	-- Convert image to source
	source = g2d.create_source_image(image)
	-- Set source offset
	source:set_matrix(g2d.create_matrix(1, 0, 0, 1, 300, 300))
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Paint blurred white point over frame
	ctx:set_source(source)
	ctx:paint()
end