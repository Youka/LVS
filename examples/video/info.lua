-- Build information string
local info = string.format("Dimension: %dx%d\nColorspace: %s\nFrames per second: %f\nFrame: %%d/%d"
							, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_HAS_ALPHA and "RGBA" or "RGB", VIDEO_FPS, VIDEO_FRAMES)
-- Create position matrix
local translation = g2d.create_matrix():translate(10,10)

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Define path
	ctx:set_matrix(translation)
	ctx:path_add_text("こんにちわ\nGuten Tag!\nGood day!\n\n" .. info:format(frame_i+1), "Arial", 20)
	-- Stroke path
	ctx:set_line_width(6)
	ctx:set_line_join("ROUND")
	ctx:set_source(g2du.black)
	ctx:path_stroke()
	-- Fill path
	ctx:set_source(g2du.yellow)
	ctx:path_fill()
end