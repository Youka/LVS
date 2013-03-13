LoadPlugin("E:\PROJEKTE\LVS\bin\LVS.dll")

AviSource("Saintseiya.avi")
ConvertToRGB24()
LVS(video = "video_filter.lua")
Info()