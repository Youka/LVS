function GetFrame(frame, frame_i)
	local width, height, format = frame:get_header()
	frame:get_data(nil, nil, nil, nil)
end
