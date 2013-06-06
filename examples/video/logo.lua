-- Create logo image
local logo_image = g2d.create_image("RGB", 150,150)
do	-- (separate block to garbage-collect drawing objects)
	-- Create logo image context
	local logo_ctx = logo_image:get_context()
	-- Create radial gradients
	local red_arc = g2d.create_rgradient(20,65,0, 20,65,40):add_color(0,1,0,0,1):add_color(1,1,0,0,0)
	local green_arc = g2d.create_rgradient(70,90,0, 70,90,40):add_color(0,0,1,0,1):add_color(1,0,1,0,0)
	local blue_arc = g2d.create_rgradient(130,60,0, 130,60,40):add_color(0,0,0,1,1):add_color(1,0,0,1,0)
	-- Create motion blur kernel
	local blur_kernel = g2du.create_motion_blur_kernel(8,45)
	-- Define text path
	logo_ctx:path_add_text(6, 23, "LVS", "Comic Sans MS", 100, true, false, false, false)
	-- Set composition
	logo_ctx:set_composition("ADD")
	-- Draw 3 times
	for i=1, 3 do
		-- Draw gradients in path
		logo_ctx:set_source(red_arc)
		logo_ctx:fill()
		logo_ctx:set_source(green_arc)
		logo_ctx:fill()
		logo_ctx:set_source(blue_arc)
		logo_ctx:fill()
		-- Apply motion blur on first 2 passes
		if i<3 then
			logo_image:convolute(blur_kernel)
		end
	end
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local frame_ctx = frame:get_context()
	-- Draw logo onto frame
	frame_ctx:set_source(g2d.create_pattern(logo_image))
	frame_ctx:paint()
end