# Castlevania: Symphony of the Night — native macOS build

A native Apple Silicon build of Castlevania: Symphony of the Night (US
version) from C source.

You need your own copy of the game: the build turns data from the disc into
C, and the game reads the disc's files while it runs.

## Requirements

- Xcode's command line tools (clang), CMake 3.21 or later
- Go (the disc and asset tools in `tools/`)
- Rust's `cargo` (`tools/sotn_str`, run by the build)

## Build

Put the US disc (SLUS-00067) image in `disks/` and name the cue sheet
`sotn.us.cue`:

```
disks/sotn.us.cue
disks/Castlevania - Symphony of the Night (USA) (Track 1).bin
disks/Castlevania - Symphony of the Night (USA) (Track 2).bin
```

Then:

```sh
./build.sh
```

The first run extracts the disc into `disks/us/`, generates the data files
(`src/**/gen/`, `assets/`) and builds. Later runs only rebuild.
`./build.sh --assets` regenerates the data after a change to
`config/assets.us.yaml`.

The result is `build/sotn` and one `.dylib` per stage, boss, weapon and
familiar beside it. The game loads them from its own directory, so keep them
together.

## Play

Run from this directory; the game reads `disks/` relative to it and writes
its memory cards (`bu00/`, `bu10/`) and input recordings (`recordings/`)
here:

```sh
./build/sotn
./build/sotn --window 1280x960 --scale 3
./build/sotn --filter xbr --fullscreen
```

Keyboard: arrows move, X jump, Z / D attack, S back dash, Q / R / E mist,
bat and wolf forms, Return pause, Backspace map. F4 fullscreen, F5 cycles
the upscaling filters, Esc quits. A game controller works too.

The game saves itself each time you enter a room, once Alucard is standing
in his own form: to the memory card slot you loaded from or last saved to,
or the first free one for a new game. Loading such a save puts you back where
you were; saves made at save points load in the coffin as usual. Set
`SOTN_AUTOSAVE=0` to turn it off. New games start with stereo sound.

## What's here

| Path | What |
| --- | --- |
| `src/dra`, `src/ric` | The main game and Richter, compiled into `build/sotn` |
| `src/st`, `src/boss` | Stages and bosses, one `.dylib` each |
| `src/weapon`, `src/servant` | Weapons and familiars, one `.dylib` each |
| `src/pc` | The PC side: start-up, file loading, overlay loading, stubs |
| `include` | Shared headers |
| `tools/psyz` | Stands in for the PlayStation's SDK and hardware (graphics, sound, CD, memory cards) on SDL3, which it builds from `tools/psyz/external/SDL` |
| `tools/sotn_str` | Re-encodes the game's strings at compile time |
| `tools/sotn-disk` | Extracts the files from the disc image |
| `tools/sotn-assets` | Turns the disc's data into C (`config/assets.us.yaml` lists what and where; the `config/splat.us.*.yaml` files give it the overlays' layouts) |

## Decompilation status

All the game code the PC build runs is C; no MIPS assembly is used. The C
was decompiled from the original, and parts are written for the port where
the PlayStation build still matches against assembly. Twenty functions are
still assembly only: 19 in BO3 (mostly copies of NO4's water code) and one
in RBO8. Nothing calls them, so the PC build leaves them out. The PlayStation's own
libraries and BIOS aren't decompiled here; psyz replaces them.

## License

AGPL-3.0-or-later (`LICENSE`), as the decompilation project; psyz and its
dependencies keep their own licenses under `tools/psyz`.
