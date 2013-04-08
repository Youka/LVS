-- Parse ASS content and fill global tables
ass.load([[
PlayResX: 720
PlayResY: 480

Style: Roumaji,Arial,26,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,0,8,10,10,15,1
Style: Kanji,Arial,26,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,0,6,15,15,15,1
Style: Subtitle,Arial,26,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,0,2,10,10,15,1

Dialogue: 0,0:00:00.28,0:00:05.15,Roumaji,,0,0,0,,{\k19}ko{\k15}re{\k12}ka{\k37}ra {\k28}no {\k34}se{\k27}ka{\k19}i {\k17}ni {\k44}wa{\k24} {\k12}da{\k17}re {\k33}mo {\k31}i{\k32}na{\k86}i
Dialogue: 0,0:00:05.58,0:00:10.29,Roumaji,,0,0,0,,{\k16}ko{\k16}e {\k16}da{\k18}ke {\k39}o {\k41}se{\k14}na{\k35}ka {\k80}ni{\k30} {\k33}ka{\k17}n{\k32}ji {\k84}te
Dialogue: 0,0:00:00.28,0:00:05.15,Kanji,,0,0,0,,{\k19}こ{\k15}れ{\k12}か{\k37}ら{\k28}の{\k34}世{\k46}界{\k17}に{\k68}は{\k29}誰{\k33}も{\k31}い{\k32}な{\k86}い
Dialogue: 0,0:00:05.58,0:00:10.29,Kanji,,0,0,0,,{\k32}声{\k16}だ{\k18}け{\k39}を{\k41}背{\k49}中{\k110}に{\k50}感{\k32}じ{\k84}て
Dialogue: 0,0:00:00.28,0:00:05.15,Subtitle,,0,0,0,,Es gibt niemanden in der Zukunft, ...
Dialogue: 0,0:00:05.58,0:00:10.29,Subtitle,,0,0,0,,...der nur Stimmen aus der Vergangenheit hört.
]])

-- Roumaji & kanji
local function roumaji_kanji(ctx, ms, line)
	-- Iterate through sylables
	for si, syl in ipairs(line.syls) do
		-- Set color dependent on sylable activity
		if ms >= syl.start_time and ms < syl.end_time then
			ctx:set_source(g2du.red)
		else
			ctx:set_source(g2du.black)
		end
		-- Draw sylable text
		ctx:set_matrix(g2d.create_matrix():translate(syl.x, syl.y))
		ctx:path_add_text(syl.text, ass.unpack_style(line.styleref))
		ctx:path_fill()
		ctx:path_clear()
	end
end

-- Subtitle
local function subtitle(ctx, ms, line)
	-- Draw line text
	ctx:set_matrix(g2d.create_matrix():translate(line.x, line.y))
	ctx:path_add_text(line.text, ass.unpack_style(line.styleref))
	ctx:set_source(g2du.black)
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
		if ms >= line.start_time and ms < line.end_time then
			-- Draw ASS line
			if line.styleref.alignment > 3 then
				roumaji_kanji(ctx, ms, line)
			else
				subtitle(ctx, ms, line)
			end
		end
	end
end