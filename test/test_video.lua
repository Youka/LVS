-- Process frames
function GetFrame(frame, frame_i)
	-- Get frame time
	local ms = frame_i / VIDEO_FPS * 1000
	-- Create drawing context
	local ctx = g2d.create_context(frame)
end