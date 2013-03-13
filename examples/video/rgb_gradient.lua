function GetFrame(frame, frame_i)
	local data = frame:get_data(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT)
	local channels = VIDEO_HAS_ALPHA and 4 or 3
	for y = 0, VIDEO_HEIGHT - 1 do
		for x = 0, VIDEO_WIDTH - 1 do
			local data_i = 1 + y * VIDEO_WIDTH * channels + x * channels
			local pct = (x / (VIDEO_WIDTH-1) + y / (VIDEO_HEIGHT-1)) / 2
			if pct <= 0.5 then
				pct = pct * 2
				data[data_i] = 255 * (1 - pct)
				data[data_i+1] = 255 * pct
				data[data_i+2] = 0
			else
				pct = (pct - 0.5) * 2
				data[data_i] = 0
				data[data_i+1] = 255 * (1-pct)
				data[data_i+2] = 255 * pct
			end
		end
	end
	frame:set_data(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, data)
end
