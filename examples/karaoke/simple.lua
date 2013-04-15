-- Parse ASS file and fill global tables
ass.load("karaoke\\chihaya_ed.ass")

-- Roumaji & kanji
local function roumaji_kanji(ctx, ms, line)
	-- Calculate transparency dependent on in- & outfade effect
	local alpha = ms < line.start_time and (ms - (line.start_time - line.infade/2)) / (line.infade/2) or ms > line.end_time and 1 - (ms - line.end_time) / (line.outfade/2) or 1
	-- Draw inactive sylables
	local active_syl
	for si, syl in ipairs(line.syls) do
		if ms >= syl.start_time and ms < syl.end_time then
			active_syl = syl.i
		else
			ctx:set_matrix(g2d.create_matrix():translate(syl.x, syl.y))
			ctx:path_add_text(syl.text, ass.unpack_style(line.styleref))
			ctx:set_source(g2d.create_source_color(line.styleref.color1.r, line.styleref.color1.g, line.styleref.color1.b, alpha))
			ctx:path_fill()
			ctx:path_clear()
		end
	end
	-- Draw active sylable with effect
	if active_syl then
		local syl = line.syls[active_syl]
		local pct = math.sin((ms - syl.start_time) / syl.duration * math.pi)
		local r, g, b = math.interpolate(line.styleref.color1.r, 1, pct), math.interpolate(line.styleref.color1.g, 0, pct), math.interpolate(line.styleref.color1.b, 0, pct)
		ctx:set_matrix(g2d.create_matrix():translate(syl.width/2 + syl.x, syl.height/2 + syl.y):scale(1+pct*0.5, 1+pct*0.5):translate(-syl.width/2,-syl.height/2))
		ctx:path_add_text(syl.text, ass.unpack_style(line.styleref))
		ctx:set_source(g2d.create_source_color(r, g, b, alpha))
		ctx:path_fill()
		ctx:path_clear()
	end
end

-- Subtitle
local function subtitle(ctx, ms, line)
	-- Calculate transparency dependent on in- & outfade effect
	local alpha = ms < line.start_time and (ms - (line.start_time - line.infade/2)) / (line.infade/2) or ms > line.end_time and 1 - (ms - line.end_time) / (line.outfade/2) or 1
	-- Draw line text
	ctx:set_matrix(g2d.create_matrix():translate(line.x, line.y))
	ctx:path_add_text(line.text, ass.unpack_style(line.styleref))
	ctx:set_source(g2d.create_source_color(line.styleref.color1.r, line.styleref.color1.g, line.styleref.color1.b, alpha))
	ctx:path_fill()
	ctx:path_clear()
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create drawing context
	local ctx = g2d.create_context(frame)
	-- Get frame time
	local ms = frame_i / VIDEO_FPS * 1000
	-- Look for frame-related ASS lines
	for _, line in ipairs(lines) do
		if ms >= line.start_time - line.infade/2 and ms < line.end_time + line.outfade/2 then
			-- Draw ASS line
			if line.styleref.alignment > 3 then
				roumaji_kanji(ctx, ms, line)
			else
				subtitle(ctx, ms, line)
			end
		end
	end
end