#!/bin/bash
# Build (and optionally run) the native macOS and Linux port of Castlevania:
# Symphony of the Night.
#
# Usage:
#   ./build.sh                 build into build/
#   ./build.sh --run [args]    build, then run build/sotn with the given args
#                              e.g. ./build.sh --run --stage nz1 --window 1920x1080
#   ./build.sh --assets        regenerate the data generated from the disc
#                              (needed after editing config/assets.us.yaml)
#
# Edited shaders (tools/psyz/psyz/src/platform/shaders/*.glsl) are recompiled
# too, with glslangValidator.
#
# Needs your disc image as disks/sotn.us.cue (+ its .bin tracks), cmake,
# clang, go and cargo.
set -euo pipefail
cd "$(dirname "$0")"

# The game is built and tested with clang, but cc is GCC on most Linux
# systems. Only read on the first configure: delete build/ to switch.
if [[ "$(uname)" == Linux && -z "${CC:-}" ]]; then
    export CC=clang CXX=clang++
fi

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

# The SDL_GPU shaders are compiled into C headers beside their GLSL sources.
# Recompile the SPIR-V (Vulkan) ones whose GLSL changed; the MSL (Metal) and
# DXIL ones need build_shaders.sh there, with spirv-cross and dxc.
SHADERS=tools/psyz/psyz/src/platform/shaders
spv_header() { # <spv file> <C name>: the header build_shaders.sh makes (xxd -i)
    od -An -v -tx1 "$1" | tr -s ' \n' '\n' | grep . | awk -v name="$2" '
        { b[n++] = "0x" $1 }
        END {
            print "// clang-format off"
            print "unsigned char " name "[] = {"
            for (i = 0; i < n; i++) {
                if (i % 12 == 0) printf "  "
                printf "%s", b[i]
                if (i < n - 1) printf (i % 12 == 11 ? ",\n" : ", ")
            }
            printf "\n};\nunsigned int %s_len = %d;\n", name, n
        }'
}
for src in "$SHADERS"/*.glsl; do
    shader=$(basename "$src" .glsl) # e.g. crt.frag
    name=$(echo "$shader" | tr . _)
    header="$SHADERS/${name}_spv.h"
    if [[ ! "$src" -nt "$header" ]]; then
        continue
    fi
    if ! command -v glslangValidator > /dev/null; then
        echo "warning: $src changed, but without glslangValidator (glslang)" \
            "the game keeps the old $header"
        continue
    fi
    echo "== compiling $shader to SPIR-V =="
    mkdir -p build/shaders
    if ! out=$(glslangValidator -V --target-env vulkan1.0 -S "${shader##*.}" \
        "$src" -o "build/shaders/$shader.spv" 2>&1); then
        echo "$out"
        exit 1
    fi
    spv_header "build/shaders/$shader.spv" "${name}_spv" \
        > "build/shaders/${name}_spv.h"
    if cmp -s "build/shaders/${name}_spv.h" "$header"; then
        touch "$header" # the same code (a comment changed): up to date now
    else
        mv "build/shaders/${name}_spv.h" "$header"
    fi
done

# Always reconfigure: CMakeLists edits (e.g. new stage files) are otherwise
# silently ignored and you link stale objects.
echo "== configuring =="
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

echo "== building =="
cmake --build build -j "$(getconf _NPROCESSORS_ONLN)"

echo "== built: build/sotn =="

if [[ $RUN -eq 1 ]]; then
    # Must run from this directory: the game reads disks/ relative to it.
    # Stage, weapon and familiar overlays load from build/ beside the binary.
    exec ./build/sotn "${RUN_ARGS[@]}"
fi
