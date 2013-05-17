-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Create 3D rotation matrix for frame
	local mat = g2d.create_matrix()
	mat:translate(VIDEO_WIDTH/2,VIDEO_HEIGHT/2)
	g2du.rotate_xy(mat,frame_i, frame_i*2)
	mat:scale(0.5,0.5)
	mat:translate(-VIDEO_WIDTH/2,-VIDEO_HEIGHT/2)
	-- Create a frame source
	local frame_src = g2d.create_source_image(g2du.create_sub_image(frame, "RGB", 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT))
	-- Set context matrix and source and paint
	ctx:set_matrix(mat)
	ctx:set_source(frame_src)
	ctx:paint()
end