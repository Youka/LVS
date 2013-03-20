-- Create blurred white rectangle source
local source
do
	-- Create image with enough space
	local image = g2d.create_image("RGBA", 120, 120)
	-- Draw white rectangle
	local ctx = g2d.create_context(image)
	ctx:path_add_rectangle(10, 10, 100, 100)
	ctx:set_source(g2du.white)
	ctx:path_fill()
	-- Blur image/rectangle
	g2d.image_convolution(image, g2du.create_motion_blur_kernel(10,0))
	-- Convert image to source
	source = g2d.create_source_image(image)
	-- Set source offset
	source:set_matrix(g2d.create_matrix(1, 0, 0, 1, 300, 300))
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Paint blurred white rectangle over frame
	ctx:set_source(source)
	ctx:paint()
end