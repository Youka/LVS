#Load plugin
LoadPlugin("E:\PROJEKTE\LVS\plugin\LVS.dll")	#fix location if needed

#Create video
fps = 24000.0/1001
ColorBars(width = 720, height = 480, pixel_type = "YV12").AssumeFPS(fps).Trim(0,Ceil(fps*90))

#AUDIO EXAMPLES
#LVS(audio="audio\audio_to_text.lua")
#LVS(audio="audio\halftone.lua")
#LVS(audio="audio\mute_channels.lua")
#LVS(audio="audio\peep.lua")

#VIDEO EXAMPLES
#LVS("video\animation.lua")
#LVS("video\composition.lua")
#LVS("video\convolution.lua")
#LVS("video\info.lua")
#LVS("video\logo.lua")
#LVS("video\noise.lua")
#LVS("video\overlay.lua")
#LVS("video\shaded_triangle.lua")
#LVS("video\wobble.lua")

#KARAOKE EXAMPLES
#LVS("karaoke\basic.lua")
LVS("karaoke\simple.lua")