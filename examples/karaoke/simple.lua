-- Parse ASS file and fill global tables
ass.load("karaoke\\res\\chihaya_ed.ass")

-- Roumaji & kanji
local function roumaji_kanji(ctx, ms, line)
	-- Get line color
	local color = {ass.unpack_color(line.styleref.color1)}
	color[4] = ms < line.start_time and math.inrange(line.start_time - line.infade/2, line.start_time, ms) or
				ms > line.end_time and 1 - math.inrange(line.end_time, line.end_time + line.outfade/2, ms) or
				1
	-- Draw inactive sylables
	for si, syl in ipairs(line.styleref.alignment < 7 and line.chars or line.syls) do	-- Use characters in case of kanjis
		if ms < syl.start_time or ms >= syl.end_time and syl.text ~= "" then
			ctx:path_add_text(syl.x, syl.y, syl.text, ass.unpack_font(line.styleref))
			ctx:set_source(g2d.create_color(table.unpack(color)))
			ctx:fill()
			ctx:path_clear()
		end
	end
	-- Draw active sylables
	for si, syl in ipairs(line.styleref.alignment < 7 and line.chars or line.syls) do
		if ms >= syl.start_time and ms < syl.end_time and syl.text ~= "" then
			local pct = math.sin(math.inrange(syl.start_time, syl.end_time, ms) * math.pi)
			ctx:set_matrix(g2d.create_matrix()
							:translate(syl.width/2 + syl.x, syl.height/2 + syl.y)
							:scale(1+pct*0.5, 1+pct*0.5)
							:translate(-syl.width/2,-syl.height/2)
						)
			ctx:path_add_text(0, 0, syl.text, ass.unpack_font(line.styleref))
			ctx:set_matrix(g2du.identity)
			ctx:set_source(g2d.create_color(math.interpolate(color[1], 1, pct), math.interpolate(color[2], 0, pct), math.interpolate(color[3], 0, pct), color[4]))
			ctx:fill()
			ctx:path_clear()
		end
	end
end

-- Subtitle
local function subtitle(ctx, ms, line)
	-- Get line color
	local color = {ass.unpack_color(line.styleref.color1)}
	color[4] = ms < line.start_time and math.inrange(line.start_time - line.infade/2, line.start_time, ms) or
				ms > line.end_time and 1 - math.inrange(line.end_time, line.end_time + line.outfade/2, ms) or
				1
	-- Draw line text
	ctx:path_add_text(line.x, line.y, line.text, ass.unpack_font(line.styleref))
	ctx:set_source(g2d.create_color(table.unpack(color)))
	ctx:fill()
	ctx:path_clear()
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create drawing context
	local ctx = frame:get_context()
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