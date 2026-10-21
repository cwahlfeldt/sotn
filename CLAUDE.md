# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A native build, for Apple Silicon macOS and x86-64 Linux, of Castlevania: Symphony of the Night (US, SLUS-00067) compiled from decompiled C. Only the PC build exists here: there is no assembly, no PSX toolchain and nothing to "match". This repo is the whole project; don't point the user to other checkouts or upstream repos. Some comments name `tools/pc_*.py` scripts that aren't in this repo.

The user's disc image is needed both at build time (data is converted to C) and at run time (the game reads files from `disks/us/`). Put the cue sheet at `disks/sotn.us.cue`. Requirements: clang (Xcode CLT on macOS), CMake ≥ 3.21, Go ≥ 1.24 and cargo. On Linux, also a Vulkan driver and the development headers SDL needs for its Wayland/X11 and audio backends (package lists in `README.md`).

## Commands

```sh
./build.sh                          # first run: extract the disc to disks/us/ and generate src/**/gen + assets/; always: configure and build everything
./build.sh --assets                 # also regenerate the data (after editing config/assets.us.yaml)
./build.sh --run --stage nz1        # build, then run build/sotn with these args
cmake --build build --target are    # rebuild one overlay; the target is the module name (are, rno0, bo4, mar, w0_012, tt_003, ...)
cargo test --manifest-path tools/sotn_str/Cargo.toml   # sotn_str unit tests, the only unit tests in the repo
./build/sotn --help                 # all runtime flags (--stage/--player/--spawn-point/--scale/--filter/...)
```

- Each source file is listed by hand in `CMakeLists.txt` (`SOURCE_FILES_*` lists, then `add_overlay(...)`). A new `.c` file does nothing until you add it there. `build.sh` reruns the configure step every time for this reason.
- The `sotn` target depends on every module, so `--target sotn` (or a plain build) builds all ~180 modules (`.dylib` on macOS, `.so` on Linux).
- Run from the repo root. The game reads `disks/` relative to the working directory and loads its modules from beside `build/sotn`. It also creates empty `bu00/` and `bu10/` there on start-up.
- On Linux, `build.sh` builds with clang: it sets `CC=clang` unless `CC` is set, which CMake only reads on the first configure (delete `build/` to switch). GCC, Linux's usual `cc`, stops on GCC-only `-Werror` flags in the decompiled code.
- ASan is off by default (`-DSOTN_ASAN=ON`). On macOS it reports false positives on Metal buffer maps after overlays are `dlclose`d. On Linux it works; run with `ASAN_OPTIONS=detect_leaks=0`, since the leak checker flags system libraries at exit.
- Formatting follows the root `.clang-format` (4 spaces, 80 columns). Format only the code you change, because some `src/pc` files already differ from clang-format output. Don't run `sotn-assets format`: it is the upstream repo's whole-tree formatter.

## Verifying changes

There is no test suite for the game itself. Check a change by running the game with no one at the controls; the environment variables are documented in the header comment of `src/pc/framedump.c`:

```sh
SOTN_SHOT_DIR=$OUT SOTN_SHOT_EVERY=120 SOTN_EXIT_AFTER=240 SOTN_FAST=1 \
  ./build/sotn --stage are --record $OUT/rec.bin
# writes $OUT/frame_000120.png and frame_000240.png, then quits; SOTN_FAST skips vsync and audio
```

- Autosave (`src/pc/autosave.c`) writes to the memory card on every room change in normal play. It is off in scripted runs, `--stage`/`--spawn-point` starts and replays. Set `SOTN_AUTOSAVE=1` to test it, and do that in a scratch directory that has a `disks` symlink, so the user's `bu00/` stays untouched.
- **Always pass `--record <path>` in scripted runs.** Without it, every run records to `recordings/rec1.bin` and shifts rec1 to rec2 and onward, deleting the user's oldest recording (`rec4.bin`).
- Scripted runs (any `SOTN_INPUT` or `SOTN_EXIT_AFTER`) don't attach the disc image, so there's no music or voice acting (only "no track found" warnings) unless you pass `--disk disks/sotn.us.cue`. Cutscenes that wait for a voice line also need audio running, so leave out `SOTN_FAST`. Set `SDL_AUDIO_DRIVER=disk` to send the sound to a file instead of the speakers.
- `SOTN_INPUT="120:none,90:right,15:right+cross"` drives pad 1. To start somewhere specific, use `--spawn-point <name>` (named points are in `src/pc/spawn_point.c`) or `SOTN_SPAWN="<room>,<x>,<y>"` together with `--stage`. Other variables set game state: `SOTN_GOD`, `SOTN_EQUIP`, `SOTN_RELICS`, `SOTN_EVENTS`, `SOTN_CASTLE_FLAGS`, `SOTN_KILL`, `SOTN_SEEK`, `SOTN_TRACE`.
- To check determinism, run `./build/sotn --replay rec.bin --replay-and-exit --replay-fast`. It exits non-zero if the game state (position, HP, RNG, game step and so on) drifts from the recording. `--demo <n> --replay-and-exit` exits non-zero if the player dies during a built-in demo. A recording stores the keyboard or controller, not the `SOTN_INPUT` script (`Replay_OnFrame` records before `FrameDump_OnFrame` sets the pads), so a scripted run that presses buttons drifts on replay. Check determinism with runs that use no script, or by running a demo twice with `SOTN_TRACE` and comparing.
- Logs go to stderr as `[I]/[W]/[E][file:line][function]`.
- After changing overlay code, load that overlay (`--stage <name>`). A missing symbol only shows up at that point (see below).

