-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = frame:get_context()
	-- Create 3D rotation matrix for frame
	local mat = g2d.create_matrix()
	mat:translate(VIDEO_WIDTH/2,VIDEO_HEIGHT/2)
	g2du.rotate_xy(mat,frame_i, frame_i*2)
	mat:scale(0.5,0.5)
	mat:translate(-VIDEO_WIDTH/2,-VIDEO_HEIGHT/2)
	-- Create rotated frame source
	local frame_src = g2d.create_pattern(g2du.create_converted_image(frame, "RGB"))
	frame_src:set_matrix(mat)
	-- Paint frame source
	ctx:set_source(frame_src)
	ctx:paint()
end