LoadPlugin("E:\PROJEKTE\LVS\bin\LVS.dll")

AviSource("Saintseiya.avi")
ConvertToRGB24()
LVS(audio = "audio_filter.lua", video = "video_filter.lua")
Info()