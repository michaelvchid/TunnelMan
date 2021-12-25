# About
This project aims to create a C++ based video game of a miner -- the TunnelMan -- that is searching for oil by digging through earth. However, he must watch out for protestors that are trying to stop the TunnelMan. Read the instructions below on how to play the game on your own computer, as well as the gameplay mechanics to know how to play!

## Setup
The following guide will provide instructions on how to reproduce this game on your laptop. Because this game was created on a Windows machine and uses tools that may be Windows-specific, there is the possibility that the following may not work as intended on a Linux or Mac. 

First, head to the repository on GitHub at https://github.com/michaelvchid/TunnelMan (if you're viewing this on GitHub, you're most definitely on the correct page already). Next, look for the green button with the word "Code", and click it to bring a dropdown menu. Then, click download ZIP. 

Once this is downloaded, unzip the folder and send the extracted folder to somewhere memorable or where you would like to store the project. 

To simply run the game, open the folder "executable" and double-click "TunnelMan.exe". Just like that, the game should start up! If you are interested in altering the game in any way or are curious as to how the program works, check out the "Under The Hood" section which discusses the underlying code. Note that due to the complexity of this project and the use of many other outside libraries, the code may be difficult to understand; because of the large amount of files and their sizes, it is also very space-inefficient. For this reason, I plan on recreating the entire game using Python so that it is easier to both understand the underlying code as well as be less costly in terms of space needed on your machine. If you want a challenge, try to recreate the game on Python as well! All the important files such as the sounds and photos are in the code/Assets folder. 

## How To Play
The main goal of the game is to dig through the earth and find the remaining number of oil barrels that are hidden in the field. To move, simply use the arrow keys or ASWD. Make sure to avoid the protestors who are out to chase you! Getting hit 5 times will end with you losing one out of three lives. However, you can fight back using a squirt gun; starting out with 5 squirts, go up to a protestor and press the spacebar to use. The regular protestors (all blue) require 3 squirts. However, the hardcore protestors (red shirt) will need a bit more to fully irritate! They are also much smarter and can follow you if they are within a certain number of steps of the TunnelMan. If you use all your squirts in a level, you can gain more by stepping on water puddles that will randomly appear in locations you have already dug up. 

Another way to get rid of protestors is by hitting them with a falling boulder. Completely dig out the earth immediately below a boulder, and within seconds, it'll start falling. If a protestor is in the way, it will instantly knock them out and will leave the field. But watch out! If you are in the path of a falling boulder, it will completely damage you and you will lose a life. If the boulder hits even one bit of earth, it will crumble and no longer fall. 

You will also start off every level with zero pieces of gold. But don't fret, you can find them hidden throughout the field. When you have a piece of gold, you can drop it by pressing the tab button. If a regular protestor picks it up, it will be bribed and instantly leave the field. However, a hardcore protestor will only be stalled by the gold for a short period of time and then resume its goal of irritating the TunnelMan. If no protestor picks it up on time -- and the TunnelMan cannot pick it up again after dropping it -- the gold will become one with the earth and disappear!

Lastly, the player starts off with one sonar each level. This item will reveal whether or not any hidden barrels of oil or gold are within a certain radius of the player. To use, simply press "Z". If nothing happens, then there was unfortunately nothing in close proximity to the player. Every so often, a sonar kit will appear on the top-left corner of the field. Get it in time for an extra sonar kit!

As you progress through the game, more barrels will be hidden in the field, and more protestors will be out to catch the TunnelMan. Collect items, irritate protestors and pass levels to get as many points as possible!

## Under The Hood
This section will provide a guide on what files can be manipulated to change the gameplay. From the downloaded ZIP file, go to the "code" directory. This is where the Visual Studio project and solution lives. If you open the .sln file and try to compile and run the program, you may run into errors. 

Make sure that your Configuration Manager is set correctly for this project: at the top bar to the left of a "Local Windows Debugger" button, there are two dropdown menus, most likely stating "Debug" or "Release" for the first, and "x86" or "x64" for the second. Click either dropdown, then click "Configuration Manager...". In the popup, make sure that Active Solution Configuration is set to "Debug" and the Active Solution Platform is "x86". For the TunnelMan project, it should be set to Debug and Win32, with the Build checkbox checked. Using any other settings may not allow for proper compilation. 

If the compiler cannot find the Assets directory, open main.cpp and follow the commented code at the start of the file to fix the error. 

Entering the "TunnelMan" directory, you will find many .h and .cpp files, as well as a couple other files and folders. In Actor.h, all different game objects and their associated functions are declared. The functions are therefore all defined in Actor.cpp. Note that all objects must inherit from GraphObject in order to work properly. StudentWorld.h and .cpp include code that generates each level, processes a tick (a short time in which an action happens), and the cleanup of the level; it also handles all game-level variables such as number of sonar kits, gold, points, etc. All other files are used to help display graphics, make key presses turn into movements, play sounds, and more. However, to alter gameplay and change settings, these files should be enough.

## Contact
If you have any questions regarding this project, including bugs, inability to run the executable, build the solution, or anything else, feel free to open an issue on the GitHub repository and I will do my best to get back in a timely manner.

## Acknowledgements
Thank you to professor Edwin Ambrosio and the Department of Computer Science at the University of California, Los Angeles for providing the starting point for this project, including but not limited to the sounds, graphics, and basic user-computer interactions such as key presses. I would also like to thank the TAs of the course CS32 and Ambrosio for assistance and guidance on how to create an organized and well-designed program with this level of complexity. 
