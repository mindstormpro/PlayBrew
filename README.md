# PlayBrew Loader

[![Build and Release](https://github.com/mindstormpro/PlayBrew/actions/workflows/release.yml/badge.svg)](https://github.com/mindstormpro/PlayBrew/actions/workflows/release.yml)

PlayBrew Loader is an experimental homebrew loader for the
[Playdate](https://play.date). It builds small ARM loader binaries that can be
appended to supported Playdate firmware images and then used to run a custom
`/payload.bin` from the Playdate filesystem.

This project is early-stage firmware work. It uses hard-coded firmware
addresses, so every supported OS version and hardware revision needs its own
verified address data.

## Current Status

The checked-in code currently targets Playdate OS 3.0.2. Rev. B has the most
complete patching path. Rev. A can build a loader binary, but the firmware
patching hook is not complete yet.

| Version | Signed? | Rev. A | Rev. B | Notes |
| :-- | :--: | :--: | :--: | :-- |
| 3.1.0 | Not yet | Planned | Planned | No checked-in address table yet |
| 3.0.3-5 | 3.0.5 is signed | No | No | SDK/symbol data makes this harder today |
| 3.0.2 | No | Build only | Partial | Only version with checked-in constants today |
| Older than 3.0.2 | No | No | No | No local dumps available for address work |

## Safety Notes

* Do not use this on firmware versions that are not listed as supported.
* Keep original firmware dumps backed up.
* The GitHub release artifacts contain raw loader binaries only. They do not
  contain Playdate firmware dumps or patched firmware images.
* `combine.py` expects local firmware dumps and writes patched firmware files
  under `build/`.

## Repository Layout

| Path | Purpose |
| :-- | :-- |
| `src/loader/main.c` | Loader entry code that opens and runs `/payload.bin` |
| `include/3.0.2.h` | Playdate OS 3.0.2 firmware function addresses and payload API |
| `ld/loader.ld` | Linker script for raw loader binaries |
| `generate_start.py` | Generates `src/loader/start.inc`, the startup hook included by `main.c` |
| `combine.py` | Appends loader binaries to local firmware dumps and updates firmware headers |
| `.github/workflows/release.yml` | CI build and draft-release workflow |

## Prerequisites

For local builds:

* CMake
* Python 3
* `arm-none-eabi-gcc`
* `arm-none-eabi-g++`
* `arm-none-eabi-objcopy`
* ARM Newlib libraries for `arm-none-eabi`

For firmware patching:

* A local Rev. A firmware dump named `pdfw-a`, and/or
* A local Rev. B firmware dump named `pdfw-b`

Firmware dumps are ignored by git on purpose. Put them at the repository root
only on your local machine.

## Build Locally

Generate the startup include, configure CMake, and build both loader targets:

```sh
python3 generate_start.py
cmake -S . -B build
cmake --build build
```

Expected outputs:

```text
build/loader-a.bin
build/loader-b.bin
```

`src/loader/start.inc` is generated and ignored. If you change
`generate_start.py`, regenerate `start.inc` before rebuilding locally.

## GitHub Actions

The `Build and Release` workflow runs on:

* pull requests
* pushes to `main`
* tags that start with `v`
* manual workflow dispatch

The workflow:

1. Checks out the repository.
2. Installs the ARM embedded toolchain.
3. Runs `python3 generate_start.py`.
4. Configures and builds the CMake project with Ninja.
5. Packages `loader-a.bin`, `loader-b.bin`, `README.md`, `LICENSE`, and
   `SHA256SUMS`.
6. Uploads the package as a workflow artifact.

## Releases

To create a release from GitHub Actions, push a version tag:

```sh
git tag v0.1.0
git push origin v0.1.0
```

The workflow creates a draft GitHub Release with:

* `playbrew-loader-<tag>.tar.gz`
* `SHA256SUMS`

The release is draft by default so the attached files and notes can be reviewed
before publication.

You can also run the workflow manually and set `create_release` to `true`. For
manual releases, provide a release tag such as `v0.1.0`.

## Patch Firmware Locally

Build the loader first, then run:

```sh
python3 combine.py
```

The script currently patches Rev. B only:

* `B` for Rev. B

Inputs:

```text
pdfw-b
build/loader-b.bin
```

Outputs:

```text
build/pdfw-b-patched
```

Rev. A patching is still incomplete. The script exits with a clear error if Rev.
A is selected, because producing a file that looks patched but cannot run the
loader is worse than stopping early.

## Payload Format

At runtime, the loader opens `/payload.bin` from the Playdate filesystem.
The file uses this custom little-endian layout:

| Offset | Size | Meaning |
| :-- | :-- | :-- |
| `0x00` | 4 bytes | Payload byte size |
| `0x04` | 4 bytes | Entry offset from the start of the payload |
| `0x08` | payload size | Payload bytes |

The loader places the payload near the end of Playdate RAM and calls:

```c
void entry(PlayBrewAPI *api);
```

`PlayBrewAPI` is declared in `include/3.0.2.h` and currently exposes:

* `sd_open`
* `sd_close`
* `sd_read`
* `printf`
* `gfx_clear`
* `gfx_drawLine`
* `gfx_setPixel`

The entry offset is added to the payload base address and called as Thumb code.

## Troubleshooting

### `start.inc: No such file or directory`

Run:

```sh
python3 generate_start.py
```

Then rebuild.

### CMake cannot find `arm-none-eabi-gcc`

Install an ARM embedded GCC toolchain and make sure it is on `PATH`.

### `combine.py` cannot find `pdfw-a` or `pdfw-b`

Place the local firmware dump at the repository root with the exact filename the
script expects.

### Release workflow does not include patched firmware

That is intentional. The workflow only builds loader binaries from public source
files. Patched firmware depends on local firmware dumps and must be produced
locally.

## Development Notes

Keep changes split by risk area:

* loader runtime behavior in `src/loader/main.c`
* firmware address data in version-specific headers
* firmware packaging logic in `combine.py`
* startup hook generation in `generate_start.py`
* build/release automation in `.github/workflows/`

Before opening a PR, run:

```sh
python3 generate_start.py
cmake -S . -B build
cmake --build build
```

If you change Python scripts, also run:

```sh
python3 -m py_compile combine.py generate_start.py
```

## Credits

@scratchminer helped heavily with this project.
