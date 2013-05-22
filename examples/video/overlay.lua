-- Create PNG source
local source = g2d.create_pattern(g2d.create_image_from_png("video\\check.png"))
source:set_matrix(g2d.create_matrix():translate(300,200))

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = frame:get_context()
	-- Paint PNG over frame
	ctx:set_source(source)
	ctx:paint()
end