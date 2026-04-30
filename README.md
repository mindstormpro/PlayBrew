# PlayBrew Loader
Homebrew loader for the Playdate! (https://play.date)

# Todo:  
* add extra functions
	* screen drawing access -- In Progress
	* input access
	* more filesystem access
* make installer via system api buffer overflow
* add hook inside of loadPDX function to allow adding a custom menu item to open playbrew -- in Progress

# Supported Versions Of Playdate OS

| Version | Signed? | Comp. Rev. A? | Comp Rev. B? | Notes |
| :----: | :----: | :----: | :----: | :---- |
| 3.1.0 | Not Yet | Planned | Planned | Version hasn't been released yet |
| 3.0.3-5 | 3.0.5 is | Never | Never | SDK issue makes it very dificult w/ `symbols.db` |
| 3.0.2 | No | Not Yet | Kinda | In Progress, Patch runs but issue with opening `payload.bin` |
| < 3.0.2 | No | Never | Never | I don't have dumps so I cannot make a version for this |



@scratchminer helped SOO much for this project I cannot thank him enough!
