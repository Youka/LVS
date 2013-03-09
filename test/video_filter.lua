function GetFrame(frame, frame_i)
	local width, height, format = frame:header()
	frame:data(nil, nil, nil, nil)
end
