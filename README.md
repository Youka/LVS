# LVS
---
## Definition
**LVS** (Lua Video Sync) is a *video frameserver plugin* to edit video frames 
with the capability of scripting language *Lua* + 2D image processing functionality.

Implented plugin interfaces are currently *Avisynth*, *VirtualDub* and *DirectShow*.  
Avisynth permits also a way to edit audio data and pass userdata.  
The provided plugin already includes a Lua 5.2 interpreter with a graphic library, 
so there's no additionaly dependency to install.

LVS aims at user groups like programmers, video editors or karaoke effects makers. 
Because of the work flow by writing code it gives more control to users than a GUI-based video editor, 
but (maybe) requires more time to see something.


## Requirements
* MSVC 9 runtime library
* SSE2 supporting processor


## Content
***docs***  
LVS documentation in HTML format.

***examples***  
Example scripts for LVS.

***release***  
Plugin + scripts (external libraries).

***src***  
LVS source code.

***gitignore***  
Git ignored files.

***LICENSE***  
LVS license (LGPLv3).

***README.md***  
This file!