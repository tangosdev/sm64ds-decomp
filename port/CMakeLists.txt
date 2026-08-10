cmake_minimum_required(VERSION 3.16)
project(sm64ds-port C CXX)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MAP")
add_compile_options(/Oy-)

# Gate 1: compile the manifest slice out of ../src plus the HAL, link the
# smoke runner. 32-bit host only -- platform.h enforces it at compile time.

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# ---- Linux port (Phase 1 Lane A): the SDL2 host window/input/blit backend ----
# On Windows this stays OFF and nothing about the build changes -- the MSVC
# walk_window path is byte-for-byte the Win32 GDI/user32 one. On a non-MSVC
# (Linux/g++) build the platform seam in walk_window.cpp compiles the SDL2
# backend instead, and this wires SDL2 in. Default follows the platform: ON when
# not MSVC, OFF on MSVC. See port/hal/host_platform_linux.{h,cpp}.
if(MSVC)
    option(PORT_ENABLE_SDL2 "Build the SDL2 host backend (Linux port)" OFF)
else()
    option(PORT_ENABLE_SDL2 "Build the SDL2 host backend (Linux port)" ON)
endif()
if(PORT_ENABLE_SDL2 AND NOT MSVC)
    find_package(SDL2 REQUIRED)
    message(STATUS "PORT_ENABLE_SDL2: SDL2 found (${SDL2_LIBRARIES})")
    # The interactive-window targets (walk_window, walk_window_hires) get the
    # SDL2 backend TU + link. Applied where those targets are defined below via
    # port_enable_sdl2(<target>).
    function(port_enable_sdl2 tgt)
        target_sources(${tgt} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/hal/host_platform_linux.cpp")
        target_include_directories(${tgt} PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(${tgt} PRIVATE ${SDL2_LIBRARIES})
    endfunction()
else()
    function(port_enable_sdl2 tgt)
    endfunction()
endif()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate1.txt" SLICE_LINES)
set(SLICE_SOURCES "")
foreach(line IN LISTS SLICE_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

add_executable(smoke
    tests/smoke.cpp
    hal/cstd_div.c
    hal/os_time.cpp
    hal/shims.cpp
    ${SLICE_SOURCES})

target_include_directories(smoke PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_compile_definitions(smoke PRIVATE SM64DS_PLATFORM_PC)
if(MSVC)
    # The decomp C uses guest idioms MSVC warns about; keep the build quiet
    # enough that a NEW warning is visible. /W3 stays on.
    target_compile_options(smoke PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 2: the allocator family + torture smoke, its own binary so gate 1
# stays a stable baseline.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate2.txt" SLICE2_LINES)
set(SLICE2_SOURCES "")
foreach(line IN LISTS SLICE2_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

add_executable(smoke_heap tests/smoke_heap.cpp hal/heap_globals.cpp ${SLICE2_SOURCES})
target_include_directories(smoke_heap PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_compile_definitions(smoke_heap PRIVATE SM64DS_PLATFORM_PC)
if(MSVC)
    target_compile_options(smoke_heap PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 3a: root heap + Memory:: layer over the gate-2 allocator.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate3a.txt" SLICE3A_LINES)
set(SLICE3A_SOURCES "")
foreach(line IN LISTS SLICE3A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE3A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

add_executable(smoke_roots tests/smoke_roots.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES})
target_include_directories(smoke_roots PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_compile_definitions(smoke_roots PRIVATE SM64DS_PLATFORM_PC)
if(MSVC)
    target_compile_options(smoke_roots PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# The ntr platform layer: imported host DS hardware (see ntr/README.md).
# Compiled as its own lib so every gate binary can link the pieces it needs.
add_library(ntr STATIC
    ntr/io.cpp ntr/gx.cpp ntr/texture.cpp ntr/ppu.cpp ntr/ppu_sub.cpp ntr/lz77.cpp
    ntr/rt.cpp ntr/runtime.cpp ntr/fs.cpp ntr/bmd.cpp)
target_include_directories(ntr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ntr/include")
if(MSVC)
    target_compile_options(ntr PRIVATE /wd4311 /wd4312)
endif()

# 512x384 middle tier: 4x the pixels of native (~12ms software raster),
# half the chunk of DS-native. The interactive window's default.
add_library(ntr_2x STATIC
    ntr/io.cpp ntr/gx.cpp ntr/texture.cpp ntr/ppu.cpp ntr/ppu_sub.cpp ntr/lz77.cpp
    ntr/rt.cpp ntr/runtime.cpp ntr/fs.cpp ntr/bmd.cpp)
target_include_directories(ntr_2x PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ntr/include")
target_compile_definitions(ntr_2x PUBLIC NTR_HIRES2)
if(MSVC)
    target_compile_options(ntr_2x PRIVATE /wd4311 /wd4312)
endif()

# The same layer at 1024x768 for the interactive window; the smokes keep
# the DS-native lib (their reference pixel counts depend on 256x192).
add_library(ntr_hires STATIC
    ntr/io.cpp ntr/gx.cpp ntr/texture.cpp ntr/ppu.cpp ntr/ppu_sub.cpp ntr/lz77.cpp
    ntr/rt.cpp ntr/runtime.cpp ntr/fs.cpp ntr/bmd.cpp)
target_include_directories(ntr_hires PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ntr/include")
target_compile_definitions(ntr_hires PUBLIC NTR_HIRES)
if(MSVC)
    target_compile_options(ntr_hires PRIVATE /wd4311 /wd4312)
endif()

# Gate 3b: SharedFilePtr over the host card seam; assets come from the
# repo's extracted/ tree + asset catalog, root baked in (env-overridable).
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate3b.txt" SLICE3B_LINES)
set(SLICE3B_SOURCES "")
foreach(line IN LISTS SLICE3B_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE3B_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

get_filename_component(PORT_REPO_ROOT_ABS "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# Build-time git tip embed (fault_probe.h's rich dump reports which build
# crashed). Resolved at configure time via `git rev-parse`; a re-configure on a
# new commit refreshes it. Emitted as a tiny .c so any target that wants the tip
# just links it. Falls back to "unknown" outside a git checkout, which matches
# fault_probe.h's weak default.
find_program(GIT_EXECUTABLE git)
set(PORT_GIT_TIP "unknown")
if(GIT_EXECUTABLE)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" -C "${PORT_REPO_ROOT_ABS}" rev-parse --short HEAD
        OUTPUT_VARIABLE PORT_GIT_TIP_OUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET RESULT_VARIABLE PORT_GIT_RC)
    if(PORT_GIT_RC EQUAL 0 AND PORT_GIT_TIP_OUT)
        set(PORT_GIT_TIP "${PORT_GIT_TIP_OUT}")
    endif()
endif()
set(PORT_GITTIP_C "${CMAKE_BINARY_DIR}/host-src/port_gittip.c")
file(WRITE "${PORT_GITTIP_C}"
    "/* generated by CMake -- the git tip this build was cut from */\n"
    "const char port_build_gittip[] = \"${PORT_GIT_TIP}\";\n")

# Gate 4a: the game's display-list submission machinery over the ntr GX.
# These src/ files touch MMIO textually, so they build from hostgen-emitted
# copies under the build tree -- src/ itself is never edited.
set(GATE4A_SYMS
    func_0205532c func_02055388 func_020553a4 func_02055780
    func_0205a358 func_0205a290 func_0205a21c func_02056e98 func_02059fa8)
set(GATE4A_GEN "")
foreach(sym IN LISTS GATE4A_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE4A_GEN "${sym_out}")
endforeach()

add_executable(smoke_gx tests/smoke_gx.cpp ${GATE4A_GEN})
target_include_directories(smoke_gx PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_gx PRIVATE ntr)
target_compile_definitions(smoke_gx PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_gx PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 4b: the whole Model pipeline. Adds the material-bind and matrix
# feeders to the hostgen set and the Model/render slice on top of everything.
set(GATE4B_SYMS func_02044b30 Geometry_MatrixMultiply3x3 func_02055998
    func_02044534 func_0204488c NormalizeVec3)
set(GATE4B_GEN "")
foreach(sym IN LISTS GATE4B_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE4B_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate4b.txt" SLICE4B_LINES)
set(SLICE4B_SOURCES "")
foreach(line IN LISTS SLICE4B_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE4B_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# ROM constants (sine table, default matrices): generated locally from the
# extracted arm9 image so Nintendo bytes never enter the repo.
# ROM-CLEAN (2026-08-10): when ON, romdata.py / ovdata.py emit ZEROED arrays and
# a sidecar blob part; the ROMBLOB command below consolidates every part into
# build/assets/romdata.bin (+ manifest + dispatch) and hal/romdata_loader.cpp
# loads it at boot, so the shipped exe embeds no ROM bytes. Default OFF: every
# other target keeps baking the bytes in exactly as before -- see
# out/rom-clean/DESIGN.md and port/tools/romblob_common.py.
option(PORT_ROM_CLEAN "Move baked ROM tables out of the exe into a runtime blob" OFF)
if(PORT_ROM_CLEAN)
    set(ROMCLEAN_FLAG --rom-clean)
else()
    set(ROMCLEAN_FLAG "")
endif()
# Every emitter that ships ROM bytes appends its .blobmap here; ROMBLOB reads them.
set(ROMCLEAN_PARTS "")

set(ROMDATA_C "${CMAKE_BINARY_DIR}/host-src/romdata.c")
add_custom_command(OUTPUT "${ROMDATA_C}"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/romdata.py" "${ROMDATA_C}"
            ${ROMCLEAN_FLAG}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/romdata.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_common.py"
    COMMENT "romdata from extracted/arm9_dec.bin")
if(PORT_ROM_CLEAN)
    list(APPEND ROMCLEAN_PARTS "${ROMDATA_C}.blobmap")
endif()

add_executable(smoke_model tests/smoke_model.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} "${ROMDATA_C}")
target_include_directories(smoke_model PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_model PRIVATE ntr)
target_compile_definitions(smoke_model PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_model PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 5: the fiber frame loop -- game-shaped frames, host-rastered.
add_executable(smoke_frames tests/smoke_frames.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} "${ROMDATA_C}")
target_include_directories(smoke_frames PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_frames PRIVATE ntr)
target_compile_definitions(smoke_frames PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_frames PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 4d: the catalog soak -- every model through the game pipeline.
add_executable(smoke_soak tests/smoke_soak.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} "${ROMDATA_C}")
target_include_directories(smoke_soak PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_soak PRIVATE ntr)
target_compile_definitions(smoke_soak PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_soak PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 8: collision -- the octree walk over real KCL data. cstd::sqrt
# drives the hardware sqrt unit through literal MMIO -> hostgen.
set(GATE8_SYMS _ZN4cstd4sqrtEy _ZN4cstd11fdiv_resultEv _ZN4cstd16reciprocal_asyncE5Fix12IiE
    Matrix4x3_ApplyInPlaceToScale)
set(GATE8_GEN "")
foreach(sym IN LISTS GATE8_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE8_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate8.txt" SLICE8_LINES)
set(SLICE8_SOURCES "")
foreach(line IN LISTS SLICE8_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE8_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Three files the SPHERE pass needs that later gates already carry: gate 10 has
# func_02039794 and func_02037e58, gate 13 has DotVec3. The binaries that stop
# at gate 8/9 (smoke_clsn, smoke_actor) get them here; smoke_player and
# walk_window get them from their own slices, and adding them in both places
# would be a duplicate source in one target.
set(GATE8_EXTRA_SOURCES
    "${PORT_REPO_ROOT_ABS}/src/func_02039794.c"
    "${PORT_REPO_ROOT_ABS}/src/func_02037e58.c"
    "${PORT_REPO_ROOT_ABS}/src/DotVec3.c")

add_executable(smoke_clsn tests/smoke_clsn.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE8_SOURCES} ${GATE8_EXTRA_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} "${ROMDATA_C}")
target_include_directories(smoke_clsn PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_clsn PRIVATE ntr)
target_compile_definitions(smoke_clsn PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_clsn PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 7: ModelAnim -- the game owns frame progression.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate7.txt" SLICE7_LINES)
set(SLICE7_SOURCES "")
foreach(line IN LISTS SLICE7_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE7_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 24: BlendModelAnim -- the cross-fading ModelAnim. Rides on gate 7's
# stack and is proven by the same smoke; see slice_gate24.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate24.txt" SLICE24_LINES)
set(SLICE24_SOURCES "")
foreach(line IN LISTS SLICE24_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE24_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

add_executable(smoke_modelanim tests/smoke_modelanim.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/anim_bridge.cpp
    hal/blend_vtable.cpp hal/cstd_div.c
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE24_SOURCES} ${GATE4A_GEN} ${GATE4B_GEN}
    # BlendModelAnim::D0 calls func_0203cbc0, whose src form is an ARM r0
    # ride-through. The host copy that replaces it defines func_02073244
    # beside it and otherwise only reaches targets through gate 31.
    "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/func_02073244.c"
    "${ROMDATA_C}")
target_include_directories(smoke_modelanim PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_modelanim PRIVATE ntr)
target_compile_definitions(smoke_modelanim PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_modelanim PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 6: the 2D sprite engine's software side. OAM::Load reads POWCNT
# textually -> hostgen; the rest of the slice is plain.
set(GATE6_SYMS _ZN3OAM4LoadEv)
set(GATE6_GEN "")
foreach(sym IN LISTS GATE6_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE6_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate6.txt" SLICE6_LINES)
set(SLICE6_SOURCES "")
foreach(line IN LISTS SLICE6_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    if(line MATCHES "_ZN3OAM4LoadEv")
        continue()
    endif()
    list(APPEND SLICE6_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

add_executable(smoke_oam tests/smoke_oam.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE6_SOURCES} ${GATE4A_GEN} ${GATE4B_GEN} ${GATE6_GEN} "${ROMDATA_C}")
target_include_directories(smoke_oam PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_oam PRIVATE ntr)
target_compile_definitions(smoke_oam PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_oam PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 5b: the animation soak -- every compatible model+BCA pair.
add_executable(smoke_soak_anim tests/smoke_soak_anim.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} "${ROMDATA_C}")
target_include_directories(smoke_soak_anim PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_soak_anim PRIVATE ntr)
target_compile_definitions(smoke_soak_anim PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_soak_anim PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 4c: animation -- same slice (the 4c files live in slice_gate4b.txt),
# different driver: the piano posed by its BCA through the game's walk.
add_executable(smoke_anim tests/smoke_anim.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} "${ROMDATA_C}")
target_include_directories(smoke_anim PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_anim PRIVATE ntr)
target_compile_definitions(smoke_anim PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_anim PRIVATE /wd4311 /wd4312 /wd4068)
endif()

add_executable(smoke_fs tests/smoke_fs.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} "${ROMDATA_C}")
target_include_directories(smoke_fs PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_compile_definitions(smoke_fs PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_fs PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 9: the actor framework -- ArrowSignRight's full lifecycle.
# One .c file carries C++ syntax (mwccarm accepted it); hostgen's
# emit-as-cpp with extern "C" wrap makes it legal.
set(GATE9_SYMS _ZN9ActorBase21AfterCleanupResourcesEj)
set(GATE9_GEN "")
foreach(sym IN LISTS GATE9_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE9_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate9.txt" SLICE9_LINES)
set(SLICE9_SOURCES "")
foreach(line IN LISTS SLICE9_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE9_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

add_executable(smoke_actor tests/smoke_actor.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_SOURCES} ${GATE8_EXTRA_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE9_GEN} "${ROMDATA_C}")
target_include_directories(smoke_actor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_actor PRIVATE ntr)
target_compile_definitions(smoke_actor PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_actor PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 10: Player spawns and stands (the walking campaign).
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate10.txt" SLICE10_LINES)
set(SLICE10_SOURCES "")
foreach(line IN LISTS SLICE10_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE10_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# fix round: func_ov002_020e5948's HOST COPY (the TexSeq Prepare SHORT-1
# caller behind the L11 ONE_UP_MUSHROOM quarantine; Brain A's handoff,
# re-verified against the byte-locked src -- exactly the Prepare arity
# edits, nothing else). Replaces src/func_ov002_020e5948.c, commented out
# of slice_gate10.txt.
list(APPEND SLICE10_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/TexSeq_Caller_ov002_020e5948.cpp")

# Two Actor entry points are C++ inside a .c, but a different shape from the
# hostgen cases below: each is a real METHOD against Actor.h, so hostgen's
# wrap is wrong for them -- it would put the whole file, Actor.h include and
# all, inside extern "C". They compile as C++ instead, and
# hal/method_faces.cpp carries the C-linkage faces their callers spell. Both
# were plain C free functions until main rewrote them as methods.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN5Actor13SpawnSoundObjEj.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN5Actor24KillAndTrackInDeathTableEv.c"
    # gate 176: a //cpp-marked state body (local method shadows over extern "C"
    # bodies), the same shape -- compiles as C++, no face needed (its own
    # symbol is extern "C" inside the file).
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov071_0211f7d4.c"
    # gate 177: two //cpp-marked .c bodies plus one unmarked C++-ism (an empty
    # struct Callback {}), same treatment.
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN5Bully8BehaviorEv.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov064_02116220.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov064_0211616c.c"
    # gate 22 backfill: CanEnterDoor matched 2026-08-07 (PR #1223); //cpp-marked
    # .c whose Door dummy-virtual struct needs C++, own symbol extern "C".
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN6Player12CanEnterDoorEh.c"
    # gate 194: a //cpp-marked .c (a top-level `extern "C" {` block, invalid C),
    # HootTheOwl's own state-transition helper.
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov094_02136024.c"
    PROPERTIES LANGUAGE CXX)

# func_ov002_020d708c is C++ inside a .c (a `virtual` vtable shadow), which
# MSVC will not compile as C. hostgen.py already emits .cpp with the symbols
# wrapped back to C linkage, so route it through hostgen rather than editing
# the byte-verified tree.
# engine/fader/StartExitFaderWipe.c is the same case: a `virtual` class in a
# .c, this time the FaderWipe the death path calls SetToStart on. hostgen
# resolves a path as well as a bare name, so the fader subdirectory works.
# func_ov002_020c897c does void-pointer arithmetic (the NoControl body's
# `*(void**)(c + id * 4 + 0xdc) + 0x50` anim probe), which MSVC C rejects
# with C2036; hostgen's VOIDPP_ARITH rewrite retypes the deref to char*.
# func_ov002_020cfbdc and func_ov002_020d6c60 are a third shape: decl_common.h
# declares each with a different pointer parameter type than its own TU defines
# it with (void* vs char*, and (char*,void*) vs (char*,char*)). Two separate
# symbols while the decl_*.h headers were C++-mangled, one extern "C" overload
# since main gave them C linkage -- error C2733. hostgen's HEADER_SHADOW hides
# the header's declaration of just those names, so neither src/ nor include/
# has to move.
set(GATE10_SYMS func_ov002_020dec70 func_ov002_020d708c
    engine/fader/StartExitFaderWipe func_ov002_020c897c
    engine/fader/StartEntranceFaderWipe func_ov002_020cfbdc
    func_ov002_020d6c60)
set(GATE10_GEN "")
foreach(sym IN LISTS GATE10_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE10_GEN "${sym_out}")
endforeach()

# Every ovdata mount also writes a <out>.c.map sidecar, and OV_DATA_MAPS
# collects them for the --cross pass at the end of this block. See the module
# docstring in tools/ovdata.py: one invocation cannot see another's symbols, so
# a pointer from one mounted overlay into another needs a pass that has all of
# them in hand. tools/ovsweep.py reads the same sidecars to check the result.
set(OV_DATA_MAPS "")

set(OV002DATA_C "${CMAKE_BINARY_DIR}/host-src/ov002_data.c")
add_custom_command(OUTPUT "${OV002DATA_C}" "${OV002DATA_C}.map"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ov002
            "${OV002DATA_C}" --from-list "${CMAKE_CURRENT_SOURCE_DIR}/ov002_syms.txt"
            ${ROMCLEAN_FLAG}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_common.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/ov002_syms.txt"
    COMMENT "ovdata ov002")
list(APPEND OV_DATA_MAPS "${OV002DATA_C}.map")
if(PORT_ROM_CLEAN)
    list(APPEND ROMCLEAN_PARTS "${OV002DATA_C}.blobmap")
endif()

# Gate 30: the LEVEL overlays, one whole-image mount each.
#
# A level overlay cannot be mounted symbol by symbol: its LVL_Overlay hangs
# nine object tables off one base and the loaders walk them by count, straight
# across whatever symbol boundaries dsd happened to name. --whole emits the
# entire image as one array and rebases its internal pointers from the reloc
# table.
#
# Every level overlay is linked at the SAME DS base (0x021111a0) because the
# DS only ever has one loaded. On the host they are separate arrays and
# port_ovNNN_at() is per-overlay, so hosting several at once is just several
# mounts; hal/level_boot.cpp's table picks which one the boot walks.
#
#   ov009 = level 1, castle grounds  (data/stage/main_castle), course 29
#   ov014 = level 6, Bob-omb Battlefield  (data/stage/bombhei_map), course 0
#   ov010 = level 2, a castle interior floor, course 29 (interior-shaped)
#   ov015 = level 7, Whomp's Fortress, course 1 (open course)
#   ov011 = level 3, the castle garden  (data/stage/main_garden), course 29
#   ov016 = level 8, Jolly Roger Bay  (data/stage/kaizoku_irie), course 2
#
# Both facts come from the ROM, not from the overlay numbering: the arm9's
# level-to-overlay table data_020758c8 maps level -> overlay (it is level+8),
# and each overlay's LVL_Overlay names its own kcl/bmd/icg/icl OV0 HANDLES,
# resolved through build/assets/handles.tsv rather than by arithmetic. See the
# evidence note at the top of hal/level_boot.cpp, including what a derived
# handle constant got wrong; and the course numbers are the arm9
# SUBLEVEL_LEVEL_TABLE (0x02075298) which picks levels of two shapes here.
#
# ADDING A LEVEL IS MECHANICAL, and this list plus a row in hal/level_boot.cpp's
# port_level_table[] is the whole of it. The mount recipe lives in the header
# docstring of tools/ovdata.py; the level-boot recipe beside port_level_table[].
set(PORT_LEVEL_OVERLAYS ov009 ov014 ov010 ov015)
# gate 95: ov011 is level 3's own overlay (data/stage/main_garden, the castle
# garden, course 29). Mounted whole like the four above; the level->overlay map
# data_020758c8[3] = 11, the LVL_Overlay table data_02092208[3] = 0x021113ac,
# and the four handles at LVL_Overlay+8 resolve through build/assets/handles.tsv
# to main_garden's kcl/bmd/icg/icl. Its actor-overlay roster (ov063 ov085 ov089
# ov098 ov100 ov102) is all already mounted except ov063, so its shared cast
# spawns for free and only ov063-owned classes are skipped by name at the gate.
list(APPEND PORT_LEVEL_OVERLAYS ov011)
# gate 128: ov017 is level 9's own overlay (data/stage/kaizoku_ship, the sunken
# pirate ship, course 2 -- Jolly Roger Bay's ship sublevel). Mounted whole like
# the five above; the level->overlay map data_020758c8[9] = 17 (level+8, a read
# not an assumption), the LVL_Overlay table data_02092208[9] = 0x02111a78, and
# the four handles at LVL_Overlay+8 (bmd 1894 / kcl 1892 / icg 1895 / icl 1896)
# resolve through build/assets/handles.tsv to kaizoku_ship's bmd/kcl/icg/icl.
# SUBLEVEL_LEVEL_TABLE[9] (0x02075298) is 0x02 = course 2. Its actor-overlay
# roster (ov064 plus the always-loaded ov098/ov102) has ov098 and ov102 already
# mounted; ov064-owned classes are skipped by name at the gate. own_sinits 0.
list(APPEND PORT_LEVEL_OVERLAYS ov017)
# gate 121: ov016 is level 8's own overlay, the first MAIN COURSE the port
# mounts (data/stage/kaizoku_irie, Jolly Roger Bay, course 2). Read from the
# ROM like the rest: data_020758c8[8] = 16, LVL_Overlay data_02092208[8] =
# 0x02113b90, handles at +8 (bmd 0x0761, kcl 0x075f, icg 0x0762, icl 0x0763)
# resolve through build/assets/handles.tsv to kaizoku_irie's all.bmd/kcl/icg/icl.
# SUBLEVEL_LEVEL_TABLE[8] = 2 = course 2, a big open course (384 load relocs, a
# 25609-byte KCL). Mounted --whole like the rest; own_sinits stays 0.
list(APPEND PORT_LEVEL_OVERLAYS ov016)
# gate 114: ov012 is level 4's own overlay (data/stage/castle_b1, a castle
# basement floor, course 29). Mounted whole like the five above; read from the
# ROM: the level->overlay map data_020758c8[4] = 12, the LVL_Overlay table
# data_02092208[4] = 0x02111c54, and the four OV0 handles at LVL_Overlay+8
# (raw 0x0615/0x061b/0x061c/0x061d) resolve through build/assets/handles.tsv --
# the overlay's handle base is a constant +232 above the tsv index, confirmed
# across all five known-good levels -- to castle_b1's kcl/bmd/icg/icl.
# SUBLEVEL_LEVEL_TABLE[4] (0x02075298) is 0x1d = course 29, the castle family,
# interior-shaped like level 2. own_sinits stays 0.
list(APPEND PORT_LEVEL_OVERLAYS ov012)
# gate 117: ov013 is level 5's own overlay (data/stage/castle_2f, the castle
# second floor, course 29). Same evidence path: data_020758c8[5] = 13, the
# LVL_Overlay table data_02092208[5] = 0x02111844, and the four handles at
# LVL_Overlay+8 (raw 0x060b/0x0612/0x0613/0x0614, +232) resolve to castle_2f's
# kcl/bmd/icg/icl. SUBLEVEL_LEVEL_TABLE[5] = 0x1d = course 29. own_sinits 0.
list(APPEND PORT_LEVEL_OVERLAYS ov013)
# gate 135: ov018 is level 10's own overlay (data/stage/snow_mt, Cool Cool
# Mountain, course 3 -- the mountain main course). Mounted whole like the eight
# above; read from the ROM: the level->overlay map data_020758c8[10] = 18 (it is
# level+8, a read not an assumption), the LVL_Overlay table data_02092208[10] =
# 0x02112f2c, and the four OV0 handles at LVL_Overlay+8 (raw 0x06d7/0x06d5/
# 0x06d8/0x06d9) resolve through build/assets/handles.tsv -- the overlay's handle
# base is a constant +232 above the tsv index, confirmed across all nine
# known-good levels -- to snow_mt's all.bmd/kcl/icg/icl (a 46876-byte KCL, a big
# main course). SUBLEVEL_LEVEL_TABLE[10] (0x02075298) is 0x03 = course 3.
# Mounted --whole like the rest; own_sinits stays 0.
list(APPEND PORT_LEVEL_OVERLAYS ov018)
# gate 142: ov019 is level 11's own overlay (data/stage/snow_slider, Cool Cool
# Mountain's slide sublevel, course 3). Mounted whole like the seven above; read
# from the ROM: the level->overlay map data_020758c8[11] = 19 (level+8, a read
# not an assumption), the LVL_Overlay table data_02092208[11] = 0x02112c68, and
# the four OV0 handles at LVL_Overlay+8 (raw bmd 0x07c4 / kcl 0x07c2 / icg 0x07c5
# / icl 0x07c6) resolve through build/assets/handles.tsv -- looked up directly in
# the handle column, the same delta-0 mapping the known-good levels 8 and 9 use
# -- to snow_slider's all.bmd/kcl/icg.bin/icl.bin. SUBLEVEL_LEVEL_TABLE[11]
# (0x02075298) is 3 = course 3, an outdoor slide with subCount = 1 (43 Standard
# and Simple entries) and a 57805-byte KCL, the largest mounted so far.
# own_sinits stays 0.
list(APPEND PORT_LEVEL_OVERLAYS ov019)
# gate 149: ov020 = level 12, Big Boo's Haunt (data/stage/teresa_house), course 4.
# data_020758c8[12] = 20, LVL_Overlay data_02092208[12] = 0x021138fc, OV0 handles
# at +8 (bmd 0x07d2/kcl 0x07cc/icg 0x07d3/icl 0x07d4) resolve directly through
# handles.tsv to teresa_house's all.bmd/kcl/icg/icl; SUBLEVEL_LEVEL_TABLE[12] = 4.
# Mounted --whole; own_sinits 0.
list(APPEND PORT_LEVEL_OVERLAYS ov020)
# gate 163: ov022 = level 14, Lethal Lava Land (data/stage/fire_land), course 6.
# Read from the ROM, not the overlay numbering: the level->overlay map
# data_020758c8[14] = 22 (level+8, a read not an assumption), the LVL_Overlay
# table data_02092208[14] = 0x02113228, and the four OV0 handles at LVL_Overlay+8
# (bmd 0x0742 / kcl 0x0740 / icg 0x0743 / icl 0x0744) resolve DIRECTLY through
# build/assets/handles.tsv -- the ov0 handle in the overlay IS the tsv handle, no
# arithmetic, the delta-0 mapping the known-good levels use -- to fire_land's
# all.bmd (26833) / kcl (22144) / icg.bin (21732) / icl.bin (512). Handles read
# from the RAW ndspy overlay image (extracted/overlays/overlay_0022.bin), the
# same image ovdata.py mounts; the dsd export is stale against config
# re-addressings. SUBLEVEL_LEVEL_TABLE[14] (0x02075298) is 0x06 = course 6, a
# main course with subCount 1. Mounted --whole like the rest; own_sinits stays 0.
list(APPEND PORT_LEVEL_OVERLAYS ov022)
# gate 170: ov023 = level 15, Lethal Lava Land (data/stage/fire_mt), course 6 --
# a sublevel of the fire main course (level 14 is the same course 6, ov022, the
# main stage; 15 is its second area). Read from the ROM like the rest, all three
# facts measured out of the arm9 tables in extracted/arm9_dec.bin at base
# 0x02004000, verified against all twelve known-good levels first: the
# level->overlay map data_020758c8[15] = 23 (level+8, a read not an assumption),
# the LVL_Overlay table data_02092208[15] = 0x02111b88, and the four OV0 handles
# at LVL_Overlay+8 (bmd 0x0748 / kcl 0x0745 / icg 0x0749 / icl 0x074a) resolve
# DIRECTLY through build/assets/handles.tsv -- the ov0 handle in the overlay IS
# the tsv handle, delta 0, the same mapping the known-good levels 8..12 use --
# to fire_mt's fire_mt_all.bmd / fire_mt.kcl / fire_mt_icg.bin / fire_mt_icl.bin.
# The handles must be read out of extracted/overlays/overlay_0023.bin (the
# unpack.py output ovdata itself reads), NOT extracted/dsd/arm9_overlays/ov023.bin
# -- the dsd export is stale and drifts these handle halfwords by a constant,
# which is what the +232 the castle-level comments mention actually was.
# SUBLEVEL_LEVEL_TABLE[15] (0x02075298) is 0x06 = course 6, subCount 1, a
# 38002-byte KCL. Mounted --whole like the rest; own_sinits stays 0. This block
# is append-only and self-contained (distinct overlay name); a reviewer folds it
# into the sibling level streams at merge.
list(APPEND PORT_LEVEL_OVERLAYS ov023)
# gate 156: ov021 = level 13, Hazy Maze Cave (data/stage/cave), course 5.
# data_020758c8[13] = 21 (level+8, a read not an assumption), LVL_Overlay
# data_02092208[13] = 0x021138c4, OV0 handles at +8 (bmd 0x0709/kcl 0x0706/
# icg 0x070a/icl 0x070b) resolve directly through handles.tsv to cave's
# cave_all.bmd/cave.kcl/cave_icg.bin/cave_icl.bin; SUBLEVEL_LEVEL_TABLE[13] = 5,
# subCount 8, a 61224-byte KCL. Mounted --whole; own_sinits 0.
list(APPEND PORT_LEVEL_OVERLAYS ov021)
set(LEVEL_OV_DATA "")
foreach(ovn IN LISTS PORT_LEVEL_OVERLAYS)
    set(ovn_out "${CMAKE_BINARY_DIR}/host-src/${ovn}_data.c")
    add_custom_command(OUTPUT "${ovn_out}" "${ovn_out}.map"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ${ovn}
                "${ovn_out}" --whole ${ROMCLEAN_FLAG}
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_common.py"
        COMMENT "ovdata ${ovn} (whole image)")
    list(APPEND LEVEL_OV_DATA "${ovn_out}")
    list(APPEND OV_DATA_MAPS "${ovn_out}.map")
    if(PORT_ROM_CLEAN)
        list(APPEND ROMCLEAN_PARTS "${ovn_out}.blobmap")
    endif()
endforeach()

# Gate 17: the level overlay's own actors. ov009 is mounted WHOLE for the
# loaders; its actor code reaches its SharedFilePtrs, CLPS blocks and spawn
# tables BY NAME, so those symbols are mounted a second time per symbol.
# --pack keeps the runs that are really one array contiguous -- ov009's water
# spawn points are three seven-element Vector3 arrays that dsd split into
# twenty-one symbols and func_ov009_02111b1c walks by index.
set(OV009SYMS_C "${CMAKE_BINARY_DIR}/host-src/ov009_syms.c")
add_custom_command(OUTPUT "${OV009SYMS_C}" "${OV009SYMS_C}.map"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ov009
            "${OV009SYMS_C}" --from-list
            "${CMAKE_CURRENT_SOURCE_DIR}/ov009_syms.txt" --pack ${ROMCLEAN_FLAG}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_common.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/ov009_syms.txt"
    COMMENT "ovdata ov009 (per symbol, packed)")
list(APPEND OV_DATA_MAPS "${OV009SYMS_C}.map")
if(PORT_ROM_CLEAN)
    list(APPEND ROMCLEAN_PARTS "${OV009SYMS_C}.blobmap")
endif()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate17.txt" SLICE17_LINES)
set(SLICE17_SOURCES "")
foreach(line IN LISTS SLICE17_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE17_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 18 and up: the ACTOR overlays. Neither the engine overlay nor the
# level's own -- ov085/ov098/ov100/ov102 each carry a handful of classes that
# several levels pull in, and the castle grounds names some of each. They are
# mounted PER SYMBOL like ov002 (nothing walks a table across their symbol
# boundaries the way a level overlay's loaders do), --pack so the runs that
# are really one array stay contiguous, and each list is the overlay's WHOLE
# data span rather than a hand-picked subset -- see the header of any
# ovNNN_syms.txt for why.
# ov001 joins the per-symbol mounts for the HUD's sprite templates (gate 27):
# it is not an actor overlay, but it wants exactly the same treatment --
# packed run, relocations applied, interior pointers rebased to host.
# ov003 joins them for gate 31: not an actor overlay either, mounted for
# exactly one table -- the debug level select's row list. See ov003_syms.txt.
set(PORT_ACTOR_OVERLAYS ov085 ov098 ov100 ov089 ov102 ov001 ov003 ov084
    ov014 ov062 ov091 ov078)
# gate 41-42: ov010 is level 2's own overlay, already mounted whole in
# PORT_LEVEL_OVERLAYS; it joins the per-symbol mounts here so TRAP, LIGHT_BEAM
# and PEACH_PAINTING reach their SpawnInfo records by name -- the gate-17 shape
# of ov009/ov014 applied to the castle interior. See port/ov010_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov010)
# gate 50: ov080 is an ordinary actor overlay carrying the castle interior's
# PAINTING (daPicGate_c, 307) among three other classes; mounted per symbol so
# the actor reaches its SpawnInfo record, tuning tables and twelve state
# statics by name. See port/ov080_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov080)
# gate 59: ov015 is level 7's own overlay (Whomp's Fortress mechanical
# platforms), already mounted whole in PORT_LEVEL_OVERLAYS; it joins the
# per-symbol mounts here so the eight platform classes reach their SpawnInfo
# records and construction-data statics by name -- the gate-17 shape of
# ov010/ov014 applied to a third level overlay. Its seven vtables are host
# arrays the registry fills (two are unnamed data symbols dsd left as plain
# data). See port/ov015_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov015)
# gates 64-69: ov079 is an ENEMY overlay (the ov078/ov079/ov080 set, all linked
# at base 0x02123740 because the DS loads one at a time) carrying six of Whomp's
# Fortress' living cast -- WHOMP (164), WHOMP_KING (165), BULLET_BILL (222),
# BILL_BLASTER (43), FORTRESS_WALL (47) and FORTRESS_WALL_BREAKABLE (48).
# Mounted per symbol so each class reaches its SpawnInfo record, its
# SharedFilePtr construction statics and its two pointer-to-member state tables
# by name -- the gate-32 shape of ov084/ov078. Its four vtables are host arrays
# the registry fills (BillBlaster's is a plain data symbol dsd left unnamed).
# See port/ov079_syms.txt and hal/actor_classes_wf_enemy.cpp.
list(APPEND PORT_ACTOR_OVERLAYS ov079)
# gate 83: ov095 is Bob-omb Battlefield's SEESAW BRIDGE overlay (its own base
# 0x02135700, one of the seven actor overlays level 6 loads). Mounted per symbol
# so SEESAW_BOB (39) reaches its SpawnInfo, its three parallel file-pointer
# tables and the SharedFilePtr statics the two ov095 sinits construct -- the
# ov085/ov080/ov079 shape. Its three vtables (_ZTV9SeesawBob and the UpDownLift/
# Flamethrower ones) are host arrays the registry fills. See port/ov095_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov095)
# gate 176: ov071 is SCUTTLEBUG's overlay (daSpd_c, 255), first mount. Per
# symbol so the actor reaches its SpawnInfo, its model/anim SharedFilePtrs, the
# eighteen state SOURCE PMFs and the bss state array by name. Its vtable is a
# host array the registry fills. See port/ov071_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov071)
# gate 177: ov064 is Lethal Lava Land's actor overlay, first mount -- BULLY,
# BIG_BULLY and ROTATING_FIREBAR ride it; the seat classes (LavaBubble, the
# BowserPuzzle pair) follow in their own gates. See port/ov064_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov064)
# gate 143: ov019 is level 11's own overlay (Cool Cool Mountain's slide),
# already mounted whole in PORT_LEVEL_OVERLAYS; it joins the per-symbol mounts
# here so IceSlideManager (356) reaches its SpawnInfo record and the two
# construction-data statics its InitResources reads by name -- the gate-17 shape
# of ov009/ov010/ov015 applied to an eighth level overlay. Its one vtable
# (_ZTV15IceSlideManager) is a host array the registry fills. Only this class's
# own sinit (__sinit_ov019_02112b14) runs; RacingPenguin's (259, not spawned by
# level 11) is left off. See port/ov019_syms.txt and hal/actor_classes_ccm.cpp.
list(APPEND PORT_ACTOR_OVERLAYS ov019)
# gate 149: ov020 joins the per-symbol mounts so Big Boo's Haunt's furniture
# classes (HAUNTED_CHAIR 326, plus the deferred bookend/bookshot cast) reach
# their SpawnInfo and construction statics by name; its vtables are host arrays
# the registry fills. See ov020_syms.txt and hal/actor_classes_bbh.cpp.
list(APPEND PORT_ACTOR_OVERLAYS ov020)
# gate 188: ov016 is level 8's own overlay (Jolly Roger Bay), already mounted
# whole in PORT_LEVEL_OVERLAYS; it joins the per-symbol mounts here so its six
# MOVER classes (SHIP_DOWN 56, SHIP_UP 57, ROCK_PILLAR 58,
# FLOAT_ON_WATER_PLATFORM_JRB 60, UNAGI 242, SLIDING_BOX 313) reach their
# SpawnInfo records, file-pointer tables, CLPS blocks and -- for UNAGI -- the
# five PMF-dispatch cells, all by name. Its four vtables (_ZTV5Unagi, _ZTV6ShipUp,
# the RockPillar table data_ov016_02114b00, and the FloatOnWater/SlidingBox
# _ZTV23FloatOnWaterPlatformJrb) are host arrays the registry fills. See
# port/ov016_syms.txt and hal/actor_classes_jrb.cpp.
list(APPEND PORT_ACTOR_OVERLAYS ov016)
# gate 190: ov018 is level 10's own overlay (Cool Cool Mountain, snow_mt),
# already mounted whole in PORT_LEVEL_OVERLAYS; it joins the per-symbol mounts
# here so ICE_SHEET (295) reaches its SpawnInfo and its bss SharedFilePtr
# window, and POWER_STAR_CREATE (355) reaches its SpawnInfo, both by name --
# the gate-59/gate-188 shape of ov015/ov016 applied to a fourth level overlay.
# Its two vtables (_ZTV8IceSheet and PowerStarCreate's unnamed table
# data_ov018_02113a74) are host arrays the registry fills.
# GATE 191 extends this same mount: SKI_LIFT (63, own vtable
# data_ov018_021138cc) and MOTHER_PENGUIN (257, own vtable _ZTV7SkiLift ==
# _ZTV10daPgMthr_c -- a dsd-era class-identity mislabel, see
# port/slice_gate191.txt). All four classes' sinits now run
# (__sinit_ov018_02112e00/02112c14/02112c80; PowerStarCreate has none of its
# own). See port/ov018_syms.txt and hal/actor_classes_ccm.cpp.
list(APPEND PORT_ACTOR_OVERLAYS ov018)
# gate 192: ov081 is a 100% matched ACTOR overlay, first mount -- ICE_BLOCK
# (18), MR_BLIZZARD (223) and SPINDRIFT (312) reach their SpawnInfo records,
# SharedFilePtr construction statics and (MrBlizzard) its ten-cell PMF
# dispatch table by name, the ov079/ov080 shape. Its three vtables
# (_ZTV9Spindrift, _ZTV10MrBlizzard, _ZTV8IceBlock == _ZTV15daObjIceBlock_c)
# are host arrays hal/actor_classes_ov081.cpp fills. SNOWBALL and MONEYBAG
# (also in this overlay) stay unregistered -- their own sinits do not run.
# See port/ov081_syms.txt and port/slice_gate192.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov081)
# gate 193: ov072, the ov079/ov080/ov081 shape applied a third time --
# BABY_PENGUIN (256) and the ACTUAL SNOWMAN (272, the unnamed daBgSnwmn_c
# block -- NOT SnowmanBody or SnowmanHead, a dsd class-identity gap; see
# port/ov072_syms.txt's own header) reach their SpawnInfo records,
# SharedFilePtr construction statics and (BabyPenguin) its six-cell PMF
# state-dispatch table by name. Their two vtables (_ZTV11BabyPenguin ==
# _ZTV9daPgBby_c, and daBgSnwmn_c's own unnamed data_ov072_02122978) are
# host arrays hal/actor_classes_ov072.cpp fills. SnowmanBody (274, not in
# level 10's census) and SnowmanHead (273, spawned-then-orphaned by
# daBgSnwmn_c until star 5 is collected) stay unregistered -- their own
# sinits do not run. See port/ov072_syms.txt and port/slice_gate193.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov072)
# gate 194: ov094, HOOT_THE_OWL's (234) own overlay. Fresh per-symbol mount.
# Its own SpawnInfo record, SharedFilePtr construction statics and its
# five-cell (ten-record) state-dispatch table reach it by name. Its vtable
# (_ZTV10HootTheOwl == _ZTV7daOwl_c) is a host array
# hal/actor_classes_ov094.cpp fills. The ov006/ov094 dual spawn-table
# citation is a dual-dump artifact (ov094 is address-nested inside ov006's
# much larger span), NOT co-residency -- no ov006 mount. See
# port/ov094_syms.txt and port/slice_gate194.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov094)
# gate 199: ov012 (level 4's own overlay, already whole-mounted per gate 114)
# joins the per-symbol mounts too -- SWITCH_PILLAR (34) and BASEMENT_WATER
# (35) reach their SpawnInfo records and tuning/CLPS/BTA/BMD data by name,
# the ov009/ov014/ov010 shape. Their two vtables (data_ov012_02112344 for
# SwitchPillar, _ZTV12SwitchPillar == _ZTV14daObjC0Water_c for BasementWater
# -- a dsd class-identity swap, see port/ov012_syms.txt's own header) are
# host arrays hal/actor_classes_ov012.cpp fills. See port/ov012_syms.txt and
# port/slice_gate199.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov012)
# ov084 joins for gate 32: the first ENEMY overlay, GOOMBA and BOB_OMB_BUDDY.
# ...and ov014, ov062 and ov091 with them. ov014 is a LEVEL overlay (Bob-omb
# Battlefield's own) and is mounted a second time here, per symbol, because
# CHAIN_CHOMP and CHAIN_CHOMP_FENCE reach their tables by name -- the gate-17
# treatment of ov009 applied to the second level. ov062 is KOOPA_THE_QUICK's.
# ov091 is the one the chomp drags in: its InitResources spawns actor 27, the
# post it is chained to, and a null back from that spawn is a fault. ov078 is
# KING_BOB_OMB's, and level 6 is the only level of the fifty-two that loads it.
set(ACTOR_OV_SYMS "")
foreach(ovn IN LISTS PORT_ACTOR_OVERLAYS)
    set(ovn_out "${CMAKE_BINARY_DIR}/host-src/${ovn}_syms.c")
    add_custom_command(OUTPUT "${ovn_out}" "${ovn_out}.map"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ${ovn}
                "${ovn_out}" --from-list
                "${CMAKE_CURRENT_SOURCE_DIR}/${ovn}_syms.txt" --pack ${ROMCLEAN_FLAG}
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_common.py"
                "${CMAKE_CURRENT_SOURCE_DIR}/${ovn}_syms.txt"
        COMMENT "ovdata ${ovn} (per symbol, packed)")
    list(APPEND ACTOR_OV_SYMS "${ovn_out}")
    list(APPEND OV_DATA_MAPS "${ovn_out}.map")
    if(PORT_ROM_CLEAN)
        list(APPEND ROMCLEAN_PARTS "${ovn_out}.blobmap")
    endif()
endforeach()

# THE CROSS-MOUNT POINTER PASS, last because it needs every map.
#
# A pointer from one mounted overlay into another was left holding its DS
# address by the per-mount passes above, which can only see their own symbols.
# That is not a loud failure: ntr/io.cpp reserves DS main RAM at 0x02000000 as
# zeroed pages, so the read succeeds and hands the game an OamAttr list with no
# 0xffff terminator, and the walk in OAM::Render is unbounded the moment the
# sprite goes off screen. This pass reads every mount's <out>.c.map and emits
# one patch function, port_cross_patch(), for the words that cross.
#
# It reproduces the OAM::MM_PLAYER_ICONS table (ov002 -> ov001, sixteen entries)
# that hal/sub_actors.cpp used to keep by hand, entry for entry, and finds the
# genuinely-crossing pointers a new level's whole-image mount carries (Whomp's
# Fortress -> ov098's cannon data). What it CANNOT resolve is a target in a DS
# window two mounted overlays share -- ov084/ov085 and ov089/ov091 are alternate
# actor overlays linked at one base, and the target address alone cannot say
# which. Those stay hand-resolved from relocs.txt's module: field in
# hal/actor_overlays.cpp (the ov089 key models). tools/ovsweep.py checks that
# nothing crossing was left raw with no patch.
set(OVCROSS_C "${CMAKE_BINARY_DIR}/host-src/ov_cross.c")
add_custom_command(OUTPUT "${OVCROSS_C}"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" --cross
            "${OVCROSS_C}" ${OV_DATA_MAPS}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ${OV_DATA_MAPS}
    COMMENT "ovdata --cross (pointers between mounted overlays)")

# ROM-CLEAN consolidation, last because it needs every emitter's part. It
# concatenates the .blobpart files into build/assets/romdata.bin, writes the
# manifest the loader checks, and generates romdata_dispatch.c
# (port_romdata_load_all). walk_window links the dispatch + hal/romdata_loader.
# The list is walk_window's exact emitter set; a target with a different set
# would need its own blob, but only the shipped window is ROM-clean today.
# The blob lands in the REPO-ROOT build/assets/ (beside files.tsv/handles.tsv):
# the loader resolves <asset_root>/build/assets/romdata.bin -- the same seam
# fs.cpp uses for the catalogs -- and asset_root defaults to PORT_REPO_ROOT,
# which is the repo root, NOT ${CMAKE_BINARY_DIR} (= build/port).
set(ROMDATA_BIN "${PORT_REPO_ROOT_ABS}/build/assets/romdata.bin")
set(ROMDATA_MANIFEST "${PORT_REPO_ROOT_ABS}/build/assets/romdata.manifest")
set(ROMDATA_DISPATCH_C "${CMAKE_BINARY_DIR}/host-src/romdata_dispatch.c")
if(PORT_ROM_CLEAN)
    # The .blobpart/.blobmap sidecars are written by the SAME emitter commands
    # that write the .c files, so depend on the .c OUTPUTs (which CMake knows how
    # to build) and mark the parts GENERATED. Depending on the parts directly
    # would be "no rule to make target" -- they are side-effects, not OUTPUTs.
    set(ROMCLEAN_C_OUTPUTS "${ROMDATA_C}" "${OV002DATA_C}" ${LEVEL_OV_DATA}
        "${OV009SYMS_C}" ${ACTOR_OV_SYMS})
    set_source_files_properties(${ROMCLEAN_PARTS} PROPERTIES GENERATED TRUE)
    add_custom_command(
        OUTPUT "${ROMDATA_BIN}" "${ROMDATA_MANIFEST}" "${ROMDATA_DISPATCH_C}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob.py"
                "${ROMDATA_BIN}" "${ROMDATA_MANIFEST}" "${ROMDATA_DISPATCH_C}"
                ${ROMCLEAN_PARTS}
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob.py"
                ${ROMCLEAN_C_OUTPUTS}
        COMMENT "romblob: consolidate ROM tables -> build/assets/romdata.bin")
    # A build-time self-check: the blob slices must byte-match the arrays the
    # emitters would bake. Guards against a drift between the two code paths.
    add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/assets/romblob_verified.stamp"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_verify.py"
                "${PORT_REPO_ROOT_ABS}" "${CMAKE_BINARY_DIR}"
                "${ROMDATA_MANIFEST}" "${ROMDATA_BIN}"
        COMMAND ${CMAKE_COMMAND} -E touch
                "${CMAKE_BINARY_DIR}/assets/romblob_verified.stamp"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_verify.py"
                "${ROMDATA_BIN}" "${ROMDATA_MANIFEST}"
        COMMENT "romblob self-check: blob bytes == baked bytes")
    # The portable kit rebuilds romdata.bin on the player's machine from their
    # own dump; the recipe is the shippable how (offsets and hashes, zero ROM
    # bytes). Generated here, self-verified against the local extracted images,
    # so it can never drift from the manifest it names.
    set(ROMDATA_RECIPE "${PORT_REPO_ROOT_ABS}/build/assets/romdata.recipe.tsv")
    add_custom_command(
        OUTPUT "${ROMDATA_RECIPE}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_recipe.py"
                "${ROMDATA_MANIFEST}" "${ROMDATA_RECIPE}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/romblob_recipe.py"
                "${ROMDATA_MANIFEST}"
        COMMENT "romblob recipe: manifest -> kit extraction recipe")
endif()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate18.txt" SLICE18_LINES)
set(SLICE18_SOURCES "")
foreach(line IN LISTS SLICE18_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE18_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# argfix: LakituBro::InitResources calls TextureSequence::Prepare with two
# arguments; the host wrapper reshapes them into a 3-param thiscall and derefs an
# unwritten third slot. Host copy passes the real third argument. Src line
# commented out in slice_gate18.txt.
list(APPEND SLICE18_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/TexSeq_Caller_LakituBro.cpp")
# rabbit-key: func_ov085_0212d268 (the RABBIT_KEY seat-and-enter) is a real MSVC
# pointer-to-member call over an incomplete class -- the gate-16 case. Host copy
# with the mwcc {value, adj} dispatch hand-rolled; src line commented out in
# slice_gate18.txt.
list(APPEND SLICE18_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/RabbitKey_StateSeat.cpp")

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate19.txt" SLICE19_LINES)
set(SLICE19_SOURCES "")
foreach(line IN LISTS SLICE19_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE19_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate20.txt" SLICE20_LINES)
set(SLICE20_SOURCES "")
foreach(line IN LISTS SLICE20_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE20_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate21.txt" SLICE21_LINES)
set(SLICE21_SOURCES "")
foreach(line IN LISTS SLICE21_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE21_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate22.txt" SLICE22_LINES)
set(SLICE22_SOURCES "")
foreach(line IN LISTS SLICE22_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE22_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate23.txt" SLICE23_LINES)
set(SLICE23_SOURCES "")
foreach(line IN LISTS SLICE23_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE23_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 25: the bottom screen. The OAM lifecycle, Stage::LoadGraphics2D's
# closure and Stage::CheckCameraInput; see slice_gate25.txt. OAM::Load is not
# here -- it reads POWCNT textually and comes from GATE6_GEN, which this
# target picks up alongside the slice. (Renumbered from 24 at merge: the
# animation stream took 24 for BlendModelAnim the same night.)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate25.txt" SLICE25_LINES)
set(SLICE25_SOURCES "")
foreach(line IN LISTS SLICE25_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE25_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 26: the boot spine -- the real Stage actor. See the header of
# slice_gate26.txt; the HAL seam is hal/stage_bridges.cpp. (Renumbered from
# 24 at merge: three streams picked 24 the same night.)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate26.txt" SLICE26_LINES)
set(SLICE26_SOURCES "")
foreach(line IN LISTS SLICE26_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE26_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 27: the HUD actor (id 334). Its methods are all matched; what it needed
# was gate 25's sub-OAM path, a registry row, and the ov001 sprite templates.
# InitResources comes from port/unmatched/ -- see the head of slice_gate27.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate27.txt" SLICE27_LINES)
set(SLICE27_SOURCES "")
foreach(line IN LISTS SLICE27_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE27_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE27_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/HUD_InitResources.cpp")

# Gate 28: the MINIMAP actor (id 335). Same shape as the HUD -- eighteen-slot
# vtable, constructor as factory -- plus the BG3-sub tilemap and the extended
# palette. See the head of slice_gate28.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate28.txt" SLICE28_LINES)
set(SLICE28_SOURCES "")
foreach(line IN LISTS SLICE28_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE28_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 29: the particle subsystem -- the last subsystem the boot reported as
# stubbed. 101 matched files (the other 18 in its closure already ride on
# gates 10/13/16) plus hal/particle_vtable.cpp for the thirteen Callback
# vtables the tracker's constructor installs. See the head of
# slice_gate29.txt for what the dispatch turned out to be.
#
# THIS GATE GOES THROUGH HOSTGEN, unlike most slices, and it has to. Ten of
# its files reach the geometry engine through raw 0x04xxxxxx literals -- the
# particle render family (func_0204af3c, b028, b244, b460, b81c, bbd8, be40,
# c194, c24c) and func_02049ee8, which is what Particle::RenderAll calls.
# Compiled raw, every one of those writes would latch into dead mapped memory
# and the geometry engine would never see a particle: the same failure
# hostgen.py's own header describes for MTX_SCALE. hostgen also carries the
# HEADER_SHADOW entry the tracker's Initialise needs -- decl_Particle.h
# declares it (void*) with C++ linkage and the TU defines it extern "C" over
# a struct, which MSVC rejects as an overload on C linkage (C2733).
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate29.txt" SLICE29_LINES)
set(GATE29_GEN "")
foreach(line IN LISTS SLICE29_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    get_filename_component(sym "${line}" NAME_WE)
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../${line}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE29_GEN "${sym_out}")
endforeach()

# Gate 33: Bob-omb Battlefield's mechanisms, terrain objects and pickups --
# the classes of level 6 (ov014) that are not living creatures. The roster and
# the evidence behind every actor id are in the head of slice_gate33.txt; the
# vtable fills and the debug spawn hook are hal/actor_classes_bob_world.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate33.txt" SLICE33_LINES)
set(SLICE33_SOURCES "")
foreach(line IN LISTS SLICE33_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE33_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE33_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Coin_Behavior.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Coin_CollectDispatch.cpp"
     # argfix: func_ov002_020b10a0 calls StarMarker::SpawnRedCoinStarIfNecessary()
     # with no argument and relies on func_ov002_020b1328's returned StarMarker
     # riding ARM r0. Host copy captures and passes it. Src line commented out in
     # slice_gate33.txt.
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/StarMarker_SpawnDispatch.cpp")

# Gate 32: Bob-omb Battlefield's living cast -- the first ENEMY-family actor
# classes the port hosts, and the first vtables past twenty slots. See the head
# of slice_gate32.txt; the HAL seam is hal/actor_classes_bob_enemy.cpp and the
# direct-spawn probe that brings them up before their own level exists is
# hal/actor_spawn_probe_bob.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate32.txt" SLICE32_LINES)
set(SLICE32_SOURCES "")
foreach(line IN LISTS SLICE32_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE32_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# argfix: func_ov078_02123f1c (King Bob-omb) calls Player::GetHealth() with no
# argument and relies on the held Player (c->field_494) riding ARM r0. Host copy
# passes it. Src line commented out in slice_gate32.txt.
list(APPEND SLICE32_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/KingBobOmb_HeldHealthCheck.cpp")

# Gate 31: the level HANDOFF. Small on purpose -- almost all of it was already
# linked by gate 10 (see the head of slice_gate31.txt); what is here is
# StartFile's own closure, the Player's destroy slots and the collider
# registry reset. The driver is hal/level_change.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate31.txt" SLICE31_LINES)
set(SLICE31_SOURCES "")
foreach(line IN LISTS SLICE31_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE31_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE31_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Player_CleanupResources.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/func_02073244.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/SetNumPlayers.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Door_CleanupResources.cpp")

# Gate 35: the course loop -- damage, death, respawn, coins, the star and the
# course's own sound. Small, because almost all of it was already linked; see
# the head of slice_gate35.txt for what was actually missing.
#
# func_02049018 is the 3D sound DISTANCE function and it drives the DS square-
# root unit directly (SQRTCNT/SQRT_PARAM/SQRT_RESULT at 0x40002b0). ntr maps
# that page as plain memory and hooks the write-triggered registers, but only
# through the NTR_MMIO macro hostgen substitutes -- a raw cast-deref writes the
# byte and never runs the unit, so the function returned whatever was last left
# in the result register. Measured before this: a player 95 scene units from
# the listener came back 8192, past every distance limit in the game, and every
# positional sound was culled. It goes through hostgen like every other TU that
# touches hardware; src/func_02049018.c comes out of slice 10.
set(GATE35_SYMS func_02049018)
set(GATE35_GEN "")
foreach(sym IN LISTS GATE35_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE35_GEN "${sym_out}")
endforeach()

set(GATE32_HOST_SOURCES
    hal/actor_classes_bob_enemy.cpp
    hal/actor_spawn_probe_bob.cpp
    hal/bob_enemy_bridges.cpp
    hal/bob_enemy_shadow_faces.cpp
    hal/bob_enemy_header_faces.cpp
    # the ModelAnim slot-5 collision, for the fourth gate running
    unmatched/BobEnemy_Renders.cpp
    # the mwcc pointer-to-member dispatch for the eighth time, and the first
    # one that belongs to a BASE class: Enemy's own death-type table
    unmatched/Enemy_UpdateDeath.cpp
    # ...and for the ninth: the bob-omb buddy's three-state machine
    unmatched/BobOmbBuddy_States.cpp
    # ...and for the tenth: the chain chomp's six
    unmatched/ChainChomp_States.cpp
    # the ARM r0 ride-through, in a spawn factory for the first time
    unmatched/ChainChomp_Spawn.cpp
    # ...and for the eleventh, the widest one yet: KING_BOB_OMB keeps the
    # ADDRESS of a two-PMF record rather than an index, so both his setter and
    # his Behavior do pointer arithmetic on a type MSVC doubles
    unmatched/KingBobOmb_States.cpp)

# The header/definition collisions in this gate's closure: decl_common.h
# declares each of these `(void*)` and its own TU defines it `(char*)`, which
# is one register on the ROM and C2733 to MSVC. hostgen's HEADER_SHADOW hides
# the header's declaration across the include; src/ and include/ stay untouched.
# func_ov084_021298d0 is here for a different reason: it is C++ inside a .c
# (the gate-9 case), which mwccarm accepted and MSVC will not compile as C.
# hostgen emits it as .cpp with the symbols wrapped back to C linkage.
# func_ov078_02124cf4 is the first kind again: decl_common.h declares
# func_0200fa8c `(void*, int)` and func_ov078_02125c24 `(char*, int)` while
# that TU declares both `(void*, int)`. One register on the ROM, two C2733s to
# MSVC.
set(GATE32_SYMS func_ov102_0214b248 func_ov084_021298d0 func_ov078_02124cf4)
set(GATE32_GEN "")
foreach(sym IN LISTS GATE32_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE32_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate35.txt" SLICE35_LINES)
set(SLICE35_SOURCES "")
foreach(line IN LISTS SLICE35_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # (a stray `list(APPEND GATE32_GEN ...)` lived here from the six-stream
    # merge -- lesson 4's split-loop damage, masked because CMake dedupes
    # target sources. Caught by the post-merge cross-loop-append check.)
    list(APPEND SLICE35_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 13: the real Camera actor. Adds its own slice plus the HAL seam, and
# swaps three gate-10 files for host copies that only the camera needs
# (see port/unmatched/): ChangeArea calls HideArea() with no argument and
# relies on the ARM r0 ride-through, and func_0200cae4 / func_0200ca50
# dispatch through mwcc member-function pointers. smoke_player keeps the
# matched files -- it never arms data_0209f318, so nothing there dispatches.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate13.txt" SLICE13_LINES)
set(SLICE13_SOURCES "")
foreach(line IN LISTS SLICE13_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE13_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# THE POINTER SWEEP. port/tools/ptr_audit.py intersects romdata.py's NAMED
# list with config/arm9/relocs.txt and reports every named symbol whose ROM
# bytes contain a relocated word -- a DS address the port would emit raw and
# then call through. Two of its answers are tables that matched code indexes
# and CALLS: data_02086758 (the backup-media command table) and the 39 records
# at 0x02086bc0 (the kuppa script's camera commands). Both are hosted with real
# host function pointers in hal/ptr_tables.cpp, which needs their callees to
# exist as host code -- that is slice_ptrsweep.txt, 65 files of matched src
# that were in no slice at all, because the only thing naming them was a table
# of bytes. unmatched/func_02008550.cpp is the second table's consumer,
# host-copied for the same mwcc-PMF reason func_0200cae4 is.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ptrsweep.txt" SLICE_PTR_LINES)
set(SLICE_PTR_SOURCES hal/ptr_tables.cpp unmatched/func_02008550.cpp)
foreach(line IN LISTS SLICE_PTR_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_PTR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# argfix: func_020078c4 calls ApproachAngle with four arguments; the fifth
# (maxStep=0) rides the stack, which cdecl does not stage. Host copy passes the
# fifth argument explicitly. Src line commented out in slice_ptrsweep.txt; the
# data_02086d88 pointer table (hal/ptr_tables.cpp) resolves to this copy.
list(APPEND SLICE_PTR_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Camera_ApproachAngleVeneer.cpp")

# gate 10 minus the two files the camera build replaces
set(SLICE10_CAM_SOURCES "")
foreach(src IN LISTS SLICE10_SOURCES)
    if(src MATCHES "/ChangeArea\\.c$" OR src MATCHES "/func_0200cae4\\.cpp$")
        continue()
    endif()
    # the pointer sweep: func_02008550 dispatches the camera script commands
    # through an mwcc pointer-to-member array, which MSVC cannot represent and
    # which was loading DS code addresses out of romdata besides. Host copy in
    # port/unmatched/, tables in hal/ptr_tables.cpp.
    if(src MATCHES "/func_02008550\\.cpp$")
        continue()
    endif()
    # gate 14: three more ARM ride-throughs, dormant until the level boot
    # reaches them (port/unmatched/ActorDerived_Spawn.cpp, PathPtr_FromID.cpp)
    if(src MATCHES "/_ZN12ActorDerived5SpawnEjP9ActorBaseii\\.cpp$"
       OR src MATCHES "/func_02042ffc\\.c$"
       OR src MATCHES "/_ZN7PathPtr6FromIDEj\\.cpp$"
       OR src MATCHES "/func_0204335c\\.c$")
        continue()
    endif()
    # gate 29: func_02021d1c is the particle spawn's tail, and the ONE place
    # in the subsystem that reaches a Callback vtable through a C++ `virtual`
    # -- a local shadow class, `p6->Run(...)`, which MSVC compiles thiscall
    # while every other dispatch on the same vtable is cdecl. It goes through
    # hostgen with a VIRTUAL_CALL patch instead, listed in slice_gate29.txt;
    # same treatment as func_ov002_020d708c above. Only the three targets that
    # take this filtered list exist, and all three carry gate 29.
    if(src MATCHES "/func_02021d1c\\.cpp$")
        continue()
    endif()
    # Sound: five ARM argument ride-throughs, replaced by hal/sdat/sound_abi.cpp
    # (declared with fewer parameters than their callers pass, which only
    # works when the extras sit in r1..r3), plus func_0205b5d4, the "read the
    # other core's progress" read that hal/sdat/consumer.cpp has to own so the
    # queue's spin-wait can terminate on a single-threaded host.
    if(src MATCHES "/func_0204f600\\.c$"
       OR src MATCHES "/func_0204f89c\\.c$"
       OR src MATCHES "/func_0204f7cc\\.c$"
       OR src MATCHES "/func_0204f86c\\.c$"
       OR src MATCHES "/func_0204fa2c\\.c$"
       OR src MATCHES "/func_0205b5d4\\.c$"
    # ...and Sound::Player::SetPlayableSeqCount, which writes through
    # data_020a4d84 = data_020a4d6c + 0x18, an adjacency host symbols do not
    # have. Same file, sound_abi.cpp.
       OR src MATCHES "/_ZN5Sound6Player19SetPlayableSeqCountEii\\.c$"
    # ...and Sound::Play, whose src calls func_02050cdc(void) and lets the
    # kind and id ride through in r0/r1.
       OR src MATCHES "/_ZN5Sound4PlayEjjRK7Vector3\\.c$"
    # ...and func_0203d974, "does anything still need loading off the card".
    # Its src reads the DS main-RAM mirror word 0x027ffc40 literally, which is
    # past MAIN_BASE+MAIN_SIZE (ntr/include/ntr/mmio.h) and unmapped -- and the
    # DS-faithful answer would then walk into a null sound heap. Hosted in
    # sound_abi.cpp; the header there has the whole argument. It is listed in
    # slice_gate14.txt as well, where it is commented out rather than filtered:
    # gate 14 is appended unfiltered, so a live line there puts it straight back
    # into the link (same as SetPlayableSeqCount above).
       OR src MATCHES "/func_0203d974\\.c$")
        continue()
    endif()
    # gate 35: HitDeathPlane is that shape one more time -- it calls
    # SetNextLevel with the argument riding in r0 across a call its own
    # declaration spells `(void)`. hal/star_flow.cpp names the rider.
    # func_0203d974 comes out with it (it is listed in both slice 10 and slice
    # 14): its 0x027ffc40 console-type read is in no region ntr maps, and the
    # course sound boot is its first caller.
    # ...and CollectStar, the WRITE half of the s8/int argument-width
    # disagreement port/unmatched/NumStars.cpp already carries the read half
    # of. port/unmatched/CollectStar.cpp takes the course id as s8.
    if(src MATCHES "/HitDeathPlane\\.c$"
       OR src MATCHES "/func_0203d974\\.c$"
       OR src MATCHES "/CollectStar\\.c$"
    # ...and func_02049018, the 3D sound distance function, which drives the
    # DS square-root unit through raw cast-derefs and so has to go through
    # hostgen's MMIO transform (GATE35_SYMS) rather than compile as it stands.
       OR src MATCHES "/func_02049018\\.c$")
        continue()
    endif()
    list(APPEND SLICE10_CAM_SOURCES "${src}")
endforeach()

# argfix (under-declared-implicit-arg bug class): three gate-10 callers whose
# matched src drops an argument that rode an ARM register for free. Their src
# lines are commented out in slice_gate10.txt; these host copies pass the missing
# argument explicitly (register-proven per site). See each file's header.
# (func_ov002_020e5948's TexSeq caller fix is Brain B's lane, handed over via
# out/argfix/handoff/ -- not wired here.)
list(APPEND SLICE10_CAM_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Player_ReleaseHeldDispatch.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Player_SwimMain.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/InitCrashScreen_ResetVeneer.cpp")

# The hosted ARM7: native SDAT player behind the game's own command queue.
set(SOUND_HOST_SOURCES
    hal/sdat/sdat.cpp
    hal/sdat/mixer.cpp
    hal/sdat/sseq.cpp
    hal/sdat/out_win.cpp
    hal/sdat/consumer.cpp
    hal/sdat/sound_abi.cpp
    hal/sdat/sound_bss.cpp)

set(LEVELBOOT_HOST_SOURCES
    unmatched/ActorDerived_Spawn.cpp
    unmatched/PathPtr_FromID.cpp
    unmatched/func_0204335c.cpp
    unmatched/func_02043288.cpp
    unmatched/func_0204322c.cpp
    unmatched/func_02043fdc.cpp
    # ...and a fourth, dormant one: LoadEntranceObjects ends in
    # StartEntranceFaderWipe() spelled (void), with the wipe type riding in r0
    # from the `< 0` test right above it. Every entrance the port mounts takes
    # that return, so the call never fires today -- see the file header for
    # what it did and did not fix. Commented out of slice_gate14.txt in
    # exchange.
    unmatched/LoadEntranceObjects.cpp
    unmatched/func_02016ff4.cpp
    unmatched/ActorBase_AfterCleanupResources.cpp
    unmatched/func_ov002_020c6f3c.cpp
    # gate 16: two more mwcc pointer-to-member dispatches, both in actor
    # classes the registry carries. MSVC forms a PMF over an INCOMPLETE class
    # as the four-word general representation, which doubles the stride of the
    # table it indexes; see each file's header.
    unmatched/OneUpMushroom_Behavior.cpp
    unmatched/NumStars.cpp
    # gate 35: the WRITE half of the same s8/int argument-width disagreement
    unmatched/CollectStar.cpp
    unmatched/SignPost_StateDispatch.cpp
    # gate 18: the third, LakituBro's eleven-state machine
    unmatched/LakituBro_Behavior.cpp
    # ...and gate 17's ModelAnim slot-5 case with a body attached
    unmatched/Ov085_Renders.cpp
    # gate 19: the same slot problem one slot lower, on Model
    unmatched/Cannon_Render.cpp
    unmatched/Cannon_Behavior.cpp
    # gate 21: the pointer-to-member dispatch for the fifth time
    unmatched/Fish_Behavior.cpp
    unmatched/UnchainedChomp_Behavior.cpp
    # gate 22: the sixth, plus the Model slot the dual fill cannot serve
    unmatched/Door_Behavior.cpp
    unmatched/Door_Render.cpp
    # gate 23: the seventh, and the last on this level
    unmatched/QuestionBlock_States.cpp
    unmatched/QuestionBlock_BounceDispatch.cpp
    # gate 50: the eighth -- the PAINTING's three per-mode dispatchers, the
    # deferred fix from the registry note (mode-table stride + widened PMF)
    unmatched/Painting_Dispatch.cpp
    # gates 21 and 23: the ModelAnim slot-5 collision, three more times
    unmatched/ModelAnim_Renders.cpp)

set(CAMERA_HOST_SOURCES
    unmatched/ChangeArea.c
    unmatched/func_0200cae4.cpp
    unmatched/func_0200ca50.cpp
    unmatched/G3i_PerspectiveW.cpp
    unmatched/G3i_LookAt.cpp
    hal/camera_states.cpp
    hal/camera_bridges.cpp)

# Camera::Render writes CLEAR_COLOR at a literal address; LenVec3 drives the
# hardware sqrt unit and cstd::ldiv_result reads the divider result -- all
# three only work through the NTR_MMIO proxy.
# func_02009d90 carries a GCC __attribute__((unused)) MSVC will not parse;
# hostgen strips attributes as well as rewriting MMIO.
# _ZN12ActorDerived18AfterInitResourcesEj is C++ inside a .c (the gate-9 case
# again): hostgen emits it as .cpp with the C-linkage wrap, which MSVC accepts.
set(GATE13_SYMS _ZN6Camera6RenderEv LenVec3 _ZN4cstd11ldiv_resultEv
    func_02009d90 _ZN12ActorDerived18AfterInitResourcesEj)
set(GATE13_GEN "")
foreach(sym IN LISTS GATE13_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE13_GEN "${sym_out}")
endforeach()

# LenVec3 comes from the hostgen copy now, so drop the plain gate-9 entry.
set(SLICE9_CAM_SOURCES "")
foreach(src IN LISTS SLICE9_SOURCES)
    if(src MATCHES "/LenVec3\\.c$")
        continue()
    endif()
    list(APPEND SLICE9_CAM_SOURCES "${src}")
endforeach()

# Gate 14: two more C++-in-a-.c files (the gate-9 case): Actor's
# AfterInitResources, which the spawn spine's init Process dispatches, and
# StartEntranceFaderWipe's sibling already in GATE10_SYMS.
set(GATE14_SYMS _ZN5Actor18AfterInitResourcesEj func_ov002_020c6fe4)
set(GATE14_GEN "")
foreach(sym IN LISTS GATE14_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE14_GEN "${sym_out}")
endforeach()

# Gate 14: the real level boot. Stage::LoadClsnAndObjects over ov009's own
# LVL_Overlay, the fifteen sub-loaders, and the setters they store through.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate14.txt" SLICE14_LINES)
set(SLICE14_SOURCES "")
foreach(line IN LISTS SLICE14_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # gate 35: func_0203d974 reads the console-type halfword at 0x027ffc40,
    # which no ntr region maps. It was harmless while nothing called it; the
    # course sound boot calls it. hal/star_flow.cpp hosts it.
    if(line MATCHES "/func_0203d974\\.c$")
        continue()
    endif()
    list(APPEND SLICE14_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# argfix: Sound::LoadGroupAndSetBank calls func_0203d7d4() with no argument and
# relies on func_020134d8's return riding ARM r0. Host copy passes it. Src line
# commented out in slice_gate14.txt.
list(APPEND SLICE14_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Sound_LoadGroupAndSetBank.cpp")

# Gate 36: the real Stage::InitResources, to replace port_stage_a_boot. The
# function and all 45 of its direct callees are matched already; this is
# wiring, not new code. Linked but NOT yet called -- the boot swap comes after
# the closure resolves. See the head of slice_gate36.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate36.txt" SLICE36_LINES)
set(SLICE36_SOURCES "")
foreach(line IN LISTS SLICE36_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE36_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gates 40-44: levels 1 and 2 (castle grounds ov009, castle interior ov010),
# the actor classes their object tables name that were still printing as
# skipped. Each gate's evidence is in the head of its slice_gateNN.txt and the
# registry rows are in hal/actor_classes.inc.
#   Gate 40: STAR_DOOR (354, ov100) -- the config's mislabeled _ZN4Door family.
#   Gate 41: TRAP (36) + LIGHT_BEAM (37), ov010, level 2's own overlay.
#   Gate 42: PEACH_PAINTING (38), ov010.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate40.txt" SLICE40_LINES)
set(SLICE40_SOURCES "")
foreach(line IN LISTS SLICE40_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE40_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate41.txt" SLICE41_LINES)
set(SLICE41_SOURCES "")
foreach(line IN LISTS SLICE41_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE41_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# Gate 42: PEACH_PAINTING (38), ov010. The ov052/RollingRock "blocker" was the
# shared-base illusion -- ov010 and ov052 link at the same base, so dsd named
# two of PeachPainting's OWN internal references (func_ov010_02111e84 and the
# ov010 BSS SharedFilePtr data_ov010_02112d64) as ov052/ov021 symbols at the
# shared address. The relocs say module:overlay(10) both times. No overlay is
# mounted for this gate; the two mislabeled names are closed by /alternatename
# in hal/actor_classes.cpp. See the head of slice_gate42.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate42.txt" SLICE42_LINES)
set(SLICE42_SOURCES "")
foreach(line IN LISTS SLICE42_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE42_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 50: ov080's PAINTING (daPicGate_c, 307), the castle interior's portrait
# gate. A different class from PEACH_PAINTING, in a different overlay, with no
# cross-overlay spawn -- its closure is 28 matched ov080 TUs plus three sinits.
# See the head of slice_gate50.txt and hal/actor_classes_painting.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate50.txt" SLICE50_LINES)
set(SLICE50_SOURCES "")
foreach(line IN LISTS SLICE50_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE50_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 51: the CAP (WaterfallMist, 269), ov002. Built at gate 33, registered
# here now that its nine-state PMF table is seated. See slice_gate51.txt and
# unmatched/Cap_StateDispatch.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate51.txt" SLICE51_LINES)
set(SLICE51_SOURCES "")
foreach(line IN LISTS SLICE51_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE51_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 59: ov015's seven static initialisers (Whomp's Fortress platforms), plus
# the platform classes' matched lifecycle closures (gates 60-63 append their own
# TUs here). See slice_gate59.txt and hal/actor_classes_wf.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate59.txt" SLICE59_LINES)
set(SLICE59_SOURCES "")
foreach(line IN LISTS SLICE59_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE59_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gates 70-74: Whomp's Fortress' level-7 enemies and platforms (ov084, ov091).
# Both overlays are already mounted (gate 32); these slices are the matched
# lifecycle closures of the five classes, one file each. The HAL seam is
# hal/actor_classes_l7.cpp and the piranha state seat plus the two enemy Renders
# are unmatched/PiranhaPlant_States.cpp. See each slice_gate7N.txt header.
set(SLICE_L7_SOURCES "")
foreach(gate 70 71 72 73 74)
    set(_gf "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate${gate}.txt")
    if(NOT EXISTS "${_gf}")
        continue()
    endif()
    file(STRINGS "${_gf}" _glines)
    foreach(line IN LISTS _glines)
        string(STRIP "${line}" line)
        if(line MATCHES "^#" OR line STREQUAL "")
            continue()
        endif()
        list(APPEND SLICE_L7_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
    endforeach()
endforeach()
# gate 70: func_ov084_0212f460 is C++ inside a .c (the gate-9 case) -- it opens
# an `extern "C"` block MSVC's C front end will not parse -- so it goes through
# hostgen, which re-emits it as .cpp with the symbols wrapped back to C linkage.
# (func_ov084_0212f33c uses a C99 compound literal MSVC warns on as C4576; that
# is suppressed per target below rather than transformed.)
set(GATE_L7_SYMS func_ov084_0212f460)
set(GATE_L7_GEN "")
foreach(sym IN LISTS GATE_L7_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE_L7_GEN "${sym_out}")
endforeach()

# Gate 78: BLUE_COIN_SWITCH (10), ov002. See slice_gate78.txt and the section in
# hal/actor_classes.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate78.txt" SLICE78_LINES)
set(SLICE78_SOURCES "")
foreach(line IN LISTS SLICE78_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE78_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 79: STAR_MARKER (180), ov002. See slice_gate79.txt and the section in
# hal/actor_classes.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate79.txt" SLICE79_LINES)
set(SLICE79_SOURCES "")
foreach(line IN LISTS SLICE79_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE79_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 82: SHUTTER_BOB (40), Bob-omb Battlefield's own shutter. A Platform
# subclass in the already-mounted ov014 (the chain chomp gate), so this slice
# is HAL-only closure -- the five class methods plus the four ov002 helpers
# they reach. See slice_gate82.txt and the SHUTTER_BOB section in
# hal/actor_classes_bob_world.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate82.txt" SLICE82_LINES)
set(SLICE82_SOURCES "")
foreach(line IN LISTS SLICE82_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE82_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# The one method face InitResources needs that no Actor.h-including TU can emit
# (MeshColliderBase::Enable with a class-tagged Actor); see the file's header.
# ...and the host copy of the spawn factory, which restores the vtable-store
# order the shared-header recovery lost (see the file header).
list(APPEND SLICE82_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/shutter_bob_face.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/ShutterBob_Spawn.cpp")

# Gate 83: SEESAW_BOB (39), Bob-omb Battlefield's seesaw bridge, a Platform
# subclass in a FRESH per-symbol ov095 mount (PORT_ACTOR_OVERLAYS). This slice
# is the six class methods, the five ov095 helpers they reach and the two ov095
# sinits. See slice_gate83.txt and the SEESAW_BOB section in
# hal/actor_classes_bob_world.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate83.txt" SLICE83_LINES)
set(SLICE83_SOURCES "")
foreach(line IN LISTS SLICE83_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE83_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 102: FORTRESS_TOWER (49), Whomp's Fortress (ov102). The vtable
# _ZTV13FortressTower, all five methods and the SpawnInfo are already in the
# link (WARP_PIPE, id 298, is registered at gate 33 and installs the same
# table); this slice is the one new symbol, the factory FortressTower_Spawn.
# The registry row reuses hal_fill_warp_pipe_vtable (fill 0). See
# slice_gate102.txt and the WARP_PIPE section in hal/actor_classes_bob_world.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate102.txt" SLICE102_LINES)
set(SLICE102_SOURCES "")
foreach(line IN LISTS SLICE102_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE102_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 103: STAR_SWITCH (12), Whomp's Fortress (ov002). Same class body as
# EXCLAMATION_SWITCH (id 11, registered at gate 51, installs _ZTV10StarSwitch);
# all five methods are already compiled and the vtable filled with its PMF
# state seat. This slice is the one new symbol, the factory StarSwitch_Spawn;
# the SpawnInfo mount is in ov002_syms.txt and the registry row reuses
# hal_fill_exclamation_switch_vtable (fill 0). See slice_gate103.txt and the
# EXCLAMATION_SWITCH section in hal/actor_classes_bob_world.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate103.txt" SLICE103_LINES)
set(SLICE103_SOURCES "")
foreach(line IN LISTS SLICE103_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE103_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 128: EXCLAMATION_BLOCK (21), Jolly Roger Bay's sunken-ship sublevel
# (level 9, ov017), the class living in ov102. Same class shape as QUESTION_BLOCK
# (id 20, registered at gate 23, installs _ZTV13QuestionBlock); all of
# QuestionBlock's methods are already compiled and the vtable filled. This slice
# is the one new symbol, the factory ExclamationBlock_Spawn; the SpawnInfo mount
# is already in ov102_syms.txt and the registry row reuses
# hal_fill_question_block_vtable (fill 0). See slice_gate128.txt and the
# QUESTION_BLOCK section in hal/actor_classes.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate128.txt" SLICE128_LINES)
set(SLICE128_SOURCES "")
foreach(line IN LISTS SLICE128_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE128_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 121: STATIC_ROCK (61), Jolly Roger Bay (ov102). Shares _ZTV13FortressTower
# (WARP_PIPE gate 33, FORTRESS_TOWER gate 102 install it); the one new symbol is
# the factory StaticRock_Spawn. Registry row reuses hal_fill_warp_pipe_vtable.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate121.txt" SLICE121_LINES)
set(SLICE121_SOURCES "")
foreach(line IN LISTS SLICE121_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE121_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 122: BRICK_BLOCK_SWITCH_ACTIVATED (19), Jolly Roger Bay (ov002). Same body
# as BIG_BRICK_BLOCK (_ZTV13BigBrickBlock, gate 16); the one new symbol is the
# factory BrickBlockSwitchActivated_Spawn. SpawnInfo in ov002_syms.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate122.txt" SLICE122_LINES)
set(SLICE122_SOURCES "")
foreach(line IN LISTS SLICE122_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE122_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 123: GREEN_SHELL_BLOCK_TAG (323), Jolly Roger Bay (ov002). A tag body
# sharing _ZTV10BrickBlock (gate 49); the one new symbol is the factory
# GreenShellBlockTag_Spawn. SpawnInfo in ov002_syms.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate123.txt" SLICE123_LINES)
set(SLICE123_SOURCES "")
foreach(line IN LISTS SLICE123_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE123_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 142/143: IceSlideManager (356), Cool Cool Mountain's slide (ov019), level
# 11's one own-overlay class. The matched closure (factory, InitResources,
# Behavior, sinit) is slice_gate142.txt; the vtable fill is
# hal/actor_classes_ccm.cpp and the two method faces are in hal/method_faces.cpp.
# The per-symbol ov019 mount (ov019_syms.txt) and this class's sinit
# (__sinit_ov019_02112b14, run from hal/actor_overlays.cpp) seat its SpawnInfo
# and construction data.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate142.txt" SLICE142_LINES)
set(SLICE142_SOURCES "")
foreach(line IN LISTS SLICE142_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE142_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 149: Big Boo's Haunt's level-local furniture (ov020, level 12) --
# HAUNTED_CHAIR (326) hosted, the bookend/bookshot cast deferred. ov020 is joined
# to the per-symbol mounts so the classes reach their SpawnInfo/statics by name;
# the vtable fills and faces are in hal/actor_classes_bbh.cpp. See slice_gate149.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate149.txt" SLICE149_LINES)
set(SLICE149_SOURCES "")
foreach(line IN LISTS SLICE149_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE149_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 172: CRATE (194), Hazy Maze Cave's first fresh hosting candidate.
# ov098 was already mounted per symbol; the closure is the factory, the four
# C++ methods, both destructors, the four owned virtual-slot bodies, the
# fourteen state halves and their ov098 helpers. The two state dispatchers are
# host copies (unmatched/Crate_StateDispatch.cpp, with the table seat); the
# vtable fill is hal/actor_classes_hmc.cpp. See slice_gate172.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate172.txt" SLICE172_LINES)
set(SLICE172_SOURCES "")
foreach(line IN LISTS SLICE172_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE172_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 173: UP_DOWN_LIFT_HMC (33), Hazy Maze Cave's second fresh class (ov095).
# ov095 was mounted per symbol at gate 83; the closure is the factory, the two
# lifecycle methods + Render, both destructors and the five state bodies with
# their two ov095 helpers. Behavior is a host copy
# (unmatched/UpDownLiftBbh_Behavior.cpp, with the state seat) because it
# dispatches the state table through an incomplete-class pointer-to-member. The
# fill is hal/actor_classes_updownlift.cpp. See slice_gate173.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate173.txt" SLICE173_LINES)
set(SLICE173_SOURCES "")
foreach(line IN LISTS SLICE173_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE173_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 174: MONTY_MOLE (310), first of ov080's enemy cast. ov080 was mounted
# per symbol at gate 50 and already carries every data symbol the mole reads.
# Behavior is a host copy (unmatched/MontyMole_StateDispatch.cpp, with the
# six-record state seat); D1 is a host thunk in the fill (the matched .cpp is
# the auto-emitted-member-dtor form). The fill is
# hal/actor_classes_montymole.cpp. See slice_gate174.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate174.txt" SLICE174_LINES)
set(SLICE174_SOURCES "")
foreach(line IN LISTS SLICE174_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE174_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 175: BLUE_FLAME (317) + RED_FLAME (316), the cave's flames, ov002. One
# class, two factories, one host vtable (hal/actor_classes_flame.cpp, which
# also carries the Player::Burn face); no state table, no seat, no host
# copies. Level 13 names RED_FLAME x17. See slice_gate175.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate175.txt" SLICE175_LINES)
set(SLICE175_SOURCES "")
foreach(line IN LISTS SLICE175_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE175_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 176: SCUTTLEBUG (255), ov071's first mount. The closure is the factory,
# the five lifecycle methods (three real C++ methods, two C forms), D0, the
# eighteen state halves and their ov071 helpers, plus three arm9 C-form
# helpers. The two state dispatchers are host copies with the source-side seat
# (unmatched/Scuttlebug_StateDispatch.cpp); the fill is
# hal/actor_classes_scuttlebug.cpp. See slice_gate176.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate176.txt" SLICE176_LINES)
set(SLICE176_SOURCES "")
foreach(line IN LISTS SLICE176_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE176_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 177: BULLY (215) + BIG_BULLY (216) + ROTATING_FIREBAR (81), ov064's
# first mount. No PMF tables; the bullies' D1/D0 and all three Renders are
# host thunks/copies (the fill and unmatched/ModelAnim_Renders.cpp); the
# fills are hal/actor_classes_ov064.cpp. See slice_gate177.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate177.txt" SLICE177_LINES)
set(SLICE177_SOURCES "")
foreach(line IN LISTS SLICE177_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE177_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate178.txt" SLICE178_LINES)
set(SLICE178_SOURCES "")
foreach(line IN LISTS SLICE178_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE178_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate179.txt" SLICE179_LINES)
set(SLICE179_SOURCES "")
foreach(line IN LISTS SLICE179_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE179_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate182.txt" SLICE182_LINES)
set(SLICE182_SOURCES "")
foreach(line IN LISTS SLICE182_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE182_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate180.txt" SLICE180_LINES)
set(SLICE180_SOURCES "")
foreach(line IN LISTS SLICE180_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE180_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 188: Jolly Roger Bay's six MOVER classes' matched lifecycle closure. The
# host copies (the three slot-5 renders + Unagi's PMF dispatch/Behavior + the
# body-segment near-miss) are unmatched/{Jrb_Renders,Unagi_StateDispatch,
# Unagi_BodyChain}.cpp, added beside the slice at each target.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate188.txt" SLICE188_LINES)
set(SLICE188_SOURCES "")
foreach(line IN LISTS SLICE188_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE188_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 190: Cool Cool Mountain's CCM slice A -- ICE_SHEET (295) and
# POWER_STAR_CREATE (355), ov018's own overlay, plus ONE_UP_LOGO (331) on the
# already-mounted ov002. The fill is appended to hal/actor_classes_ccm.cpp
# (already carries gate 142's IceSlideManager).
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate190.txt" SLICE190_LINES)
set(SLICE190_SOURCES "")
foreach(line IN LISTS SLICE190_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE190_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 191: SKI_LIFT (63) + MOTHER_PENGUIN (257), ov018, extending the
# gate-190 ov018 per-symbol mount (port/ov018_syms.txt). The fill is
# appended to hal/actor_classes_ccm.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate191.txt" SLICE191_LINES)
set(SLICE191_SOURCES "")
foreach(line IN LISTS SLICE191_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE191_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 192: the ov081 trio -- ICE_BLOCK (18), MR_BLIZZARD (223), SPINDRIFT
# (312). Fresh per-symbol ov081 mount (port/ov081_syms.txt). The fill is
# hal/actor_classes_ov081.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate192.txt" SLICE192_LINES)
set(SLICE192_SOURCES "")
foreach(line IN LISTS SLICE192_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE192_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 193: BABY_PENGUIN (256) + the actual SNOWMAN (272, the unnamed
# daBgSnwmn_c block), ov072. Fresh per-symbol ov072 mount
# (port/ov072_syms.txt). The fill is hal/actor_classes_ov072.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate193.txt" SLICE193_LINES)
set(SLICE193_SOURCES "")
foreach(line IN LISTS SLICE193_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE193_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 194: HOOT_THE_OWL (234), ov094. Fresh per-symbol ov094 mount
# (port/ov094_syms.txt). The fill is hal/actor_classes_ov094.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate194.txt" SLICE194_LINES)
set(SLICE194_SOURCES "")
foreach(line IN LISTS SLICE194_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE194_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 199: SWITCH_PILLAR (34) + BASEMENT_WATER (35), ov012 (level 4). Per-
# symbol ov012 mount (port/ov012_syms.txt). The fill is
# hal/actor_classes_ov012.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate199.txt" SLICE199_LINES)
set(SLICE199_SOURCES "")
foreach(line IN LISTS SLICE199_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE199_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 200: PUSH_BLOCK (306, the "PowerFlower" id-decoy) + MUGEN_BGM (351),
# both ov002. The fill is hal/actor_classes_ov002g200.cpp; the state
# dispatch + dest seat is unmatched/PushBlock_StateDispatch.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate200.txt" SLICE200_LINES)
set(SLICE200_SOURCES "")
foreach(line IN LISTS SLICE200_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE200_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gates 64-69: ov079's four static initialisers plus the six enemy classes'
# matched lifecycle closures -- WHOMP (164), WHOMP_KING (165), BULLET_BILL (222),
# BILL_BLASTER (43), FORTRESS_WALL (47), FORTRESS_WALL_BREAKABLE (48). The two
# Behaviors that are host copies (WHOMP's and BULLET_BILL's PMF dispatch) are the
# unmatched/ files appended below, not in the slice. See slice_gate64.txt and
# hal/actor_classes_wf_enemy.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate64.txt" SLICE64_LINES)
set(SLICE64_SOURCES "")
foreach(line IN LISTS SLICE64_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE64_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE64_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/actor_classes_wf_enemy.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/wf_enemy_bridges.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Whomp_Behavior.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/BulletBill_Behavior.cpp")

# Gate 89: POWER_STAR (178, ov002), and gate 90: STAR_CAMERA (177, ov002).
# PowerStar's matched lifecycle closure (slice_gate89.txt) plus the vtable
# fill / method faces (hal/actor_classes_star.cpp) and the host-copy Behavior
# with its fourteen-entry PMF state seat (unmatched/PowerStar_States.cpp).
# STAR_CAMERA needs no slice of its own: it is a bare Actor whose base vtable
# (_ZTV5Actor / data_0208e3a4) is filled by hal_fill_actor_base_vtable in the
# same star file; every base method is already linked.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate89.txt" SLICE89_LINES)
set(SLICE89_SOURCES "")
foreach(line IN LISTS SLICE89_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE89_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE89_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/actor_classes_star.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/PowerStar_States.cpp")

# Two ov079 TUs need hostgen, both mwcc-vs-MSVC syntax quirks the byte-verified
# tree must not be edited for. _ZN5Whomp13InitResourcesEv is the decl_common.h
# HEADER_SHADOW case (func_01ffb07c/func_020396d0 declared with one pointer
# type in the header and another in the TU -- one register on the ROM, C2733 to
# MSVC). func_ov079_02126f8c (BillBlaster::Behavior) declares its local struct's
# members out of line as well as in-struct, which mwccarm tolerates and MSVC
# rejects as C2761; hostgen's MEMBER_REDECL deletes the redundant copies.
set(GATE64_SYMS _ZN5Whomp13InitResourcesEv func_ov079_02126f8c)
set(GATE64_GEN "")
foreach(sym IN LISTS GATE64_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE64_GEN "${sym_out}")
endforeach()

# Gate 15: the per-frame actor spine (BeforeBehavior/Behavior/AfterBehavior),
# which is what makes the real WithMeshClsn tracking work -- see the header of
# slice_gate15.txt for why prev-pos is load-bearing.
#
# Actor::BeforeBehavior defines its eight engine BSS symbols at file scope the
# way mwccarm accepts (tentative definitions the linker merges). MSVC's C++
# front end makes them strong and they all belong to the HAL, so the file goes
# through hostgen with --extern-data.
set(GATE15_SYMS _ZN5Actor14BeforeBehaviorEv)
set(GATE15_GEN "")
foreach(sym IN LISTS GATE15_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" --extern-data ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE15_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate15.txt" SLICE15_LINES)
set(SLICE15_SOURCES "")
foreach(line IN LISTS SLICE15_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE15_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 16: real objects on the castle grounds -- the ROM's own actor frame
# (the five processing lists) plus one matched lifecycle closure per class the
# registry carries. See the header of slice_gate16.txt.
#
# The hostgen set is the gate-9 case each time: C++ syntax (a real method, or
# a `virtual` shadow class) inside a .c file, which mwccarm accepted and MSVC
# will not compile as C. hostgen emits .cpp with the symbols wrapped back to
# C linkage, so the byte-verified tree is never edited.
set(GATE16_SYMS _ZN5Actor22BeforeCleanupResourcesEv
    func_ov002_020aefb8
    # Gate 33: the WARP's Behavior is C++ inside a .c -- the same case.
    func_ov002_020ec410)
set(GATE16_GEN "")
foreach(sym IN LISTS GATE16_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND GATE16_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate16.txt" SLICE16_LINES)
set(SLICE16_SOURCES "")
foreach(line IN LISTS SLICE16_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE16_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# argfix: Actor::DistToCPlayer (gate 16) and Actor::FarthestPlayer (gate 89) both
# call Actor::ClosestPlayer() with no argument and rely on `this` riding ARM r0.
# One host copy carries both wrappers; it is added here (both gates ship on the
# same three targets, so add it once). Src lines commented out in slice_gate16.txt
# and slice_gate89.txt.
list(APPEND SLICE16_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Actor_ClosestPlayerWrappers.cpp")

# The camera slice is on both binaries. The gate-13 State objects carry real
# DS function addresses, and Player::ChangeState already reaches
# func_0200d81c -> Camera::ChangeState on every state change, so the smoke
# needs the host dispatcher too or the first ChangeState calls a mwcc
# member-function pointer.
add_executable(smoke_player tests/smoke_player.cpp "${OV002DATA_C}" ${LEVEL_OV_DATA}
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/door_ring_faces.cpp hal/fader_wipes.cpp hal/level_boot.cpp hal/cannon_probe.cpp hal/message_boot.cpp hal/message_gen_text.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_classes_bob_world.cpp hal/actor_faces_bob.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/sub_screen.cpp hal/sub_actors.cpp hal/stage_bridges.cpp hal/level_change.cpp hal/star_flow.cpp hal/oam_lists.cpp hal/blend_vtable.cpp hal/particle_vtable.cpp hal/particle_bridges.cpp unmatched/Particle_RideThroughs.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE_PTR_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES} ${SLICE25_SOURCES} ${SLICE26_SOURCES}
    ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE33_SOURCES} ${GATE6_GEN} ${SLICE31_SOURCES} ${SLICE35_SOURCES}
    ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE32_SOURCES}
    ${GATE32_HOST_SOURCES} ${GATE32_GEN} ${GATE6_GEN}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN} ${GATE35_GEN} ${GATE29_GEN}
    ${SLICE40_SOURCES} ${SLICE41_SOURCES} ${SLICE42_SOURCES}
    hal/actor_classes_painting.cpp ${SLICE50_SOURCES} hal/actor_classes_wf.cpp
    unmatched/Cap_StateDispatch.cpp unmatched/ExclamationSwitch_StateDispatch.cpp ${SLICE51_SOURCES} ${SLICE59_SOURCES} ${SLICE78_SOURCES} ${SLICE79_SOURCES} ${SLICE82_SOURCES} ${SLICE83_SOURCES} ${SLICE64_SOURCES} ${GATE64_GEN} ${SLICE89_SOURCES} ${SLICE102_SOURCES} ${SLICE103_SOURCES} ${SLICE128_SOURCES} ${SLICE121_SOURCES} ${SLICE122_SOURCES} ${SLICE123_SOURCES}
    hal/actor_classes_ccm.cpp ${SLICE142_SOURCES} ${SLICE190_SOURCES} ${SLICE191_SOURCES} unmatched/SoundObject_Behavior.cpp unmatched/OneUpLogo_InitResources.cpp unmatched/MotherPenguin_InitResources.cpp unmatched/MotherPenguin_AfterClsnSeat.cpp unmatched/MotherPenguin_Behavior.cpp hal/actor_classes_ov081.cpp ${SLICE192_SOURCES} unmatched/MrBlizzard_StateDispatch.cpp unmatched/MrBlizzard_Behavior.cpp hal/actor_classes_ov072.cpp ${SLICE193_SOURCES} unmatched/Snowman_InitResources.cpp unmatched/BabyPenguin_StateEnter.cpp unmatched/BabyPenguin_StateTick.cpp unmatched/BabyPenguin_InitResources.cpp unmatched/BabyPenguin_LandTransition.cpp hal/actor_classes_ov094.cpp ${SLICE194_SOURCES} unmatched/HootTheOwl_StateDispatch.cpp hal/actor_classes_ov012.cpp ${SLICE199_SOURCES} hal/actor_classes_ov002g200.cpp ${SLICE200_SOURCES} unmatched/PushBlock_StateDispatch.cpp
    hal/actor_classes_bbh.cpp ${SLICE149_SOURCES}
    hal/actor_classes_hmc.cpp ${SLICE172_SOURCES} unmatched/Crate_StateDispatch.cpp
    hal/actor_classes_updownlift.cpp ${SLICE173_SOURCES} unmatched/UpDownLiftBbh_Behavior.cpp
    hal/actor_classes_montymole.cpp ${SLICE174_SOURCES} unmatched/MontyMole_StateDispatch.cpp unmatched/MontyMole_1Up.cpp
    hal/actor_classes_flame.cpp ${SLICE175_SOURCES}
    hal/actor_classes_scuttlebug.cpp ${SLICE176_SOURCES} unmatched/Scuttlebug_StateDispatch.cpp
    hal/actor_classes_ov064.cpp ${SLICE177_SOURCES} unmatched/Bully_AfterClsn.cpp
    hal/actor_classes_ov064_gate178.cpp ${SLICE178_SOURCES} unmatched/Ov064Gate178_States.cpp
    hal/actor_classes_jrb.cpp ${SLICE188_SOURCES} unmatched/Jrb_Renders.cpp
    unmatched/Unagi_StateDispatch.cpp unmatched/Unagi_BodyChain.cpp unmatched/Jrb_Aliases.cpp
    hal/actor_classes_bowserpuzzle.cpp ${SLICE179_SOURCES} unmatched/BowserPuzzle_StateDispatch.cpp unmatched/BowserPuzzle_Render.cpp
    hal/actor_classes_koopa_chuckya.cpp hal/koopa_chuckya_faces.cpp ${SLICE182_SOURCES} unmatched/KoopaChuckya_StateDispatch.cpp unmatched/Koopa_Render.cpp
    ${SLICE180_SOURCES}
    hal/actor_classes_l7.cpp ${SLICE_L7_SOURCES} ${GATE_L7_GEN} unmatched/PiranhaPlant_States.cpp
    unmatched/WaterBomb_StateDispatch.cpp unmatched/WaterBomb_State0.cpp
    unmatched/KnockDownPlank_Behavior.cpp unmatched/FallBlockWf_InitVeneer.cpp
    unmatched/StarMarker_D1.cpp
    "${ROMDATA_C}" "${OV009SYMS_C}" ${ACTOR_OV_SYMS} "${OVCROSS_C}")
target_include_directories(smoke_player PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_player PRIVATE ntr)
target_compile_definitions(smoke_player PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}"
    # this target links hal/cxx_aliases.cpp, which already carries
    # the Heap::_Deallocate face hal/blend_vtable.cpp repeats
    PORT_CXX_ALIASES_LINKED)
if(MSVC)
    target_compile_options(smoke_player PRIVATE /wd4311 /wd4312 /wd4068 /wd4576)
endif()

# Gate 12: the interactive window (same stack, windowed loop).
add_executable(walk_window tests/walk_window.cpp "${OV002DATA_C}" ${LEVEL_OV_DATA}
    # dialogue pipeline (2026-08-08): the box pump (Stage::UpdateMessage's
    # dialogue arm), the engine-A 2D-over-3D compositor, and the headless probe.
    hal/message_pump.cpp hal/message_compositor.cpp hal/message_probe.cpp hal/message_gen_text.cpp hal/input_probe.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/door_ring_faces.cpp hal/fader_wipes.cpp hal/level_boot.cpp hal/cannon_probe.cpp hal/message_boot.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_classes_bob_world.cpp hal/actor_faces_bob.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/sub_actors.cpp hal/sub_screen.cpp hal/stage_bridges.cpp hal/level_change.cpp hal/star_flow.cpp hal/oam_lists.cpp hal/blend_vtable.cpp hal/particle_vtable.cpp hal/particle_bridges.cpp unmatched/Particle_RideThroughs.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE_PTR_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES}
    ${SLICE33_SOURCES} ${GATE6_GEN}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES} ${SLICE31_SOURCES} ${GATE6_GEN}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES} ${SLICE35_SOURCES} ${GATE6_GEN}
    ${SLICE32_SOURCES} ${GATE32_HOST_SOURCES} ${GATE32_GEN} ${GATE6_GEN}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN} ${GATE35_GEN} ${GATE29_GEN}
    ${SLICE40_SOURCES} ${SLICE41_SOURCES} ${SLICE42_SOURCES}
    hal/actor_classes_painting.cpp ${SLICE50_SOURCES} hal/actor_classes_wf.cpp
    unmatched/Cap_StateDispatch.cpp unmatched/ExclamationSwitch_StateDispatch.cpp ${SLICE51_SOURCES} ${SLICE59_SOURCES} ${SLICE78_SOURCES} ${SLICE79_SOURCES} ${SLICE82_SOURCES} ${SLICE83_SOURCES} ${SLICE64_SOURCES} ${GATE64_GEN} ${SLICE89_SOURCES} ${SLICE102_SOURCES} ${SLICE103_SOURCES} ${SLICE128_SOURCES} ${SLICE121_SOURCES} ${SLICE122_SOURCES} ${SLICE123_SOURCES}
    hal/actor_classes_ccm.cpp ${SLICE142_SOURCES} ${SLICE190_SOURCES} ${SLICE191_SOURCES} unmatched/SoundObject_Behavior.cpp unmatched/OneUpLogo_InitResources.cpp unmatched/MotherPenguin_InitResources.cpp unmatched/MotherPenguin_AfterClsnSeat.cpp unmatched/MotherPenguin_Behavior.cpp hal/actor_classes_ov081.cpp ${SLICE192_SOURCES} unmatched/MrBlizzard_StateDispatch.cpp unmatched/MrBlizzard_Behavior.cpp hal/actor_classes_ov072.cpp ${SLICE193_SOURCES} unmatched/Snowman_InitResources.cpp unmatched/BabyPenguin_StateEnter.cpp unmatched/BabyPenguin_StateTick.cpp unmatched/BabyPenguin_InitResources.cpp unmatched/BabyPenguin_LandTransition.cpp hal/actor_classes_ov094.cpp ${SLICE194_SOURCES} unmatched/HootTheOwl_StateDispatch.cpp hal/actor_classes_ov012.cpp ${SLICE199_SOURCES} hal/actor_classes_ov002g200.cpp ${SLICE200_SOURCES} unmatched/PushBlock_StateDispatch.cpp
    hal/actor_classes_bbh.cpp ${SLICE149_SOURCES}
    hal/actor_classes_hmc.cpp ${SLICE172_SOURCES} unmatched/Crate_StateDispatch.cpp
    hal/actor_classes_updownlift.cpp ${SLICE173_SOURCES} unmatched/UpDownLiftBbh_Behavior.cpp
    hal/actor_classes_montymole.cpp ${SLICE174_SOURCES} unmatched/MontyMole_StateDispatch.cpp unmatched/MontyMole_1Up.cpp
    hal/actor_classes_flame.cpp ${SLICE175_SOURCES}
    hal/actor_classes_scuttlebug.cpp ${SLICE176_SOURCES} unmatched/Scuttlebug_StateDispatch.cpp
    hal/actor_classes_ov064.cpp ${SLICE177_SOURCES} unmatched/Bully_AfterClsn.cpp
    hal/actor_classes_ov064_gate178.cpp ${SLICE178_SOURCES} unmatched/Ov064Gate178_States.cpp
    hal/actor_classes_jrb.cpp ${SLICE188_SOURCES} unmatched/Jrb_Renders.cpp
    unmatched/Unagi_StateDispatch.cpp unmatched/Unagi_BodyChain.cpp unmatched/Jrb_Aliases.cpp
    hal/actor_classes_bowserpuzzle.cpp ${SLICE179_SOURCES} unmatched/BowserPuzzle_StateDispatch.cpp unmatched/BowserPuzzle_Render.cpp
    hal/actor_classes_koopa_chuckya.cpp hal/koopa_chuckya_faces.cpp ${SLICE182_SOURCES} unmatched/KoopaChuckya_StateDispatch.cpp unmatched/Koopa_Render.cpp
    ${SLICE180_SOURCES}
    hal/actor_classes_l7.cpp ${SLICE_L7_SOURCES} ${GATE_L7_GEN} unmatched/PiranhaPlant_States.cpp
    unmatched/WaterBomb_StateDispatch.cpp unmatched/WaterBomb_State0.cpp
    unmatched/KnockDownPlank_Behavior.cpp unmatched/FallBlockWf_InitVeneer.cpp
    unmatched/StarMarker_D1.cpp
    "${PORT_GITTIP_C}"
    "${ROMDATA_C}" "${OV009SYMS_C}" ${ACTOR_OV_SYMS} "${OVCROSS_C}")
target_include_directories(walk_window PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
# 2x (512x384, window ZOOM 2): the software raster is fill-bound; the
# hires 1024x768 tier burns ~40ms/frame on the level alone, native
# 256x192 reads too chunky. 2x is ~12ms -> 60fps with frame pacing.
target_link_libraries(walk_window PRIVATE ntr_2x)
target_compile_definitions(walk_window PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}"
    # this target links hal/cxx_aliases.cpp, which already carries
    # the Heap::_Deallocate face hal/blend_vtable.cpp repeats
    PORT_CXX_ALIASES_LINKED)
# ROM-CLEAN: the shipped window loads its ROM tables from romdata.bin at boot.
# The dispatch (generated) + loader come in as extra sources, PORT_ROM_CLEAN
# turns on the port_romdata_load() call in walk_window.cpp, and a custom target
# makes the blob + self-check a build prerequisite.
if(PORT_ROM_CLEAN)
    target_sources(walk_window PRIVATE "${ROMDATA_DISPATCH_C}"
                   hal/romdata_loader.cpp)
    target_compile_definitions(walk_window PRIVATE PORT_ROM_CLEAN)
    add_custom_target(romblob_ready_ww DEPENDS "${ROMDATA_BIN}"
        "${CMAKE_BINARY_DIR}/assets/romblob_verified.stamp"
        "${ROMDATA_RECIPE}")
    add_dependencies(walk_window romblob_ready_ww)
endif()
if(MSVC)
    target_compile_options(walk_window PRIVATE /wd4311 /wd4312 /wd4068 /wd4576)
endif()
port_enable_sdl2(walk_window)

# hires clone for screenshot sessions (4x resolution, ~1/16 the fps)
add_executable(walk_window_hires tests/walk_window.cpp "${OV002DATA_C}" ${LEVEL_OV_DATA}
    # dialogue pipeline (2026-08-08): pump, engine-A compositor, headless probe.
    hal/message_pump.cpp hal/message_compositor.cpp hal/message_probe.cpp hal/message_gen_text.cpp hal/input_probe.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/door_ring_faces.cpp hal/fader_wipes.cpp hal/level_boot.cpp hal/cannon_probe.cpp hal/message_boot.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_classes_bob_world.cpp hal/actor_faces_bob.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/sub_actors.cpp hal/sub_screen.cpp hal/stage_bridges.cpp hal/level_change.cpp hal/star_flow.cpp hal/oam_lists.cpp hal/blend_vtable.cpp hal/particle_vtable.cpp hal/particle_bridges.cpp unmatched/Particle_RideThroughs.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE_PTR_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES}
    ${SLICE33_SOURCES} ${GATE6_GEN}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES} ${SLICE31_SOURCES} ${GATE6_GEN}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES} ${SLICE35_SOURCES} ${GATE6_GEN}
    ${SLICE32_SOURCES} ${GATE32_HOST_SOURCES} ${GATE32_GEN} ${GATE6_GEN}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN} ${GATE35_GEN} ${GATE29_GEN}
    ${SLICE40_SOURCES} ${SLICE41_SOURCES} ${SLICE42_SOURCES}
    hal/actor_classes_painting.cpp ${SLICE50_SOURCES} hal/actor_classes_wf.cpp
    unmatched/Cap_StateDispatch.cpp unmatched/ExclamationSwitch_StateDispatch.cpp ${SLICE51_SOURCES} ${SLICE59_SOURCES} ${SLICE78_SOURCES} ${SLICE79_SOURCES} ${SLICE82_SOURCES} ${SLICE83_SOURCES} ${SLICE64_SOURCES} ${GATE64_GEN} ${SLICE89_SOURCES} ${SLICE102_SOURCES} ${SLICE103_SOURCES} ${SLICE128_SOURCES} ${SLICE121_SOURCES} ${SLICE122_SOURCES} ${SLICE123_SOURCES}
    hal/actor_classes_ccm.cpp ${SLICE142_SOURCES} ${SLICE190_SOURCES} ${SLICE191_SOURCES} unmatched/SoundObject_Behavior.cpp unmatched/OneUpLogo_InitResources.cpp unmatched/MotherPenguin_InitResources.cpp unmatched/MotherPenguin_AfterClsnSeat.cpp unmatched/MotherPenguin_Behavior.cpp hal/actor_classes_ov081.cpp ${SLICE192_SOURCES} unmatched/MrBlizzard_StateDispatch.cpp unmatched/MrBlizzard_Behavior.cpp hal/actor_classes_ov072.cpp ${SLICE193_SOURCES} unmatched/Snowman_InitResources.cpp unmatched/BabyPenguin_StateEnter.cpp unmatched/BabyPenguin_StateTick.cpp unmatched/BabyPenguin_InitResources.cpp unmatched/BabyPenguin_LandTransition.cpp hal/actor_classes_ov094.cpp ${SLICE194_SOURCES} unmatched/HootTheOwl_StateDispatch.cpp hal/actor_classes_ov012.cpp ${SLICE199_SOURCES} hal/actor_classes_ov002g200.cpp ${SLICE200_SOURCES} unmatched/PushBlock_StateDispatch.cpp
    hal/actor_classes_bbh.cpp ${SLICE149_SOURCES}
    hal/actor_classes_hmc.cpp ${SLICE172_SOURCES} unmatched/Crate_StateDispatch.cpp
    hal/actor_classes_updownlift.cpp ${SLICE173_SOURCES} unmatched/UpDownLiftBbh_Behavior.cpp
    hal/actor_classes_montymole.cpp ${SLICE174_SOURCES} unmatched/MontyMole_StateDispatch.cpp unmatched/MontyMole_1Up.cpp
    hal/actor_classes_flame.cpp ${SLICE175_SOURCES}
    hal/actor_classes_scuttlebug.cpp ${SLICE176_SOURCES} unmatched/Scuttlebug_StateDispatch.cpp
    hal/actor_classes_ov064.cpp ${SLICE177_SOURCES} unmatched/Bully_AfterClsn.cpp
    hal/actor_classes_ov064_gate178.cpp ${SLICE178_SOURCES} unmatched/Ov064Gate178_States.cpp
    hal/actor_classes_jrb.cpp ${SLICE188_SOURCES} unmatched/Jrb_Renders.cpp
    unmatched/Unagi_StateDispatch.cpp unmatched/Unagi_BodyChain.cpp unmatched/Jrb_Aliases.cpp
    hal/actor_classes_bowserpuzzle.cpp ${SLICE179_SOURCES} unmatched/BowserPuzzle_StateDispatch.cpp unmatched/BowserPuzzle_Render.cpp
    hal/actor_classes_koopa_chuckya.cpp hal/koopa_chuckya_faces.cpp ${SLICE182_SOURCES} unmatched/KoopaChuckya_StateDispatch.cpp unmatched/Koopa_Render.cpp
    ${SLICE180_SOURCES}
    hal/actor_classes_l7.cpp ${SLICE_L7_SOURCES} ${GATE_L7_GEN} unmatched/PiranhaPlant_States.cpp
    unmatched/WaterBomb_StateDispatch.cpp unmatched/WaterBomb_State0.cpp
    unmatched/KnockDownPlank_Behavior.cpp unmatched/FallBlockWf_InitVeneer.cpp
    unmatched/StarMarker_D1.cpp
    "${PORT_GITTIP_C}"
    "${ROMDATA_C}" "${OV009SYMS_C}" ${ACTOR_OV_SYMS} "${OVCROSS_C}")
target_include_directories(walk_window_hires PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(walk_window_hires PRIVATE ntr_hires)
target_compile_definitions(walk_window_hires PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}"
    # this target links hal/cxx_aliases.cpp, which already carries
    # the Heap::_Deallocate face hal/blend_vtable.cpp repeats
    PORT_CXX_ALIASES_LINKED)
if(PORT_ROM_CLEAN)
    target_sources(walk_window_hires PRIVATE "${ROMDATA_DISPATCH_C}"
                   hal/romdata_loader.cpp)
    target_compile_definitions(walk_window_hires PRIVATE PORT_ROM_CLEAN)
    add_dependencies(walk_window_hires romblob_ready_ww)
endif()
if(MSVC)
    target_compile_options(walk_window_hires PRIVATE /wd4311 /wd4312 /wd4068 /wd4576)
endif()
port_enable_sdl2(walk_window_hires)


# The SDAT player on its own: parser, sequencer and mixer with no game code
# and no command queue, so a failure is localised to the player itself.
add_executable(smoke_sdat tests/smoke_sdat.cpp
    hal/sdat/sdat.cpp hal/sdat/mixer.cpp hal/sdat/sseq.cpp hal/sdat/out_win.cpp)
target_include_directories(smoke_sdat PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_compile_definitions(smoke_sdat PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_sdat PRIVATE /wd4311 /wd4312 /wd4068)
endif()