## Architecture

### Host executable + overlay modules

On the PSX, the engine (DRA) swaps stage, weapon and familiar overlays into fixed RAM. Here:

- `build/sotn` is built from `src/pc` (start-up, file I/O, overlay loading and PC replacements for PSX/BIOS behavior), `src/dra` (the engine and Alucard) and `src/ric` (Richter).
- Each of these is a module (`.dylib`, `.so` on Linux) built by `add_overlay()` → `psyz_add_module()`:
  - stages: `src/st/<name>`, where an `r` prefix means the inverted castle
  - bosses: `src/boss/<name>`
  - familiars: `src/servant/tt_NNN`
  - weapons: `src/weapon/w_NNN.c`, compiled twice as `w0_NNN`/`w1_NNN`, one per hand, via `WEAPON0`/`WEAPON1`
- Each overlay has a glue file in `src/pc/{stages,bosses,servants}/`. Weapons have theirs in `src/weapon/weapon_private.h`. The glue's `Psyz_ModuleStart(void* param)` copies the overlay's header (`g_Overlay`, `g_BossOverlay` or `*_ServantDesc`) into the struct DRA calls through (`g_api.o`, an `Overlay` from `include/game.h`). For stages and bosses it also sets the entity-update table and layout pointers.
- `src/pc/stages/overlay.c` loads overlays with `dlopen(RTLD_NOW | RTLD_LOCAL)` and closes the previous stage first. The stage name is `g_StagesLba[id].ovlName` in lowercase (`src/pc/sim_pc.c`).
- Overlays call the engine through the `g_api` function table, as on PSX. They also link directly against host symbols: the host is built with `ENABLE_EXPORTS`, and overlays with `-undefined dynamic_lookup` on macOS and `-Wl,-Bsymbolic` on Linux (where undefined symbols are allowed anyway). **So an undefined symbol in an overlay is not a link error.** The `dlopen` fails at run time instead. For a stage or boss, the game shows an error dialog and exits. A weapon calls `abort()`. A familiar quietly does nothing.
- `HARD_LINK` is defined. Helpers that each PSX overlay had its own copy of (`DestroyEntity`, parts of `src/st/st_common.h`, ...) are left out of overlays with `#ifndef HARD_LINK` and resolve to the host's copy. `src/pc/sotn.c` `#undef`s `HARD_LINK` so the host defines them.
- PSX globals that sat at fixed addresses are ordinary definitions, mostly in `src/pc/stubs.c` and `src/pc/sotn.c`.
- Data that wasn't turned into C (graphics, sound, the weapon and familiar art and so on) is read from `disks/us/` at run time (`src/pc/sim_pc.c`, `src/pc/io.c`).

### Shared code lives in headers

Code that several stages share is in `.h` files (`src/st/*.h`, `src/boss/*.h`, `src/*.h`). Each stage `#include`s them from a thin `.c` file; for example, `src/st/are/e_red_door.c` is just `#include "are.h"` plus `#include "../e_red_door.h"`. Every overlay therefore compiles its own copy, and an edit to `src/st/e_red_door.h` changes every stage that includes it. Per-stage differences use `STAGE_IS_<NAME>` macros, which each stage defines in `src/st/<name>/<name>.h`.

### Preprocessor configuration

Every target compiles with `VERSION_PC NON_MATCHING PERMUTER HARD_LINK _internal_version_us __psyz`; overlays also get `OVERLAY_BUILD`. Consequences:

- `VERSION_PSP`, `VERSION_HD` and `VERSION_BETA` branches and the `#ifndef NON_MATCHING` halves are dead code kept from upstream. Read the `VERSION_PC`/`NON_MATCHING` side.
- `INCLUDE_ASM(...)` expands to nothing (because `PERMUTER` is defined), so a function that exists only as `INCLUDE_ASM` doesn't exist in this build.
- PC-only changes go inside `#ifdef VERSION_PC`. Pointers are 64-bit, so the `/* 0x.. */` offsets and `size = 0x..` comments describe the PSX layout. Decompiled code that keeps pointers in 32-bit fields needs PC handling (see `PRIM_DR_ENV` in `include/common.h`). `STATIC_ASSERT` is active on PC.
- Code between `// begin pc_import_data.py` and `// end pc_import_data.py`, and the `pc_blobs.c` files, is retail data imported as C by scripts that aren't in this repo. It is not hand-written.
- `-funsigned-char` is set. Many warnings are silenced for the decompiled code, but some are errors (see the top of `CMakeLists.txt`).

