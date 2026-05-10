# PlayBrew Loader
Homebrew loader for the Playdate! (https://play.date)

# Status

This repo currently has checked-in constants for Playdate OS 3.0.2 only.
Other versions in the table below are tracked as research/planned work until
matching address data and patch rules are added.

# Todo:  
* add extra functions
	* more screen drawing access; basic clear, line, and pixel calls are wired
	* input access
	* more filesystem access
* make installer via system api buffer overflow
* add hook inside of loadPDX function to allow adding a custom menu item to open playbrew -- in Progress

# Prerequisites

* CMake
* Python 3
* `arm-none-eabi-gcc`, `arm-none-eabi-g++`, and `arm-none-eabi-objcopy`
* Local Playdate firmware dumps named `pdfw-a` and/or `pdfw-b`

Firmware dumps are intentionally ignored by git. Put them at the repo root when
using `combine.py`.

# Build From A Fresh Clone

The loader includes generated startup code. Until generation is wired into
CMake, run the generator before building:

```sh
python3 generate_start.py
cmake -S . -B build
cmake --build build
```

The build writes these raw loader binaries:

* `build/loader-a.bin`
* `build/loader-b.bin`

# Patch Firmware

Run the patcher after building the loader:

```sh
python3 combine.py
```

The script asks for `A`, `B`, or `AB`.

Inputs:

* `pdfw-a` for Rev. A
* `pdfw-b` for Rev. B
* `build/loader-a.bin` and/or `build/loader-b.bin`

Outputs:

* `build/pdfw-a-patched`
* `build/pdfw-b-patched`

Rev. A patching is not complete yet. The Rev. A loader can be built, but the
patcher still needs a real Rev. A execution hook before that output should be
used.

# Payload Format

At runtime the loader opens `/payload.bin` from the Playdate filesystem.
The file is a small custom binary format:

| Offset | Size | Meaning |
| :----: | :--: | :------ |
| 0 | 4 | Little-endian payload byte size |
| 4 | 4 | Little-endian entry offset from the start of the payload |
| 8 | payload size | Payload code/data bytes |

The loader places the payload near the end of Playdate RAM and calls:

```c
void entry(PlayBrewAPI *api);
```

`PlayBrewAPI` is defined in `include/3.0.2.h`. The entry offset is added to the
loaded payload base address, then called as Thumb code.

# Supported Versions Of Playdate OS

| Version | Signed? | Comp. Rev. A? | Comp Rev. B? | Notes |
| :----: | :----: | :----: | :----: | :---- |
| 3.1.0 | Not Yet | Planned | Planned | Version hasn't been released yet |
| 3.0.3-5 | 3.0.5 is | Never | Never | SDK issue makes it very dificult w/ `symbols.db` |
| 3.0.2 | No | Build only | Kinda | Only version with checked-in constants today; Rev. B patch path is in progress |
| < 3.0.2 | No | Never | Never | I don't have dumps so I cannot make a version for this |



@scratchminer helped SOO much for this project I cannot thank him enough!
