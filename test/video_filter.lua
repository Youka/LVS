local function wobbler(typ, x, y)
	return x, y
end

function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	ctx:path_add_text("Hello world!", "Times New Roman", 60)
	ctx:path_transform("flat", wobbler)
end
