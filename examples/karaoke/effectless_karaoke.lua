-- Parse ASS content and fill global tables
LoadASS([[
PlayResX: 720
PlayResY: 480

Style: Roumaji,Arial,26,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,0,8,10,10,15,1
Style: Subtitle,Arial,26,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,0,2,10,10,15,1

Dialogue: 0,0:00:00.14,0:00:00.56,Roumaji,,0,0,0,kiss,{\k27}chu{\k15}u
Dialogue: 0,0:00:02.49,0:00:03.45,Roumaji,,0,0,0,shiny,{\k24}ki{\k24}ra{\k26}ki{\k22}ra
Dialogue: 0,0:00:03.84,0:00:04.26,Roumaji,,0,0,0,kiss,{\k29}chu{\k13}u
Dialogue: 0,0:00:06.45,0:00:12.98,Roumaji,,0,0,0,,{\k24}ki{\k24}mi {\k45}to{\k94} {\k21}mi{\k24}te{\k44}ta{\k94} {\k23}na{\k24}ga{\k43}re{\k16}bo{\k33}shi {\k24}o{\k22}i{\k22}ka{\k23}ke{\k53}te
Dialogue: 0,0:00:13.82,0:00:20.34,Roumaji,,0,0,0,,{\k24}hi{\k25}to{\k44}ri{\k91}{\k46}bo{\k48}tchi{\k91} {\k25}ko{\k24}n{\k45}na {\k23}to{\k24}ko {\k21}ma{\k24}de {\k23}ki{\k23}te{\k51}ta
Dialogue: 0,0:00:00.14,0:00:00.56,Subtitle,,0,0,0,kiss,Küsschen
Dialogue: 0,0:00:02.49,0:00:03.45,Subtitle,,0,0,0,shiny,Glitzer glitzer
Dialogue: 0,0:00:03.84,0:00:04.26,Subtitle,,0,0,0,kiss,Küsschen
Dialogue: 0,0:00:06.45,0:00:12.98,Subtitle,,0,0,0,,Ich verfolge die Sternschnuppe, die ich mit dir gesehen habe.
Dialogue: 0,0:00:13.82,0:00:20.34,Subtitle,,0,0,0,,Doch bis hierhin bin ich ganz alleine gekommen.
]])

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
			ctx:set_matrix(g2d.create_matrix(1, 0, 0, 1, line.x, line.y))
			ctx:path_add_text(line.text, line.styleref.fontname, line.styleref.fontsize, line.styleref.bold, line.styleref.italic, line.styleref.underline, line.styleref.strikeout)
			ctx:set_source(g2du.white)
			ctx:path_fill()
		end
	end
end