-- Define convolution
local kernel = g2du.create_motion_blur_kernel(5, 170)

function GetFrame(frame, frame_i)
	-- Draw test line
	local ctx = g2d.create_context(frame)
	ctx:set_source(g2du.black)
	ctx:paint()
	ctx:set_source(g2du.red)
	ctx:path_move_to(300, 300)
	ctx:path_line_to(400, 300)
	ctx:set_line_width(6)
	ctx:set_line_cap("SQUARE")
	ctx:path_stroke()
	-- Test convolution
	g2d.image_convolution(frame, kernel)
end