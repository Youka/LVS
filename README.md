# LVS
---
## Definition
**LVS** (Lua Video Sync) is a *video frameserver plugin* to edit video frames 
with the capability of scripting language *Lua* + 2D image processing functionality.

Implented plugin interfaces are currently *Avisynth*, *VirtualDub* and *DirectShow*.  
(Avisynth permits also a way to edit audio data and pass userdata.)  
The provided plugin already includes a Lua 5.2 interpreter with a graphic library, 
so there's no additionaly dependency to install.

LVS aims at user groups like programmers, video editors or karaoke effects makers. 
Because of the work flow by writing code it gives more control to users than a GUI-based video editor, 
but (maybe) requires more time to see something.


## Requirements
* [MSVC 9 runtime library](http://www.microsoft.com/de-de/download/details.aspx?id=29)
* SSE2 supporting processor


## Content
***docs***  
LVS documentation in HTML format.

***examples***  
Example scripts for LVS.

***release***  
LVS plugin + scripts (external libraries).

***src***  
LVS source code.

***gitignore***  
Git ignored files.

***LICENSE***  
LVS license (LGPLv3).

***README.md***  
This file!


## Example
test.avs:

    BlankClip()
    LVS("test.lua")

test.lua:

    function GetFrame(frame, frame_i)
    	local ctx = g2d.create_context(frame)
    	ctx:set_matrix( g2d.create_matrix():translate(100,100):rotate(45) )
    	ctx:path_add_rectangle(-50,-50,100,100)
    	ctx:set_source(g2d.create_source_color(1,0,0,1))
    	ctx:path_fill()
    end