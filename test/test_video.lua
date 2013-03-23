-- Process frames
function GetFrame(frame, frame_i)
	-- Create text image
	local text_image = g2d.create_image("RGB", 150,150)
	local text_ctx = g2d.create_context(text_image)
	local red_arc = g2d.create_source_radial_gradient(20,65,0, 20,65,40):add_color(0,1,0,0,1):add_color(1,1,0,0,0)
	local green_arc = g2d.create_source_radial_gradient(70,90,0, 70,90,40):add_color(0,0,1,0,1):add_color(1,0,1,0,0)
	local blue_arc = g2d.create_source_radial_gradient(130,60,0, 130,60,40):add_color(0,0,0,1,1):add_color(1,0,0,1,0)
	text_ctx:set_matrix(g2d.create_matrix():translate(6,23))
	text_ctx:path_add_text("LVS", "Comic Sans MS", 100, true, false, false, false)
	text_ctx:set_matrix(g2du.identity)
	text_ctx:set_composition("ADD")
	for i=1, 3 do
		text_ctx:set_source(red_arc)
		text_ctx:path_fill()
		text_ctx:set_source(green_arc)
		text_ctx:path_fill()
		text_ctx:set_source(blue_arc)
		text_ctx:path_fill()
		if i<3 then
			g2d.image_convolution(text_image, g2du.create_motion_blur_kernel(8,45))
		end
	end
	-- Output
	g2d.create_png_from_image(text_image, "E:\\PROJEKTE\\LVS\\test\\LVS.png")
end