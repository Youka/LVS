function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	ctx:path_add_arc(350,300,50,0,-180)
	ctx:path_close()
	g2du.path_transform(ctx, function(typ, x, y)
		if typ ~= "close" then
			return x + math.sin(y/4) * 2, y + math.cos(x/4) * 2
		end
	end)
	ctx:set_source(g2du.orange)
	ctx:path_fill()
end