function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	ctx:path_move_to(300,300)
	ctx:path_line_to(350,220)
	ctx:path_line_to(400,300)
	ctx:path_close()
	g2du.path_transform(ctx, function(x, y)
		return x + math.sin(y/4) * 2, y + math.cos(x/4) * 2
	end)
	ctx:set_source(g2du.orange)
	ctx:path_fill()
end