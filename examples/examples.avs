#Load plugin
LoadPlugin("E:\PROJEKTE\LVS\release\LVS.dll")	#fix location if needed

#Create video
fps = 24000.0/1001
ColorBars(width = 720, height = 480, pixel_type = "RGB32").ConvertFPS(fps).Trim(0,Ceil(fps*20))

#AUDIO EXAMPLES
#LVS(audio="audio\audio_to_text.lua")
#LVS(audio="audio\halftone.lua")
#LVS(audio="audio\mute_channels.lua")
#LVS(audio="audio\peep.lua")

#VIDEO EXAMPLES
#LVS("video\advanced_methods.lua")
#LVS("video\animation.lua")
#LVS("video\convolution.lua")
#LVS("video\info.lua")
#LVS("video\logo.lua")
#LVS("video\noise.lua")
#LVS("video\overlay.lua")
#LVS("video\shaded_triangle.lua")

#KARAOKE EXAMPLES
#LVS("karaoke\simple.lua")