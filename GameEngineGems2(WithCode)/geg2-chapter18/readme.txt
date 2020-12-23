Game Engine Gems 2
Believable Dead Reckoning for Networked Games
Author: Curtiss Murphy

*******************
INTRODUCTION
*******************

This text file goes along with the Game Engine Gems 2 chapter described above. The source material found on the DVD for this chapter was taken from several open source repositories including: the Delta3D Open Source Engine for Serious Games (Delta3D); Open Scene Graph (OSG); and the Delta3D Simulation Core (SimCore). Because the application is part of a fully working M&S system, the code that is relevant to dead reckoning is not encapsulted neatly into one or two files. 

Instead, the source section includes snippets and excerpts from the three respective repositories. Each of these projects uses the LGPL open source license and the full projects with source can be found on their respective project pages: http://www.delta3d.org, http://www.openscenegraph.org, and http://www.delta3d.org/article.php?story=20090818163549896&topic=news. If you want to build and modify the NetDemo application yourself, you will need to download Delta3D, the Delta3D dependencies (includes OSG), and the SimulationCore following the guidelines in the above links. The exact version of the executable on this disk was compiled using the Delta3D 2.5 dependency package, using Delta3D SVN revision 7437, dtPhysics version 2704, and SimCore version 2739. As these are all heavily active projects and are likely to have been improved since the book release, you will probably get the best results just pulling down the trunks of these projects.   

This application is intended for Windows XP or Windows 7. The application is compatible with Windows and Linux, but binaries are not included on the DVD. 

To run this application, simply run GameStart.bat in the DeadReckoningDemo. To experiment with various dead reckoning modes for the main vehicle, there are a few keyboard commands you will want to use. F1 will show help; G will turn on the dead reckoning ghost; F2 will show the dead reckoning debug window; 9/0 will allow you to change the various DR settings; 7/8 will allow you to decrease/increase the publish rate. The Insert key will cycle through various performance debug displays. To change the dead reckoning behavior of the mines or helixes, you will need to edit the actor prototypes in ProjectAssets/maps/NetDemoPrototypes.xml. 

If you do not already have Visual Studio installed, you may need to run 'vcredist_x86.exe' to install the msvc dlls. This app has been tested on both Win 7 (32 or 64) and Win XP. 

NOTE - The hover vehicle works but does not dead reckon well because it uses hacks that make it unstable. Specifically the rotation is wonky - try turning while driving to see some bizarre behavior. I considered removing it, but left it for giggles.

*******************
SOURCE CODE
*******************

The source code on this DVD is provided as example materials and is released under the LGPL license. These files are part of the Delta3D, Open Scene Graph, and Simulation Core repositories (as marked by directory). For more information, see the appropriate links above or contact the chapter author (cmmurphy at alionscience dot com).


*******************
NETWORKING THE GAME
*******************

This demo does support networking. You can have more than one client, however, clients cannot reconnect once disconnected without restarting. To run on a network, simply start one server, then start the clients. The server controls the main actors (enemies, forts, etc). You can run them on the same computer if your hardware runs fast enough.

*******************
INTERFACE COMMANDS
*******************
 
F1 will show help, F2 will show debugging info. The following commands can be used in game: 

W/A/S/D: Move
Arrows: Move
H: Change Weapon - Machine gun (red tracer) or Grenade
V: Change View - attached camera perspective on the truck
Space: Brakes
Esc: Menu
L-Click: Fire
Mouse: Look
G: DR Ghost - renders a purple translucent version of the truck
F2: Debug Info

*******************
DEBUG COMMANDS
*******************
 
The demo supports a variety of debug options for displaying and modifying debug settings. 

F2: Show Debug Info Window
Insert: Toggle render performance statistics
P: Reloads and reapplies all the shaders - see Game Engine Gems 7 article, "Supporting Your Local Artist, Adding Shaders to Your Engine". If your performance is low, it's probably because there are too many dynamic lights running. Try reducing the number in the ProjectAssets\Shaders\Base\dynamic_lights.frag.

The following debug lines will show in the Debug Info Window

7/8: Change Publish Rate - decrease or increase the publishes per second.
9: Cycle Current Debug Variables - changes which debug variable will be modified when pressing '0'
0: Toggle Variable Value - change the current debug variable value. 

Cur Var: DR Algorithm
   -- This indicates which variable will be changed if you press 0. This entry 
   -- shows green and the matching variable below will also show green. 
   -- The options include: DR Algorithm, DR Publish Ang Vel, DR Blending Type, 
   -- DR Ground Clamp, or DR Fixed Blend.  

DR Algorithm: Velocity and Acceleration
   -- This indicates the general behavior of the dead reckoning. 
   -- The options include: Static, Velocity Only, or Velocity and Acceleration

DR Ground Clamp: KeepAbove
   -- This indicates what the ground clamper will do. In this demo, the terrain
   -- is not optimized for doing isectors, so ground clamping is very slow. 
   -- The options include: KeepAbove, Full, or None

DR Blending Type: Projective
   -- Indicates whether the DR helper is doing Projective Velocity Blending
   -- or Cubic Bezier Splines. The options include: Projective or Splines

DR Ghost Mode: ON
   -- Indicates whether the translucent truck is shown. 
   -- Options include: ON, OFF, Attach Ghost, Hide Real, Detached

Publish Rate: 5/sec  
   -- Indicates the minimum time that must occur before the truck will 
   -- publish. Ex 1/s, 5/s, 60/s

DR Fixed Blend: TRUE
   -- Indicates that the dead reckoning algorithm is using and therefore 
   -- expecting a fixed rate blending time to do the blend. If False, it 
   -- will attempt to compute an avg value based on how often it gets 
   -- updates. Options include: TRUE or FALSE.

DR Publish Ang Vel: TRUE
   -- Indicates whether the vehicle will publish angular velocity. Without 
   -- angular velocity the vehicle will obviously be pointing in the wrong 
   -- direction and will look wrong. Options include: TRUE or FALSE.

Spd: 11.400 DRErr: 2.946
   -- Indicates additional debug stats. The speed is the current m/s of the 
   -- vehicle. The DRErr indicates how far the DR position is from the 
   -- actual rendered position (averaged every 60 frames and updated 
   -- once/second). Although imperfect, this helps give a sense of the 
   -- relative impact of various DR techniques.  


