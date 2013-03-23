-- Create PNG source
local source = g2d.create_source_image(g2d.create_image_from_png("check.png"))

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Paint PNG over frame
	source:set_matrix(g2d.create_matrix(1, 0, 0, 1, 300, 200))
	ctx:set_source(source)
	ctx:paint()
end