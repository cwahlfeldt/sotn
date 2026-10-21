# Castlevania: Symphony of the Night — native macOS and Linux build

A native build of Castlevania: Symphony of the Night (US version) from C
source, for Apple Silicon Macs and for Linux (x86-64, Vulkan).

You need your own copy of the game: the build turns data from the disc into
C, and the game reads the disc's files while it runs.

## Requirements

- clang and CMake 3.21 or later
- Go 1.24 or later (the disc and asset tools in `tools/`)
- Rust's `cargo` (`tools/sotn_str`, run by the build)

On macOS, clang comes with Xcode's command line tools.

On Linux, the game draws with Vulkan, so it needs a GPU with a Vulkan driver
(Mesa's RADV, ANV or NVK, or NVIDIA's). The build compiles SDL from
`tools/psyz/external/SDL`, and SDL only includes the window and sound
backends whose development headers it finds, so install those before the
first build:

```sh
# Arch, CachyOS, Manjaro
sudo pacman -S --needed clang cmake go rust wayland wayland-protocols \
    libxkbcommon libdecor libx11 libxcursor libxext libxfixes libxi \
    libxrandr libxss libxtst alsa-lib libpulse pipewire vulkan-headers

# Debian, Ubuntu (22.04 or later; Ubuntu's own Go may be too old)
sudo apt install clang cmake golang cargo libwayland-dev wayland-protocols \
    libxkbcommon-dev libdecor-0-dev libx11-dev libxcursor-dev libxext-dev \
    libxfixes-dev libxi-dev libxrandr-dev libxss-dev libxtst-dev \
    libasound2-dev libpulse-dev libpipewire-0.3-dev libudev-dev \
    libdbus-1-dev mesa-vulkan-drivers

# Fedora
sudo dnf install clang cmake golang cargo wayland-devel \
    wayland-protocols-devel libxkbcommon-devel libdecor-devel libX11-devel \
    libXcursor-devel libXext-devel libXfixes-devel libXi-devel \
    libXrandr-devel libXScrnSaver-devel libXtst-devel alsa-lib-devel \
    pulseaudio-libs-devel pipewire-devel systemd-devel dbus-devel \
    mesa-vulkan-drivers
```

SDL's full lists are in `tools/psyz/external/SDL/docs/README-linux.md`. If
the game opens no window or plays no sound, look for `wayland`, `x11` and
`pipewire` or `pulseaudio` in the "Video drivers" and "Audio drivers" lines
that `./build.sh` prints while configuring; after installing what was
missing, delete `build/` and build again.

`./build.sh` builds with clang. GCC is not supported: it stops on warnings
the build treats as errors in the decompiled code.

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

The result is `build/sotn` and one module (`.dylib` on macOS, `.so` on
Linux) per stage, boss, weapon and familiar beside it. The game loads them
from its own directory, so keep them together.

## Play

Run from this directory; the game reads `disks/` relative to it and writes
its memory cards (`bu00/`, `bu10/`) and input recordings (`recordings/`)
here:

```sh
./build/sotn
./build/sotn --window 1280x960 --scale 3
./build/sotn --filter xbr --fullscreen
./build/sotn --filter crt
./build/sotn --filter modern
```

Keyboard: arrows move, X jump, Z / D attack, S back dash, Q / R / E mist,
bat and wolf forms, Return pause, Backspace map. F4 fullscreen, F5 cycles
the filters (MMPX and xBR upscaling, a CRT look with scanlines, and Modern:
the original pixels, crisp and sharpened, with glowing lights and richer
colour; CRT and Modern are Linux only for now), Esc quits. A game controller
works too.

On a display refreshing at a multiple of 60 Hz (120 or 240 Hz), the game
draws in-between frames so motion looks smoother (`--no-interp` turns that
off). That needs VSync that waits for every refresh. On Linux, SDL draws
through XWayland unless the compositor supports the fifo-v1 protocol, and
XWayland may not wait (it doesn't on COSMIC): the game notices within a second
and shows each frame once instead. To get the in-between frames there, run it
on Wayland directly with `SDL_VIDEO_DRIVER=wayland ./build/sotn`.

The game saves itself each time you enter a room, once Alucard is standing
in his own form: to the memory card slot you loaded from or last saved to,
or the first free one for a new game. Loading such a save puts you back where
you were; saves made at save points load in the coffin as usual. Set
`SOTN_AUTOSAVE=0` to turn it off. New games start with stereo sound.

## What's here

| Path | What |
| --- | --- |
| `src/dra`, `src/ric` | The main game and Richter, compiled into `build/sotn` |
| `src/st`, `src/boss` | Stages and bosses, one module each |
| `src/weapon`, `src/servant` | Weapons and familiars, one module each |
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
