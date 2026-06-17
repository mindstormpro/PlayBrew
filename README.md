# PlayBrew Loader
Homebrew loader for the Playdate! (https://play.date)  
NOTE: REV A. Devices ARE NOT SUPPORTED AS OF NOW  
please check to see your hardware revision using Rae's [Rev Checker](https://rae.wtf/revchecker/)

Please note that installing homebrew or doing any firmware modification VOIDS YOUR WARANTY. INSTALL AT YOUR OWN RISK

# Building PlayBrew Loader
## Windows
### Prerequisites
* The [Arm-None-Eabi-Gcc compiler](https://developer.arm.com/downloads/-/gnu-rm) (It is deprecated, make sure to get downloads from that page, specifically version 10.3-2021.10. The newer non-depricated version does not work to my knoledge)
* CMake
* MinGW
* python
### Building
clone the repo  
Then place your decrypted `pdfw-b` firmware file in the root directory
next do the following commands:  
```Powershell
cd build
echo i need to look at my laptop to get the command that go here :D
mingw32-make
cd ..
py combine.py
```
`Combine.py` will ask for your rev, currently rev A isn't supported so do B.  
Then get your `pdfw-b-patched` from the build folder and install it by opening the boot disk, deleting `VERSION.txt`, moving your new fw to the disk, and then renaming it to `pdfw` with no file extension before ejecting.
# Supported Versions Of Playdate OS

| Version | Downloadable? | Comp. Rev. A? | Comp Rev. B? | Notes |
| :----: | :----: | :----: | :----: | :---- |
| 3.1.0 | Not Yet | Planned | Planned | Version hasn't been released yet |
| 3.0.3-6 | 3.0.6 is | Never | Never | SDK issue makes it very dificult w/ `symbols.db` + general fw instability |
| 3.0.2 | No | Planned | Kinda | In Progress, Patch runs but issue with opening `payload.bin` |
| < 3.0.2 | No | Never | Never | I don't have dumps so I cannot make a version for this |

# Todo:  
* MAKE FS USABLE!!!
  * I need to figure out what is what in the UCFS table and then call those funcs with proper formatting instead of the sd_read or fread type stuff
  * also need to save annotations in Ghidra more so I can understand wtf I'm actually looking at
* add extra functions
	* screen drawing access -- In Progress
	* input access
	* more filesystem access
* make installer CFW and don't get DMCA'd
* find a way to add a custom menu item to open PBL so you don't have to restart

@scratchminer helped SOO much for this project I cannot thank him enough!

Me nor anyone who helped the development with this project is affiliated nor indorsed with/by Panic Inc. 
