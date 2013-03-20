LoadPlugin("E:\PROJEKTE\LVS\bin\LVS.dll")

AviSource("Saintseiya.avi")
ConvertToRGB24()
LVS(video = "test_video.lua")
Info()