#!/bin/bash
# Build (and optionally run) the native macOS port of Castlevania: Symphony
# of the Night.
#
# Usage:
#   ./build.sh                 build into build/
#   ./build.sh --run [args]    build, then run build/sotn with the given args
#                              e.g. ./build.sh --run --stage nz1 --window 1920x1080
#   ./build.sh --assets        regenerate the data generated from the disc
#                              (needed after editing config/assets.us.yaml)
#
# Needs your disc image as disks/sotn.us.cue (+ its .bin tracks), cmake, a C
# compiler, go and cargo.
set -euo pipefail
cd "$(dirname "$0")"

RUN=0
ASSETS=0
RUN_ARGS=()
while [[ $# -gt 0 ]]; do
    case "$1" in
    --run) RUN=1; shift; RUN_ARGS=("$@"); break ;;
    --assets) ASSETS=1; shift ;;
    *) echo "unknown option: $1 (use --run or --assets)"; exit 1 ;;
    esac
done

# the game files, read at build time (to generate data) and at run time
if [[ ! -f disks/us/DRA.BIN ]]; then
    if [[ ! -f disks/sotn.us.cue ]]; then
        echo "put your disc image in disks/ and name the CUE file sotn.us.cue"
        exit 1
    fi
    echo "== extracting the disc into disks/us =="
    go run ./tools/sotn-disk extract disks/sotn.us.cue disks/us > /dev/null
fi

# rooms, layouts, sprites, palettes... (src/**/gen) come from the disc
if [[ ! -f src/st/are/gen/rooms.c ]]; then
    ASSETS=1
fi
if [[ $ASSETS -eq 1 ]]; then
    echo "== generating data from the disc =="
    go run ./tools/sotn-assets extract-assets config/assets.us.yaml
    go run ./tools/sotn-assets build-assets config/assets.us.yaml
fi

# Always reconfigure: CMakeLists edits (e.g. new stage files) are otherwise
# silently ignored and you link stale objects.
echo "== configuring =="
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

echo "== building =="
cmake --build build -j "$(sysctl -n hw.ncpu)"

echo "== built: build/sotn =="

if [[ $RUN -eq 1 ]]; then
    # Must run from this directory: the game reads disks/ relative to it.
    # Stage, weapon and familiar overlays load from build/ beside the binary.
    exec ./build/sotn "${RUN_ARGS[@]}"
fi
