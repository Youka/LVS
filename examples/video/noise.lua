-- Process frames
function GetFrame(frame, frame_i)
	-- Get frame pixels
	local data = frame:get_data(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT)
	local channels = VIDEO_HAS_ALPHA and 4 or 3
	-- Iterate through frame pixels
	for y = 0, VIDEO_HEIGHT - 1 do
		for x = 0, VIDEO_WIDTH - 1 do
			-- Set new pixel color
			if math.random(0,2) == 0 then
				local data_i = 1 + y * VIDEO_WIDTH * channels + x * channels
				data[data_i] = 0
				data[data_i+1] = 0
				data[data_i+2] = 0
			end
		end
	end
	-- Set new frame pixels
	frame:set_data(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, data)
end