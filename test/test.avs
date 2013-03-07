LoadPlugin("E:\PROJEKTE\LVS\bin\LVS.dll")

AviSource("Saintseiya.avi")
ConvertToRGB32()
LVS(audio = "audio_filter.lua", video = "video_filter.lua")
Info()