### Data layout: x86-64 vs arm64

Decompiled code sometimes reads past the end of one data table into the next one, which worked on PSX because the overlay's data was contiguous (an `AnimateEntity` call left at a stale `pose` is the usual case). On PC each table is a separate global, and clang gives every global array of 16 bytes or more 16-byte alignment on x86-64 (a psABI rule) but not on arm64. So such a read gets the next table on macOS, as on PSX, but zero padding on Linux, and the game diverges, for example in a built-in demo. Fix it by giving the tables one storage that reproduces the PSX layout (`D_us_801806B0` in `src/boss/rbo2/unk_1B284.c`, `src/pc/stages/stage_mad.c`), checked against the original bytes in `disks/us/` (stage and boss overlays load at `0x80180000`). Don't rely on declaration order or alignment attributes. An ASan build finds these reads as `global-buffer-overflow`. Two known leftovers are harmless: `LoadWeaponPalette` (`src/weapon/shared.h`) copies 24 palettes from weapons that define fewer, filling slots nothing draws with, and `rbo1`'s minions read `frames[-1]` from a preceding 8-byte table, which stays adjacent on both architectures.

### Strings: `_S("...")`

`tools/sotn_str` (Rust) is the `C_COMPILER_LAUNCHER` for every target. It preprocesses each file with `-DSOTN_STR` and re-encodes `_S("...")` literals into the game's 8x8-font encoding. Without the launcher, `_S(x)` is just `x`. Other non-ASCII literals (`"Ａ"`, `"Select file to be loaded．"`) stay UTF-8 on PC, while the PSX build ran its sources through iconv to Shift-JIS, so code that walks Shift-JIS byte pairs must read a byte ≥ 0xC0 with `PcUtf8ToSjis` (`src/pc/krom_pc.c`), as DRA's menu (`src/dra/menu.c`) and SEL's file-select text (`src/st/sel/318CC.c`) do.

### Generated data

`src/**/gen/` and `assets/` are gitignored and generated from the disc by `tools/sotn-assets`: `extract-assets`, then `build-assets`, both run on `config/assets.us.yaml`. The `config/splat.us.*.yaml` files describe the overlays' segment layouts. Don't hand-edit `gen/`. Change the yaml or the tool, then run `./build.sh --assets`. `tools/sotn-disk` extracts the files from the disc image. Most other `sotn-assets` subcommands (objdiff, bindiff, progress, disk, format) belong to the full decomp repo.

### psyz (`tools/psyz`)

A vendored replacement for the PlayStation SDK (PSY-Q): GPU, SPU, CD, pads, memory cards and GTE, built on SDL3, which is built statically from `tools/psyz/external/SDL`. The renderer is SDL_GPU (`src/platform/sdl3_gpu.c`): Metal on macOS, Vulkan on Linux. Shaders are compiled into headers in `src/platform/shaders/` by `build_shaders.sh`: `psx`/`clear` from GLSL to SPIR-V, MSL and DXIL, and the MMPX/xBR filters written twice by hand (`.metal` and `.glsl`), so a change to one needs the same change in the other. The CRT filter (`crt.frag.glsl`) and the Modern filter (`modern_sharpen.frag.glsl`, ScaleFX in `scalefx_pass0-4.frag.glsl` ported from libretro, `modern_bloom.frag.glsl`, `modern.frag.glsl`, plus `modern_composite.frag.glsl` and `modern_crt.frag.glsl` for Modern + CRT) have no MSL version yet, so on macOS `Filter_Init` reports them unavailable. To check a filter, `SOTN_SHOT_MMPX=1` dumps every pass of the last filter run (Modern: the 1x input, ScaleFX's 3x picture, the final image; Modern + CRT adds the composite picture and the CRT one). On a 120/240 Hz display the backend presents several refreshes per game frame with interpolated frames in between (`sdl3_gpu_interp.h`), trusting driver VSync to block once per refresh; `CheckDriverVsync` in `sdl3_common.h` falls back to the frame limiter when it doesn't (XWayland on COSMIC returns about twice per refresh, which ran the game at 2x speed). SDL picks XWayland over Wayland when the compositor lacks fifo-v1; `SDL_VIDEO_DRIVER=wayland` overrides that. The overlay loader is in `tools/psyz/psyz/src/platform/module_unix.c` and the CMake helpers (`psyz_add_module`, `psyz_exports`) in `tools/psyz/psyz/cmake/module.cmake`. Its own test runner (`ztest`) isn't vendored, so `make test` in `tools/psyz/psyz` doesn't work.
