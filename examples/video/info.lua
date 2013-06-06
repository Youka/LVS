-- Build information string
local info = string.format("こんにちは\nGuten Tag!\nGood day!\n%s\n\nDimension: %dx%d\nColorspace: %s\nFrames per second: %f\nFrame: %%d/%d"
							, USERDATA or "", VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_HAS_ALPHA and "RGBA" or "RGB", VIDEO_FPS, VIDEO_FRAMES)

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = frame:get_context()
	-- Define path
	ctx:path_add_text(10, 10, info:format(frame_i+1), "Arial", 30)
	-- Stroke path
	ctx:set_line_width(6)
	ctx:set_source(g2du.black)
	ctx:stroke()
	-- Fill path
	ctx:set_source(g2du.yellow)
	ctx:fill()
end