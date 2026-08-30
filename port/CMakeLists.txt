cmake_minimum_required(VERSION 3.16)
project(sm64ds-port C CXX)
# Link the static CRT so shipped exes never depend on the VC++ redist (CMP0091).
# Release 0.1.6 shipped dynamically linked because nothing pinned this and CMake
# defaults to the DLL runtime, so a player without the redist could not start the
# game at all, and that failure produces no crash dump and never reaches intake.
# This must stay GLOBAL and stay above the targets: a per-target property on the
# shipped exe alone would leave the test harnesses on the dynamic default, and a
# harness linked differently from the shipped artifact resolves addresses that do
# not match what players run. The generator expression keeps the debug CRT for
# Debug builds, so heap and iterator assertions still fire while chasing
# allocator bugs; plain "MultiThreaded" would silently link the release CRT there.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MAP")
add_compile_options(/Oy-)

# Test-only fault injection: SM64DS_TEST_QUARANTINE, the hook in
# port/unmatched/func_02043fdc_hostcopy.cpp that force-freezes an actor by
# raising the access violation a real per-actor decline raises. A proof run
# needs it to reproduce a quarantine on demand; a player must never carry a
# way to force a fault on an arbitrary actor id read out of the environment,
# so it is OFF unless a build asks for it by name. build-port.cmd forwards its
# arguments to cmake:
#
#     build-port.cmd -DPORT_TEST_HOOKS=ON
#
# It is a cache variable, so a tree configured for a proof STAYS a proof tree;
# configure a fresh build directory (or pass -DPORT_TEST_HOOKS=OFF) for anything
# that ships. The warning is there so a proof binary is identifiable from the
# build log and not only from the source.
option(PORT_TEST_HOOKS
       "compile the SM64DS_TEST_QUARANTINE fault-injection hook (TEST BUILDS ONLY)"
       OFF)
if(PORT_TEST_HOOKS)
    add_compile_definitions(PORT_TEST_HOOKS=1)
    message(WARNING
        "PORT_TEST_HOOKS=ON: these binaries carry the SM64DS_TEST_QUARANTINE "
        "fault-injection hook and are TEST ONLY. Do not ship them.")
endif()

# Gate 1: compile the manifest slice out of ../src plus the HAL, link the
# smoke runner. 32-bit host only -- platform.h enforces it at compile time.

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

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
    ntr/io.cpp ntr/gx.cpp ntr/texture.cpp ntr/ppu.cpp ntr/ppu_sub.cpp ntr/ppu_audit.cpp ntr/lz77.cpp
    ntr/rt.cpp ntr/runtime.cpp ntr/fs.cpp ntr/bmd.cpp)
target_include_directories(ntr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ntr/include")
# ntr/runtime.cpp hosts two DS DMA globals (data_020a60c4/data_020a6460) that
# are real save state, so it needs hal/dsstate_seg.h. PRIVATE: only this
# library reads it, consumers keep their own include set.
target_include_directories(ntr PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
if(MSVC)
    target_compile_options(ntr PRIVATE /wd4311 /wd4312)
endif()

# 512x384 middle tier: 4x the pixels of native (~12ms software raster),
# half the chunk of DS-native. The interactive window's default.
add_library(ntr_2x STATIC
    ntr/io.cpp ntr/gx.cpp ntr/texture.cpp ntr/ppu.cpp ntr/ppu_sub.cpp ntr/ppu_audit.cpp ntr/lz77.cpp
    ntr/rt.cpp ntr/runtime.cpp ntr/fs.cpp ntr/bmd.cpp)
target_include_directories(ntr_2x PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ntr/include")
# ntr/runtime.cpp hosts two DS DMA globals (data_020a60c4/data_020a6460) that
# are real save state, so it needs hal/dsstate_seg.h. PRIVATE: only this
# library reads it, consumers keep their own include set.
target_include_directories(ntr_2x PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(ntr_2x PUBLIC NTR_HIRES2)
if(MSVC)
    target_compile_options(ntr_2x PRIVATE /wd4311 /wd4312)
endif()

# The same layer at 1024x768 for the interactive window; the smokes keep
# the DS-native lib (their reference pixel counts depend on 256x192).
add_library(ntr_hires STATIC
    ntr/io.cpp ntr/gx.cpp ntr/texture.cpp ntr/ppu.cpp ntr/ppu_sub.cpp ntr/ppu_audit.cpp ntr/lz77.cpp
    ntr/rt.cpp ntr/runtime.cpp ntr/fs.cpp ntr/bmd.cpp)
target_include_directories(ntr_hires PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ntr/include")
# ntr/runtime.cpp hosts two DS DMA globals (data_020a60c4/data_020a6460) that
# are real save state, so it needs hal/dsstate_seg.h. PRIVATE: only this
# library reads it, consumers keep their own include set.
target_include_directories(ntr_hires PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
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
    "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/func_02073244_hostcopy.c"
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

# The OBJ window's bit contract. No game code and no ROM data: it programs
# engine B's registers, VRAM and OAM by hand and reads ppu_scanout_sub back, so
# it links ntr alone and cannot be perturbed by a slice change.
add_executable(smoke_objwin tests/smoke_objwin.cpp)
target_include_directories(smoke_objwin PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(smoke_objwin PRIVATE ntr)
target_compile_definitions(smoke_objwin PRIVATE SM64DS_PLATFORM_PC)

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
#
# --extern-data is load-bearing here and was missing until w6-c. The file
# spells its three engine globals by ROLE (gGlobalA / gGlobalB /
# Memory::gameHeapPtr) rather than as data_<hex>, so without the flag hostgen
# emitted them as private file-scope objects: this TU compiled, linked and
# unlinked dead actors from a one-byte char while the real scene tree in
# hal/actor_vtables.cpp kept them. Every target below ran that body. See
# hostgen's ROLE_DATA map for the address of each name.
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
                --out "${CMAKE_BINARY_DIR}/host-src" --extern-data ${sym}
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

# THE GAME HEAP for the three actor-world smokes below.
#
# tests/walk_window.cpp and tests/smoke_player.cpp call
# Heap::InitializeGameHeap(0x3b000, 0), the carve func_0201a054 performs at
# arm9 0x0201a108 (re-derived here from extracted/arm9_dec.bin: the prelude
# reads e3a00a3b / e3a01000 / eb00884f, so r0 = 0x3b000, r1 = 0, and the BL
# lands on 0x0203c24c). smoke_actor, smoke_savestate and smoke_persist were
# instead hand-seeding `data_020a0eac_c = data_020a0ea0` -- the game-heap word
# aliased straight onto the root heap, so the ROM's heap object never existed
# and the smokes disagreed with the boot spine about how the heap comes up.
# Lane w3-c measured the link failure that blocked the conversion and left the
# reason at each seat; this is the CMake half it named.
#
# The TU is listed in slice_w1l3.txt, but SLICE_W1L3_SOURCES is built further
# down this file, after these three targets, so the one file is named directly
# rather than hoisting that block over three gates. One line is all it takes:
# Heap::CreateExpandingHeap, the body it calls, has been compiled into these
# targets since gate 3a and was being discarded by /OPT:REF because nothing
# reached it.
set(HEAP_INITGAME_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN4Heap18InitializeGameHeapEjPS_.c")

# THE HEAP DESTROY CHAIN for the same three actor smokes, and it is here for
# the same reason HEAP_INITGAME_SRC is: two src lines that live in a slice these
# targets do not carry.
#
# hal/actor_vtables.cpp hosts Heap_Destroy, the role-named spelling of arm9
# 0x0203c74c that src/_ZN9ActorBase21AfterCleanupResourcesEj.c calls when a
# dying actor owns a heap. It used to abort. Run link60 lane LK5 made the ROM's
# own veneer carry a receiver, so it can now forward instead, and the forward
# needs the veneer TU and the method behind it:
#
#   src/_ZN4Heap8_DestroyEv.cpp   the ROM's tail-call veneer, byte matched
#   src/_ZN4Heap7DestroyEv.cpp    Heap::Destroy, the method it tail-calls
#
# Both are in slice_gate16.txt, which rides smoke_player, walk_window and
# walk_window_hires only. actor_vtables.cpp rides those three AND smoke_actor,
# smoke_savestate and smoke_persist, and GATE9_GEN puts the calling teardown TU
# in all six, so without these two lines the three smokes link-fail on
# __ZN4Heap8_DestroyEv. Measured, not predicted: all three reported that exact
# LNK2019 before this was added. The C face _ZN4Heap7DestroyEv sits in
# hal/heap_vtable.cpp, which all six already compile.
#
# No target gets a duplicate: the three big targets take these two TUs from
# slice_gate16 and are not listed here.
set(HEAP_DESTROY_CHAIN_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN4Heap8_DestroyEv.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN4Heap7DestroyEv.cpp")

add_executable(smoke_actor tests/smoke_actor.cpp "${HEAP_INITGAME_SRC}"
    ${HEAP_DESTROY_CHAIN_SRC}
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    # auto_bss.cpp owns data_020a4ba8, the cleanup list. Gate 9's matched TU
    # reads it from the HAL now (--extern-data + ROLE_DATA) instead of from a
    # private one-byte object of its own, so this smoke has to link the file
    # that actually holds it -- the sibling smokes at the same gate already do.
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp hal/auto_bss.cpp
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

# Save-state verification (lane lk6): the same actor world as smoke_actor, plus
# hal/lk6_savestate.cpp, driving save -> diverge -> load -> byte-exact compare.
add_executable(smoke_savestate tests/smoke_savestate.cpp "${HEAP_INITGAME_SRC}"
    ${HEAP_DESTROY_CHAIN_SRC}
    hal/lk6_savestate.cpp hal/dsstate_seg.cpp hal/auto_bss.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_SOURCES} ${GATE8_EXTRA_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE9_GEN} "${ROMDATA_C}")
target_include_directories(smoke_savestate PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_savestate PRIVATE ntr)
target_compile_definitions(smoke_savestate PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_savestate PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Disk-persistence verification (lane lk7): the same actor world, plus
# hal/lk7_persist.cpp. The parent saves and mirrors to savestate.bin, then
# spawns a SECOND PROCESS of itself which loads the disk state and must land on
# the identical arena hash -- the save surviving the process ending is the
# whole point, so the test crosses a real process boundary. Six header cases
# then run in the parent, each doctoring ONE field of an otherwise valid file:
# corrupted magic, stale gittip, DIFFERENT GAME DATA (an asset folder that
# outlived the build that filled it), the same file loaded anyway under
# SM64DS_SAVESTATE_NO_GUARD=1, a legacy format-2 file, and a truncated one --
# and finally a clean valid file, which must refuse nothing at all, so a green
# run cannot be one where every read happens to fail.
add_executable(smoke_persist tests/smoke_persist.cpp "${HEAP_INITGAME_SRC}"
    ${HEAP_DESTROY_CHAIN_SRC}
    hal/lk6_savestate.cpp hal/lk7_persist.cpp hal/dsstate_seg.cpp
    hal/auto_bss.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp
    "${PORT_GITTIP_C}"
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_SOURCES} ${GATE8_EXTRA_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE9_GEN} "${ROMDATA_C}")
target_include_directories(smoke_persist PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_persist PRIVATE ntr)
target_compile_definitions(smoke_persist PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_persist PRIVATE /wd4311 /wd4312 /wd4068)
    # A disk state stores absolute pointers into the exe image (every actor's
    # vtable), so the image must load at the same base every run. See the
    # matching flag on walk_window, which is the base the shipped states use.
    target_link_options(smoke_persist PRIVATE /DYNAMICBASE:NO)
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
# Slot 28 has two dispatch sites and the two matched transcriptions disagree
# on calling convention; see unmatched/Player_HeadBonk.cpp. This copy is
# src/func_ov002_020cef84.cpp with the dispatch remodelled as a C++ virtual
# so both sites are thiscall, which is what lets slot 28 hold a real body.
list(APPEND SLICE10_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Player_HeadBonk.cpp")
# Slots 23 and 24 have exactly one dispatch site each, and both model the vtable
# as plain cdecl function pointers while every seated slot-23/24 body is
# thiscall; see unmatched/Actor_OnAttacked2Dispatch.cpp and
# unmatched/Actor_OnKickedDispatch.cpp. These copies are
# src/func_ov002_020ef070.cpp and src/func_ov002_020eeeb8.cpp with the dispatch
# remodelled as a C++ virtual so each site is thiscall, which is what lets slots
# 23/24 hold real bodies (and stops the base ac_atk2/ac_kicked seats over-popping
# on every dispatch). Both src lines are commented out of slice_gate10.txt.
list(APPEND SLICE10_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Actor_OnAttacked2Dispatch.cpp")
list(APPEND SLICE10_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Actor_OnKickedDispatch.cpp")

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
    # run linkw wave 7, lane w7b: MrI's InitResources, the one body of the Mr. I
    # closure that is NONMATCHING-but-decompiled. //cpp-marked .c with extern "C"
    # blocks and SharedFilePtr& reference params -- invalid C, same treatment as
    # the gate-176 entry above.
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN3MrI13InitResourcesEv.c"
    # run linkw wave 12, lane w12: the one .c in ov073's closure that is really
    # C++ -- a top-level `extern "C" {` block, invalid C. ChiefChilly's
    # jumped-on / hurt-the-player state helper. Same treatment as the entries
    # above; it is the only one of slice_w12.txt's 67 TUs that needs it.
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov073_0211f61c.c"
    # run link60 lane L2: the ONE .c in ov007's 619-TU closure that is really
    # C++ -- a top-level `extern "C" {` block, invalid C. The whole slice was
    # swept for the //cpp marker rather than this one being found by a compile
    # error and the rest left to luck.
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov007_020bfd70.c"
    # run mg5 lane CCN: the ONE .c in dScMgCoin_c's 61-TU closure that is
    # really C++ -- a //cpp marker, a top-level `extern "C"` block and a
    # thirty-six-virtual dummy struct it dispatches a slot through. The whole
    # slice was swept for the marker and for `extern "C"`, `virtual` and
    # `class`, rather than this one being found by a compile error and the rest
    # left to luck; it is the only hit.
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020dd880.c"
    # run mg9 lane S371: THREE .c files in dScMgAmida_c's 29-TU slice are
    # really C++ -- a //cpp marker, top-level `extern "C"` blocks and, in all
    # three, a dummy virtual struct they dispatch a slot through. That struct
    # is THIRTY-SEVEN virtuals wide in these, not thirty-six, because this
    # class is the one true width-37 class of the minigame family and the slot
    # they reach is its 37th (port/slice_s371.txt section 3). The whole slice
    # was swept for the marker AND for `extern "C"`, `virtual` and `class`, the
    # way lane CCN's entry above prescribes, rather than these being found one
    # at a time by compile errors; these three are the only hits.
    # func_ov006_020d3ba0.c IS THE THIRD HIT AND IT IS NOT HERE. It needs C++
    # for the same reason as the two above -- a 37-virtual dummy struct and a
    # namespace -- but its extern "C" block also writes a bare
    # `int data_0209d4b8;`, which C++ makes a strong definition of an arm9
    # global hal/scene_mg_faces.cpp already owns inside DSSTATE. It is
    # host-copied in unmatched/MgAmida_Slot3ba0.cpp instead, one token changed,
    # and that file's header carries the ruling and the sed line that
    # reproduces it.
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020d122c.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020d4b7c.c"
    # run mg11 lane PKR: the ONE .c in dScMgCard_c's 38-TU slice that is really
    # C++ -- a //cpp marker, a top-level `extern "C" {` block and a two-virtual
    # dummy struct it dispatches ELEMENT vtable slot 1 through, ten times a
    # frame. It is the class's whole state machine (a 14-arm ARM jump table).
    # The slice was swept for the marker AND for `extern "C"`, `virtual` and
    # `class`, the way lane CCN's entry above prescribes, rather than this being
    # found by a compile error and the rest left to luck. Three other files hit
    # the `class` filter and all three are the words "Class_Method" and "RTTI
    # class fields" inside recovered-name COMMENTS, which is the false positive
    # that sweep has to tolerate to stay cheap; this is the only real hit.
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020dac34.c"
    PROPERTIES LANGUAGE CXX)

# Wave-5 close surgery (w5b_review.md R1/R2): two /alternatename routes died
# when w5-b sliced the real bodies under the aliases' LHS names -- a defined
# LHS defeats /alternatename silently. The affected caller TUs get per-source
# renames instead; the dead pragmas are deleted from hal/cxx_aliases.cpp and
# hal/actor_classes_koopa_chuckya.cpp (the alternatename guard enforces the
# class from here on).
# R1: PeachPainting::InitResources spells ov010's SharedFilePtr BSS as
# RollingRock_Spawn (dsd shared-base misname, gate 42); the real
# RollingRock_Spawn (ov021, id 221's factory) links since wave 5.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN13PeachPainting13InitResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS "RollingRock_Spawn=data_ov010_02112d64")
# R2: three ov062 caller TUs spell a shared-base address by its ov065 name,
# and that name is now Swoop's genuinely different real body (sliced in
# wave 5) -- rename the reference back to the ov062 body they always called.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov062_02118058.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov062_021180d4.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov062_02118a00.cpp"
    PROPERTIES COMPILE_DEFINITIONS "func_ov065_02117994=func_ov062_02117994")
# R2b, run rel0215 (lane cast-ov077): THE SAME DISEASE, from the ov077 side,
# and this one was LIVE ON EVERY LEVEL THAT PLACES A LAKITU rather than latent.
# src/func_ov077_0212390c.cpp is LAKITU's per-frame chase body. Its last
# statement is written `_ZN9Spindrift8BehaviorEv(c, c + 0x204)` -- two
# arguments, which Spindrift::Behavior does not take. The ROM's own relocation
# at 0x02123a04 reads `to:0x02123c6c module:overlay(77)`, and 0x02123c6c is
# func_ov077_02123c6c (size 0xd4), ov077's own WithMeshClsn update, whose
# signature IS (actor, WithMeshClsn*). ov081 has _ZN9Spindrift8BehaviorEv
# (size 0x264) at the SAME address because ov077 and ov081 are alternates in
# the window at 0x02123740, and dsd raced the name.
#
# THE COFFIN/SPINDRIFT SHAPE, and the reason this lane's brief calls the alias
# race its defining hazard: nothing about it is visible to the byte gate (the
# ROM bytes are untouched) or to the linkage count (both names are defined, so
# it links clean). What it does at run time is call ov081's Spindrift body with
# a Lakitu `this` every frame. It faulted c0000005 in
# WithMeshClsn::UpdateContinuous on level 43 at frame ~250 and NOT within 300
# frames on levels 24, 28 or 44 -- which is the point: three of the four levels
# were running the wrong body and reading green.
#
# A per-source rename rather than an /alternatename ON PURPOSE, the R1/R2/R3/R4
# rule: _ZN9Spindrift8BehaviorEv is DEFINED in this link (ov081 is mounted and
# hal/actor_classes_ov081.cpp faces it), and an /alternatename onto a defined
# LHS is defeated silently. Found by sweeping every overlay(77) relocation in
# all 91 ov077 TUs against what each source spells; this is the only one.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov077_0212390c.cpp"
    PROPERTIES COMPILE_DEFINITIONS "_ZN9Spindrift8BehaviorEv=func_ov077_02123c6c")
# R3, run linkw wave 12 (lane w12): THE SAME DISEASE, a fourth time, and this
# one arrived from the OTHER side -- not a newly sliced body under an alias's
# LHS but a newly MOUNTED datum under it. src/func_ov071_0211fbf4.c (Scuttlebug's
# anim load) spells ov071's SharedFilePtr `data_ov073_02122f88` -- a dsd
# shared-window misname; the load site's reloc at 0x0211fc5c targets 0x02122f88
# module:overlay(71). hal/actor_classes_scuttlebug.cpp routed that misspelling
# with /alternatename:_data_ov073_02122f88=_data_ov071_02122f88, and mounting
# ov073 DEFINES the LHS (0x02122f88 is ChiefChilly's tenth state record), which
# defeats the alias silently: the reference would have bound to the state record
# and handed Animation::LoadFile a code address. tools/alternatename_guard.py
# caught it at the first link with ov073 mounted. The dead pragma is deleted
# from that file and the routing lands here, the R1/R2 remedy exactly.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov071_0211fbf4.c"
    PROPERTIES COMPILE_DEFINITIONS "data_ov073_02122f88=data_ov071_02122f88")
# R4, run linkw wave 12 (lane w12): src/func_ov052_0211123c.c (id 174's
# CleanupResources) spells ov052's OWN file table `data_ov056_021124d4` -- ov052
# and ov056 link at overlapping bases and dsd raced the name. Its sibling
# src/func_ov052_02111348.c spells the same address `data_ov052_021124d4`, which
# is what port/ov052_syms.txt mounts, and the load site's relocs at
# 0x02111260/0x02111268 say module:overlay(52). A per-source rename rather than
# an /alternatename ON PURPOSE: ov056 is a live target (id 173, nine instances
# on level 48) and an alias with an ov056 LHS would be defeated the day ov056 is
# mounted, which is the R3 failure this same lane already hit on ov071/ov073.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov052_0211123c.c"
    PROPERTIES COMPILE_DEFINITIONS "data_ov056_021124d4=data_ov052_021124d4")
# R-w3d, run rel0215 (lane w3-d): ov072's two newly seated classes each have
# their vtable spelled TWO ways by their own matched src. SnowmanBody_Spawn.c
# stores `_ZTV11SnowmanBody`, which is what hal/actor_classes_ov072.cpp
# defines as the host array; _ZN11SnowmanBodyD0Ev.c stores the SAME ADDRESS
# (0x021227c0) under dsd's other name for it, `_ZTV12daBgSnmBdy_c`.
# config/arm9/overlays/ov072/symbols.txt carries both names on that one
# address, and the pair repeats for SnowmanHead at 0x021228bc
# (_ZTV11SnowmanHead / _ZTV12daBgSnmHed_c). Left alone the D0s are two
# LNK2019s.
#
# A per-source rename rather than an /alternatename, and NOT because the LHS
# would be defeated -- neither daBgSnm* name is defined anywhere in this link,
# so an alias would in fact hold. It is a -D because this is one TU per name
# reaching for its own class's own table, which is exactly what a per-source
# rename is for, and because it needs no append to the shared
# tools/alternatename_baseline.txt. The R3 defeat mode does not apply here and
# is not being claimed.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN11SnowmanBodyD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "_ZTV12daBgSnmBdy_c=_ZTV11SnowmanBody")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN11SnowmanHeadD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "_ZTV12daBgSnmHed_c=_ZTV11SnowmanHead")
# Same shape once more, ov070's FlyGuy (lane w3-d): FlyGuy_Spawn.c and
# _ZN6FlyGuyD1Ev.c both store `_ZTV6FlyGuy`, which is what
# hal/actor_classes_ov070.cpp defines, while _ZN6FlyGuyD0Ev.c stores the SAME
# ADDRESS (0x02123168) as `_ZTV19daPropeller_Heyho_c` -- symbols.txt carries
# both names on it. One TU, one name, so a per-source rename rather than an
# append to the shared alternatename baseline. As with the two ov072 D0s, the
# LHS is undefined everywhere in this link, so this is not the R3 defeat case.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN6FlyGuyD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "_ZTV19daPropeller_Heyho_c=_ZTV6FlyGuy")

# R5, run linkw wave 15 (lane w15): THREE ov025 TUs spell ov025's OWN vtable
# 0x021139d4 (_ZTV11PyramidLift) as `data_ov027_021139d4` -- ov025, ov027,
# ov032 and ov036 all link at 0x021111a0 and dsd raced the name. The three load
# sites (0x0211213c in the D1, 0x021121b0 in the D0, 0x02112648 in the Spawn)
# all say module:overlay(25). Routed with a PER-SOURCE RENAME rather than an
# /alternatename on purpose: ov027 HAS a real data_ov027_021139d4 of its own in
# its config, and an alias with that LHS would be defeated the day ov027 is
# mounted per symbol -- the ov071/ov073 R3 failure. A -D cannot be defeated.
# The target is the HOST ARRAY, since 0x021139d4 is excluded from the mount.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN11PyramidLiftD0Ev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN11PyramidLiftD1Ev.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/PyramidLift_Spawn.cpp"
    PROPERTIES COMPILE_DEFINITIONS "data_ov027_021139d4=_ZTV11PyramidLift")
# R6, the same shape one address over: src/__sinit_ov025_02112a44.c spells
# ov025's own destructor-chain node 0x02113acc as `data_ov032_02113acc`. The
# load site 0x02112a9c says module:overlay(25), and ov032 has a real
# data_ov032_02113acc with four readers already in src, so the same argument
# applies and this is a -D too.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/__sinit_ov025_02112a44.c"
    PROPERTIES COMPILE_DEFINITIONS "data_ov032_02113acc=data_ov025_02113acc")
# R7, intro-cutscene landing: the intro's cutscene sprite body
# src/func_ov002_020f2340.cpp calls OAM::RenderSub(attr,int,int) by its MSVC
# spelling, and the intro lane's alternatename routed that to the ii ROM body
# it was verified against. The VS lane's width bridge (vs_aliases.inc, the
# WHF3 class) targets the SAME spelling and won by DEFINITION: the iiii TU's
# object exports the HH name, so the alias died at the first merged link
# (alternatename_guard caught it). Rename the intro reference; the alias for
# the renamed spelling lives in hal/actor_classes_intro.cpp. A -D cannot be
# defeated -- the R4/R5 argument. The four ov075 HH callers stay on their
# bridge, each side bound to the body it was proven against.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov002_020f2340.cpp"
    PROPERTIES COMPILE_DEFINITIONS "RenderSub=RenderSub_ii")

# run linkw wave 6, lane w6-D: THE SAME DISEASE A THIRD TIME, and this one
# would have linked SILENTLY on two of its three references.
# src/func_ov100_02147054.c (PATH_LIFT's CleanupResources) releases its three
# SharedFilePtrs through the shared-header ROLE-NAME placeholders G0/G1/G2 --
# and G0/G1 are SINGLE GLOBAL NAMES hal/cxx_aliases.cpp has already bound to
# other objects (_G0 -> _data_020a0eac, the GAME HEAP pointer; _G1 ->
# _data_ov002_0210e05c, SignPost's KCL). Only G2 came back as a link error;
# the other two would have resolved quietly and released the wrong storage --
# exactly the CannonHatch/MetalNet failure cxx_aliases.cpp:1313 describes,
# which was fixed there at the SOURCE. The byte-verified tree is not editable
# from here, so the caller gets the per-source rename instead (the R1/R2
# remedy above).
# The three targets are the ROM's own, read off this body's literal pool in
# release order (0x02147098/9c/a0, each feeding the next
# SharedFilePtr::Release at 0x02147078/80/88):
#   G0 = data_ov002_0210d9f0   G1 = data_ov100_02148a54   G2 = data_ov100_02148a5c
# The rename cannot go straight to the mount names: G1/G2 arrive from
# include/decl_common.h:396-397 as `extern int G1[];` while the same header
# already declares data_ov100_02148a54/5c as scalars at 1280-1281, and MSVC
# rejects the redeclaration (C2040, measured). So the three go to private
# names this lane owns, and hal/actor_classes_ov100pl.cpp binds each onto the
# mounted object with a C-name /alternatename -- one indirection, no header
# edit, and each LHS is undefined everywhere so the alias cannot be defeated.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov100_02147054.c"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_pathlift_file0;G1=port_pathlift_file1;G2=port_pathlift_file2")

# RUN LINKW WAVE C: the same remedy for the three OTHER linked TUs that
# collide on G0/G1/VT. port/tools/alias_audit.py found them; each target below
# is the address that TU's own ROM body loads, read out of
# extracted/overlays/overlay_NNNN.bin, and the derivation with the disassembly
# is in hal/cxx_aliases.cpp beside the five /alternatename rows these point at.
# All three are LIVE wrong-object bugs in the shipping binary, none of which
# fails at link -- the CannonHatch/MetalNet shape.
#
# Coffin's two targets are ov071's bss cells and NOT the identically-addressed
# ov070 ones the config names first: the two overlays share base 0x0211f000
# and the port mounts both as separate host objects, so the wrong pick here is
# the ov043/ov047 mistake rather than a harmless synonym.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN6Coffin16CleanupResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_coffin_file0;G1=port_coffin_file1")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN15BookShotSpawner13InitResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_bookshotspawner_file0;G1=port_bookshotspawner_file1")
# The spawner's deleting destructor stores its OWN table, which in this port is
# the host array hal/actor_classes_ov063.cpp declares. HEAP is left alone: its
# binding to the game heap pointer 0x020a0eac is what this body means.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN15BookShotSpawnerD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT=port_bookshotspawner_vt")

# RUN LINK60 lane A2: SPINDRIFT'S KILL-BY-INVINCIBLE EDGE, misrouted since the
# ov081 seat. One ROM address, 0x020ada40, resolved two ways in one binary.
#
# The port defines both occupants: _func_ov002_020ada40 out of
# src/func_ov002_020ada40.cpp (the real 0x100-byte body, 0x020ada40..0x020adb40
# in ov002's delinks) and __ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player
# out of the ov004 TU. dsd tagged every level-overlay call into that address
# module:overlays(2,4) because it cannot tell the occupants apart, and ov004
# loads only alongside ov006 for a minigame scene while ov081 is a level actor
# overlay co-resident with ov002. TWELVE other TUs in src/ spell the target
# `func_020ada40` -- ov062 x2, ov063, ov064, ov065 x2, ov081's own
# func_ov081_021243cc, ov084, ov090, ov100, ov102 and BobOmb::Behavior -- and
# land on hal/bob_enemy_bridges.cpp's
# /alternatename:_func_020ada40=_func_ov002_020ada40. NINE of those twelve are
# in this link; func_ov081_021243cc, func_ov090_021310b4 and
# func_ov102_0214cbec are not. Nine is the number that matters, because it is
# the count of live callers the routing this rename joins already carries.
# src/func_ov081_02123910.cpp
# is the single outlier: it takes the name from include/decl_Enemy.h:24 and so
# binds to the ov004 body, whose first statement is a null guard on
# data_ov004_020beb68 -- the permanently-zero host global in
# hal/actor_classes_ov081.cpp that nothing in this build ever writes. The kill
# path returned doing nothing, every frame, and neither the byte gate nor the
# linkage count could see it.
#
# The rename spells this one TU's reference the way those twelve already spell
# it, so the existing alias does the routing and no new symbol is added.
# A -D and not an alias of its own: the mangled Enemy name is DEFINED in this
# binary by the ov004 TU, so an /alternatename with it on the left would be
# defeated silently -- the R3 ov071/ov073 failure, already realised here.
#
# Arity, measured rather than assumed. The ROM body reads r0/r1/r2 and never
# reads r3 on entry (it recomputes the slot-0x74 result itself at 0x020adaf0),
# so 0x020ada40 takes THREE parameters, which is what src/func_ov081_021243cc.cpp
# declares and what src/func_ov002_020ada40.cpp defines. The ROM's own call at
# 0x02123988 still loads r3 from the vcall it just made, and the recovered
# .cpp mirrors that, so the caller passes a fourth argument the callee ignores.
# Both sides are cdecl, so the caller cleans it up and the extra argument is
# inert. hal/bob_enemy_bridges.cpp:34 declares four and is a declaration only.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov081_02123910.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player=func_020ada40")

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
# gate 204: ov045 = level 37, Bowser in the Fire Sea (data/stage/koopa2_map),
# course 16 -- the first level the port mounts outside the 1..15 block, and the
# platform overlay the collision family's ExtendingMeshCollider has been waiting
# on (port/slice_w1l4.txt records why the class could not be seated without it).
# Read from the ROM like the rest, and calibrated against all fifteen mounted
# rows before anything about ov045 was believed:
#   data_020758c8[37] = 45, LVL_Overlay data_02092208[37] = 0x021124d4, OV0
#   handles at LVL_Overlay+8 (bmd 0x0782 / kcl 0x077a / icg 0x0783 / icl 0x0784)
#   resolve DIRECTLY through build/assets/handles.tsv -- delta 0, the mapping
#   levels 8..15 use -- to koopa2_map's koopa2_map_all.bmd (30726) /
#   koopa2_map.kcl (33715) / koopa2_map_icg.bin (34352) / koopa2_map_icl.bin.
#   SUBLEVEL_LEVEL_TABLE[37] (0x02075298) is 0x10 = course 16, subCount 1.
# koopa2 is the ROM's own name for the second Bowser stage: levels 35/36 are
# course 15 (koopa1, Dark World), 37/38 course 16, 39 course 17.
#
# A CORRECTION TO THE COMMENTS ABOVE, since a later level had to re-derive it:
# data_020758c8 is a WORD table, stride 4, not the byte table "data_020758c8[N]"
# reads as. Byte-stride gets every one of the fifteen known levels wrong (level
# 4 comes back as ov9); word-stride reproduces all fifteen exactly. Same for
# data_02092208. hal/level_change.cpp's port_level_overlay_id already reads it
# with the *4, so only the prose was loose.
#
# The handles must come from extracted/overlays/overlay_0045.bin -- ov045 is
# compressed:true in overlays.yaml, so extracted/dsd/arm9_overlays/ov045.bin is
# the COMPRESSED image and reading handle halfwords out of it is reading noise,
# the trap the ov023 block above describes from the other direction.
#
# THE SHARED-WINDOW QUESTION, asked before the mount rather than after: ov045's
# footprint is 0x021111a0..0x02113280 (code 8160 + bss 256), and ovdata.py
# --cross drops a target that lands in MORE THAN ONE mounted window. Every
# address in ov045's window already sits inside both ov009's (..0x02113ee0) and
# ov016's (..0x02114ea0), so it is already contested by two mounts and this
# mount cannot flip a single target from resolved to raw. Measured, not
# reasoned: before the mount 127 rebased / 4115 raw / 704 contested; after,
# 133 / 4327 / 705 -- the six new rebased pointers are all ov045's own and
# ZERO pre-existing pointers changed or were lost (patch-body diff, w3-d
# review). The raw total grows because ov045's image adds addresses; do not
# read 4327 as a regression when calibrating the next mount.
list(APPEND PORT_LEVEL_OVERLAYS ov045)
# w5-e: ov046 = level 38, the Bowser in the Fire Sea BATTLE ARENA
# (data/stage/koopa2_boss), course 16 -- the first boss arena the port mounts.
# Read from the ROM like the rest and re-validated against all sixteen mounted
# rows first: data_020758c8[38] = 46 (word table), LVL_Overlay data_02092208[38]
# = 0x02111560, OV0 handles at +8 (bmd 0x0777 / kcl 0x0775 / icg 0x0778 /
# icl 0x0779) resolve DIRECTLY through build/assets/handles.tsv (delta 0) to
# koopa2_boss_all.bmd (4908) / koopa2_boss.kcl (2561) / koopa2_boss_icg.bin
# (17580) / koopa2_boss_icl.bin (512); handles read from
# extracted/overlays/overlay_0046.bin, never the dsd export.
# SUBLEVEL_LEVEL_TABLE[38] = 0x10 = course 16, subCount 1, flags 0. The arena's
# object-overlay set is ov060 + ov089: LoadOrUnloadObjectOverlays
# short-circuits idx 0x24/0x26/0x28 (the three koopaN_boss arenas) to ov060
# with an early return, skipping even the ov098/ov102 tail -- but the
# selector loop runs first, and its row also loads ov089 (KEY 282 /
# LAST_STAR 283, the boss-reward spawns; the KEY-drop wave's hook).
# Mounted --whole; own_sinits 0.
# Shared-window note (the ov045 calibration above): ov046's footprint is
# 0x021111a0..0x021116e0 (code 1344, bss 0, overlays.yaml; compressed:true like
# ov045) and sits entirely inside windows already contested by the sixteen
# prior level mounts, so this mount cannot flip a resolved target raw; the
# cross-pass counts are re-measured in this lane's report.
list(APPEND PORT_LEVEL_OVERLAYS ov046)
# run linkw wave 7 (lane w7-c): the other two koopaN_boss arenas, so all three
# boss arenas are mounted. Every number below was re-derived on this lane from
# extracted/arm9_dec.bin (base 0x02004000) and the raw overlay images, and the
# whole pipeline was re-validated first: all 52 words of data_02092208 dumped
# and checked against the seventeen already-mounted rows, 17/17 exact.
#
# ov044 = LEVEL 36, Bowser in the Dark World arena (data/stage/koopa1_boss),
# course 15. data_020758c8[36] (word at 0x02075958) = 0x2c = 44 -- level+8, and
# the +8 is a READ: all 52 words are contiguous 8..59. LVL_Overlay
# data_02092208[36] (word at 0x02092298) = 0x021113b0. OV0 handles at +8 read
# out of extracted/overlays/overlay_0044.bin (offset 0x210; ov044 is
# compressed:true, so the dsd export is the compressed image and its halfwords
# are noise): bmd 0x076b / kcl 0x0769 / icg 0x076c / icl 0x076d, resolved
# DIRECTLY through build/assets/handles.tsv (delta 0) to koopa1_boss_all.bmd
# (1899, 4411 bytes) / koopa1_boss.kcl (1897, 1624) / koopa1_boss_icg.bin
# (1900, 13728) / koopa1_boss_icl.bin (1901, 512). SUBLEVEL_LEVEL_TABLE[36]
# (byte at 0x020752bc) = 0x0f = course 15; subCount 1, flags 0.
list(APPEND PORT_LEVEL_OVERLAYS ov044)
# ov048 = LEVEL 40, Bowser in the Sky arena (data/stage/koopa3_boss), course 17.
# data_020758c8[40] (word at 0x02075968) = 0x30 = 48. LVL_Overlay
# data_02092208[40] (word at 0x020922a8) = 0x02111624. OV0 handles at +8 out of
# extracted/overlays/overlay_0048.bin (offset 0x484; compressed:true as well):
# bmd 0x0787 / kcl 0x0785 / icg 0x0788 / icl 0x0789 -> koopa3_boss_all.bmd
# (1927, 7228) / koopa3_boss.kcl (1925, 1889) / koopa3_boss_icg.bin (1928,
# 15028) / koopa3_boss_icl.bin (1929, 512). SUBLEVEL_LEVEL_TABLE[40] (byte at
# 0x020752c0) = 0x11 = course 17; subCount 1, flags 0.
#
# Shared-window note (the ov045 calibration two blocks up, asked before the
# mount rather than after): ov044 is 0x021111a0..0x021116a0 (code 1248 + bss 32)
# and ov048 is 0x021111a0..0x02111900 (code 1888, bss 0). Both are strictly
# inside windows the seventeen prior level mounts already contest -- ov009 runs
# to 0x02113ee0 and ov016 to 0x02114ea0 -- so neither mount can flip a target
# from resolved to raw. The cross-pass counts are re-measured in this lane's
# report; the raw total grows because two more images add addresses, which is
# not a regression.
list(APPEND PORT_LEVEL_OVERLAYS ov048)
# run linkw wave 8, lane w8-mounts: every remaining data/stage level the ROM
# has a row for was derived; the sixteen that BOOT are mounted --whole here.
# The full
# derivation -- which read produced which field, the nine already-mounted rows
# the reader reproduced first, the ten level ids that are NOT stages, and each
# level's object-overlay roster out of LoadOrUnloadObjectOverlays -- lives in
# one block beside these overlays' extern declarations in hal/level_boot.cpp.
# Only the two things a CMake reader needs are repeated here.
#
# THE SHARED-WINDOW QUESTION, measured before the mount the way the ov045 and
# ov046 notes above ask for. All sixteen load at 0x021111a0, the same base as
# every level overlay already mounted, and ovdata.py --cross drops a target
# landing
# in more than one mounted window -- so a new mount CAN flip an already-resolved
# pointer to raw and regress a level that already works. Ran the cross pass
# over the old maps and then over old + new: 156 rebased / 4620 raw / 888
# contested across 54 mounts becomes 202 / 5875 / 917 across 70. Every one of
# the 46 new patches belongs to a new overlay, and the set difference the other
# way is EMPTY -- no baseline patch was lost. The raw and contested totals grow
# because the new images add addresses; per the ov045 note, that is not a
# regression to calibrate against.
#
# THAT BASELINE IS THE PRE-ov044 TREE, not this one. The counts above were
# measured against the seventeen-row tree this lane branched from, before the
# ov044/ov048 block directly above landed; the ov044/ov048 lane bounded its own
# two by footprint rather than by these counts. Neither lane measured the
# merged tree, so it was measured on the integration lane instead: 208 rebased
# / 5944 raw / 940 contested across 74 mounts, the +6 over this lane's 202
# being ov022's, with the two koopaN_boss arenas adding no rebased pointer of
# their own. What both lanes established, and what the question is actually
# asking, is that no mount on either side dropped a baseline patch.
#
# ov030 is the one that needed asking: at 19168 bytes it is the LARGEST level
# overlay the port mounts and it extends the shared window to 0x02115ce0, past
# every prior mount's end. That extension is what the measurement above covers,
# and it cost nothing.
#
# Seven more level ids were derived the same way and are NOT here: 16, 21, 25,
# 27, 28, 31 and 33. Each mounts correctly and prints a census; each is blocked
# past the mount by a class this build already registers (FLAME_CHOMP, AMP,
# SNUFIT) or, for 31, by the player's own render path. The level_boot.cpp block
# names each blocker and the SM64DS_SKIP_CLASS run that isolates it, so adding
# their overlays back here is the only CMake work they need.
#
# Levels 36 and 40 (ov044, ov048) are the two koopaN_boss arenas appended just
# above by lane w7-c. This lane derived them independently, agreed, and held
# them for that lane; the two lists union-merged here as intended.
list(APPEND PORT_LEVEL_OVERLAYS
     ov025 ov026 ov027 ov028 ov030 ov031 ov032 ov034 ov043 ov047 ov052 ov053
     ov055 ov056 ov057 ov058)
# run linkw wave 17, lane w17: ov035 is level 27's own overlay
# (data/stage/clock_tower, Tick Tock Clock, course 13) -- the thirty-sixth level
# mount and the first of the seven the wave-8 block marked NOT LANDED to land.
# Read from the ROM like the rest and cross-checked against three EXISTING rows
# (17, 22, 26) before it was written: data_020758c8[27] = 35,
# data_02092208[27] = 0x021120bc, SUBLEVEL_LEVEL_TABLE[27] (0x02075298) = 0x0d
# = course 13, the four handles at LVL_Overlay+8 are bmd 0x070e / kcl 0x070c /
# icg 0x070f / icl 0x0710, subCount 1 and flags 0x00. A small overlay: 0x1ac0
# of code and 0x80 of bss. Mounted --whole; own_sinits stays 0. Its object
# roster is ov065 ov070 ov077 ov084 ov091 ov098 ov102 and only ov077 is
# unmounted, so ov077's classes alone are skipped by name at the gate.
# VS wiring lane: the four VS battle maps the ROM's own list at
# data_ov075_0211c6ec names (levels 51/43/29/42; port/slice_vs.txt section 4).
list(APPEND PORT_LEVEL_OVERLAYS ov059 ov051 ov037 ov050)
list(APPEND PORT_LEVEL_OVERLAYS ov035)
# run linkw wave 21, lane w21: the SIX ids the wave-8 block derived, proved
# mountable and then held back as "NOT LANDED" -- everything that was left of
# the fifty-two after the ten non-stages. Level 27 (ov035, just above) was the
# first of the seven to land; these are the other six.
#
# Every field re-derived from the ROM by this lane rather than copied off that
# block, and the reader proved before it was trusted: it reproduces the
# lvl_overlay of ALL THIRTY-SIX already-mounted rows exactly, and rows 8, 9,
# 17, 22, 26 and 27 field for field -- overlay id, LVL_Overlay address, course,
# subCount, flags and all four handles. The object-overlay reader reproduces
# level 3's documented roster (ov063 ov085 ov089 ov098 ov100 ov102) and the
# ov060+ov089 pair for levels 36/38/40, which is the short-circuit trap.
#
#   lvl  ov     LVL_Overlay   crs sub flg  stage           bmd /kcl /icg /icl
#    16  ov024  0x021129b4   7  1  0x00  desert_land      0713/0711/0714/0715
#    21  ov029  0x02112fd0  10  2  0x00  water_city       07f0/07ed/07f1/07f2
#    25  ov033  0x02111be0  12  1  0x00  tibi_deka_t      07ea/07e8/07eb/07ec
#    28  ov036  0x02112a6c  14  1  0x01  rainbow_cruise   07ab/07a9/07ac/07ad
#    31  ov039  0x02111438  23  1  0x00  habatake         074d/074b/074e/074f
#    33  ov041  0x0211192c  25  1  0x00  metal_switch     07a1/079f/07a2/07a3
#
# All four handles of all six resolve through build/assets/handles.tsv at
# DELTA 0 to one data/stage directory each -- all.bmd, .kcl, _icg.bin, _icl.bin
# -- which is the test the ten non-stage ids fail.
#
# What was blocking them was never the mount. The wave-8 block measured each
# one faulting past a correct mount inside a class this build ALREADY
# registers: FLAME_CHOMP (270) on 16/21/25/28, AMP (266) on 28, SNUFIT (236) on
# 33. Those classes were fixed on the tip between that wave and this one (the
# pointer-to-member width sweep and the ModelAnim slot-5 collision), which is
# what freed these six -- no mount field changed.
#
# Object rosters, with the packs this build does not mount called out; those
# are the worklist the cast lanes take from here:
#    16 ov062 ov070 ov080 ov084 ov092 ov096 ov098 ov102  [unmounted ov092 ov096]
#    21 ov062 ov070 ov077 ov084 ov090 ov098 ov102        [unmounted ov077 ov090]
#    25 ov062 ov070 ov084 ov094 ov098 ov100 ov102        [all mounted]
#    28 ov062 ov070 ov077 ov084 ov091 ov095 ov098 ov102  [unmounted ov077]
#    31 ov084 ov098 ov102                                [all mounted]
#    33 ov065 ov084 ov098 ov102                          [all mounted]
#
# ov030 (19168 bytes) is still the largest level overlay and still sets the
# shared window's end; the largest here is ov029 at 12416, well inside it, so
# none of these six extends the window that ovdata.py --cross contests over.
# Mounted --whole like the rest; own_sinits stays 0 for all six.
list(APPEND PORT_LEVEL_OVERLAYS ov024 ov029 ov033 ov036 ov039 ov041)
# run linkw wave C, lane cat-levels: FIVE MORE, out of the ten ids the wave-8
# block declined as "not stages". They are stages. See the block in
# hal/level_boot.cpp for the measurement that says so and for what the wave-8
# reading actually caught; the short form is that only handle 0 reads as MG/
# minigame art, and only one declined id has a handle equal to 0.
#
#   lvl  ov     LVL_Overlay   crs sub flg  stage           bmd /kcl /icg /icl
#     0  ov008  0x02111210  255  1  0x01  test_map         07d7/07d5/07d8/07d9
#    30  ov038  0x02111218   22  1  0x00  suisou           07c9/07c7/07ca/07cb
#    32  ov040  0x02111228   24  1  0x01  horisoko         075c/075a/075d/075e
#    34  ov042  0x02111218   26  1  0x00  rainbow_mario    07b0/07ae/07b1/07b2
#    46  ov054  0x021112b0   19  5  0x00  ex_l_map         0729/0723/072a/072b
#
# Every handle resolves through build/assets/handles.tsv at DELTA 0 to one
# data/stage directory holding the full four-file set -- <name>_all.bmd,
# <name>.kcl, <name>_icg.bin, <name>_icl.bin -- with real sizes, which is the
# same test the mounted rows pass and the test the five genuinely declined ids
# (29, 41, 42, 43, 51) fail.
#
# THE SHARED WINDOW COSTS NOTHING HERE, by the footprint argument the ov044 and
# ov048 notes use rather than by a re-run of the cross pass. All five load at
# the shared 0x021111a0 level-overlay base and the largest is ov054 at 3008
# bytes, so every one of them is strictly inside the window ov030 (19168 bytes,
# level 22, already mounted) already contests. An overlay that adds no new
# ground cannot take a resolved target away from another mount.
#
# Mounted --whole like the rest; own_sinits stays 0 for all five.
list(APPEND PORT_LEVEL_OVERLAYS ov008 ov038 ov040 ov042 ov054)
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
# host arrays hal/actor_classes_ov072.cpp fills.
#
# LANE w3-d EXTENDED THIS GATE TO THE WHOLE OVERLAY. SnowmanHead (273) and
# SnowmanBody (274) used to stay unregistered with their sinits off; both are
# hosted now, so ALL FOUR of ov072's vtables are host arrays and all four of
# its class sinits run. The two new classes are plain 31-slot Actors whose
# shared slots are byte-identical to the two above, so ov72_fill_shared serves
# all four. Their four pointer-to-member dispatchers are host copies
# (unmatched/Snowman_StateDispatch.cpp) for the BabyPenguin_StateEnter reason,
# and their state cells are seated with host addresses. One method face
# (unmatched/Ov072_MethodFaces.cpp) closes Player::StartTalk, which two of
# the new state handlers call as a C++ method and no linked TU spells that
# way -- the Coffin closure addendum's shape.
# See port/ov072_syms.txt and port/slice_gate193.txt.
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
# gate 202: ov013 (level 5's own overlay, already whole-mounted per gate 117)
# joins the per-symbol mounts too -- CLOCK_HAND_SHORT (292), CLOCK_HAND_LONG
# (293) and CLOCK_PENDULUM (294) reach their SpawnInfo records, their
# SharedFilePtrs and the hand-step table by name, the ov012/gate-199 shape.
# Their two vtables (data_ov013_02112128 for the pendulum, unnamed and
# five-way split by dsd; _ZTV22ClockPaintingHandShort == _ZTV12daObjClock_c
# shared by BOTH hands) are host arrays hal/actor_classes_ov013.cpp fills.
# See port/ov013_syms.txt and port/slice_gate202.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov013)
# run linkw wave 4 (lane w4-b): ov045 (level 37's own overlay, already
# whole-mounted per gate 204) joins the per-symbol mounts too, so Bowser in the
# Fire Sea's six-class platform cast -- FALL_BLOCK_BFS (139), FIRE_SEA_ELEVATOR
# (140), TILTING_PLATFORM_BFS (141), POLE_LIFT (142), FLOATING_FLOOR_BFS (145)
# and EXTENDING_PLATFORM (148) -- reaches its SpawnInfo records, SharedFilePtrs,
# CLPS blocks and file tables by name. Same dual-mount shape as ov012/ov013.
# Its six vtables are host arrays hal/actor_classes_ov045.cpp fills. This mount
# runs port_ov045_pack_check/port_ov045_syms_patch and the five linkable ov045
# sinits from the registry fill rather than from hal/actor_overlays.cpp; see the
# LANE OWNERSHIP block in hal/actor_classes_ov045.cpp for why and for the
# actor_overlays.cpp block that should replace it. See port/ov045_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov045)
# run linkw wave 5 (lane w5-e): ov060, the Bowser-fight actor pack -- the
# overlay all three koopaN_boss arenas load (alongside ov089, which the
# selector row loads BEFORE LoadOrUnloadObjectOverlays' idx-0x24/0x26/0x28
# early return: KEY 282 / LAST_STAR 283, the boss-reward spawns). First
# mount, per symbol; the full seven-class cast map, the one dsd label shift
# and the width pins are in port/ov060_syms.txt. Wave 5 registered ONE
# class (BOWSER_FIRE_SEA_ARENA 166, level 38's floor) and left the pack's 105
# TUs and six sinits in the link but /OPT:REF-stripped; wave 6 (lane w6-A)
# added the other five rows -- SpikeBomb 284, Bowser 279, BowserTail 278,
# BowserFire 280, BowserSkyPlatform 167 -- plus the seat of the six state
# tables they dispatch. port/slice_w5e.txt + port/slice_w6a.txt. See
# hal/actor_classes_ov060.cpp.
list(APPEND PORT_ACTOR_OVERLAYS ov060)
# w5-e: ov046 (level 38's own overlay, whole-mounted this same lane) joins
# the per-symbol mounts with ONE symbol -- the arena platform's private CLPS
# block, which BowserFireSeaArena::InitResources reaches by name under a
# shared-window spelling. See port/ov046_syms.txt's header.
list(APPEND PORT_ACTOR_OVERLAYS ov046)
# run linkw wave 7 (lane w7-c): ov044 (level 36's own overlay, whole-mounted
# this same lane) joins the per-symbol mounts too, because the arena's ONE
# non-ov060 class -- ORANGE_BALL_BILLBOARD / daObjKb1Billboard_c (301), the
# sixteen orange balls ringing the arena -- lives in the LEVEL overlay and
# reaches its SpawnInfo and its two bss cells by name. Same dual-mount shape as
# ov012/ov013/ov021/ov045/ov046. Its one vtable (0x02111604, 31 slots, pinned by
# both the reloc run and the .data/.bss boundary) is a host array
# hal/actor_classes_ov044.cpp fills; the mount bring-up and the single sinit run
# from the registry fill (the ov045/ov060 lane-ownership pattern -- no lane owns
# hal/actor_overlays.cpp this wave). See port/ov044_syms.txt and
# port/slice_w7c.txt. ov048 (level 40) gets NO per-symbol half: its whole cast
# is ov060's and nothing in ov048 is reached by name.
list(APPEND PORT_ACTOR_OVERLAYS ov044)
# run linkw wave 5 (lane w5-c): ov070, the Amp/FlameChomp/FlameChompFire pack
# (level 37's roaming enemies; FlyGuy rides the mount unregistered). Fresh
# per-symbol mount, ids re-read from the raw overlay image; the four vtables
# are host arrays hal/actor_classes_ov070.cpp fills, and the state PMF source
# records are seated with host bodies before the sinits copy them (the
# Scuttlebug treatment; see port/ov070_syms.txt). The bring-up runs from the
# first registry fill (ov70_bringup) -- the same actor_overlays.cpp handoff
# hal/actor_classes_ov045.cpp records.
list(APPEND PORT_ACTOR_OVERLAYS ov070)
# run linkw wave 5 (lane w5-B): ov065, first mount -- Hazy Maze Cave's enemy
# overlay (Snufit 236, Swoop 237, Dorrie 168, DorrieCap 169; the ten Ttc*
# classes ride the mount as data but stay unregistered, the parked subset).
# Its four HMC vtables are host arrays hal/actor_classes_ov065.cpp fills; the
# five Ttc vtables are excluded and hosted nowhere (the ov045 id-141
# precedent). Bring-up rides the registry fill (ov65_bringup), the w4-b
# pattern -- no lane owns hal/actor_overlays.cpp this wave. See
# port/ov065_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov065)
# run linkw wave 18 (lane w18): ov035 is level 27's own overlay (Tick Tock
# Clock, data/stage/clock_tower), already whole-mounted in PORT_LEVEL_OVERLAYS
# since wave 17 landed the level-27 row. It joins the per-symbol mounts here so
# the ov065 Ttc cluster's TWELVE cross-overlay reads -- ten CLPS collision
# blocks and two animation descriptors -- resolve onto host storage instead of
# staying DS addresses. Without this, ovdata.py's cross pass has nothing to
# rebase into (a whole-image mount deliberately provides the cross pass
# nothing), and wave 17 MEASURED what that costs: ids 108 and 114 ticked 300
# frames without faulting on a garbage CLPS, silently, and id 112 divided by
# zero on an animation descriptor that read zero frames. The quiet pair is why
# that seat came back out. The gate-17 dual-mount shape of ov009/ov010/ov012/
# ov013/ov014/ov015/ov016/ov018/ov019/ov020/ov021/ov022/ov031/ov045 applied to a
# fifteenth overlay and a sixth level overlay; ov022 is the worked example.
# Nothing here is excluded -- ov035 contributes no vtables to this mount, all 21
# blocks are plain data. See port/ov035_syms.txt for the twelve seeds (wave 17's
# note names five of them), the closure, and the three width routes of which two
# lie.
list(APPEND PORT_ACTOR_OVERLAYS ov035)
# ...and ov021 (level 13's own overlay, already whole-mounted per gate 156)
# joins the per-symbol mounts too, so WORK_ELEVATOR (68), ROLLING_ROCK (221)
# and SHUTTER_HMC (67) reach their SpawnInfo records, CLPS blocks, tuning
# tables and sinit-built SharedFilePtrs by name -- the ov010/ov019/ov045
# dual-mount shape. Its three vtables are host arrays the same file fills.
# See port/ov021_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov021)
# run linkw wave 5 (lane w5-A): ov063, Big Boo's Haunt's ACTOR overlay, first
# mount -- Boo/BigBoo/BigBooIcon/BooCage, the four daTrsTrap_c furniture ids,
# FallBlockBbh and MadPiano reach their SpawnInfo records, tuning tables and
# SharedFilePtr statics by name (the ov079/ov080/ov081 shape; ov063 is not a
# level overlay, so there is no whole-image half). Its six vtables are host
# arrays hal/actor_classes_ov063.cpp fills; the mount bring-up and the four
# sinits run from the registry fill (the ov045 lane-ownership pattern). See
# port/ov063_syms.txt and port/slice_w5a.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov063)
# run linkw wave 8 (lane w8-packs): ov022 is level 14's own overlay (Lethal
# Lava Land, data/stage/fire_land), already mounted whole in
# PORT_LEVEL_OVERLAYS since gate 163; it joins the per-symbol mounts here so
# its platform cast reaches its SpawnInfo records, its SharedFilePtr statics
# and the three-word file/CLPS tables its InitResources bodies index -- the
# gate-17 shape of ov009/ov010/ov015/ov016/ov018/ov019/ov020/ov021/ov045
# applied to a thirteenth overlay and a fifth level overlay. Its ten vtables
# are excluded from the mount; the four this wave seats are host arrays
# hal/actor_classes_ov022.cpp fills, and the mount bring-up plus four of the
# ten ov022 sinits run from the registry fill (the ov045 lane-ownership
# pattern). See port/ov022_syms.txt and port/slice_w8packs.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov022)
# run linkw wave 10 (lane w10-shared): ov031 (level 23, Tall Tall Mountain's
# slide, already whole-mounted per the w8-mounts sweep) joins the per-symbol
# mounts, so the SLIDE DECORATION QUAD reaches its four SpawnInfo records, its
# four-entry SharedFilePtr table and that table's bss by name. Same dual-mount
# shape as ov010/ov012/ov013/ov015/ov016/ov018/ov019/ov020/ov044/ov045/ov046.
# ONE vtable is excluded from the mount and hosted as an array
# hal/actor_classes_ov031_w10.cpp fills; that file also runs the mount bring-up
# and the overlay's single sinit from the registry fill (the ov045 lane-
# ownership pattern). See port/ov031_syms.txt and port/slice_w10a.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov031)
# run linkw wave 12 (lane w12): ov073, level 49's OBJECT overlay -- the first
# mount of this overlay, per symbol. Level 49 (ex_wario, "Wario's key arena")
# was mounted and booting with four actors placed and eighteen skipped: every
# actor the level places is ov073's, and hal/level_boot.cpp's row for 49 carried
# the note "[unmounted ov073]". CCC_ARENA (170), CCC_BIG_ICE (171) and
# CCC_SMALL_ICE (172) are ONE class under three ids sharing table 0x021231e8;
# CHIEF_CHILLY (218) is the boss on 0x02123090. Both vtables are excluded from
# the mount and hosted as arrays hal/actor_classes_ov073.cpp fills; that file
# also runs the mount bring-up, the forty state records' seat and BOTH sinits
# from the registry fill (the ov045 lane-ownership pattern), in that order,
# because the sinits COPY the records the seat rewrites. The ov052-shaped width
# trap this overlay carries -- ChiefChilly's reloc run reading one word past a
# 31-slot table into CccArena's own state table -- is derived in
# port/ov073_syms.txt. See also port/slice_w12.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov073)
# run linkw wave 12 (lane w12): ov052, LEVEL 44's own overlay ("Mario's key
# course", ex_m_map) -- already whole-mounted since level 44 landed, joining the
# per-symbol mounts too so its two platform classes reach their SpawnInfo
# records, their file tables and their sinit-built SharedFilePtrs by name. Same
# dual-mount shape as ov012/ov013/ov045. BOBBING_LOG_EMM (174) and
# SQUARE_PATH_LIFT (175) are sixteen of level 44's seventeen skipped instances.
# Both vtables are excluded from the mount and hosted as arrays
# hal/actor_classes_ov052.cpp fills; that file also runs the mount bring-up and
# both sinits from the registry fill (the ov045 lane-ownership pattern). The
# width trap this overlay carries -- id 174's reloc run reading SIX words past a
# 32-slot table, straight through id 175's own file table and typeinfo -- is
# derived in port/ov052_syms.txt. See also port/slice_w12b.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov052)
# run linkw wave 15 (lane w15): ov056, LEVEL 48's own overlay ("Wario's key
# course", ex_w_map, course 20) -- already whole-mounted since lane w8-mounts,
# joining the per-symbol mounts too so BIG_MOVING_ICE_BLOCK (173) reaches its
# SpawnInfo record, its file table and its sinit-built SharedFilePtrs by name.
# Same dual-mount shape as ov012/ov013/ov045/ov052. Nine of level 48's fourteen
# skipped instances are this class; the other five are MR_BLIZZARD, refused on
# purpose. Its vtable is excluded from the mount and hosted as an array
# hal/actor_classes_ov056.cpp fills; that file also runs the mount bring-up,
# hal_fill_platform_vtable and the overlay's single sinit from the registry
# fill (the ov045 lane-ownership pattern). The width trap this overlay carries
# -- a dsd split at slot 16 making the next-symbol landing read a 32-slot table
# as SIXTEEN -- is derived in port/ov056_syms.txt. See also port/slice_w15.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov056)
# run linkw wave 15 (lane w15): ov025, LEVEL 17's own overlay -- already
# whole-mounted since lane w8-mounts, joining the per-symbol mounts too so
# PYRAMID_STEP (87) and PYRAMID_LIFT (88) reach their SpawnInfo records, their
# CLPS blocks and their sinit-built SharedFilePtrs by name. Five of level 17's
# eleven skipped instances. Both vtables are excluded from the mount and hosted
# as arrays hal/actor_classes_ov025.cpp fills; that file also runs the mount
# bring-up, hal_fill_platform_vtable and the TWO of the overlay's four sinits
# these classes own (the other two belong to classes with no reach and stay
# unrun, the ov019 rule). TWO width traps are derived in port/ov025_syms.txt:
# dsd split BOTH 32-slot tables at slot 8, and it also split a SharedFilePtr in
# half so an unpinned mount would have sized it FOUR bytes. See also
# port/slice_w15c.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov025)
# run link60 lane A2, gate 206: ov043 (level 35's own overlay, Bowser in the
# Dark World, already whole-mounted in PORT_LEVEL_OVERLAYS) joins the
# per-symbol mounts so DIAMOND_LIFT (135, daObjKm1_Ukishima_c, six instances)
# reaches its SpawnInfo, its two bss SharedFilePtrs and its CLPS block by name.
# Same dual-mount shape as ov012/ov013/ov021/ov044/ov045/ov046. 105 symbols;
# all four vtable spans are excluded and id 135's is the host array
# hal/actor_classes_ov043.cpp fills. Ids 134 and 137 stay skipped on purpose --
# 137's factory hides its real table behind a placeholder AND spells an
# invented class name that means another overlay's base table, and both need
# ov002 base bodies this slice does not pull in. The cast map, the four widths
# and the naming shift are derived in port/ov043_syms.txt; this gate's reading
# and the four re-verified traps are in port/slice_gate206.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov043)

# run link60 lane s2-m7: ov007, the TITLE SCREEN and FILE SELECT scene
# (dScDSMT_c). Not an actor overlay and not reachable today -- it is the fourth
# occupant of the scene slot at 0x020ad660, walk_window runs the ov002 occupant,
# and nothing in a level walk spawns actor id 1. It joins the per-symbol mounts
# for the same reason ov001 and ov003 did: the treatment is right for it and the
# storage has to exist before anything can be wired to read it.
#
# THIS ADDS NO LINKAGE and is not meant to. Mounted bytes are not matched TUs,
# none of ov007's 527 source TUs is compiled in, and nothing calls
# port_ov007_syms_patch(). What lands is 716 symbols, the overlay's whole
# .rodata + .data + .bss contiguous from 0x020ccb54 to 0x02104c40, including the
# TITLE_SCREEN spawninfo record at 0x02103264 that the S1 spine lane's registry
# row will read. ov007 has no sinits to run: its static-init table is one NULL
# word. See port/ov007_syms.txt for the list and the eight ROM width pins.
#
# It is the first mount of a scene-slot occupant, so it is the first real test
# of the residency model merged at 11242964f -- an address-only contest test
# would have taken three of the build's resolved cross pointers back to raw DS
# addresses. It takes none: 228 rebased rows before, the identical 228 after,
# contested unmoved at 105. The measurement is port/ov007_binding_diff.txt,
# including the three findings it turned up and which of them are fixed.
#
# It also grows .dsstate by 229612 bytes, which MOVES THE SELFTEST BMP BASE.
# That is the landed span: 229604 is the sum of the 716 symbol sizes and the
# other 8 are the pack gap at 0x020d7db8. The three water pixels track the
# section base, so the BMP is comparable only at an equal base and must not be
# byte-compared across this commit.
list(APPEND PORT_ACTOR_OVERLAYS ov007)
# run link60 lane s2-m46: ov004 AND ov006, the MINIGAME SCENE PAIR, mounted
# together because the loader loads them together. func_0201a798 loads ov004
# first and then ov006, func_0201a754 unloads both, so ov004 is live exactly
# when ov006 is -- the PASSENGER RULE, and the only pair anywhere in the game
# for which the residency model answers coresident() = True between two scene
# slot occupants. ov006 is the twenty-three Rec Room minigames (1839 matched
# TUs, the largest family left); ov004 is dScMgBase_c, the framework class they
# derive from, and it is never loaded on its own.
#
# THIS ADDS NO LINKAGE and is not meant to. Mounted bytes are not matched TUs,
# none of the 1839 + 276 matched source TUs is compiled in, and nothing calls
# either patch function. No level can reach a minigame either: func_0201a694
# unloads the scene overlay the level is running out of before it loads ov006,
# which is a proof from the load code, so reach needs the scene-switch spine and
# that is not wired here. What lands is storage.
#
# 3524 symbols. ov004 is 597, contiguous 0x020b944c..0x020bfec0; ov006 is 2927,
# contiguous 0x0212b890..0x021430a0. Each carries ONE pack gap, over its own
# .init and .ctor, and unlike ov007's eight zero bytes those gaps cover ROM code
# that is not zero -- the same class as 220 of the 245 pack gaps already here,
# and the largest members of it. Twenty sizes are pinned to their ROM widths.
# The 35 .ctor words are left out (dsd's `.p` names are not C identifiers) and
# the 35 sinits behind them are inventoried in the two list headers, not run.
#
# THE PAIR IS THE PERMISSIVE DIRECTION'S FIRST EXERCISE and it comes out the
# way the model says. 229 rebased rows before, 284 after: nothing lost, nothing
# changed, and 55 rows APPEARED, every one of them a pointer between the two
# overlays that only resolves because they are co-resident. Contested does not
# move at all, 105 either side, because the two footprints abut at 0x020bfec0
# and never overlap. Under the address-only test the residency model replaced,
# these two windows would have taken 195 of the 229 back to raw DS addresses.
# The measurement, with every appeared row adjudicated, is
# port/ov004_ov006_binding_diff.txt.
#
# FOUR HAND-HOSTED STAND-INS RETIRE WITH IT, and they have to go in this same
# commit or the link has two definitions of each: data_ov004_020beb60 out of
# hal/oam2d_ov004_bss.cpp (the file existed for that one word and is deleted),
# data_ov004_020beb68 out of hal/actor_classes_ov081.cpp, and
# data_ov006_02140330 / data_ov006_02140338 out of hal/cxx_aliases.cpp. All four
# are .bss, all four are the ROM span the mount gives them (4, 4, 8 and 8
# bytes), and all four are zero on both sides, so the swap is byte-identical and
# St_LevelEnter_Main's Release still reads numRefs 0. The ov006 scout predicted
# "ZERO duplicates, no host stand-ins to retire" for this territory; there were
# four, and the correction is recorded in port/ov004_ov006_binding_diff.txt.
#
# It grows .dsstate by 123492 bytes, MEASURED, which MOVES THE SELFTEST BMP
# BASE: dsstate_guard reports 810684 bytes captured before and 934176 after, and
# the base moves 0xae2000 to 0xafa000. It reconciles as the landed span of the
# two mounts less what the retired stand-ins give back less what section
# alignment reclaims: 27252 + 96272 - 24 - 8 = 123492. The symbol sums, 24832
# and 79572, are not the number that moves the section. The three water pixels track the section
# base, so the BMP is comparable only at an equal base and must not be
# byte-compared across this commit.
list(APPEND PORT_ACTOR_OVERLAYS ov004 ov006)
# run mg15 lane MENU: ov005, the MINIGAME SELECTION MENU's data. The scene the
# pair above is reached FROM -- dScMiniGm_c, actor id 5 -- and the SECOND
# occupant of the 0x020bfec0 scene slot to be mounted, ov006 being the first.
#
# THE TWO CAN NEVER BE LOADED TOGETHER ON THE DS and the residency model
# already knows it without anything being asserted here: both footprints start
# at 0x020bfec0, so rule 1 (overlap, pure arithmetic) makes them exclusive, and
# rule 2 independently puts both in the scene-occupant set because
# src/GetSceneOverlayID.c returns overlay_5 and overlay_6 from its own arms.
# That is what stops the cross pass resolving an ov005 target into ov006's
# window: the two contest, and a contested target stays raw rather than being
# guessed at. Nothing in the mount below crosses into ov006 anyway.
#
# 51 symbols, the overlay's whole .rodata + .data contiguous from 0x020c2250 to
# 0x020c3020. .bss is start:0x020c3020 end:0x020c3020 -- empty -- so there is
# nothing to mount there, and .ctor is zero length, so there are no sinits to
# run (the ov003 and ov007 shape, not the ov004/ov006 one). No size is pinned:
# every delta between consecutive symbols is >= 4, so ovdata's default IS the
# ROM width in all 51 cases.
#
# UNLIKE ov003/ov004/ov006/ov007 THIS MOUNT IS WIRED THE SAME COMMIT IT LANDS.
# hal/scene_mg_menu.cpp fills data_ov005_020c2490 (the class vtable, 18 words)
# and hal/scene_boot.cpp's registry row reads data_ov005_020c2440 (the
# SpawnInfo), so the storage and its readers arrive together rather than the
# mount sitting ahead of a wiring lane.
#
# THE MOUNT'S OWN CROSS-CHECK, which is why this is not a claim on trust: the
# 36-row launch table at data_ov005_020c24d8 (36 x 0x34 = 1872 bytes) that this
# mount emits is BYTE-IDENTICAL to the verbatim transcription
# hal/scene_mg_sound.cpp has carried since run mg8. Two independent readings of
# the same ROM bytes at the same address agree exactly, which is a stronger
# statement about the mount's addressing than any run-time probe.
#
# IT GROWS .dsstate BY 3544 BYTES, MEASURED, and the symbol count by exactly
# 54 with NOTHING LOST: dsstate_guard reports 959440 bytes captured before and
# 962984 after, and the base moves 0xc8e000 to 0xc92000. The 54 reconcile
# completely -- the mount's 51 symbols, the one pack gap pk005_gap_020c2430
# (the 16 bytes between .rodata's end at 0x020c2430 and .data's start at
# 0x020c2440, which are in the ROM image too), and the two arm9 .bss words
# hal/auto_bss.cpp gained for this scene. A map diff of the two captured spans
# lists those 54 as NEW and zero as GONE.
#
# The three water pixels track the section base, so the selftest BMP is
# comparable only at an equal base and must not be byte-compared across this
# commit.
list(APPEND PORT_ACTOR_OVERLAYS ov005)
# ov075 joins for the VS wiring lane (dScEntry_c, scene 6); the slice block
# and the derivation pointer live beside SLICE_VS_SOURCES further down.
list(APPEND PORT_ACTOR_OVERLAYS ov075)
# run rel0215 wave 1 (lane cast-ov026): ov026 (level 18's own overlay, Dire
# Dire Docks, already whole-mounted in PORT_LEVEL_OVERLAYS) joins the
# per-symbol mounts too, so the level's WHOLE five-class cast --
# daObjWlPolelift_c (89), Submarine (90), BowserShutter (91), Whirlpool (246)
# and WaterSuction (247) -- reaches its SpawnInfo records, SharedFilePtr
# construction statics, CLPS/BTA/BMD blocks and its two pointer-to-member state
# tables by name. The gate-17 dual-mount shape of ov009/ov012/ov013/ov021/
# ov035/ov045 applied to a further level overlay; --whole emits
# port_ov026_image/_patch/_at and nothing by name, --from-list --pack emits the
# named arrays plus port_ov026_syms_patch.
# All five vtables are excluded from the mount and are host arrays
# hal/actor_classes_ov026.cpp fills: _ZTV17daObjWlPolelift_c (31),
# _ZTV21daObjWlKoopaShutter_c (32), _ZTV18daObjWlSubmarine_c (32),
# _ZTV18daWater_Tatumaki_c (31) and _ZTV12WaterSuction (31). Two of those five
# widths are OVER-READ by the typeinfo-terminator route because a
# pointer-to-member source table is packed behind the vtable; those pairs
# belong to the NEXT class in each case, and both the widths and that
# correction are ROM-byte verified in port/ov026_syms.txt.
# DSD CLASS-IDENTITY SWAP: every `_ZN9Submarine*` TU in this overlay is the
# WHIRLPOOL's, not the Submarine's -- config/arm9/overlays/ov026/symbols.txt:201
# puts _ZTV9Submarine on 0x02113d54, whose own typeinfo name string reads
# 18daWater_Tatumaki_c. See port/ov026_syms.txt and port/slice_ov026cast.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov026)
# run rel0215 wave 1 (lane cast-sweep1): ov058 (level 50's own overlay, the Rec
# Room, already whole-mounted in PORT_LEVEL_OVERLAYS) joins the per-symbol
# mounts for ONE SYMBOL -- REC_ROOM_CUPBOARD's (182) SpawnInfo record. It is
# the smallest per-symbol mount in the port and it is one line because ov058
# has no .rodata, no .init, no .ctor and no .bss: zero static initialisers, and
# the class loads no file of its own. It is also the WHOLE of level 50's skip
# list, so this seat takes that level to zero skipped. Its vtable is excluded
# from the mount and hosted as an array hal/actor_classes_ov058.cpp fills, and
# it is a 31-slot ACTOR table, not a 32-slot Platform one -- the ov024
# PYRAMID_TAG shape; dsd's landing on it reads TWO. That file also runs the
# mount bring-up from the registry fill (the ov025/ov045 lane-ownership
# pattern), with no sinit and no hal_fill_platform_vtable to run.
# See port/ov058_syms.txt and port/slice_sweep1_ov058.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov058)
# run rel0215 wave 1 (lane cast-sweep1): ov017 (level 9's own overlay, Jolly
# Roger Bay's sunken ship, already whole-mounted in PORT_LEVEL_OVERLAYS) joins
# the per-symbol mounts so SHIP_WATER (62) reaches its SpawnInfo record, its
# BTA file descriptor, its CLPS block and both sinit-built SharedFilePtrs by
# name. This is the mount hal/level_boot.cpp's own level-9 block asked for by
# name and deferred: "hosting it needs a per-symbol ov017 mount
# (ShipWater_SpawnInfo, _ZTV9ShipWater, data_ov017_02111c88) PLUS an
# alias-by-address that resolves the ov055/ov056 spellings onto ov017's host
# bytes". Its vtable is excluded from the mount and hosted as an array
# hal/actor_classes_ov017.cpp fills; that file also runs the mount bring-up,
# hal_fill_platform_vtable and the overlay's single sinit from the registry
# fill (the ov025/ov045 lane-ownership pattern). The block named two raced
# spellings; there are three, and the third is LIVE on this tip. See
# port/ov017_syms.txt and port/slice_sweep1_ov017.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov017)
# run rel0215 wave 1 (lane cast-sweep1): ov023 (level 15's own overlay, Lethal
# Lava Land's volcano interior, already whole-mounted in PORT_LEVEL_OVERLAYS)
# joins the per-symbol mounts so SQUASHER (84) reaches its SpawnInfo record and
# both sinit-built SharedFilePtrs by name. It is the WHOLE of level 15's skip
# list -- one instance, one class -- so this seat takes that level to zero
# skipped. Its vtable is excluded from the mount and hosted as an array
# hal/actor_classes_ov023.cpp fills; dsd's landing on it OVER-reads by seven
# slots (39 against a real 32), the opposite error to ov024's and ov033's.
# That file also runs the mount bring-up, hal_fill_platform_vtable and the
# overlay's single sinit from the registry fill (the ov025/ov045 pattern).
# The three alias races its InitResources and destructors carry, all pinned to
# ROM relocations, are in port/ov023_syms.txt. See port/slice_sweep1_ov023.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov023)
# run rel0215 wave 1 (lane cast-sweep1): ov033 (level 25's own overlay, Tiny-
# Huge Island's small side, already whole-mounted in PORT_LEVEL_OVERLAYS) joins
# the per-symbol mounts so TINY_COVER (104) and TINY_WATER (105) reach their
# SpawnInfo records, their two CLPS blocks, id 105's BTA file descriptor and
# all four sinit-built SharedFilePtrs by name. Two of level 25's three skipped
# instances. Both vtables are excluded from the mount and hosted as arrays
# hal/actor_classes_ov033.cpp fills -- and dsd's ONE vtable name here is on the
# WRONG table (_ZTV9TinyCover is the WATER class's; the cover's is unnamed),
# proven three ways in port/ov033_syms.txt. That file also runs the mount
# bring-up, hal_fill_platform_vtable and both sinits from the registry fill
# (the ov025/ov045 lane-ownership pattern). See port/slice_sweep1_ov033.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov033)
# run rel0215 wave 1 (lane cast-sweep1): ov024 (level 16's own overlay,
# Shifting Sand Land, already whole-mounted in PORT_LEVEL_OVERLAYS) joins the
# per-symbol mounts so PYRAMID_TOP (85) and PYRAMID_TAG (86) reach their
# SpawnInfo records, id 85's CLPS block and both sinit-built SharedFilePtrs by
# name. Same dual-mount shape as ov012/ov013/ov025/ov035/ov045/ov052/ov056.
# Five of level 16's nineteen skipped instances. Both vtables are excluded from
# the mount and hosted as arrays hal/actor_classes_ov024.cpp fills -- and they
# are DIFFERENT LENGTHS, 31 for the Actor-derived tag and 32 for the
# Platform-derived top, which is the width finding this overlay carries. That
# file also runs the mount bring-up, hal_fill_platform_vtable and the overlay's
# single sinit from the registry fill (the ov025/ov045 lane-ownership pattern).
# See port/ov024_syms.txt and port/slice_sweep1_ov024.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov024)
# ov084 joins for gate 32: the first ENEMY overlay, GOOMBA and BOB_OMB_BUDDY.
# ...and ov014, ov062 and ov091 with them. ov014 is a LEVEL overlay (Bob-omb
# Battlefield's own) and is mounted a second time here, per symbol, because
# CHAIN_CHOMP and CHAIN_CHOMP_FENCE reach their tables by name -- the gate-17
# treatment of ov009 applied to the second level. ov062 is KOOPA_THE_QUICK's.
# ov091 is the one the chomp drags in: its InitResources spawns actor 27, the
# post it is chained to, and a null back from that spawn is a fault. ov078 is
# KING_BOB_OMB's, and level 6 is the only level of the fifty-two that loads it.
# run rel0215 wave 1 (lane cast-ov077): ov077, THE SHARED ENEMY PACK -- HEAVE_HO
# (238, daPopoi_c), SPINY (260, daTgz_c) and LAKITU (265, daJgm_c). Fresh
# per-symbol mount, the ov079/ov080/ov081/ov072/ov094 shape; ov077 is not a
# level overlay so there is no whole-image half. All three classes reach their
# SpawnInfo records, their SharedFilePtr construction statics and their
# pointer-to-member state tables by name. Their three vtables (_ZTV6Lakitu ==
# _ZTV7daJgm_c, _ZTV5Spiny == _ZTV7daTgz_c, _ZTV7HeaveHo == _ZTV9daPopoi_c --
# three RTTI-spelling pairs, each confirmed by reading that table's OWN
# vtable[-1] typeinfo record through to its name string) are host arrays
# hal/actor_classes_ov077.cpp fills, and all three of the overlay's sinits run.
#
# THE FOURTH OCCUPANT of the 0x02123740 window, joining ov079, ov080 and ov081.
# It adds NO new ground -- ov077 ends 0x02127d40, inside ov081's 0x02129000 --
# but three of its symbols are contested by a mounted sibling and dsd marks all
# three `ambiguous`. Each is settled from the raw overlay bytes in
# port/ov077_syms.txt, including one that ov080's mount ALREADY HOSTS today
# (0x02127848: ov077's bytes there are Lakitu's SpawnInfo, ov080's are a
# repeating tuning tuple) and two that are ov079's Whomp SpawnInfos, whose
# addresses split one 0x60-byte ov077 bss array into three -- pinned, and the
# two phantom splits left out of the list. See port/ov077_syms.txt and
# port/slice_ov077.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov077)
# run rel0215 wave 1 (lane cast-ov030): ov030, TALL TALL MOUNTAIN's own cast.
# ROLLING_LOG_TTM (102), UKIKI_CAGE (103), UKIKI_THIEF (267) and UKIKI_STAR
# (268) reach their SpawnInfo records, their fifteen SharedFilePtr construction
# statics, their four Fix12 tuning records and the Ukiki's ELEVEN-cell PMF
# state-dispatch storage by name -- the ov072/ov079/ov080/ov081 shape. ov030 is
# already whole-mounted in PORT_LEVEL_OVERLAYS (line 1346); this is the SECOND,
# per-symbol mount, the ov009/ov012/ov013/ov014 dual-mount shape.
# FOUR IDS, THREE CLASSES: 267 and 268 are one class (7daMky_c) under two spawn
# entry points -- both factories store the same table 0x02115bfc. Its three
# vtables (_ZTV13daObjHmBskt_c, _ZTV15daObjHmMaruta_c and _ZTV13RollingLogTtm ==
# _ZTV7daMky_c) are host arrays hal/actor_classes_ov030.cpp fills. NO class in
# this overlay stays unregistered, so all three of its sinits run.
# THE CONFIG'S _ZTV LABELS AND MANGLED METHOD NAMES ARE SHIFTED ONE CLASS here
# -- the ov012/ov072 dsd class-identity shape, third instance -- and everything
# is seated BY ADDRESS. See port/ov030_syms.txt's own header for the ROM
# evidence and port/slice_ov030cast.txt for the four displaced TUs.
list(APPEND PORT_ACTOR_OVERLAYS ov030)
# run rel0215 wave 1 (lane cast-ov036): ov036 (level 28's own overlay, already
# whole-mounted in PORT_LEVEL_OVERLAYS) joins the per-symbol mounts too, so
# Rainbow Cruise's whole seven-class platform cast -- SWINGING_PLATFORM (127),
# ROTATING_PLATFORM_RR (129), SHIP_WING (125), DONUT_BLOCK (126),
# ARMED_ROTATING_PLATFORM (132), TRICKY_TRIANGLES (123) and FLYING_CARPET (130)
# -- reaches its SpawnInfo records, SharedFilePtrs, CLPS blocks and per-class
# file records by name. Same dual-mount shape as ov012/ov013/ov045.
# THE CONFIG'S C++ METHOD AND _ZTV NAMES ARE ATTACHED ONE CLASS EARLY in this
# overlay (the ov045 off-by-one, measured here rather than inherited), so the
# seven host vtable arrays hal/actor_classes_ov036.cpp fills are paired to ids
# BY ADDRESS: _ZTV18RotatingPlatformRr is SHIP_WING's table, _ZTV8ShipWing is
# DONUT_BLOCK's, _ZTV10DonutBlock is ARMED_ROTATING_PLATFORM's and
# _ZTV21ArmedRotatingPlatform is TRICKY_TRIANGLES'. The derivation, both
# attribution routes, all seven widths and the four size pins are in
# port/ov036_syms.txt. See also port/slice_ov036cast.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov036)
# run rel0215 wave 2 (lane cast-ov090): ov090, THE WATER ENEMY PACK -- SKEETER
# (230, 9daMenbo_c), MANTA_RAY (226, 9daManta_c), CHEEP_CHEEP (227,
# 12daPukupuku_c) and SHARK (225, 9daShark_c). Fresh per-symbol mount, the
# ov077/ov079/ov080/ov081/ov072/ov094 shape; ov090 is not a level overlay so
# there is no whole-image half. All four classes reach their SpawnInfo records,
# their ten SharedFilePtr construction statics and their sixteen
# pointer-to-member state records by name. Their four vtables (_ZTV7Skeeter ==
# _ZTV9daMenbo_c, _ZTV10CheepCheep == _ZTV12daPukupuku_c, _ZTV5Shark ==
# _ZTV9daShark_c -- three RTTI-spelling pairs, each confirmed by reading that
# table's OWN vtable[-1] typeinfo record through to its name string; MantaRay
# has only one spelling) are host arrays hal/actor_classes_ov090.cpp fills, and
# all four of the overlay's sinits run.
#
# THE WINDOW (T6): base 0x02130f00 with ov091 mounted and ov092 not. ov090 ends
# 0x021345e0, STRICTLY INSIDE ov091's 0x021356e0, so it adds no new ground and
# cannot take a resolved target away from another mount. ov006's much larger
# span (0x020bfec0..0x021430a0) also contains all of it, which is why dsd marks
# all four SpawnInfos `ambiguous` and the arm9 spawn-table relocations read
# `module:overlays(6,90)`. That is a dual-dump artifact, not a contest: ov006's
# own raw bytes at all four addresses are minigame tuning tuples with no
# plausible function pointer, while ov090's are real SpawnInfos whose word[0]
# is an e92d4010 prologue in ov090's own .text and whose word[1] low halfword
# is the id that reached them. IDS 225/226/227/230 ARE ALL ov090's -- the
# derivation, both routes and ov006's own bytes are in port/ov090_syms.txt.
#
# NO SIZE PIN IS NEEDED in this overlay, which is measured rather than assumed:
# every mounted symbol's next-symbol delta was checked against what the four
# sinits and the bodies actually write through it. See port/ov090_syms.txt and
# port/slice_ov090.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov090)
# run rel0215 wave 2 (lane cast-ov066): ov066, the EYEROK pack -- EYEROK (176,
# 10daIwante_c), the SSL pyramid boss, wanted by level 17 alone. Fresh
# per-symbol mount, the ov077/ov079/ov080/ov081 shape; ov066 is not a level
# overlay, so this is its only mount. Its 112 data and bss symbols -- one
# SpawnInfo, one typeinfo record, THIRTY-EIGHT pointer-to-member source pairs,
# twenty-two SharedFilePtrs, twenty-two destructor list nodes and NINETEEN
# 16-byte state records -- reach the class by name. Its ONE vtable
# (_ZTV6Eyerok == _ZTV12daIwante_c, 32 slots because Eyerok is Platform-derived,
# confirmed three ways) is a host array hal/actor_classes_ov066.cpp fills, and
# its one sinit runs.
# ONE CLASS, NOT THREE: the two hands and the head are STATES, which is why
# .data carries a 38-entry PMF table instead of three class blocks. See
# port/ov066_syms.txt for the derivation and port/slice_ov066.txt for the four
# host-copied TUs and the SIX /alternatename directives this mount defeats.
list(APPEND PORT_ACTOR_OVERLAYS ov066)
# run rel0215 wave 2 (lane cast-ov027): ov027 (level 19's own overlay, already
# whole-mounted in PORT_LEVEL_OVERLAYS) joins the per-symbol mounts too, so
# Snowman's Land's whole cast -- SLIDING_ICE_SPAWNER (92), SLIDING_ICE (93),
# CHILL_BULLY (217), DA_PG_DFDR (258) and SNOWMAN_BREATH (275) -- reaches its
# SpawnInfo records, SharedFilePtrs, CLPS blocks and per-class file records by
# name. Same dual-mount shape as ov012/ov013/ov036/ov045.
# FIVE IDS, NOT THE FOUR THE TASK SHEET NAMED: route 2 found id 258, whose
# SpawnInfo and factory config leaves unnamed (data_ov027_02113a00 /
# func_ov027_0211207c) and which owns sixteen of the overlay's 53 functions.
# dsd's names ARE correct by address on this overlay -- no ov036/ov045
# off-by-one -- but ids 92 and 93 SHARE ONE VTABLE and one fill. The
# derivation, both attribution routes, all four widths (including the 37-slot
# ChillBully table whose tail is a pointer-to-member source table, not more
# vtable) and the four size pins are in port/ov027_syms.txt. See also
# port/slice_ov027cast.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov027)
# run rel0215 wave 2 (lane cast-lvl16pair, sub-job 1 of 2): ov092, THE TOX_BOX
# PACK -- TOX_BOX (309, 8daOnms_c), level 16's crushing block, x3 placed there
# and on no other level of the fifty-two. Fresh per-symbol mount, the
# ov017/ov077/ov079/ov080/ov081/ov072/ov094 shape; ov092 is not a level overlay
# so there is no whole-image half. The class reaches its SpawnInfo record, its
# two SharedFilePtr construction statics, its CLPS block, its three path blocks
# and its nine-entry pointer-to-member state table by name. Its one vtable
# (_ZTV6ToxBox, 32 slots, a PLATFORM close rather than the plain Actor close --
# slot 31 is 0x020ee55c _ZN8Platform4KillEv) is a host array
# hal/actor_classes_ov092.cpp fills, and the overlay's single sinit runs.
#
# THE FOOTPRINT ARGUMENT, and it is short. ov092's span is
# 0x02130f00..0x021325c0 and ov091's, already mounted, is
# 0x02130f00..0x021356e0 -- both re-derived from each overlay's own
# delinks.txt, not quoted. ov092 is STRICTLY INSIDE its mounted sibling, so it
# adds no new ground and cannot take a resolved --cross target from anyone
# (the ov044/ov045/wave-C argument). dsd marks ZERO ov092 symbols `ambiguous`
# and the arm9 spawn-table relocation for id 309 reads `module:overlay(92)`,
# singular -- this overlay has no contested id and needed no adjudication.
# ov006 also covers this window (0x020bfec0..0x021430a0) and is mounted, but
# the two mounts emit different C names at different host storage and the
# registry reaches its record by C name, never by DS address; the one ov006
# reloc that lands inside a symbol this mount hosts points at ov006 CODE. See
# port/ov092_syms.txt and port/slice_ov092.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov092)
# run rel0215 wave 2 (lane cast-lvl16pair, sub-job 2 of 2): ov096, THE POKEY
# PACK -- POKEY (240, daSanbo_c), POKEY_SEGMENT (241, THE SAME CLASS under a
# second spawn entry point) and TORNADO (308, daTor_c). Level 16 places POKEY
# x4 and TORNADO x3; POKEY_SEGMENT is placed on no level and is spawned by
# POKEY. Fresh per-symbol mount, the ov017/ov077/ov079/ov080/ov081/ov072/ov094
# shape; ov096 is not a level overlay so there is no whole-image half. The
# classes reach their three SpawnInfo records, their four SharedFilePtr
# construction statics and POKEY's twelve-record pointer-to-member state table
# by name. The two vtables (_ZTV5Pokey == _ZTV9daSanbo_c, _ZTV7Tornado ==
# _ZTV7daTor_c -- two RTTI-spelling pairs, each confirmed by reading that
# table's OWN vtable[-1] typeinfo record through to its name string) are host
# arrays hal/actor_classes_ov096.cpp fills, and both of the overlay's sinits
# run in the ROM's own .ctor order.
#
# THIS IS THE CAMPAIGN'S ONE WINDOW-EXTENDING MOUNT, and the plan's T6 table is
# right about the arithmetic and incomplete about the consequence. Extents
# re-derived from each overlay's own delinks.txt rather than quoted:
#   ov094  0x02135700..0x02136b80  MOUNTED
#   ov095  0x02135700..0x02137940  MOUNTED, the widest before this
#   ov096  0x02135700..0x02137be0  = +0x2a0 PAST ov095
# and 0x02137be0 is exactly ov098's base, so the extension is ADJACENT, not
# overlapping. But ov006 ALSO covers this window -- its span is
# 0x020bfec0..0x021430a0, it is mounted, and port/ov006_syms.txt already hosts
# 338 symbols inside ov096's range -- so every byte ov096 "adds" is already
# inside a mounted window and the extension cannot flip a --cross target from
# resolved to raw for anyone. The census was re-run both ways regardless,
# because that is what the plan asked of this lane; the numbers are in the
# lane's report and the CONTESTED count does not move.
#
# THREE CONTESTED IDS, ALL THREE LIVE, ALL THREE ADJUDICATED. dsd marks all
# three of this overlay's SpawnInfo rows `ambiguous`, every arm9 spawn-table
# relocation reads `module:overlays(6,96)`, and ov006's mount already hosts all
# three addresses today. Each was settled from the RAW BYTES of both claimants:
# ov096 has a real e92d4010 prologue in word[0] and the reaching table index in
# word[1]'s low halfword at every one, ov006 has NITRO GX command data with no
# plausible function pointer at any. No bridge is needed -- the two mounts emit
# different C names at different host storage and the registry reaches its
# record by C name, never by DS address. See port/ov096_syms.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov096)
# run rel0215 wave 2 (lane w2-ov074): ov074, THE GOOMBOSS PACK -- GOOMBOSS
# (198) and EXPLOSION_GOOMBA (199), both 12daKuriKing_c. Fresh per-symbol
# mount, the ov077/ov079/ov080/ov081/ov072/ov094 shape; ov074 is not a level
# overlay so there is no whole-image half. Level 45 (Mario's key arena,
# ex_mario, ov053) is the only level of the fifty-two that names it, and its
# skip list is exactly one row.
# TWO IDS, ONE CLASS: src/Goomboss_Spawn.cpp and src/ExplosionGoomba_Spawn.cpp
# are the same 0xd4 body twice and both store the SAME table, so the overlay
# has ONE vtable (_ZTV8Goomboss, 31 slots, host array
# hal/actor_classes_ov074.cpp fills) and ONE typeinfo record, whose own
# vtable[-1] reads through to "12daKuriKing_c" -- no label shift here.
#
# THE FOURTH OCCUPANT of the 0x0211f000 window, joining ov070, ov071 and
# ov073. It adds NO new ground -- ov074 ends 0x021231a0, inside ov073's
# 0x021234e0 -- but eight of its symbols are marked `ambiguous` by dsd and
# fifteen arm9 spawn-table slots relocate into its span. Both attribution
# routes were run on every one: exactly TWO are ov074's (198, 199) and the
# other thirteen belong to a mounted sibling, including one (id 264 ->
# 0x02122dc4) whose word[0] IS a real ov074 function and which only route 2
# rejects. FIVE of the eight ambiguous rows are PHANTOM SPLITS inside ov074
# objects -- one splits a 0x30 pointer run, one a 0xc destructor list node,
# and THREE split the 0x90 nine-cell state table dsd sizes 0x4c. All three
# heads are pinned and all five phantoms left out of the list. See
# port/ov074_syms.txt and port/slice_ov074.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov074)
# ============================================================================
# RUN REL0215 WAVE 2, LANE CAST-SWEEP2 -- four level overlays, one per commit.
# This region grows by one list(APPEND) per overlay; the source-list region far
# below grows by one block per overlay and feeds ONE ${SWEEP2_SOURCES} into the
# three target lists, so a second overlay costs the coordinator nothing outside
# those two regions. Every overlay here is already whole-mounted in
# PORT_LEVEL_OVERLAYS, so each per-symbol mount is the SECOND mount of the same
# overlay -- the ov012/ov013/ov024/ov025/ov033/ov045 dual shape. --whole emits
# port_ovNNN_image/_patch/_at and nothing by name; --from-list --pack emits the
# named arrays plus port_ovNNN_syms_patch. Every mount bring-up runs from the
# lane's own hal/actor_classes_ovNNN.cpp behind a DSSTATE-bracketed done-guard
# (the ov025/ov033/ov045/ov052/ov056/ov073 lane-ownership pattern), so this lane
# appends NOTHING to hal/actor_overlays.cpp.
#
# 1/4: ov032 (level 24, Tiny-Huge Island's BIG side) joins the per-symbol
# mounts so HUGE_COVER (106), HUGE_WATER (107) and BUBBA (228) reach their three
# SpawnInfo records, their two CLPS blocks, id 107's BTA file descriptor, the
# seven sinit-built SharedFilePtrs, the seven destructor chain nodes and BUBBA's
# ten PMF source pairs and five state cells by name. Four of level 24's seven
# skipped instances. THE CAMPAIGN PLAN LISTS TWO CLASSES HERE AND THE ROM HAS
# THREE: id 228, RTTI 12daBakubaku_c, is the big fish (tools/actor_names.py
# reads it as BUBBA) and carries twenty-four of the overlay's forty-one TUs.
# All three vtables are excluded from the mount and hosted as arrays
# hal/actor_classes_ov032.cpp fills -- and dsd's ONE vtable address here carries
# TWO names, both on the WATER class (_ZTV9HugeCover and _ZTV14daObjTdWater_c at
# 0x021139a4), while the cover's own table and BUBBA's are unnamed. Proven three
# ways in port/ov032_syms.txt. See port/slice_sweep2_ov032.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov032)
# 2/4: ov034 (level 26, Tiny-Huge Island's CAVE) joins the per-symbol mounts so
# WIGGLER (248) reaches its SpawnInfo record, its seven five-element animation
# handle rows, its thirty-five sinit-built SharedFilePtrs, its thirty-five
# destructor chain nodes, its twenty-two PMF source pairs and -- the important
# one -- its ELEVEN-ENTRY, twenty-byte-per-entry state table at
# data_ov034_02114538, whose dsd landing reads ONE ENTRY OF ELEVEN. That is
# level 26's whole skip list, so this mount takes the level to zero skipped.
# Unlike ov032 and ov033 there is NO class-identity shift here: _ZTV7Wiggler is
# on the right class (RTTI "12daHanachan_c") and dsd's inferred 31-slot width is
# right. The table is excluded from the mount and hosted as an array
# hal/actor_classes_ov034.cpp fills. Derivation in port/ov034_syms.txt, slice in
# port/slice_sweep2_ov034.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov034)
# 3/4: ov047 (level 39, Bowser in the Sky) joins the per-symbol mounts so
# RICKSHAW_PLATFORM_BS (151), RICKSHAW_BS (152), STAIRS_BS (153) and
# ROTATING_PLATFORM_BS (156) reach their four SpawnInfo records, their seven
# CLPS collision blocks, their four resource arguments and their thirty-two
# sinit-built SharedFilePtrs and destructor chain nodes by name. Seven of level
# 39's nine skipped instances. THE CAMPAIGN PLAN LISTS THREE CLASSES HERE AND
# THE ROM HAS FOUR: id 156 (RTTI 20daObjKm3_Kaitendai_c, the rotating stand;
# tools/actor_names.py reads it as ROTATING_PLATFORM_BS). All four vtables are
# excluded from the mount and hosted as arrays hal/actor_classes_ov047.cpp
# fills -- and dsd's two vtable names are each attached ONE TABLE LATE, so
# _ZTV10RickshawBs is id 156's and _ZTV18RickshawPlatformBs is id 153's. Proven
# three ways in port/ov047_syms.txt. See port/slice_sweep2_ov047.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov047)
# 4/4: ov055 (level 47, Luigi's key arena) joins the per-symbol mounts so
# MIRROR_LUIGI (195) reaches its SpawnInfo record, its four state words, its two
# PMF source pairs and -- the important one -- the SIXTEEN-BYTE state cell at
# data_ov055_02111b70, whose dsd landing reads ONE WORD OF FOUR. That is level
# 47's whole skip list, so this mount takes the level to zero skipped. No
# identity shift: _ZTV11MirrorLuigi is on the right class (RTTI "9daLuigi_c",
# base typeinfo Actor's), the table is 31 slots because a MirrorLuigi is a plain
# Actor and not a Platform, and no ov055 TU spells a resolver placeholder. This
# mount DEFINES data_ov055_02111a94, which src/_ZN9ShipWater13InitResourcesEv.c
# (ov017, LINKED) spells for OV017's object at the same shared-window address --
# already routed by the wave-1 cast-sweep1 lane's per-source -D, which foresaw
# this mount and is not defeated by a definition the way an /alternatename would
# be. Derivation in port/ov055_syms.txt, slice in port/slice_sweep2_ov055.txt.
list(APPEND PORT_ACTOR_OVERLAYS ov055)
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
# The residency model reads its rules OUT OF the matched loader sources, so
# those five TUs are real inputs of the cross step: without these edges a
# semantic change to a loader body leaves ov_cross.c carrying bindings from
# the superseded model, silently (the stale-artifact shape). A rename still
# fails the next configure loudly, which is the intended trade.
set(OVCROSS_LOADER_SRC
    "${PORT_REPO_ROOT_ABS}/src/GetSceneOverlayID.c"
    "${PORT_REPO_ROOT_ABS}/src/func_0201a694.c"
    "${PORT_REPO_ROOT_ABS}/src/func_0201a798.c"
    "${PORT_REPO_ROOT_ABS}/src/func_0201a754.c"
    "${PORT_REPO_ROOT_ABS}/src/func_0201a614.c")
add_custom_command(OUTPUT "${OVCROSS_C}"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" --cross
            "${OVCROSS_C}" ${OV_DATA_MAPS}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ${OV_DATA_MAPS}
            ${OVCROSS_LOADER_SRC}
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
# r0-passthrough: func_ov085_0212b8dc (the RABBIT's state Main, daMip_c) calls
# Actor::ClosestPlayer() with no argument, relying on `this` riding ARM r0. Host
# copy passes `c`; src line commented out in slice_gate18.txt. (the actor-187
# RABBIT null-this +0x5c crash, reachable once slot 19 made the rabbit swallowable)
list(APPEND SLICE18_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov085_Rabbit_b8dc.cpp")
# r0-passthrough: four LakituBro (daC_Jugem_c) state bodies -- func_ov085_
# 0212d9b8/dd10/e310/e4a4 -- each call Actor::ClosestPlayer() with no argument,
# relying on `this` riding ARM r0. Host copies pass each body's own receiver; src
# lines commented out in slice_gate18.txt. e4a4 (caught/grab state) is the site
# caught taking a leaked receiver on a level-1 run.
list(APPEND SLICE18_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov085_LakituBro_ClosestPlayer.cpp")

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

# Linkage-campaign lane slices (run linkw): five files, one OWNER each, so
# five parallel seat lanes add matched TUs without ever editing this file or
# each other's lists. Same reader shape as the gate slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w1l1.txt" SLICE_W1L1_LINES)
set(SLICE_W1L1_SOURCES "")
foreach(line IN LISTS SLICE_W1L1_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W1L1_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# EXPERIMENT (LIGHTS lane): route Scene::Initialise3dGraphics through hostgen.
set(SCN3D_HOSTGEN_SKIP "src/_ZN5Scene20Initialise3dGraphicsEv.c")
set(SCN3D_HOSTGEN_GEN
    "${CMAKE_BINARY_DIR}/host-src/src/_ZN5Scene20Initialise3dGraphicsEv.cpp")
add_custom_command(OUTPUT "${SCN3D_HOSTGEN_GEN}"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
            --out "${CMAKE_BINARY_DIR}/host-src" _ZN5Scene20Initialise3dGraphicsEv
    DEPENDS "${PORT_REPO_ROOT_ABS}/src/_ZN5Scene20Initialise3dGraphicsEv.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
    COMMENT "hostgen _ZN5Scene20Initialise3dGraphicsEv")

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w1l2.txt" SLICE_W1L2_LINES)
set(SLICE_W1L2_SOURCES ${SCN3D_HOSTGEN_GEN})
foreach(line IN LISTS SLICE_W1L2_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    if(line IN_LIST SCN3D_HOSTGEN_SKIP)
        continue()
    endif()
    list(APPEND SLICE_W1L2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w1l3.txt" SLICE_W1L3_LINES)
set(SLICE_W1L3_SOURCES "")
foreach(line IN LISTS SLICE_W1L3_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W1L3_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w1l4.txt" SLICE_W1L4_LINES)
set(SLICE_W1L4_SOURCES "")
foreach(line IN LISTS SLICE_W1L4_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W1L4_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# run linkw wave 5 (lane w5-c): the ov070 enemy pack + the level-37 census
# closers. Two hostgen groups ride with the slice:
#   SQRTUNIT -- the hardware square-root unit's four drivers. func_02053008 /
#   func_020531a4 write the unit through bound pointers, func_02052fdc reads
#   it through literal derefs, and func_02053130 reaches it through role-named
#   externs (hostgen's MMIO_EXTERN table). All four route through run_sqrt via
#   ntr/io.cpp's widened SQRT_PARAM trigger; the numeric proof is in the
#   commit that landed the mechanism. Quaternion_Normalize (slice_w1l4's
#   restored id-141 chain) and Quaternion_FromVector3 are the callers.
#   Folded from lane w5-B (the level-13 reach evidence): the same unit is
#   pulled in by WORK_ELEVATOR's rider-push callback (func_ov021_02112024
#   -> Quaternion_Normalize), closing the id-141 sqrt gap
#   hal/actor_classes_ov045.cpp names from the HMC cast as well.
#   OV70GEN -- func_ov070_0211f0a4, the census's measured compile fail (C4716;
#   hostgen's FALLS_OFF_RETURN table documents the returned value).
set(SQRTUNIT_SYMS func_02053008 func_02053130 func_020531a4 func_02052fdc)
set(SQRTUNIT_GEN "")
foreach(sym IN LISTS SQRTUNIT_SYMS)
    set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND SQRTUNIT_GEN "${sym_out}")
endforeach()
set(OV70GEN_SYMS func_ov070_0211f0a4)
set(OV70GEN "")
foreach(sym IN LISTS OV70GEN_SYMS)
    set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND OV70GEN "${sym_out}")
endforeach()
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w5c.txt" SLICE_W5C_LINES)
set(SLICE_W5C_SOURCES "")
foreach(line IN LISTS SLICE_W5C_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W5C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w1l5.txt" SLICE_W1L5_LINES)
set(SLICE_W1L5_SOURCES "")
foreach(line IN LISTS SLICE_W1L5_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W1L5_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# Wave-5 lane slice (run linkw), same one-owner contract as the five above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w5d.txt" SLICE_W5D_LINES)
set(SLICE_W5D_SOURCES "")
foreach(line IN LISTS SLICE_W5D_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W5D_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# run linkw wave 5, lane w5-e: the Bowser arena pack (ov060 + the arena class)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w5e.txt" SLICE_W5E_LINES)
set(SLICE_W5E_SOURCES "")
foreach(line IN LISTS SLICE_W5E_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W5E_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# run linkw wave 6, lane w6-A: THE FIGHT -- the five remaining ov060 classes
# (SpikeBomb 284, Bowser 279, BowserTail 278, BowserFire 280, BowserSkyPlatform
# 167) and ov089's boss reward (Key 282, LastStar 283). Same one-owner
# contract; the coupling between 279/278/280/282 is measured, see that file's
# header and hal/actor_classes.inc's wave-6 row block.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w6a.txt" SLICE_W6A_LINES)
set(SLICE_W6A_SOURCES "")
foreach(line IN LISTS SLICE_W6A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W6A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# run linkw wave 5, lane w5-B (Hazy Maze Cave's cast); same reader shape,
# one owner.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w5b.txt" SLICE_W5B_LINES)
set(SLICE_W5B_SOURCES "")
foreach(line IN LISTS SLICE_W5B_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W5B_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 5, lane w5-A: Big Boo's Haunt's cast (ov063 + host copies).
# See the header of port/slice_w5a.txt for the six exclusions and the four
# byte-verified host copies.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w5a.txt" SLICE_W5A_LINES)
set(SLICE_W5A_SOURCES "")
foreach(line IN LISTS SLICE_W5A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W5A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# run linkw wave 6, lane w6-f: FLAMETHROWER (318, ov095). The four matched
# methods plus the four particle emitters the Behavior fires; the Behavior
# itself is the host copy unmatched/Flamethrower_Behavior.c, listed on the
# targets beside the fill. Same one-owner contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w6f.txt" SLICE_W6F_LINES)
set(SLICE_W6F_SOURCES "")
foreach(line IN LISTS SLICE_W6F_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W6F_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 7, lane w7-c: ORANGE_BALL_BILLBOARD (301, ov044) -- level 36's
# only cast item that was not already registered, plus the fill/registry host
# file. Level 40 needs no slice: its three ids are all ov060, all landed in
# wave 6. One TU is held out (the shadow-class MSVC destructor); the header of
# port/slice_w7c.txt carries the reason and the closure walk. Same one-owner
# contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w7c.txt" SLICE_W7C_LINES)
set(SLICE_W7C_SOURCES "")
foreach(line IN LISTS SLICE_W7C_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W7C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 8, lane w8-faces: THE FACE QUEUE. This lane audits the two
# forwarder-face files against the emitted bytes; the slice carries the one
# matched TU that audit turned from a host stand-in into real decomp code
# (TextureTransformer::Prepare -- the header of port/slice_w8faces.txt has the
# reading, including the parked note it refutes). Same one-owner contract as
# the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w8faces.txt" SLICE_W8FACES_LINES)
set(SLICE_W8FACES_SOURCES "")
foreach(line IN LISTS SLICE_W8FACES_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W8FACES_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 9, lane w9-harvest: THE STALE-FORK HARVEST. This branch forked
# from main at 7b2f913fe (2026-08-04) and is 626 commits behind it, so every TU
# the decomp has matched since is invisible from here -- including ones the port
# hand-hosts under a banner saying no matched TU exists. The slice carries the
# matched TUs that audit recovered; port/slice_w9harvest.txt's header has the
# measured pool (543 main-only TUs: 496 rename-duplicates, 7 host retirements,
# 40 unreferenced) and the per-file evidence. Same one-owner contract as the
# lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w9harvest.txt" SLICE_W9HARVEST_LINES)
set(SLICE_W9HARVEST_SOURCES "")
foreach(line IN LISTS SLICE_W9HARVEST_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W9HARVEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 6, lane w6-D: PATH_LIFT (31) and the PathLift base branch --
# ov100's daObjPathLift_c over ov002's PathLiftActor_c over Platform, the
# first three-level hierarchy this port hosts that is split across overlays.
# Same reader shape, one owner. The header of port/slice_w6d.txt carries the
# reloc-derived closure, the two PMF host copies and the ov006 naming trap;
# the hierarchy map and both 33-slot widths are in
# hal/actor_classes_ov100pl.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w6d.txt" SLICE_W6D_LINES)
set(SLICE_W6D_SOURCES "")
foreach(line IN LISTS SLICE_W6D_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # VS wiring lane: the PathLift path-follow body is substituted by
    # port/unmatched/VS_PathDivGuard.cpp (in SLICE_VS_SOURCES) -- its two
    # `x / dv` sites need the ARM divider's x/0 = 0, and a short-or-slow
    # path segment makes dv 0 (measured: c0000094 in the VS castle grounds,
    # reproduced with plain SM64DS_LEVEL=51). The manifest line is skipped
    # HERE so slice_w6d.txt itself stays another lane's file, the TTE
    # hostgen-skip shape.
    if(line STREQUAL "src/func_ov002_020ef670.c")
        continue()
    endif()
    list(APPEND SLICE_W6D_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 8, lane w8-packs: ov022's Lethal Lava Land platform cast --
# VOLCANO_RING (71), FLOAT_ON_LAVA_PLATFORM (74), FLOATING_FLOOR_LLL_BIG (75)
# and _SMALL (76), LAVA_PLANK (82), plus the four ov022 static initialisers
# those four tables need. Same reader shape, one owner. The cast map (three
# independent routes), the two shifted name blocks, both width routes and the
# four classes held back on the inferred-stub ratchet are in
# hal/actor_classes_ov022.cpp; the mount is port/ov022_syms.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w8packs.txt" SLICE_W8PACKS_LINES)
set(SLICE_W8PACKS_SOURCES "")
foreach(line IN LISTS SLICE_W8PACKS_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W8PACKS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 10, lane w10-shared: ov031's SLIDE DECORATION QUAD -- FOUR
# spawn-table ids (302/303/304/305) onto ONE ROM class and ONE table
# (0x02111984), x3 each on level 23, whose entire baseline skip set was those
# twelve instances. The class reads its variant out of its own actor id, so one
# fill and one closure of bodies serve all four rows. One TU is held out (the
# shadow-class MSVC destructor, slot 16, spelled inline in the fill instead);
# the header of port/slice_w10a.txt carries the reason, both attribution routes
# for each id, the width derivation and the Render slot-5 ruling. Same one-owner
# contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w10a.txt" SLICE_W10A_LINES)
set(SLICE_W10A_SOURCES "")
foreach(line IN LISTS SLICE_W10A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W10A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 10, lane w10-shared, slice B: MEGA_MUSHROOM (277, ov002) x1 on
# level 22 -- ONE TU. Its factory stores _ZTV13OneUpMushroom, the same ROM table
# the already-registered ONE_UP_MUSHROOM (276) installs, so it needs no fill, no
# vtable and no new mount: one record in port/ov002_syms.txt and this one line.
# The header of port/slice_w10b.txt carries both attribution routes, the width
# agreement and the record pin. Same one-owner contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w10b.txt" SLICE_W10B_LINES)
set(SLICE_W10B_SOURCES "")
foreach(line IN LISTS SLICE_W10B_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W10B_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 11, lane w11: NINETEEN ids whose vtable is already a FILLED
# host array and whose overlay is already mounted -- the tier the census-built
# worklist could not see, found by widening the shared-table sweep from ~70
# classes to all 512 spawn-table ids. One factory TU each and nothing else: no
# mount, no vtable, no fill, no wrapper. Six records were added to
# port/ov095_syms.txt and port/ov002_syms.txt; the other thirteen were mounted
# already. The header of port/slice_w11.txt carries both attribution routes for
# all nineteen, the ten shared tables with the registered sibling that fills
# each, and the done-guard evidence for every fill reused. Same one-owner
# contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w11.txt" SLICE_W11_LINES)
set(SLICE_W11_SOURCES "")
foreach(line IN LISTS SLICE_W11_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W11_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 12, lane w12: ov073, the WHOLE CAST OF LEVEL 49 (ex_wario,
# "Wario's key arena"). The level was mounted and booting with four actors
# placed and EIGHTEEN skipped, because every actor it places is ov073's and
# ov073 had no mount -- hal/level_boot.cpp's own row said "[unmounted ov073]".
# Sixty-seven TUs, the reloc closure of both vtables' ov073 slots, both
# ChangeState veneers, the four Spawns, both sinits and the forty {code, adj}
# state records the sinits copy (those forty are reachable only through the
# state tables, so a reloc-only closure misses all of them). Five TUs are held
# out and host-copied -- one ARM r0 ride-through factory, two MSVC
# pointer-to-member dispatchers, one inline copy of the same, and one
# BlendModelAnim slot-5 collision -- with the derivation for each in the header
# of port/slice_w12.txt. Same one-owner contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w12.txt" SLICE_W12_LINES)
set(SLICE_W12_SOURCES "")
foreach(line IN LISTS SLICE_W12_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W12_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 12, lane w12: ov052's two platform classes (level 44). Sixteen
# TUs, the whole of that overlay's actor code, and the reloc closure leaves
# ov052 for nothing that was not already linked. The header of
# port/slice_w12b.txt carries both attribution routes, the 38-reads-as-32 width
# derivation and the shared-window misspelling this lane renames per source.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w12b.txt" SLICE_W12B_LINES)
set(SLICE_W12B_SOURCES "")
foreach(line IN LISTS SLICE_W12B_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W12B_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 12, lane w12: CLAM (315), ov064 -- nine instances on a table
# nothing had filled, in an overlay already mounted whole and per symbol. Seven
# TUs; two more bodies of its closure are held out and host-copied (a ModelAnim
# slot-5 Render and a real MSVC destructor with no C name). The header of
# port/slice_w12c.txt carries both, and the measured reason its sibling
# TREASURE_CHEST (13) is not here.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w12c.txt" SLICE_W12C_LINES)
set(SLICE_W12C_SOURCES "")
foreach(line IN LISTS SLICE_W12C_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W12C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 15, lane w15: BIG_MOVING_ICE_BLOCK (173), ov056 -- nine
# instances on level 48, on a table nothing had filled, in an overlay mounted
# whole but not yet per symbol. EIGHT TUs, the whole of ov056's actor code
# (six vtable slots, the factory and the overlay's single sinit), and the reloc
# closure leaves ov056 for nothing that was not already linked. None of the
# eight carries the inferred-stub guess marker. The header of
# port/slice_w15.txt carries both attribution routes, the Render slot-5 ruling
# and the two destructors' placeholder-store reading; the width derivation is
# in port/ov056_syms.txt. Same one-owner contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w15.txt" SLICE_W15_LINES)
set(SLICE_W15_SOURCES "")
foreach(line IN LISTS SLICE_W15_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W15_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 15, lane w15: the MEGA MUSHROOM TAG pair (319 + 320, ov002) --
# ONE ROM class under two spawn-table ids on one table (0x02108cf4), one
# instance of each on levels 22 and 24. ov002 has been mounted since gate 14,
# so this is two records in port/ov002_syms.txt, one host array with one fill,
# two registry rows and EIGHT TUs, with no new mount and no new sinit. The
# class branches on its own actor id, so one closure serves both rows. Slot 16
# is held out and transcribed in the fill (the SlideDecorationSilverStar
# shadow-class MSVC destructor shape); the ROM disassembly it was transcribed
# from is in hal/actor_classes_ov002_w15.cpp. None of the eight carries the
# inferred-stub guess marker. Same one-owner contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w15b.txt" SLICE_W15B_LINES)
set(SLICE_W15B_SOURCES "")
foreach(line IN LISTS SLICE_W15B_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W15B_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 15, lane w15: ov025's PYRAMID_STEP (87) and PYRAMID_LIFT (88)
# -- five of level 17's eleven skipped instances. NINETEEN TUs: twelve vtable
# bodies, two factories, four helpers and the two sinits these classes own. One
# more body is held out and host-thunked (PyramidStep::CleanupResources, whose
# src spells its two SharedFilePtrs G0/G1 in C++ and would Release SignPost's
# live ones -- the ov022 FloatingFloorLllBig failure); the ROM disassembly it
# was transcribed from is in hal/actor_classes_ov025.cpp. None of the nineteen
# carries the inferred-stub guess marker. Same one-owner contract as the lane
# slices above.
# run linkw wave 16, lane w16-unblock: FALL_BLOCK_LLL (id 83, ov022) -- nine
# instances on level 14, the sixth of Lethal Lava Land's eleven ids. NO NEW
# MOUNT: lane w8-packs already mounted FallBlockLll_SpawnInfo and the file
# table data_ov022_0211427c by name and already excluded the vtable 0x021142c4
# so it can be host storage. What held this id back was inferred_stub_guard.py
# -- three of its five bodies carry the "recovered from vtable slot identity"
# marker. All three were disassembled out of the shipped overlay image and
# ruled REAL DECOMP (port/slice_w16a.txt for the derivation, the rulings are
# recorded against their symbols in port/tools/inferred_stub_debt.txt). The
# class is FallBlockBfs' twin: slots 6/9/27/31 are the same four ov098 bodies
# ov045 and ov015 already seat, so the new code is a host array, two
# destructors and one sinit. Width is 32, pinned three ways -- the reloc run
# over-reads to 38 through two typeinfo records, the next-symbol landing
# under-reads to 30 because dsd split the table at slot 30, and slot 31 is a
# real 32nd virtual (func_ov098_0213a17c, NOT Platform::Kill). Same one-owner
# contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w16a.txt" SLICE_W16A_LINES)
set(SLICE_W16A_SOURCES "")
foreach(line IN LISTS SLICE_W16A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W16A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run linkw wave 17, lane w17: SEAWEED (id 296, ov002), RTTI 13daObjWakame_c --
# x8 on level 18 and x4 on level 8. Lane w16 ruled its five marker-carrying
# bodies REAL DECOMP and left it blocked on one mount line; that line is
# Seaweed_SpawnInfo:0x1c in port/ov002_syms.txt, NOT the 0x24 the dsd
# next-symbol delta reads (the two extra words are the vtable's own Itanium
# prologue, the wave-15 MEGA_MUSHROOM shape). The vtable 0x02109bb8 is excluded
# from the mount and hosted as an array; its identity is the RTTI record at
# vtable[-1], since dsd's _ZTV7Seaweed is one class further on and is really
# HealingHeart's (RTTI 12daObjHeart_c). Slot 9 is the ModelAnim slot-5
# collision and is a host copy in hal/actor_classes_ov002_w17.cpp, so
# src/func_ov002_020bc4f8.cpp is deliberately NOT in this slice. Same one-owner
# contract as the lane slices above.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w17a.txt" SLICE_W17A_LINES)
set(SLICE_W17A_SOURCES "")
foreach(line IN LISTS SLICE_W17A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W17A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# R7, run linkw wave 17 (lane w17): src/func_ov002_020bc444.c (SEAWEED's
# deleting destructor) restores its vtable through the shared-header
# placeholder `VT0` -- and VT0 is DEFINED in this link, as `int VT0[20]` in
# hal/actor_vtables.cpp, whose own comment warns that "two classes can spell
# DIFFERENT vtables VT0 and both resolve here. A slot that is actually
# dispatched must never come through one of them." Slot 17 IS dispatched, so
# this would have linked silently and installed a 20-word dummy as the vptr
# ahead of ModelAnim::D1 and Actor::D2. A -D and not an /alternatename for the
# R4/R5 reason squared: the LHS is already defined, so an alias would be
# defeated on the spot. The target is the HOST ARRAY, since 0x02109bb8 is
# excluded from the mount.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov002_020bc444.c"
    PROPERTIES COMPILE_DEFINITIONS "VT0=_ZTV13daObjWakame_c")

# R8, run linkw wave 17 (lane w17): the mwcc POINTER-TO-MEMBER width bug, and
# the reason level 27 could be mounted green. src/func_ov070_02120d34.cpp and
# src/func_ov070_0212180c.cpp are `PMF *p = c->pp + 1; (c->**p)();` over an
# INCOMPLETE class -- mwcc's PMF there is the Itanium 8-byte {fn, adj} record,
# MSVC's is the 4-byte single-inheritance form, so `+ 1` strides four and lands
# on the adj word, which every seated record holds as 0. Measured as `call 0`
# on frame 0 of level 27 for AMP (0x10a) and FLAME_CHOMP (0x10e) -- the fault
# wave 8 filed as "DATA-dependent". The stride is pinned by the setters
# func_ov070_02120da8 / func_ov070_02121880, which both index the state table
# by `state << 4`, i.e. two 8-byte PMFs per State. Routed at the two CALLERS
# rather than by editing another lane's slice: the matched TUs stay in src/ and
# on slice_w5c.txt and are now dead. Full reading in
# port/unmatched/Ov070_PmfDispatch.cpp, including the twenty-one OTHER TUs in
# src/ that carry the identical shape and are a lane of their own.
# The remedy is a COMPILE OPTION on the two dispatchers themselves, not a
# rename of their callers: MSVC's /vmg /vmm pair says "for a pointer-to-member
# declared before its class, use the MULTIPLE-INHERITANCE representation",
# which is {void *fn; int delta;} -- eight bytes, the ROM's own record, with a
# call that adds delta to `this` and calls fn. So `c->pp + 1` strides eight and
# the dispatch is the ROM's even-adj branch exactly. The ROM stores adj and
# shifts it right by one where MSVC stores delta directly; every record in
# ov070 carries adj 0, so the two agree word for word here.
# Nothing is renamed, nothing goes dead, and the two matched TUs keep doing
# their own job. THE SAME OPTION IS THE FIX FOR THE OTHER TWENTY-ONE TUs with
# this shape (port/unmatched/Ov070_PmfDispatch.cpp lists them); only these two
# are turned on here, because only these two are on level 27's critical path
# and each one wants its own measured before/after.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov070_02120d34.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov070_0212180c.cpp"
    PROPERTIES COMPILE_OPTIONS "/vmg;/vmm")

# R9, run linkw wave 18 (lane w18): THE REST OF THE POINTER-TO-MEMBER WIDTH
# SWEEP. R8 above fixed the two ov070 dispatchers that stood between level 27
# and a green battery and left the rest of the class for a lane with a measured
# before/after per TU. This is that lane.
#
# The predicate that finds them is NOT "the TU mentions a pointer-to-member" --
# 302 TUs in src/ do. It is the conjunction of a pointer-to-member of an
# INCOMPLETE class AND a NONZERO offset applied to it. At index 0 the wrong
# stride cannot bite: MSVC's 4-byte PMF at offset 0 IS the fn word, and every
# seated record in this game carries adj 0, so the pp[0] siblings are correct
# BY LUCK and are deliberately NOT listed here. Scanning src/ for that
# conjunction returns exactly TWENTY-ONE TUs, the three above included.
#
# THE STRIDE IS THE ROM'S, READ OFF THE ROM, TWENTY-ONE FOR TWENTY-ONE. Each
# body was disassembled at its own address out of extracted/overlays/ and the
# instruction that forms `pp + 1` read directly: `add r3, r1, #8`, every time,
# feeding the Itanium odd-adj branch. Three of them (KingBobOmb, MrBlizzard,
# ChiefChilly) prove it a second way without leaving src/: each guards the
# dispatch with `*(void **)((char *)pp + 8) != 0` -- a BYTE offset of 8 on the
# same word `pp + 1` then dereferences, so sizeof(PMF) is 8 by the source's own
# arithmetic.
#
# MSVC'S SIDE IS MEASURED TOO, and it settles a disagreement in this tree.
# unmatched/KingBobOmb_States.cpp and unmatched/Bbh_PmfDispatch.c both say MSVC
# "widens PMF to its four-word general representation"/"the 16-byte general
# form"; unmatched/Scuttlebug_StateDispatch.cpp says MSVC's is 4 bytes. The
# objs say 4. Same build, same shape, one flag apart:
#
#   func_ov070_02121fd0 (no option)   mov eax,[ecx+31Ch]   <- pp
#                                     mov eax,[eax+4]      <- ADJ of record 0 = 0
#                                     jmp eax              <- jmp 0
#   func_ov070_02120d34 (/vmg /vmm)   mov edx,[eax+41Ch]   <- pp
#                                     mov ecx,[edx+0Ch]    <- delta of record 1
#                                     add ecx,eax          <- this += delta
#                                     mov eax,[edx+8]      <- FN of record 1
#                                     jmp eax
#
# `[eax+4]` versus `[edx+8]`: the fix moves the call target from the adj word,
# which is zero on every seated record, onto the record's function word. A
# 16-byte representation would have produced a wrong-but-nonzero target, not
# the EIP=0 wave 17 measured.
#
# PER-SOURCE, NEVER GLOBAL. /vmg /vmm changes the representation of every
# pointer-to-member declared in a TU, so a TU that already agrees with MSVC's
# default would silently change layout under a blanket flag. It is safe here
# because none of these TUs takes a pointer-to-member across a header boundary:
# the 4-line dispatchers include nothing at all, and KingBobOmb.h /
# MrBlizzard.h / ChiefChilly.h / decl_common.h / types.h declare no `::*`
# between them. `pp` itself is a POINTER and stays four bytes either way, so no
# `struct C` in this set changes size.
#
# WHICH OF THESE ARE LIVE. Four states, and only two of them are measurable:
#
#   MATCHED+LINKED (3, all ov070): func_ov070_02121fd0 is the one that matters.
#       It is on slice_w5c.txt, it is in walk_window.map today, and it is a
#       live `jmp 0` in the shipping binary -- the same latent crash wave 17
#       fixed on its two siblings and did not fix here. Its class is not on a
#       mounted level yet, which is the only reason no battery caught it.
#   HOSTCOPY->RECLAIMED (4): on slice_w18a.txt, replacing four host bodies.
#   HOSTCOPY->RANKED (2): KingBobOmb::Behavior and ChiefChilly::Behavior are
#       real linkage but their host files also carry setters and table seats.
#   ABSENT (12): not linked, and nothing in the port references them, so
#       /OPT:REF strips them the moment they are sliced. The option is set
#       anyway and is INERT today -- it is not a fix that has been measured,
#       it is a trap disarmed ahead of the lane that mounts the class.
set_source_files_properties(
    # --- live: matched, linked, and a `jmp 0` in the binary until this line ---
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov070_02121fd0.cpp"
    # --- reclaimed from host copies by slice_w18a.txt (see that file) ---
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov072_02121cdc.cpp"  # BabyPenguin tick, ov072 +0x35c
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov071_02120278.cpp"  # Scuttlebug main, ov071 +0x380
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov071_021215c0.cpp"  # MrI main,        ov071 +0x1e4
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov080_021250c8.cpp"  # Bbh tick,        ov080 +0x36c
    # --- inert today: no host copy, no reference, /OPT:REF strips them ---
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10KingBobOmb8BehaviorEv.cpp"   # KingBobOmb, ov078 +0x420
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10MrBlizzard8BehaviorEv.cpp"   # MrBlizzard, ov081 +0x3f8
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN11ChiefChilly8BehaviorEv.cpp"  # ChiefChilly, ov073 +0x37c
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov018_0211235c.cpp"  # ov018 +0x370
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov019_02112268.cpp"  # ov019 +0x370
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov027_02111cfc.cpp"  # ov027 +0x3cc
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov030_02114134.cpp"  # ov030 +0x3a4
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov072_0211fc3c.cpp"  # ov072 +0x38c
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov072_02120560.cpp"  # ov072 +0x328
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov077_02124718.cpp"  # ov077 +0x3f0
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov077_02125e20.cpp"  # ov077 +0x3d0
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov081_02127708.cpp"  # ov081 +0x3dc
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov085_0212a430.cpp"  # ov085 +0x350
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov096_021368b4.cpp"  # ov096 +0x384
    PROPERTIES COMPILE_OPTIONS "/vmg;/vmm")

# run linkw wave 18 (lane w18): the four matched dispatchers reclaimed from
# host copies by the R9 sweep above. Their four host bodies are deleted in the
# same commit -- BabyPenguin_StateTick.cpp entirely (the whole file was that
# one body), and one function each out of Scuttlebug_StateDispatch.cpp,
# MrI_StateDispatch.cpp and Bbh_PmfDispatch.c, whose remaining bodies and table
# seats are untouched and still needed. Same one-owner contract as the lane
# slices above: a symbol is defined by the matched TU or by a host copy, never
# both.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w18a.txt" SLICE_W18A_LINES)
set(SLICE_W18A_SOURCES "")
foreach(line IN LISTS SLICE_W18A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W18A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w15c.txt" SLICE_W15C_LINES)
set(SLICE_W15C_SOURCES "")
foreach(line IN LISTS SLICE_W15C_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W15C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
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
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/func_02073244_hostcopy.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/SetNumPlayers_hostcopy.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Door_CleanupResources.cpp")

# ---- the NEW-FILE OPENING (run lvled, lane intro-cutscene) ------------------
# Everything the ROM's own intro gate reaches once it is allowed to fire. All of
# it was already MATCHED and simply never offered, because with the intro bit
# force-set in hal/level_boot.cpp none of it was reachable. See slice_intro.txt
# for what each group is and the fault that named it.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_intro.txt" SLICEINTRO_LINES)
set(SLICEINTRO_SOURCES "")
foreach(line IN LISTS SLICEINTRO_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICEINTRO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# The kuppa command dispatcher is a HOST COPY (the gate-16 pointer-to-member
# case; its src line is commented out in slice_gate10.txt), and CUTSCENE_OBJECT
# gets its own class file rather than an append to actor_classes.cpp.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/intro_kuppa_dispatch.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/actor_classes_intro.cpp")
# r0-passthrough: func_ov085_0212d5dc is LakituBro's last opening state and it
# calls func_ov002_020c3e8c() with the player riding ARM r0 -- the fifth
# instance of the seam its four siblings already have host copies for, and the
# three lines that END the opening (hand control back, then the ROM's own
# "intro seen" write). Src line commented out in slice_gate18.txt.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov085_LakituBro_IntroEnd.cpp")
# void* arithmetic: func_ov002_020f7780 (cutscene-object command slot 2) adds
# 0x8e to a void*, which mwcc takes as the GNU extension and MSVC C refuses with
# C2036. One cast, same address; src line commented out in slice_intro.txt.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov002_CutsceneCmd_020f7780.c")
# The opening's 55 script ARGUMENT RECORDS, hosted with real host pointers the
# way ptr_tables.cpp table 2 hosts the 39 camera-script records. They are NOT in
# romdata.py's byte-copy list on purpose: each carries a relocated word at +4,
# and a byte-copied pointer is the data_020876e4 crash.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/intro_script_records.cpp")
# data_02089dcc is hosted rather than byte-copied because port/tools/ptr_audit.py
# flagged it and was right: its single relocation points at CODE
# (func_ov007_020c020c, unhosted, and ov007 is outside this lane's grant). The
# blob is carried with that one word seated to a loud trap; the opening never
# reaches it. Removed from romdata.py's list in the same change.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/intro_dcc_blob.cpp")
# The ov002 mount's POINTER WORDS, seated with host addresses. The mount
# byte-copies, so the pointer arrays this lane added to port/ov002_syms.txt
# carried DS addresses -- one of them handed SharedFilePtr::Load 0x02130744.
# Generated and individually checked against relocs.txt; see the file header.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/intro_ov002_seat.cpp")
# The cutscene cast's LOAD SPINE, host-copied to trace the three LoadFile paths
# per object. The render floor is an unrebased BMD offset and Model::LoadFile
# only rebases at numRefs == 1; this names which cast member is the FIRST holder
# of each file and through which class. Src line commented out in slice_intro.txt.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov002_CutsceneLoad_020f6618.cpp")
# CutsceneObject::Render dispatches its model through a LOCAL SHADOW class
# counted in ROM/Itanium numbering, so its `m` reaches ROM slot 5 =
# ModelAnim::Render(Vector3 const *). MSVC folds the D1/D0 pair, so index 5 on
# the host table is _ZTV9ModelAnim[5] = ModelAnim::Virtual18(u32, Vector3 const*)
# -- a different function with a different arity, and the renderer then walked an
# Entry chosen by a garbage index. cxxname_bridge.cpp predicted this TU by name.
# One dropped declaration; body otherwise the matched source. Src line commented
# out in slice_intro.txt.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/CutsceneObject_Render.cpp")
# The same one-slot shift, one file over. CutsceneObject::CleanupResources
# destroys its model through a shadow whose `m04` is index 1 -- the ROM's
# DELETING destructor, and MSVC's Model::DoSetFile. Cleaning up the cast would
# have called DoSetFile on a model being freed. Src line commented out in
# slice_intro.txt.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/CutsceneObject_CleanupResources.cpp")
# And the third instance, one layer down: the cutscene ModelAnim's own deleting
# destructor destroys its TextureSequence through a hand-indexed vtable word at
# ROM slot 1. MSVC folds D1/D0, so hal/model_dtor_seat.cpp seats that body at
# index 0 and leaves index 1 NULL -- measured as a call through a null pointer
# (+ffc00000 is address 0 against this image's 0x00400000 base). The calling
# convention moves with the index: what is seated there is __fastcall.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov002_ModelAnimD0_020f6778.cpp")
# Cutscene-object command 4, the character voice. hal/intro_kuppa_dispatch.cpp
# trapped it as "needs Sound::PlayCharVoice (unhosted)"; the running chain
# reached it and the trap said so, which is what the trap was left loud for. The
# premise was stale -- Sound::PlayCharVoice is in this binary already
# (slice_gate10.txt:1225, called twice by unmatched/Player_Behavior.cpp) -- so
# this hosts the command and drags in nothing. A host copy only because the
# matched TU declares its callee inside `namespace Sound`, whose MSVC mangling
# has no alias in hal/cxx_aliases.cpp.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov002_KuppaCmd_020bd480.cpp")
# And command 9, the knockback, for the same reason on the next run: the trap
# fired, and Player::Hurt / Vec3_RotateYAndTranslate / Particle::System::
# NewSimple are all already in this binary too. A host copy because the matched
# TU reaches Hurt through a local shadow CLASS, which makes MSVC mangle it as a
# thiscall member and ride `this` in ecx.
list(APPEND SLICEINTRO_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov002_KuppaCmd_020bd250.cpp")

# THE SCENE BOOT: the star select, the one non-level scene the port can enter.
# ov003 carries three scene classes -- dScTitle_c (id 2), dScStarSel_c (id 4)
# and dScGameOver_c (id 8) -- and ONLY id 4 is seated; the other two are
# derived to the same depth and blocked, which slice_scene1.txt spells out. It
# is reached through the ROM's own Scene::SetSceneToSpawn ->
# Scene::SpawnIfNecessary -> func_02043098 spawn spine. hal/scene_boot.cpp
# seats that one ACTOR_SPAWN_TABLE record and fills its vtable; port_scene_run
# drives the frame. SM64DS_SCENE=<id> in walk_window takes the run, and an
# unhosted id is refused by name. Eighteen of the sixty src TUs this slice
# lists are /OPT:REF-dropped on purpose; see slice_scene1.txt for why every TU
# here links off the ROM's own edges rather than a link root, and
# scene_boot_map.txt section 8 for which eighteen do not link at all.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_scene1.txt" SLICE_SCENE1_LINES)
set(SLICE_SCENE1_SOURCES "")
foreach(line IN LISTS SLICE_SCENE1_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SCENE1_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run link60 lane L2: THE SECOND SCENE, and the biggest single linkage move the
# port has made. dScDSMT_c is ov007's one scene class -- the TITLE SCREEN and
# FILE SELECT, actor id 1 -- and it rides the identical spine: the ROM's
# ACTOR_SPAWN_TABLE record at 0x02103264 (hosted by the ov007 mount), the
# eighteen-slot _ZTV5Scene shape at 0x021032e8 filled in place, and
# Scene::SetSceneToSpawn -> Scene::SpawnIfNecessary. SM64DS_SCENE=1 takes it.
#
# 619 src TUs, which is not a slice of ov007 but very nearly the whole overlay:
# a relocation BFS from the class's eight entry points reaches all 548 of its
# functions, so the title screen is what ov007 is for. See slice_ov007.txt for
# the derivation and the one exclusion, and port/ov007_seat.txt for the vtable
# table, the two spine-caveat audits and what actually links.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov007.txt" SLICE_OV007_LINES)
set(SLICE_OV007_SOURCES "")
# TWO FLAGS AND NOT ONE, because the review caught them being conflated. A
# single "displaced" flag answers "did the cut run", and the marker block below
# needs "is the matched body in this binary", which is NOT the same question:
# both a cut line and a MISSING line leave the TU out of the build, and only
# one of those is a healthy tree. Read separately, every message below can be
# scoped to the state it actually names.
set(RT1_BE980_IN_SLICE FALSE)      # the slice file lists the line
set(RT1_BE980_DISPLACED FALSE)     # and this block cut it, so a host copy runs
set(RT1_ADD3C_IN_SLICE FALSE)
set(AE1_ADD3C_DISPLACED FALSE)     # run link60 lane AE1, the same pair one
                                   # frame deeper: listed, and cut
set(AE1_BBFF0_COMPILED FALSE)      # and the NEXT blocker's caller: compiled,
                                   # set where the append happens, not where
                                   # the line is read
set(TS1_B6EB4_IN_SLICE FALSE)      # run link60 Stage 5 lane TS1, the third
set(TS1_B6EB4_DISPLACED FALSE)     # displacement: listed, and cut
set(IR1_C2B9C_IN_SLICE TRUE)       # mg16 arc 4: the matched TU is back in the
                                   # slice as itself, so the VOID_DEF guard far
                                   # below can still fire if anyone reintroduces
                                   # the `void` spelling. Left TRUE deliberately:
                                   # with it FALSE the guard is silently dead.      # run mg15 lane IMPLICITR, the fourth
set(IR1_C2B9C_DISPLACED FALSE)     # displacement and the first on the RETURN
                                   # side: listed, and cut
set(TIDEP_B4B5C_DISPLACED FALSE)   # run mg15 lane TITLEDEPTH, the fourth
# DECLARED HERE AND NOT BESIDE ITS SIBLING SWITCH, because the loop below reads
# it and CMake evaluates this file top to bottom. Declared further down it would
# still work from the command line -- -D writes the cache before this file runs
# -- and its DEFAULT would be an empty variable up here, which is the shape of a
# switch that is right for the wrong reason.
option(TIDEP_ARITY_PLAIN
       "run mg15 TITLEDEPTH: keep src/func_ov007_020b4b5c.c in the port slice, \
so the title's cell sizer is called without its camera (pre-fix arm)"
       OFF)
foreach(line IN LISTS SLICE_OV007_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # run link60 lane RT1: THE ONE DISPLACEMENT IN THIS SLICE.
    #
    # func_ov007_020be980 is MATCHED and stays matched. It is cut from the port
    # only, because it is the frame where scene 1's ARM register ride-through
    # destroys information: its caller sets r1 and r2, its callee reads them,
    # and this body -- correctly, on ARM -- names neither. On MSVC that is two
    # stack slots nobody wrote. The seat's shape was ruled by the PC2 review
    # before it was taken; the derivation, the caller sweep that fixes the
    # signature, and why the other two frames are the wrong ones to displace
    # are all in port/unmatched/Ov007_RideThrough_020be980.cpp's header and
    # port/ov007_seat.txt sections 5b and 5c.
    #
    # THE FLAGS ARE NOT BOOKKEEPING. The PC2 marker block below reads them, so
    # the cut and the marker's retirement are the same fact and cannot drift:
    # delete this if-block to put the TU back and the marker re-arms in that
    # configure, unasked, because the same edit that restores the body clears
    # the flag that retires the marker.
    if(line MATCHES "^src/func_ov007_020be980\\.c$")
        set(RT1_BE980_IN_SLICE TRUE)
        set(RT1_BE980_DISPLACED TRUE)
        continue()
    endif()
    # run link60 lane AE1: THE SECOND DISPLACEMENT IN THIS SLICE, and the
    # OPPOSITE FRAME from the one above.
    #
    # func_ov007_020add3c is MATCHED and stays matched. It is cut from the port
    # only, because it is the frame where scene 1's implicit r0 argument is
    # destroyed: the value is add3c's OWN incoming argument, its callee
    # func_ov007_020ae558 reads r0 directly, and on ARM the caller's
    # `mov r4, r0` leaves r0 holding it at the branch so there is nothing to
    # emit. On MSVC the caller pushes zero arguments and the callee reads its
    # saved esi. The seat's shape was ruled by the RT1 and SC1N reviews before
    # it was taken; the derivation, the one-call-site sweep and why the callee
    # is the wrong frame to displace are in
    # port/unmatched/Ov007_ImplicitR0_020add3c.cpp's header and
    # port/ov007_seat.txt sections 5c, 5d and 5e.
    #
    # THE FLAG IS NOT BOOKKEEPING, same as the be980 block above: the RT1
    # marker block below reads it, so the cut and the marker's retirement are
    # the same fact and cannot drift.
    #
    # WHAT THE COMMENT THAT STOOD HERE ASKED FOR WAS ONE FLAG SHORT, and it is
    # worth recording rather than quietly fixing. It said a lane seating this
    # seam "adds a `continue()` here and the marker retires in the same
    # configure". A bare `continue()` leaves RT1_ADD3C_IN_SLICE set TRUE, so
    # the predicate below would have gone on arming a marker for a blocker that
    # no longer shipped -- the exact failure RT1 caught in PC2's predicate,
    # arriving a third time. The cut therefore sets its own DISPLACED flag and
    # the predicate reads both, which is the shape the be980 block already had.
    if(line MATCHES "^src/func_ov007_020add3c\\.c$")
        set(RT1_ADD3C_IN_SLICE TRUE)
        set(AE1_ADD3C_DISPLACED TRUE)
        continue()
    endif()
    # run link60 Stage 5 lane TS1: THE THIRD DISPLACEMENT IN THIS SLICE, and
    # the same frame shape as the two above.
    #
    # func_ov007_020b6eb4 is MATCHED and stays matched. It is cut from the port
    # only, because it is the frame where the title screen's PENDING GRAPHICS
    # MODE is destroyed: the body's own guard loads *(int *)(g + 0x1c) to test
    # it, the ROM leaves that word in r0 across the branch, and
    # func_ov007_020bd648 -- which is the only writer of DISPCNT bits 8-12 in
    # the whole overlay -- reads it as its `mode`. The declaration in the
    # matched TU says `(void)` and MSVC pushes nothing.
    #
    # port/ov007_seat.txt section 5d screened this row and named THIS frame as
    # the one to displace ("displacing bd648 would be displacing the blameless
    # end", because src/func_ov007_020b7138.c's own call passes the argument
    # correctly). Section 5h is the measurement that turned the candidate into
    # a defect and the frame-0 register readings either side of the fix.
    #
    # THE FLAG IS NOT BOOKKEEPING, same as the two blocks above: the branch
    # below reads it, so the cut and the host copy's membership are the same
    # fact and cannot drift.
    if(line MATCHES "^src/func_ov007_020b6eb4\\.c$")
        set(TS1_B6EB4_IN_SLICE TRUE)
        set(TS1_B6EB4_DISPLACED TRUE)
        continue()
    endif()
    # run mg15 lane IMPLICITR: THE FOURTH DISPLACEMENT IN THIS SLICE, and the
    # FIRST ONE ON THE RETURN SIDE.
    #
    # func_ov007_020c2b9c is MATCHED and stays matched. It is cut from the port
    # only, because it is the frame where the title's IMPLICIT RETURN VALUE is
    # destroyed: its own call to func_ov007_020c2c68 leaves the new object in
    # r0, nothing between that branch and the single `bx lr` writes r0, so the
    # ROM's callee hands the object back by falling out and the matched body is
    # correct to declare `void`. Both matched callers -- func_ov007_020c2acc
    # and func_ov007_020c2b38, the byte-array and int-array twins -- already
    # declare it `void *` and take it out of r0 with `mov r5, r0`. On MSVC the
    # callee never writes EAX and both callers read whatever was left there;
    # 020c2acc then faults at +0x3f on `mov dword ptr [eax+esi*4], ecx` with
    # eax = 0x62.
    #
    # THE CALLEE IS THE ONLY FRAME THAT CAN BE DISPLACED, which is the
    # difference from the three seams above. The value is allocated inside
    # func_ov007_020c2c68 and lives in the callee's own local; no caller frame
    # holds it to forward. One cut therefore answers BOTH call sites and
    # neither caller is touched. Derivation, the ROM disassembly of every path,
    # the two-row reloc sweep and the measured src/ alternative are in
    # port/unmatched/Ov007_ImplicitReturn_020c2b9c.cpp's header.
    #
    # THE FLAGS ARE NOT BOOKKEEPING, same as the three blocks above: the
    # IMPLICITR predicate near the marker blocks reads them, so the cut and
    # what the predicate says about it cannot drift.
    # RETIRED, run mg16 arc 4: src/func_ov007_020c2b9c.c now SPELLS its return
    # (void*, `return r;`) and still byte-matches at 2004/b56, so the matched TU
    # is built as itself and the host copy is gone. The VOID_DEF regex guard
    # below is what makes that safe to assert rather than hope: it stops matching
    # the moment the definition is no longer spelled `void`.
    # run mg15 lane TITLEDEPTH: THE FOURTH DISPLACEMENT IN THIS SLICE, and the
    # same frame shape as add3c above -- one register over.
    #
    # func_ov007_020b4b5c is MATCHED and stays matched. It is cut from the port
    # only, because it is the frame where the title screen's CAMERA is
    # destroyed: it declares func_ov007_020c43bc with one parameter and calls it
    # with one, and the ROM leaves the camera in r1 across the branch (it saves
    # its own second argument into r5 at 0x020b4b78 and never writes r1 before
    # the bl at 0x020b4b80). On MSVC the callee reads a stack slot nobody
    # filled.
    #
    # THE ROW WAS ALREADY KNOWN AND ALREADY ACCEPTED: port/ov007_seat.txt
    # section 5d lists it among the sixteen faulting-direction rows, and
    # port/tools/aritycheck_plainfunc_baseline.txt line 159 carries it. What
    # this lane adds is the SYMPTOM. func_ov007_020c43bc computes the cell
    # system's world quad size as texels * depth * tan(halfFov) / 96, reading
    # the field of view from the camera's +0xd4; with the garbage pointer it
    # read an index of 256 (22.5 degrees) where the title's camera carries 6.77,
    # so every cell was cot(6.77)/cot(22.5) = 3.4877 times too large. The
    # arithmetic reproduces the observed record to the unit -- a 16-texel cell
    # came out 583 and should be 167 -- and the title's backdrop, sized through
    # the OTHER caller (func_ov007_020c41dc, which passes the camera), measures
    # exactly 1.000 DS pixels per texel in the same frame. The derivation is in
    # port/unmatched/Ov007_ImplicitR1_020b4b5c.cpp's header.
    #
    # THE FLAG IS NOT BOOKKEEPING, same as the three blocks above: the branch
    # below reads it, so the cut and the host copy's membership are one fact.
    #
    # KILL SWITCH -DTIDEP_ARITY_PLAIN=ON puts the matched TU back and drops the
    # host copy, which is the PRE-FIX arm and is a real control rather than a
    # reconstruction: that arm's binary contains exactly the translation unit
    # the base tree compiled. It is a REBUILD and not one binary, so the two
    # arms' .dsstate bases have to be read before their selftest BMPs are
    # compared -- notes/port-selftest-bmp-gate.md. The arm is printed on every
    # configure in BOTH directions, because run mg15's SQRT lane proved a CMake
    # option() caches and a switch that only announces one arm silently keeps
    # the wrong one.
    # RETIRED, run mg16 arc 4: src/func_ov007_020b4b5c.c now FORWARDS the camera
    # -- its own second argument, which the ROM leaves in r1 across the branch --
    # and still byte-matches at 2004/b56, so the matched TU is back as itself.
    # run link60 lane AE1: THE NEXT BLOCKER'S CALLER, and this flag is placed
    # where it is on purpose.
    #
    # Every cut in this loop is a `continue()` ABOVE this line, so a flag set
    # HERE, on the same statement run that appends the source, cannot say
    # "compiled" about a TU that was cut. That is the property the add3c note
    # above did not have: it recorded "listed", the cut recorded nothing, and a
    # lane following its instructions to the letter would have left the marker
    # armed. Membership flags for this loop belong here from now on.
    if(line MATCHES "^src/func_ov007_020bbff0\\.c$")
        set(AE1_BBFF0_COMPILED TRUE)
    endif()
    list(APPEND SLICE_OV007_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# run mg12 lane TITLE seated THE TWO COMMS SLOT-INDEX FRAMES here as host
# copies. RUN MG15 LANE MP1 RETIRED THEM: the defect was never the frames, it
# was include/decl_common.h declaring func_0203dabc and func_0203dae4 as (void)
# when both matched definitions take the comms slot index -- which the ROM
# leaves in r0 from the func_0203da9c() call immediately before, so ARM is
# correct and a stack ABI reads an unwritten slot.
#
# The header now declares both with the parameter, and the one linked ROM
# caller passes it: src/func_ov007_020cc2cc.c does
#     func_0203dabc(func_0203da9c())
# which is the same two bl's on ARM. STILL A MATCH at 2004/b56 with strict
# relocs after the change; that gate is the whole reason the header could be
# fixed instead of worked around. The two matched TUs are therefore back in
# the link as themselves, and port/unmatched/Comms_SlotIndex_*.cpp are gone.
#
# WHY IT MATTERED NOW: mg15's MP1 lane links src/func_0203bb60.c and
# src/func_0203bc7c.c, the ROM's four-player fan-out, and those are precisely
# the two callers that pass a REAL index rather than riding slot 0 through r0.
# A face that ignores its argument would have served all four players slot 0's
# record and the four-slot path would have looked like it worked.
# RETIRED, run mg16 arc 4. src/func_ov007_020b4b5c.c now forwards the camera --
# its own second argument, the one the ROM leaves in r1 across the branch at
# 0x020b4b80 -- and it STILL byte-matches at 2004/b56 (30 words, 0 MISMATCH,
# verified against func_ov007_020b63e4 as the alignment control). So the matched
# TU is in the build as itself, unmatched/Ov007_ImplicitR1_020b4b5c.cpp is
# deleted, and TIDEP_ARITY_PLAIN is gone with it: the pre-fix arm it selected
# cannot be reconstructed from a source that no longer contains the defect.
#
# THE DISPLACEMENT WAS NEVER WRONG -- it was correct for a tree where the fix
# could not be spelled in the matched source. What changed is that it can:
# three implicit-argument defects in a row (020c78b0, 020cc600, the
# 02013edc/02042fe4 veneer pair) recovered with the byte match intact, which is
# what made this worth re-attempting rather than assuming settled.
if(AE1_ADD3C_DISPLACED)
    list(APPEND SLICE_OV007_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov007_ImplicitR0_020add3c.cpp")
elseif(NOT RT1_ADD3C_IN_SLICE)
    # Same two opposite states the be980 branch below distinguishes, and for
    # the same reason: the slice line is the only thing that puts the matched
    # TU in the build, so if the line is gone the host copy must not be added
    # blind. Two bodies for one address is an LNK2005 and none is a silent
    # undefined symbol.
    message(STATUS "AE1: slice_ov007.txt no longer lists "
                   "src/func_ov007_020add3c.c, so the implicit-r0 host copy is "
                   "NOT added and the link will fail on it. Restore the line: "
                   "this block does the cutting, not the slice file")
endif()
# RETIRED, run mg16 arc 4. src/func_ov007_020c2b9c.c now SPELLS the return its two
# matched callers already declare -- `void*` and `return r;` -- and STILL
# byte-matches at 2004/b56 (23 words, 0 MISMATCH). The matched TU is in the build
# as itself and unmatched/Ov007_ImplicitReturn_020c2b9c.cpp is deleted.
#
# The VOID_DEF regex guard further down is deliberately LEFT IN PLACE: it greps
# the source for a `void func_ov007_020c2b9c(` definition, so it stops matching
# now and starts matching again the moment anyone reintroduces the void spelling.
# That is the check that makes this retirement safe to assert rather than hope.
if(TS1_B6EB4_DISPLACED)
    list(APPEND SLICE_OV007_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov007_BdPending_020b6eb4.cpp")
elseif(NOT TS1_B6EB4_IN_SLICE)
    # The two states the branches below distinguish, for the same reason: the
    # slice line is the only thing that puts the matched TU in the build, so if
    # the line is gone the host copy must not be added blind. Two bodies for
    # one address is an LNK2005 and none is a silent undefined symbol.
    message(STATUS "TS1: slice_ov007.txt no longer lists "
                   "src/func_ov007_020b6eb4.c, so the pending-mode host copy is "
                   "NOT added and the link will fail on it. Restore the line: "
                   "this block does the cutting, not the slice file")
endif()
if(RT1_BE980_DISPLACED)
    list(APPEND SLICE_OV007_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov007_RideThrough_020be980.cpp")
elseif(NOT RT1_BE980_IN_SLICE)
    # The slice line is the only thing that puts the matched TU in the build,
    # so if it is gone the host copy must not be added blind: two bodies for
    # one address is an LNK2005, and none is a silent undefined symbol. Say
    # which state the tree is in rather than guessing.
    #
    # THE LIKELY WAY TO LAND HERE is following the port's OTHER idiom for this.
    # unmatched/Actor_ClosestPlayerWrappers.cpp answers the same defect class by
    # COMMENTING the src line out of slice_gate16.txt and slice_gate89.txt and
    # appending its host copy unconditionally; gate 10's and gate 14's
    # displacements instead leave the line and filter it here. Both are in the
    # tree and neither is wrong. This seam uses the filter because the marker
    # predicate below has to read whether the TU is in the build, and a flag set
    # by the cut itself is the only form of that which cannot drift. If you
    # commented the line out, put it back and let this filter do the cutting.
    message(STATUS "RT1: slice_ov007.txt no longer lists "
                   "src/func_ov007_020be980.c, so the ride-through host copy is "
                   "NOT added and the link will fail on it. Restore the line: "
                   "this block does the cutting, not the slice file")
endif()

# run link60 Stage 5 lane T2: THE HARDWARE DIVIDER'S LAST RAW CALLER.
#
# THE TITLE SCREEN'S 3D LAYER RASTERISED NOTHING BECAUSE ITS PROJECTION MATRIX
# HAD A ZERO X SCALE, and the zero comes from here rather than from anything
# graphical. The chain, measured rather than argued (port/ov007_seat.txt 5i):
#
#   src/func_ov007_020c0128.cpp   the title camera's init writes its ASPECT
#                                 field as `_ZN4cstd4ldivEii(0x4000, 0x3000)`,
#                                 4/3 through the DS hardware divider
#   src/func_ov007_020c0078.c     reads it back, `>> 0x14`, and hands it to
#                                 G3i::PerspectiveW_ as the aspect
#   PerspectiveW_                 divides the cotangent by it for mtx[0]
#
# and cstd::ldiv returned a dead word, so the aspect arrived 0, so mtx[0]
# arrived 0, so every vertex projected to clip x = 0 -- the framebuffer's
# vertical centre line, which is exactly where the audit found all 96
# triangles.
#
# WHY IT RETURNED A DEAD WORD. src/_ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE.c
# reaches DIVCNT, DIV_NUMER and DIV_DENOM through plain literal-address stores,
# and src/_ZN4cstd4ldivEii.c reads DIV_RESULT the same way. ntr maps real
# memory across the DS I/O window, so those stores latch and ntr::run_divide
# is never entered -- the identical failure shape hostgen.py's own header
# records for MTX_SCALE. hostgen rewrites the derefs to NTR_MMIO, which is the
# door into ntr::io_write, and the unit runs.
#
# THE BLAST RADIUS IS ONE CALLER AND IT WAS SWEPT, not assumed. cstd::fdiv --
# the divider's other client and the one every level uses -- is a HOST FACE in
# hal/cstd_div.c (pure C, no register), so src/_ZN4cstd4fdivEii.c is not in the
# link at all; the map has __ZN4cstd4fdivEii coming from cstd_div.c.obj.
# cstd::ldiv's only caller anywhere in src/ is func_ov007_020c0128, and
# fdiv_async's only callers are cstd::fdiv and cstd::ldiv. So the only live
# path this changes is the title screen's camera init.
#
# BOTH TUs AND NOT JUST THE WRITE SIDE. run_divide publishes its result with
# raw_write into the same mapped window, so a raw READ of DIV_RESULT would
# have worked once the writes were proxied, and hostgen'ing ldiv is not what
# makes the frame change. It is here so the busy-spin, the result read and the
# stores all go through one door: the day ntr models DIV_BUSY, or serves
# DIV_RESULT from anywhere but the window, a half-routed caller would desync
# silently and this is the caller that already cost one lane a frame.
# The measurement is in 5i: fdiv_async alone is load-bearing today.
set(T2_DIV_SYMS _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE _ZN4cstd4ldivEii)
set(T2_DIV_GEN "")
foreach(sym IN LISTS T2_DIV_SYMS)
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${PORT_REPO_ROOT_ABS}/src/${sym}.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND T2_DIV_GEN "${sym_out}")
endforeach()

# The plain entries have to go, or the hostgen copy is a duplicate symbol. Each
# of the two is listed in exactly ONE slice file (checked at the tip, and the
# count below is the guard: if a third listing appears the message fires and
# the link is the thing that tells you, not a silent double body).
set(T2_DIV_CUT 0)
set(SLICE_OV007_SRC_T2 "")
foreach(src IN LISTS SLICE_OV007_SOURCES)
    if(src MATCHES "/_ZN4cstd4ldivEii\\.c$")
        math(EXPR T2_DIV_CUT "${T2_DIV_CUT}+1")
        continue()
    endif()
    list(APPEND SLICE_OV007_SRC_T2 "${src}")
endforeach()
set(SLICE_OV007_SOURCES ${SLICE_OV007_SRC_T2})
set(SLICE10_SRC_T2 "")
foreach(src IN LISTS SLICE10_SOURCES)
    if(src MATCHES "/_ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE\\.c$")
        math(EXPR T2_DIV_CUT "${T2_DIV_CUT}+1")
        continue()
    endif()
    list(APPEND SLICE10_SRC_T2 "${src}")
endforeach()
set(SLICE10_SOURCES ${SLICE10_SRC_T2})
if(NOT T2_DIV_CUT EQUAL 2)
    message(STATUS "T2: expected to cut 2 plain divider entries and cut "
                   "${T2_DIV_CUT}. If a slice file stopped listing one of "
                   "src/_ZN4cstd4ldivEii.c or "
                   "src/_ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE.c the hostgen "
                   "copy is still added and the link will say so; if a THIRD "
                   "slice started listing one, this cut missed it and the link "
                   "will say that too")
endif()

# run mg14 lane TITLE2: THE DIVIDER'S ACTUAL LAST RAW CALLER, cstd::mod.
#
# The T2 block above says "THE HARDWARE DIVIDER'S LAST RAW CALLER" and it was
# wrong by one. Its blast-radius sweep bounded itself to cstd::ldiv and
# fdiv_async and to cstd::fdiv (which is a host face and not in the link at
# all). It never asked about src/_ZN4cstd3modEii.c, which reaches DIVCNT,
# DIV_NUMER and DIV_DENOM through the same plain literal-address stores and
# reads DIV_REM (0x040002a8) back the same way, and which slice_ov007.txt
# listed as a plain entry. Same failure, same fix, one caller later.
#
# WHAT IT ACTUALLY DID, measured on this tree's own binary rather than argued.
# ntr maps real memory across the DS I/O window, so a plain TU's stores latch
# and ntr::run_divide is never entered; the read of DIV_REM then returns
# whatever the last HOSTGEN-ROUTED divide left in the window. cstd::mod was
# therefore a function that returns the PREVIOUS divide's remainder. The probe
# in hal/scene_boot.cpp prints the demonstration on one line:
#
#     div1000_7=142 mod419_40=6(want19)
#
# 1000/7 is 142 remainder 6, and cstd::mod(419, 40) answered 6. cstd::div is
# correct because slice_gate29.txt routes its whole gate through hostgen; this
# is the asymmetry that hid it, because the two live one line apart in src/.
#
# WHAT IT COST, and this is why a title-screen lane is editing a divider.
# func_ov007_020b0da0 is the title's attract callback (MATCH at 2004/b56). It
# computes var_r4 from cstd::mod(counter, 0x28) and ends the attract with
#
#     if ((st4 & 2) && (var_r4 == 0 || var_r4 == 0x800)) scene->f_100->f_18 = 0;
#
# With a stale mod, var_r4 was never 0 and never 0x800 (measured 614), so the
# callback pointer was never dropped, so func_ov007_020b0a20's
# `if (scene->f_100->f_18 == 0)` re-arm never fired, so func_ov007_020be980
# never restarted the logo animation, so it stayed PAUSED at frame 0 of 157 --
# f0 == 1 being the one value func_ov007_020c9110 refuses to step -- and
# scene 1 sat in state 11 forever. The touch was being HEARD the whole time:
# the same body's stylus hotspot sets st4 |= 2 on the press frame.
#
# BLAST RADIUS, SWEPT NOT ASSUMED. `grep -rl _ZN4cstd3modEii src/` is 15 files:
# the definition and fourteen callers, ALL of them ov007 --
# 020ae2d0 020aefb4 020af260 020b0da0 020b1cf0 020b2e64 020b9ca8 020ba368
# 020baa10 020bad8c 020bb4b0 020bed60 020c49bc 020c9d90. So nothing outside the
# title screen and file select changes behaviour here, and inside it every one
# of those fourteen was reading a stale remainder. src/ is NOT edited: this is
# a build-time routing exactly like T2's, so the byte gate cannot move.
#
# ONE SLICE LISTS IT (port/slice_ov007.txt), and the cut below is guarded on
# that count the way T2's is: if a second slice ever lists it the hostgen copy
# becomes a duplicate symbol and the link says so, and if this cut ever misses
# the plain entry the link says that too.
# THE A/B THIS ROUTING GETS AND T2's DID NOT. -DTI2_MOD_PLAIN=ON puts the plain
# entry back and drops the hostgen copy, so the before/after is two builds off
# ONE tree with ONE difference, which is what a regression on a build-time
# change can honestly be. It is a REBUILD and not one binary, so the two arms'
# .dsstate bases have to be read before their BMPs are compared -- see
# notes/port-selftest-bmp-gate.md, whose rule this switch exists to satisfy
# rather than to dodge. Default OFF: the routing is the shipped behaviour.
option(TI2_MOD_PLAIN "run mg14 TITLE2: build cstd::mod PLAIN (pre-fix arm)" OFF)
set(TI2_MOD_SYMS _ZN4cstd3modEii)
if(TI2_MOD_PLAIN)
    set(TI2_MOD_SYMS "")
    message(STATUS "TI2: TI2_MOD_PLAIN=ON -- cstd::mod builds PLAIN. This is "
                   "the PRE-FIX arm: the DS divider is never entered and "
                   "cstd::mod returns the previous proxied divide's remainder.")
endif()
set(TI2_MOD_GEN "")
foreach(sym IN LISTS TI2_MOD_SYMS)
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${PORT_REPO_ROOT_ABS}/src/${sym}.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND TI2_MOD_GEN "${sym_out}")
endforeach()

set(TI2_MOD_CUT 0)
set(SLICE_OV007_SRC_TI2 "")
foreach(src IN LISTS SLICE_OV007_SOURCES)
    if(src MATCHES "/_ZN4cstd3modEii\\.c$" AND NOT TI2_MOD_PLAIN)
        math(EXPR TI2_MOD_CUT "${TI2_MOD_CUT}+1")
        continue()
    endif()
    list(APPEND SLICE_OV007_SRC_TI2 "${src}")
endforeach()
set(SLICE_OV007_SOURCES ${SLICE_OV007_SRC_TI2})
if(TI2_MOD_PLAIN)
    # The A arm cuts nothing on purpose, so satisfy the guard rather than
    # letting it print a scary line about a cut that was never meant to happen.
    set(TI2_MOD_CUT 1)
endif()
if(NOT TI2_MOD_CUT EQUAL 1)
    message(STATUS "TI2: expected to cut 1 plain cstd::mod entry and cut "
                   "${TI2_MOD_CUT}. If slice_ov007.txt stopped listing "
                   "src/_ZN4cstd3modEii.c the hostgen copy is still added and "
                   "the link will say so; if a SECOND slice started listing "
                   "it, this cut missed it and the link will say that too")
endif()

# run mg15 lane SQRT: THE SQUARE-ROOT UNIT'S FIFTH CLIENT, func_02053274.
#
# SQRTUNIT_SYMS above routes the unit's four DRIVERS (func_02053008,
# func_02053130, func_020531a4, func_02052fdc). It does not route the unit's
# fifth client, and that one is not a driver at all: src/func_02053274.c is the
# 3D DISTANCE function. It squares the three axis deltas, shifts the sum left by
# 2, drives the unit with that as a 64-bit operand and returns
# (SQRT_RESULT + 1) >> 1, the rounded distance. slice_gate10.txt listed it plain.
#
# WHY IT SURVIVED FOUR ROUNDS OF THIS. Every sweep for raw MMIO before mg14
# looked for a CAST-DEREF of a literal. This body reaches SQRTCNT (0x40002b0)
# and BOTH HALVES of SQRT_PARAM (0x40002b8, 0x40002bc) through a POINTER BOUND
# TO A LITERAL --
#
#     volatile unsigned int *param = (volatile unsigned int *)0x40002b8;
#     param[0] = (unsigned int)sq;  param[1] = (unsigned int)(sq >> 32);
#
# -- and reaches only SQRT_RESULT by a cast-deref. So a deref-only scan sees one
# harmless RESULT READ and files the body as safe, while the two stores that
# actually start the unit are invisible to it. hostgen's MMIO_BIND transform is
# exactly the thing that sees them; it emits NINE rewrites on this body, and
# port/tools/hostgen.py's own audit of source-touches-versus-NTR_MMIO-sites is
# the check that says none was missed (9 touches, 9 sites, same address
# multiset, nothing left plain, and param[1] scaled to 0x40002bc).
#
# WHAT IT DID. ntr maps real memory across the DS I/O window, so a plain TU's
# stores latch and ntr::run_sqrt is never entered; the read of SQRT_RESULT then
# returns whatever the last HOSTGEN-ROUTED sqrt left there. func_02053274 was a
# distance function that answers the PREVIOUS square root. The self-test in
# hal/scene_boot.cpp (SM64DS_SQRT_SELFTEST=1) asks it two roots it cannot get
# wrong, each seeded by the routed func_020531a4 so the broken answer is
# PREDICTED and not merely wrong.
#
# AND IT CORRECTS A COMMENT THAT SAID THIS WAS ALREADY FIXED. ntr/io.cpp's
# sqrt-trigger block claimed the widening "silently fixed a SECOND split-half
# SQRT_PARAM writer already in the build: func_02053274". It cannot have:
# widening io_write's dispatch only helps a store that REACHES io_write, and a
# plain TU's stores never do. That comment is corrected in the same commit as
# this routing, because it is the reason nobody looked here for a year.
#
# BLAST RADIUS, SWEPT NOT ASSUMED. Three direct callers, eight TUs to depth 3,
# and every one of the eight is in a slice list, so every one is in the link:
#
#   in-level engine (slice_gate10)
#     Player::St_NoControl_Main -> func_ov002_020c8a4c     dist < 0xa000 gate
#     Player::St_YoshiPower_Main -> func_ov002_020d6998
#                                -> func_ov002_020d6dac    raycast hit distance
#                                                          -> self+0x654, then a
#                                                          Particle::System::New
#   title / ov007 (slice_ov007)
#     func_ov007_020c65bc -> func_ov007_020c25fc
#                         -> func_ov007_020c2e90            (d<<12) > r range test
#
# Nothing reaches it indirectly: the address 0x02053274 appears in no pointer
# table in the tree. src/ is NOT edited -- this is a build-time routing, so the
# byte gate cannot move.
#
# WHAT THE BREAKAGE COST, in the callers' own terms. func_ov002_020c8a4c is
# sub-state 15 of Player::St_NoControl_Main: it faces the player at a target
# point (self+0x744), and the whole body turns on
#
#     if (func_02053274(self+0x5c, self+0x744) < 0xa000)   /* 0xa000 = 10.0 */
#
# which is an ARRIVAL test -- "am I close enough to the target yet". With a
# stale sqrt that test compares a number that never changes against a constant,
# so it is either permanently true or permanently false for the whole scripted
# move, and which one it is depends on what the last routed sqrt happened to
# leave in the register. func_ov002_020d6dac stores the raycast hit distance
# straight into self+0x654 and then spawns a particle at the hit point, so its
# stale value is a wrong distance written into player state. func_ov007_020c2e90
# is a two-sided range test, (d<<12) > r and r <= (e<<12).
#
# AND WHAT THE REGRESSION COULD AND COULD NOT SEE, measured rather than
# inferred. Five paired captures (scenes 1, 4, 374 and level 1 at two frame
# counts each) came back BYTE-IDENTICAL across the two arms. That is only
# reassuring once you know whether the body ran in those frames at all, so
# ntr::sqrt_runs() was added to count entries to run_sqrt -- a plain build never
# enters it, so the arms' difference IS the call count the routing added:
#
#     scene 1  @300   4734 vs  4734     delta 0
#     scene 4  @300   1200 vs  1200     delta 0
#     scene 374@300      0 vs     0     delta 0
#     level 1  @300  56404 vs 56404     delta 0
#     level 1  @450  82988 vs 82988     delta 0
#
# The counter is demonstrably live (level 1 runs the unit 56,404 times in 300
# frames); func_02053274 is simply not on any of those paths. Both Player
# callers are sub-states a W-held walk never enters, and the ov007 chain hangs
# off func_ov007_020c65bc, which nothing in src/ calls by name. So the identical
# pixels are a measured NOT-REACHED, not a fix that does nothing -- and no run
# in the standard regression set can confirm the repair in pixels. The self-test
# is the confirmation, and it is why this change has one.
#
# ONE SLICE LISTS IT (port/slice_gate10.txt) and the cut below is guarded on
# that count the way TI2's and T2's are: if a second slice ever lists it the
# hostgen copy becomes a duplicate symbol and the link says so, and if this cut
# ever misses the plain entry the link says that too.
#
# THE A/B. -DSQ_DIST_PLAIN=ON puts the plain entry back and drops the hostgen
# copy, so the before/after is two builds off ONE tree with ONE difference. It
# is a REBUILD and not one binary, so the two arms' .dsstate bases have to be
# read before their BMPs are compared -- see notes/port-selftest-bmp-gate.md.
# Default OFF: the routing is the shipped behaviour.
# AND IT SAYS WHICH ARM IT IS ON EVERY CONFIGURE, not only when the switch is
# on. `option()` writes to CMakeCache.txt, so a build dir that was once
# configured with -DSQ_DIST_PLAIN=ON STAYS on the pre-fix arm when the next
# configure omits the flag entirely. That cost this lane two builds: both were
# labelled fix-on, both were actually pre-fix, and the only reason it did not
# reach the write-up is that the self-test printed BROKEN out of a binary that
# was supposed to be fixed. A guard that only speaks when the switch is set
# cannot catch its own stale cache, so this one always speaks. Pass the flag
# explicitly on BOTH arms (-DSQ_DIST_PLAIN=OFF and =ON) and read this line back
# out of the build log before trusting either binary.
option(SQ_DIST_PLAIN "run mg15 SQRT: build func_02053274 PLAIN (pre-fix arm)" OFF)
set(SQ_DIST_SYMS func_02053274)
if(SQ_DIST_PLAIN)
    set(SQ_DIST_SYMS "")
    message(STATUS "SQRT ARM: SQ_DIST_PLAIN=ON -- func_02053274 builds PLAIN. "
                   "This is the PRE-FIX arm: the DS square-root unit is never "
                   "entered and the 3D distance function returns the previous "
                   "routed sqrt's result.")
else()
    message(STATUS "SQRT ARM: SQ_DIST_PLAIN=OFF -- func_02053274 is ROUTED "
                   "through hostgen. This is the SHIPPED arm.")
endif()
set(SQ_DIST_GEN "")
foreach(sym IN LISTS SQ_DIST_SYMS)
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${PORT_REPO_ROOT_ABS}/src/${sym}.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND SQ_DIST_GEN "${sym_out}")
endforeach()

set(SQ_DIST_CUT 0)
set(SLICE10_SRC_SQ "")
foreach(src IN LISTS SLICE10_SOURCES)
    if(src MATCHES "/func_02053274\\.c$" AND NOT SQ_DIST_PLAIN)
        math(EXPR SQ_DIST_CUT "${SQ_DIST_CUT}+1")
        continue()
    endif()
    list(APPEND SLICE10_SRC_SQ "${src}")
endforeach()
set(SLICE10_SOURCES ${SLICE10_SRC_SQ})
if(SQ_DIST_PLAIN)
    # The pre-fix arm cuts nothing on purpose, so satisfy the guard rather than
    # letting it print a scary line about a cut that was never meant to happen.
    set(SQ_DIST_CUT 1)
endif()
if(NOT SQ_DIST_CUT EQUAL 1)
    message(STATUS "SQRT: expected to cut 1 plain func_02053274 entry and cut "
                   "${SQ_DIST_CUT}. If slice_gate10.txt stopped listing "
                   "src/func_02053274.c the hostgen copy is still added and the "
                   "link will say so; if a SECOND slice started listing it, this "
                   "cut missed it and the link will say that too")
endif()
# ---- run mg15 lane TITLETEX: ov007's GEOMETRY COMMAND PORTS ----------------
#
# THE SAME DEFECT AS TI2 ABOVE, one register block over, and this is the one
# the owner has been looking at since his first title-screen test: "any other
# graphics than those have jumbled glitchy textures", and on the fresh f300 /
# f450 captures, "its just a big glitched yellow texture".
#
# WHAT WAS MEASURED, before anything was changed. SM64DS_MTX_BALANCE=1 (added
# to port/ntr/gx.cpp by this lane) prints, per command port, the commands the
# geometry engine EXECUTED next to the word sitting in the mapped I/O window at
# that same port. On SM64DS_SCENE=1, 300 frames, off the unmodified tree:
#
#     MTX_PUSH   04000444  exec        0  latch 00000000
#     MTX_POP    04000448  exec        0  latch 00000001   <-- STORED, NEVER EXECUTED
#     MTX_MULT_4x3 04000464 exec    17951
#     MTX_SCALE  0400046c  exec    11051
#     VIEWPORT   04000580  exec        0  latch bfff0000   <-- STORED, NEVER EXECUTED
#     SWAP_BUFFERS 04000540 exec       0  latch 00000002   <-- STORED, NEVER EXECUTED
#     pos_sp = 0 on every one of the 300 frames, and 0 at exit
#
# ZERO MATRIX PUSHES AND ZERO POPS IN A WHOLE RUN, against 17,951 matrix
# multiplies. The position stack level never leaves 0 -- read per frame, not at
# one endpoint, because an endpoint cannot tell a bracket that balanced from
# one that never ran. The title screen brackets each of the ~27 objects it
# places per frame with MTX_PUSH ... MTX_POP, and not one of those brackets
# reaches the engine: src/func_ov007_020c421c.c, _020ca5f0.c and _020b413c.c
# write the push and the pop as PLAIN literal stores, and ntr maps real memory
# across 0x04000000, so each one latches a word and the geometry engine is
# never told. Everything the bracket was meant to contain -- the MTX_MULT_4x3
# that src/func_ov007_020c42f8.c submits through func_0205532c, which IS routed
# and DOES arrive -- stays applied to the live matrix for the rest of the
# frame, and the model drawn after it is transformed by an accumulated product
# nothing in the ROM ever computes. The audit's geometry census is the picture
# of that: bounding box x -227.094 .. 1025.594, y -480.132 .. 811.985 on a
# 512x384 panel, 0 degenerate, projection correct. The frame is one surface
# stretched past every edge, which is the yellow.
#
# THE SAME THREE FILES DROP THEIR MTX_SCALE the same way, so every object also
# loses the scale it was placed at, and src/func_ov007_020c0078.c -- the
# title's own perspective setup -- drops MTX_STORE and VIEWPORT. The viewport
# word it stores is bfff0000, which decodes to (0,0)-(255,191), the whole DS
# screen; ntr::gx_reset's fallback rectangle is the same rectangle at the
# host's 2x tier, so THAT one costs nothing today and is routed for
# faithfulness rather than for a symptom. MTX_STORE is not free: its slot is
# what func_ov007_020c0078's own MTX_RESTORE readers come back to.
#
# THE TREATMENT IS THE ONE THE PORT ALREADY USES FOR THIS CLASS and it costs
# nothing in the ledger: port/tools/hostgen.py rewrites the literal derefs to
# NTR_MMIO so the store dispatches through ntr::io_write, src/ is not edited,
# and every one of these translation units stays byte-matched. It is the same
# block shape as T2's divider, TI2's cstd::mod above and the SQRTUNIT group.
#
# THE SET IS THE MEASURED ONE, not a guess. port/tools/hostgen.py's own MMIO
# patterns were run over every source in walk_window's build.ninja that is
# compiled straight out of the tree, with comments stripped so a banner
# quoting a register does not count as a writer. Twelve ov007 translation
# units reach a geometry command port that way; they are all here. THE THREE
# ov007 DIVIDER TUs THAT THE SAME SCAN FOUND (func_ov007_020c30ac,
# _020c31e0, _020c7d60) ARE DELIBERATELY NOT IN THIS SET: they are the
# DIV/SQRT unit, run mg15's SQRT lane owns that register block in the same
# wave, and two lanes routing the same class in one run is how a duplicate
# symbol arrives. They are recorded in this lane's results for the next wave.
#
# THE KILL SWITCH IS -DTITEX_GX_PLAIN=ON and it gives the plain arm back on the
# same tree, which is what a build-time routing can honestly offer. It is a
# REBUILD and not one binary, so the two arms' .dsstate bases have to be read
# before their BMPs are compared -- notes/port-selftest-bmp-gate.md. THE ARM IS
# PRINTED ON EVERY CONFIGURE, in both directions, because run mg15's SQRT lane
# proved that a CMake option() CACHES and an A/B switch that only announces one
# arm silently keeps the wrong one.
option(TITEX_GX_PLAIN
       "run mg15 TITLETEX: build ov007's geometry-port TUs PLAIN (pre-fix arm)"
       OFF)
set(TITEX_GX_SYMS
    func_ov007_020b413c    # MTX_PUSH  MTX_POP
    func_ov007_020b6c54    # SWAP_BUFFERS
    func_ov007_020c0078    # MTX_STORE  VIEWPORT
    func_ov007_020c1404    # PLTT_BASE
    func_ov007_020c1448    # TEXIMAGE_PARAM
    func_ov007_020c232c    # LIGHT_VECTOR  LIGHT_COLOR
    func_ov007_020c421c    # MTX_PUSH  MTX_POP  MTX_SCALE
    func_ov007_020c42f8    # MTX_SCALE
    func_ov007_020c49bc    # TEXCOORD VTX_16 VTX_XZ VTX_YZ BEGIN_VTXS END_VTXS
    func_ov007_020c4fcc    # COLOR  POLYGON_ATTR  TEXIMAGE_PARAM
    func_ov007_020ca5f0    # MTX_PUSH MTX_POP MTX_SCALE MTX_TRANS POLYGON_ATTR
                           # DIF_AMB SPE_EMI
    func_ov007_020ca86c)   # NORMAL  VTX_16  BEGIN_VTXS  END_VTXS
list(LENGTH TITEX_GX_SYMS TITEX_GX_WANT)
if(TITEX_GX_PLAIN)
    message(STATUS "TITEX: TITEX_GX_PLAIN=ON -- ov007's ${TITEX_GX_WANT} "
                   "geometry-port TUs build PLAIN. This is the PRE-FIX arm: "
                   "MTX_PUSH/MTX_POP/MTX_SCALE/VIEWPORT latch in mapped memory "
                   "and the geometry engine is never told.")
else()
    message(STATUS "TITEX: TITEX_GX_PLAIN=OFF -- ov007's ${TITEX_GX_WANT} "
                   "geometry-port TUs route through hostgen (shipped arm).")
endif()
set(TITEX_GX_GEN "")
if(NOT TITEX_GX_PLAIN)
    foreach(sym IN LISTS TITEX_GX_SYMS)
        # The tree spells these .c except func_ov007_020c4fcc, which is .cpp;
        # hostgen emits .cpp either way, so only the DEPENDS has to know.
        set(sym_in "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        if(NOT EXISTS "${sym_in}")
            set(sym_in "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        endif()
        set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
        add_custom_command(OUTPUT "${sym_out}"
            COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                    --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
            DEPENDS "${sym_in}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
            COMMENT "hostgen ${sym}")
        list(APPEND TITEX_GX_GEN "${sym_out}")
    endforeach()
endif()
# Cut the plain slice entries in the SAME block that adds the generated ones,
# so the cut cannot drift from the branch that adds the copy. The real net is
# the LINKER either way: a slice that stopped listing one of these answers with
# an undefined symbol, and one that started listing a second copy answers with
# a duplicate. The count guard below is a signpost for a reader looking at that
# failed link, which is what the T2 block's own comment says of its own.
set(TITEX_GX_CUT 0)
set(SLICE_OV007_SRC_TITEX "")
foreach(src IN LISTS SLICE_OV007_SOURCES)
    set(_cut OFF)
    if(NOT TITEX_GX_PLAIN)
        foreach(sym IN LISTS TITEX_GX_SYMS)
            if(src MATCHES "/${sym}\\.(c|cpp)$")
                set(_cut ON)
            endif()
        endforeach()
    endif()
    if(_cut)
        math(EXPR TITEX_GX_CUT "${TITEX_GX_CUT}+1")
    else()
        list(APPEND SLICE_OV007_SRC_TITEX "${src}")
    endif()
endforeach()
set(SLICE_OV007_SOURCES ${SLICE_OV007_SRC_TITEX})
if(TITEX_GX_PLAIN)
    # The pre-fix arm cuts nothing on purpose; satisfy the guard rather than
    # print a scary line about a cut that was never meant to happen.
    set(TITEX_GX_CUT ${TITEX_GX_WANT})
endif()
if(NOT TITEX_GX_CUT EQUAL TITEX_GX_WANT)
    message(STATUS "TITEX: expected to cut ${TITEX_GX_WANT} plain ov007 "
                   "geometry-port entries and cut ${TITEX_GX_CUT}. If "
                   "slice_ov007.txt stopped listing one of them the hostgen "
                   "copy is still added and the link will say so; if a SECOND "
                   "slice started listing one, this cut missed it and the link "
                   "will say that too")
endif()

# ---- run mg15 lane TITLEDEPTH: THE PER-FRAME GEOMETRY RESET ----------------
#
# One row past the TITLETEX block above, and the same defect: two arm9 helpers
# that slice_ov007.txt lists and no other slice does, every store in them
# PLAIN, so none of it reaches the geometry engine.
#
# WHAT THEY ARE, read off extracted/arm9_dec.bin at 0x02004000 rather than
# taken on the source's word. Alignment proven first on the two MATCHED
# neighbours func_02055464 and func_02055490, whose disassembly reproduces
# their src TUs instruction for instruction (the 0x2000>>13 and 0x1f00>>8
# field extractions, the GXSTAT pool word 0x04000600 at each tail):
#
#   func_020557b4  GXSTAT bit-27 busy wait; GXSTAT |= 0x8000 (clear the
#                  matrix-stack error flag); DISP3DCNT |= 0x2000 and |= 0x1000
#                  (the two 3D error acknowledges); call func_02055624; then
#                  POLYGON_ATTR = 0x1f0080, TEXIMAGE_PARAM = 0, PLTT_BASE = 0
#                  -- the DEFAULT MATERIAL every frame starts from.
#   func_02055624  read the position/vector stack level into a and the
#                  projection stack level into b (through the two matched
#                  helpers above, which read GXSTAT); then
#                  MTX_MODE 3 + MTX_IDENTITY   -- the TEXTURE matrix
#                  MTX_MODE 0 + MTX_POP b      -- unwind the projection stack
#                  MTX_MODE 2 + MTX_POP a + MTX_IDENTITY
#                                              -- unwind the position stack and
#                                                 start the frame from identity
#
# IT IS THE PER-FRAME RESET, from the ROM's own call graph and not from the
# name: src/func_ov007_020bcf90.c is the title screen's Render, and it opens
# with func_020557b4(); func_02055624(); func_02055574(...) before it draws
# anything. func_020557b4's only other caller is func_0200f4b4.
#
# WHY IT COULD NOT BE ROUTED UNTIL NOW. func_02055624 POPS BY A NUMBER IT
# READS OUT OF GXSTAT. Until the commit before this one those bits were a
# snapshot refreshed only when something touched the register through the MMIO
# proxy -- measured 1620 disagreements in 2743 samples on 60 frames of scene 1
# -- so routing this while they were dead would have popped by a stale number,
# which is worse than not popping at all. The geometry engine publishes the
# level at push and pop time now, so the two reads are true.
#
# ONE MODELLING DIVERGENCE, NAMED. On hardware DISP3DCNT bits 12 and 13 are
# status bits and writing 1 ACKNOWLEDGES them; against the host's mapped window
# the same store SETS them. Nothing in the port reads DISP3DCNT -- the only
# reader in the tree is ntr/ppu_audit.cpp's census row -- so this costs
# nothing today, and it is written down rather than left for someone to find.
#
# KILL SWITCH -DTIDEP_RESET_PLAIN=ON, printed on EVERY configure in BOTH
# directions (run mg15's SQRT lane proved a CMake option() caches and a switch
# that only announces one arm silently keeps the wrong one). It is a rebuild
# rather than one binary, so the two arms' .dsstate bases have to be read
# before their BMPs are compared -- notes/port-selftest-bmp-gate.md.
option(TIDEP_RESET_PLAIN
       "run mg15 TITLEDEPTH: build the per-frame geometry reset PLAIN (pre-fix arm)"
       OFF)
set(TIDEP_RESET_SYMS
    func_020557b4          # GXSTAT  DISP3DCNT  POLYGON_ATTR  TEXIMAGE_PARAM
                           # PLTT_BASE
    func_02055624)         # GXSTAT  MTX_MODE  MTX_POP  MTX_IDENTITY
list(LENGTH TIDEP_RESET_SYMS TIDEP_RESET_WANT)
if(TIDEP_RESET_PLAIN)
    message(STATUS "TIDEP: TIDEP_RESET_PLAIN=ON -- the ${TIDEP_RESET_WANT} "
                   "per-frame geometry reset TUs build PLAIN. This is the "
                   "PRE-FIX arm: the position and texture matrices are never "
                   "made identity, the stacks are never unwound and the "
                   "default material is never set.")
else()
    message(STATUS "TIDEP: TIDEP_RESET_PLAIN=OFF -- the ${TIDEP_RESET_WANT} "
                   "per-frame geometry reset TUs route through hostgen "
                   "(shipped arm).")
endif()
set(TIDEP_RESET_GEN "")
if(NOT TIDEP_RESET_PLAIN)
    foreach(sym IN LISTS TIDEP_RESET_SYMS)
        set(sym_in "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        if(NOT EXISTS "${sym_in}")
            set(sym_in "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        endif()
        set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
        add_custom_command(OUTPUT "${sym_out}"
            COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                    --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
            DEPENDS "${sym_in}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
            COMMENT "hostgen ${sym}")
        list(APPEND TIDEP_RESET_GEN "${sym_out}")
    endforeach()
endif()
# Cut the plain slice entries in the SAME block that adds the generated ones,
# for the TITEX block's reason: the real net is the LINKER either way, and the
# count guard below is only a signpost for a reader looking at a failed link.
set(TIDEP_RESET_CUT 0)
set(SLICE_OV007_SRC_TIDEP "")
foreach(src IN LISTS SLICE_OV007_SOURCES)
    set(_cut OFF)
    if(NOT TIDEP_RESET_PLAIN)
        foreach(sym IN LISTS TIDEP_RESET_SYMS)
            if(src MATCHES "/${sym}\\.(c|cpp)$")
                set(_cut ON)
            endif()
        endforeach()
    endif()
    if(_cut)
        math(EXPR TIDEP_RESET_CUT "${TIDEP_RESET_CUT}+1")
    else()
        list(APPEND SLICE_OV007_SRC_TIDEP "${src}")
    endif()
endforeach()
set(SLICE_OV007_SOURCES ${SLICE_OV007_SRC_TIDEP})
if(TIDEP_RESET_PLAIN)
    set(TIDEP_RESET_CUT ${TIDEP_RESET_WANT})
endif()
if(NOT TIDEP_RESET_CUT EQUAL TIDEP_RESET_WANT)
    message(STATUS "TIDEP: expected to cut ${TIDEP_RESET_WANT} plain "
                   "geometry-reset entries and cut ${TIDEP_RESET_CUT}. If "
                   "slice_ov007.txt stopped listing one of them the hostgen "
                   "copy is still added and the link will say so; if a SECOND "
                   "slice started listing one, this cut missed it and the link "
                   "will say that too")
endif()

# run link60 lane SC1: THE SCENE 1 INTERIM, AND ITS OWN RETIREMENT SWITCH.
#
# func_ov007_020c9688 is scene 1's blocker and it is a hole in the DECOMP:
# config/arm9/overlays/ov007/delinks.txt runs one block to end:0x020c9688 and
# starts the next at 0x020c9988, so nothing covers the 768 bytes between them
# and no src TU exists. hal/scene_boot.cpp stood a trap there, the trap returned
# 0 six times and func_ov007_020ade58 dereferenced one of them.
#
# port/unmatched/Ov007_OamCellBank_020c9688.cpp is a host transcription of the
# ROM's instructions at that address. Read its banner for the derivation; read
# this block for why it cannot outstay its welcome.
#
# THE GUARD IS THE POINT. An interim that has to be remembered is an interim
# that gets forgotten, and the failure mode is not a link error, it is a host
# body quietly winning over a matched one and the port silently ceasing to be
# the decomp. So the list is EMPTY the moment src/ has the match, whichever
# extension the decomp lands it under, and the matched TU arrives on its own
# through slice_ov007.txt. Configure is where that gets noticed, and the STATUS
# line below says which state this tree is in every time it runs.
set(SC1_INTERIM_SOURCES "")
if(EXISTS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020c9688.c"
   OR EXISTS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020c9688.cpp")
    message(STATUS "SC1: src/func_ov007_020c9688 EXISTS, the scene 1 interim is "
                   "RETIRED and port/unmatched/Ov007_OamCellBank_020c9688.cpp "
                   "is deletable")
else()
    message(STATUS "SC1: func_ov007_020c9688 still unmatched, scene 1 running "
                   "on the host transcription interim")
    list(APPEND SC1_INTERIM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov007_OamCellBank_020c9688.cpp")
endif()

# run mg15 lane TITLETEX: THE SECOND SCENE 1 INTERIM, ON SC1's OWN TERMS.
#
# func_ov007_020c4684 is the routine the title screen DRAWS THROUGH -- its
# literal pool is twelve geometry command ports and nothing else -- and
# hal/scene_boot.cpp stood an L2_UNMATCHED trap there that a 300-frame scene-1
# run entered 16,200 times, 54 a frame, returning 0 every time. Nothing the
# title places came out of it. port/unmatched/Ov007_CellQuad_020c4684.cpp is a
# host transcription read off extracted/overlays/overlay_0007.bin at the
# config-aligned base, with all four of its relocations resolved and its
# alignment proven first on its own byte-matched caller.
#
# THE GUARD IS SC1's GUARD, for SC1's reason: an interim that has to be
# remembered is an interim that gets forgotten, and the failure mode is a host
# body quietly winning over a matched one. The list is EMPTY the moment src/
# has the match, whichever extension it lands under, and the matched TU then
# arrives on its own through slice_ov007.txt.
if(EXISTS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020c4684.c"
   OR EXISTS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020c4684.cpp")
    message(STATUS "TITEX: src/func_ov007_020c4684 EXISTS, the title's "
                   "polygon-submitter interim is RETIRED and "
                   "port/unmatched/Ov007_CellQuad_020c4684.cpp is deletable")
else()
    message(STATUS "TITEX: func_ov007_020c4684 still unmatched, the title "
                   "screen draws through the host transcription interim")
    list(APPEND SC1_INTERIM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov007_CellQuad_020c4684.cpp")
endif()

# run push0829 lane TITLE-DOODLE: THE DOODLE CANVAS INTERIM, ON SC1's OWN TERMS.
#
# func_ov007_020b8fd4 is the title-screen DOODLE CANVAS/PAPER renderer -- its one
# caller src/func_ov007_020bcf90.c calls it in outer states 1..4 (the draw
# screen) right before the seated stroke hub func_ov007_020b91b4. hal/scene_boot.
# cpp stood an L2_UNMATCHED trap there. Measured on this tree, scene 1 driven into
# the doodle screen (a tap chain that reaches title element state 2 with screen
# mode data_ov007_0210342c+0x20 == 1): the trap was entered 402 times over 1200
# frames and returned 0 every time, so the paper was never sized and the two
# billboards were never positioned. port/unmatched/Ov007_DoodleCanvas_020b8fd4.cpp
# is read off the fanout-opus near-miss draft, re-verified against
# extracted/overlays/overlay_0007.bin at the config-aligned base: EVERY store and
# EVERY call target matches the ROM byte for byte (of 36 residual diffs, 0 are a
# str/stm/bl/b), so only the register coloring is unmatched and the observable
# behaviour is the ROM's exactly.
#
# THE GUARD IS SC1's GUARD, for SC1's reason: the list is EMPTY the moment src/
# has the byte-match, whichever extension it lands under, and the matched TU then
# arrives on its own through slice_ov007.txt. The trap line
# L2_UNMATCHED(func_ov007_020b8fd4) in hal/scene_boot.cpp is removed in the same
# change; leaving it would be an LNK2005 against this body.
if(EXISTS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020b8fd4.c"
   OR EXISTS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020b8fd4.cpp")
    message(STATUS "DOODLE: src/func_ov007_020b8fd4 EXISTS, the doodle canvas "
                   "interim is RETIRED and "
                   "port/unmatched/Ov007_DoodleCanvas_020b8fd4.cpp is deletable")
else()
    message(STATUS "DOODLE: func_ov007_020b8fd4 still unmatched, the title "
                   "doodle canvas draws through the host transcription interim")
    list(APPEND SC1_INTERIM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ov007_DoodleCanvas_020b8fd4.cpp")
endif()

# run link60 lane PC2: SCENE 1'S NEXT BLOCKER, AND THE PREDICATE THAT NAMES IT.
#
# src/func_ov007_020b2998.c landed and the +0xF4 field is written, so the run
# walks past the old stop and dies one call chain further on, inside a pair of
# matched TUs. It is NOT a missing body this time. It is an ARM REGISTER
# RIDE-THROUGH the host ABI cannot carry:
#
#   src/func_ov007_020b0a20.c    calls func_ov007_020be980(scene->f_100, 1, 0)
#   src/func_ov007_020be980.c    DEFINES it as (int r0) and declares its own
#                                callee `extern int func_ov007_020bc3dc(int)`
#   src/func_ov007_020bc3dc.c    DEFINES it as (void *c, int i, int a2)
#
# On ARM all three are byte-faithful and the middle frame never touches r1 or
# r2, so the 1 and the 0 ride through it into the callee. On MSVC every
# argument is a stack slot, the middle frame pushes one, and the callee reads
# two slots nobody wrote.
#
# WHY A PREDICATE AND NOT A STRING SOMEBODY GREPS FOR. port/tools/battery.py's
# scene-1 row keys on a marker, and a marker that always prints would let ANY
# InitResources fault read as this one. This asks the source fact that IS the
# blocker: the middle TU's one-parameter declaration of its own callee. Change
# that line, or host-copy the seam the way port/CMakeLists.txt answers the
# other ARM ride-throughs, and the marker leaves the build in the configure
# that notices -- the same shape as SC1's interim switch above.
#
# THE ONE THING THIS PREDICATE DOES NOT DO, said plainly: it is armed at scene
# entry, not at the seam, because every frame between ti_init and the fault is
# a matched TU and the port owns none of them. So a DIFFERENT fault inside
# InitResources would still carry the marker. It is tighter than nothing and
# looser than the trap print it replaces, and the row says so too.
#
# run link60 lane RT1 AMENDS THE PREDICATE, and the amendment is the half the
# block above promised and did not implement. PC2 wrote "change that line, OR
# host-copy the seam", but only the first branch was detectable: the regex asks
# the source file, and the source file is untouched by a displacement. Taken
# literally after the seat, it would have kept asserting a blocker that no
# longer ships.
#
# So the predicate now asks the question it always meant: IS THE ONE-PARAMETER
# BODY IN THIS BINARY. That is two facts, and both are read rather than
# remembered -- the declaration still in src/ (unchanged, because the byte gate
# owns that file) AND the TU still in the port's ov007 slice
# (RT1_BE980_DISPLACED, set by the cut itself a few hundred lines up). Either
# one alone is a half-truth. Together they re-arm the marker by themselves if a
# later lane puts the TU back without fixing the seam, which is the property
# that makes this a guard and not a comment.
set(PC2_RIDETHROUGH_DEFS "")
file(STRINGS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020be980.c"
     PC2_RIDETHROUGH_DECL
     REGEX "^[ \t]*extern[ \t]+int[ \t]+func_ov007_020bc3dc[ \t]*\\([ \t]*int[ \t]*\\)[ \t]*;")
if(PC2_RIDETHROUGH_DECL AND RT1_BE980_IN_SLICE AND NOT RT1_BE980_DISPLACED)
    message(STATUS "PC2: func_ov007_020be980 still declares its callee with one "
                   "parameter and its TU is still built, so scene 1's "
                   "ride-through blocker STANDS and the battery marker is armed")
    list(APPEND PC2_RIDETHROUGH_DEFS "PORT_OV007_RIDETHROUGH_UNSEATED=1")
elseif(RT1_BE980_DISPLACED)
    message(STATUS "RT1: func_ov007_020be980 is DISPLACED to "
                   "unmatched/Ov007_RideThrough_020be980.cpp, so the "
                   "one-parameter body is not in this binary and scene 1's "
                   "ride-through marker is RETIRED")
elseif(NOT RT1_BE980_IN_SLICE)
    # Retired for the wrong reason, and the block above has already said so.
    # Kept as its own branch rather than folded into the declaration case
    # because "the marker is off because the seam is fixed" and "the marker is
    # off because the TU vanished and the link is about to fail" are opposite
    # states, and one message covering both is how a broken tree reads healthy.
    message(STATUS "RT1: func_ov007_020be980 is not in the slice at all, so the "
                   "ride-through marker is RETIRED but NOT because the seam was "
                   "seated -- see the RT1 slice warning above, the link fails "
                   "before scene 1 can be judged")
else()
    message(STATUS "PC2: func_ov007_020be980's one-parameter callee declaration "
                   "is GONE, scene 1's ride-through marker is RETIRED -- if the "
                   "scene now runs, delete its SCENE_BLOCKED row")
endif()

# run link60 lane RT1: SCENE 1'S NEXT BLOCKER, AND IT IS THE SAME DEFECT CLASS
# ONE FRAME DEEPER. The seam above is seated and the scene now walks past it,
# reaching twenty-four calls into func_ov007_020ae834 that no earlier run
# reached, and dies on another ARM register that no host frame carries:
#
#   src/func_ov007_020add3c.c   extern int func_ov007_020ae558(void);
#                               func_ov007_020ae558();          <- passes none
#   src/func_ov007_020ae558.c   int func_ov007_020ae558(char *self)
#
# On ARM that is exact. The ROM's func_ov007_020add3c does `push {r4, lr} /
# mov r4, r0 / bl func_ov007_020ae558` and never writes r0 in between, so its
# OWN incoming argument is still sitting in r0 when the callee reads it. Both
# TUs are matched and both are right. On MSVC the caller pushes nothing and the
# callee reads [ebp+8], which is the caller's saved esi.
#
# THE DIFFERENCE FROM THE SEAM ABOVE, and it is the reason this gets its own
# marker rather than a reworded one: that seam lost values a MIDDLE frame was
# handed, and the frame to displace was the middle one. This one loses a value
# the CALLER already owns and simply does not pass, so the frame where
# information is destroyed is func_ov007_020add3c, not the callee. The seat is
# derived in port/ov007_seat.txt section 5c and deliberately NOT taken here:
# displacing a matched TU picks which of two matched bodies stops being the
# decomp, and this file's own precedent is that a review rules that and a lane
# executes it. This lane's brief was the seam above and this row.
#
# SAME PREDICATE SHAPE, for the same reason: the source fact that IS the
# blocker, plus whether the TU carrying it is actually in this binary.
set(RT1_AE558_DEFS "")
file(STRINGS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020add3c.c"
     RT1_AE558_DECL
     REGEX "^[ \t]*extern[ \t]+int[ \t]+func_ov007_020ae558[ \t]*\\([ \t]*void[ \t]*\\)[ \t]*;")
if(RT1_AE558_DECL AND RT1_ADD3C_IN_SLICE AND NOT AE1_ADD3C_DISPLACED)
    message(STATUS "RT1: func_ov007_020add3c still calls func_ov007_020ae558 "
                   "with no arguments and its TU is still in the slice, so "
                   "scene 1's implicit-r0 blocker STANDS and the battery "
                   "marker is armed")
    list(APPEND RT1_AE558_DEFS "PORT_OV007_AE558_UNSEATED=1")
elseif(AE1_ADD3C_DISPLACED)
    message(STATUS "AE1: func_ov007_020add3c is DISPLACED to "
                   "unmatched/Ov007_ImplicitR0_020add3c.cpp, which forwards "
                   "its own argument, so the no-argument call is not in this "
                   "binary and scene 1's implicit-r0 marker is RETIRED")
elseif(NOT RT1_ADD3C_IN_SLICE)
    # Retired for the wrong reason, and the AE1 slice warning above has
    # already said so. Kept as its own branch for the reason the be980 block
    # gives: "the marker is off because the seam is fixed" and "the marker is
    # off because the TU vanished and the link is about to fail" are opposite
    # states, and one message covering both is how a broken tree reads healthy.
    message(STATUS "AE1: func_ov007_020add3c is not in the slice at all, so "
                   "the implicit-r0 marker is RETIRED but NOT because the seam "
                   "was seated -- see the AE1 slice warning above, the link "
                   "fails before scene 1 can be judged")
else()
    message(STATUS "RT1: func_ov007_020add3c's no-argument call to "
                   "func_ov007_020ae558 is GONE from this binary, scene 1's "
                   "implicit-r0 marker is RETIRED -- if the scene now runs, "
                   "delete its SCENE_BLOCKED row")
endif()

# run mg15 lane IMPLICITR: THE SEAM BEHIND THE TOUCH-TO-START HANDOFF, and the
# predicate exists to answer ONE question a later lane will make stale.
#
# The displacement above is the fix. The thing that can rot is the reason for
# it: src/func_ov007_020c2b9c.c defines the body `void`, and this lane MEASURED
# that spelling the return there (`void *` + `return r;`) still matches, 0 of 23
# words at 2004/b56 with strict relocs. That is the MP1 shape -- run mg15 lane
# MP1 retired two host copies the moment include/decl_common.h could carry the
# fix -- and if a later lane takes it, this host copy becomes dead weight that
# nothing in the build would complain about.
#
# So the predicate reads the SOURCE FACT that makes the displacement necessary
# (the `void` definition still in src/, which the byte gate owns) against
# whether the cut ran. It is a STATUS line and not a battery define on purpose:
# there is no SCENE_BLOCKED row for this seam to retire, and a define nothing
# consumes is a marker that cannot fire.
file(STRINGS "${PORT_REPO_ROOT_ABS}/src/func_ov007_020c2b9c.c"
     IR1_C2B9C_VOID_DEF
     REGEX "^[ \t]*void[ \t]+func_ov007_020c2b9c[ \t]*\\(")
if(IR1_C2B9C_VOID_DEF AND IR1_C2B9C_DISPLACED)
    message(STATUS "IR1: func_ov007_020c2b9c is DISPLACED to "
                   "unmatched/Ov007_ImplicitReturn_020c2b9c.cpp, which spells "
                   "the return its two matched callers already declare, so the "
                   "title's post-handoff fault at func_ov007_020c2acc+0x3f is "
                   "not in this binary")
elseif(IR1_C2B9C_VOID_DEF AND IR1_C2B9C_IN_SLICE)
    message(WARNING "IR1: func_ov007_020c2b9c still DEFINES itself void and its "
                    "TU is back in the build, so both matched callers read an "
                    "EAX nobody set and the title faults after touch-to-start. "
                    "Restore the cut in the slice loop above")
elseif(NOT IR1_C2B9C_VOID_DEF)
    # The good ending, and it is a DIFFERENT tree from the one above rather
    # than the same one worded differently: the decomp itself now returns the
    # object, so the host copy has nothing left to add and its CMake block plus
    # port/unmatched/Ov007_ImplicitReturn_020c2b9c.cpp should go, the way MP1
    # deleted Comms_SlotIndex_*.cpp.
    message(STATUS "IR1: src/func_ov007_020c2b9c.c no longer defines itself "
                   "void, so the implicit-return seam is fixed in the DECOMP "
                   "and this displacement is REDUNDANT -- retire it (delete the "
                   "cut in the slice loop and the host copy)")
endif()

# run link60 lane AE1: SCENE 1'S NEXT BLOCKER, AND IT IS A NEW DEFECT CLASS.
#
# The implicit-r0 seam above is seated, InitResources FINISHES, and Behavior
# runs 299 clean ticks. What is left is entirely inside the scene's own Render
# slot, and it is not an arity disagreement at all. It is a CALLING CONVENTION:
#
#   src/func_ov007_020bbff0.c   declares the FLAT Itanium name
#                               _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3
#                               as (void *, void *, void *) and calls it with
#                               three cdecl pushes
#   the definition             port/unmatched/ModelComponents_Render.cpp's
#                               `void ModelComponents::Render(Matrix4x3*,
#                               Vector3*)`, a real C++ member, which MSVC emits
#                               as __thiscall: `this` arrives in ECX
#
# and the flat name reaches the mangled one through an /alternatename in
# hal/scene_boot.cpp. AN ALIAS CANNOT CHANGE A CALLING CONVENTION -- that
# sentence is already in that file's own banner, next to the Scene::SetFaders
# FACE that exists for exactly this reason -- so the caller's three pushes land
# one slot off and `this` is whatever ECX happened to hold.
#
# WHY THE PREDICATE IS NOT THE ONE ABOVE, and this is the part worth reading.
# The two seams before this were fixed by DISPLACING a caller, so "is the
# caller's TU in the build" was a fact the fix changed. This seam's expected
# fix is a FACE, which leaves the caller's TU and its declaration exactly where
# they are. A declaration-plus-membership predicate would therefore survive its
# own fix and go on asserting a retired blocker -- the identical trap RT1
# caught in PC2's predicate and this lane caught in RT1's, arriving a third
# time from a third direction. So this reads the two facts that the two
# available fixes actually change:
#
#   the ALIAS, still pointing the flat name at a QAE (__thiscall) target.
#     A face stands a real cdecl definition and retires the directive.
#   the CALLER, still compiled into the ov007 slice.
#     A displacement cuts it, exactly like the two above.
#
# Either fix clears this in the configure that notices, and neither can be
# taken without clearing it.
#
# run link60 Stage 5 lane MR1: THE ALIAS FACT IS NOW ANCHORED, and this is the
# hole the AE1 review demonstrated being closed rather than inherited.
#
# file(STRINGS ... REGEX) is a line scanner with no idea what a comment is, so
# the spelling that shipped here matched the directive's TEXT anywhere on a
# line. Section 8 recorded that as "a known hole shipping on purpose" because
# the only fix that would write such a comment had not been taken yet.
#
# THIS COMMIT TAKES IT, so the hole stopped being hypothetical in the same
# change that closes it. The face in hal/scene_boot.cpp records the directive
# it replaces, verbatim, in its evidence header, the way every other retired
# directive in that file is recorded. Under the old spelling that comment kept
# the marker ARMED after a correct fix: a battery row asserting a blocker that
# no longer exists, which is the exact failure the row's own derivation warns
# about.
#
# The anchor is the pragma itself. A directive reaches link.exe only through
# #pragma comment(linker, ...) in a compiled TU (port/tools/alternatename_guard.py
# mechanism (a), and its audit re-derives every run that the port has no
# mechanism (b)), so requiring the line to BE that pragma is not a heuristic
# about comments, it is the same test the linker applies. Leading whitespace is
# allowed because the preprocessor allows it; a `//` or ` * ` in front is not a
# pragma and no longer counts.
#
# WHAT IT STILL DOES NOT COVER, stated rather than left to be discovered: a
# whole pragma sitting inside a /* */ block comment reads as anchored here and
# is not a linker input. That is the same COMMENT GAP alternatename_guard.py
# names in its own docstring and carries one live instance of, so the two tools
# now have the same blind spot instead of two different ones. Closing it needs
# a C tokenizer and neither tool has one.
set(AE1_MCRENDER_DEFS "")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/hal/scene_boot.cpp"
     AE1_MCRENDER_ALIAS
     REGEX "^[ \t]*#[ \t]*pragma[ \t]+comment[ \t]*\\([ \t]*linker[ \t]*,[ \t]*\"/alternatename:__ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3=\\?Render@ModelComponents@@QAE")
if(AE1_MCRENDER_ALIAS AND AE1_BBFF0_COMPILED)
    message(STATUS "AE1: the flat _ZN15ModelComponents6RenderE... name is still "
                   "aliased onto a __thiscall member and src/func_ov007_020bbff0.c "
                   "still calls it cdecl, so scene 1's Render-slot blocker "
                   "STANDS and the battery marker is armed")
    list(APPEND AE1_MCRENDER_DEFS "PORT_OV007_MCRENDER_UNSEATED=1")
elseif(NOT AE1_MCRENDER_ALIAS)
    message(STATUS "AE1: the flat _ZN15ModelComponents6RenderE... alias onto a "
                   "__thiscall member is GONE, so scene 1's Render-slot marker "
                   "is RETIRED -- if the scene now runs, delete its "
                   "SCENE_BLOCKED row")
else()
    message(STATUS "AE1: src/func_ov007_020bbff0.c is not compiled into the "
                   "ov007 slice, so its cdecl call is not in this binary and "
                   "scene 1's Render-slot marker is RETIRED")
endif()

# Appended, not string-joined: an empty PC2 list concatenated with a non-empty
# RT1 one produces a leading semicolon, and CMake reads that as an empty define
# to pass to the compiler. Every list goes empty the day every seam is seated.
set(SCENE_BOOT_MARKER_DEFS ${PC2_RIDETHROUGH_DEFS} ${RT1_AE558_DEFS}
    ${AE1_MCRENDER_DEFS})
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/hal/scene_boot.cpp"
    PROPERTIES COMPILE_DEFINITIONS "${SCENE_BOOT_MARKER_DEFS}")

# run link60 lane MG2: THE FIRST MINIGAME, WIRED.
#
# dScMgCurling_c is ov006's curling minigame, actor id 0x176 (its spawn symbol
# is MgShuffleShell_Spawn; Shuffle Shell is the localised name of the same
# game). Lane MG1 derived the seat and left it out of every target because the
# link did not close; this lane closes it. The wall MG1 named was the mwcc
# POINTER-TO-MEMBER ABI, and it is answered the way the port has answered it
# twice before: the six dispatching TUs are EXCLUDED from the slice and
# host-copied against an address switch (unmatched/MgBase_StateDispatch.cpp and
# unmatched/MgCurling_StateDispatch.cpp), the way
# unmatched/Player_ChangeState.cpp answers Player::ChangeState.
#
# 281 src TUs: MG1's 276 plus the sixth wave this lane measured. The
# mechanical half of the wall -- the /alternatename rows -- is generated by
# port/tools/facegen.py into hal/scene_mg_faces_gen.cpp and verified by that
# tool rather than hand-written; the rows facegen REFUSED are in
# hal/scene_mg_faces.cpp with the refusal quoted in the banner.
#
# port/mg_fanout_costs.txt is the derivation and the cost model for the other
# twenty-nine; its section 4 carries this lane's measured wall accounting.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_mg1.txt" SLICE_MG1_LINES)
set(SLICE_MG1_SOURCES "")
foreach(line IN LISTS SLICE_MG1_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_MG1_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# ONE .c IN THIS SLICE IS REALLY C++, run mg10 lane F371. Lane CCN's rule was
# followed rather than waited on: the whole slice was swept for the //cpp
# marker and for extern "C", virtual, class and namespace before the first
# build, and src/func_ov004_020ae5c4.c is the only hit. It needs C++ for real
# -- the tree's only slot-34 dispatch is spelled through a dummy 35-virtual
# struct, which is how src/func_ov006_020d3ba0.c spells its slot-36 dispatch in
# the same class. Same repair as the psy and smartball blocks below.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov004_020ae5c4.c"
    PROPERTIES LANGUAGE CXX)

# The seat's own files, on the same three targets the scene-1 and ov007 seats
# ride. They are not optional on any of the three: hal/scene_boot.cpp's
# port_scene_classes table now carries the curling row, so every target that
# compiles scene_boot.cpp references port_mg_curling_spawn and
# port_scene_fill_curling by name.
set(SCENE_MG_SOURCES
    hal/scene_mg.cpp hal/scene_mg_faces.cpp hal/scene_mg_faces_gen.cpp
    # Lane MUS1: the minigame sound record (re-hosted 0x34) and the verbatim
    # ov005 row seeder that gives a direct SM64DS_SCENE minigame boot its music.
    # Rides these same three targets because scene_boot.cpp calls
    # port_scene_mg_seat_sound by name and it defines data_0209b308.
    hal/scene_mg_sound.cpp
    unmatched/MgBase_DeclConflict.cpp unmatched/MgBase_Slot1.cpp
    unmatched/MgBase_Slot7.cpp unmatched/MgBase_StateDispatch.cpp
    unmatched/MgCurling_StateDispatch.cpp unmatched/MgBase_LangTable.cpp
    # Run mg5, lane BASESET. The framework's STATE SETTER, which was the last
    # named trap in the minigame stack and was a FREEZE in a player's hands:
    # func_ov004_020b87e0 set no state, so a class that asked to advance never
    # did. Not optional on any of the three targets and for a stronger reason
    # than the files above it, which is that its symbol is no longer defined
    # anywhere else: hal/scene_mg_faces.cpp's trap is DELETED, and
    # unmatched/MgBase_StateDispatch.cpp calls port_mg_try_base_state by name
    # from port_mg_call0, so a target that compiles the dispatch and not this
    # fails the link twice over.
    #
    # It also DEFINES func_ov004_020b7460, the slot 12 tail-jump veneer whose
    # src TU drops the pointer its two callers already pass, so
    # src/func_ov004_020b7460.c is off port/slice_mg1.txt for this file.
    unmatched/MgBase_StateSetter.cpp
    # Run mg5, lane BASESET. dScMgBase_c slot 5, the second of the two argument
    # ride-throughs port/mg_fanout_costs.txt section 6 named and left. Its src
    # TU calls func_0203cbc0 with no argument where the ROM has the pointer
    # being freed live in r0, and the port's func_0203cbc0 is operator delete,
    # so the src form frees a stack slot on every minigame teardown.
    # src/func_ov004_020b0840.c is off BOTH port/slice_mg1.txt and
    # port/slice_smb.txt for this file.
    unmatched/MgBase_Slot5.cpp
    # Lane CT1: dScMgCurling_c's twenty-fifth state, the one with no src TU.
    # Rides the same three targets as the dispatch that calls it -- it is only
    # ever reached through that switch, so any target compiling the dispatch
    # needs this or the link fails on port_mg_curling_st_020e1854.
    unmatched/MgCurling_State_020e1854.cpp
    # Lane CUR2: dScMgCurling_c's two shell-against-shell collision bodies,
    # the other two ov006 addresses with no src TU. Same three targets and the
    # same reason as the state above: hal/scene_mg_faces.cpp now calls
    # port_mg_curling_collide_020e1dc8 and _020e20bc by name from the faces
    # the matched physics TUs link against, so a target that compiles the
    # faces and not these fails the link on both symbols.
    unmatched/MgCurling_Collide_020e1dc8.cpp
    unmatched/MgCurling_Collide_020e20bc.cpp
    # Run mg5, lane PCH: dScMgPachinko_c (id 0x170, scene 368). Same three
    # targets and the same reason as every file above -- hal/scene_boot.cpp's
    # port_scene_classes[] now carries the 368 row, so every target that
    # compiles scene_boot.cpp references port_mg_pachinko_spawn and
    # port_scene_fill_pachinko by name, and hal/scene_mg.cpp section 8
    # references port_mg_pachinko_d0, the state counters and the traps.
    #
    # THE STATE DISPATCH IS NOT OPTIONAL EITHER, and for the reason the CT1
    # comment above gives about MgCurling_State_020e1854: this class's five
    # dispatching TUs are EXCLUDED from slice_pch.txt and defined only in
    # MgPachinko_StateDispatch.cpp, so a target that takes the slice and not
    # the dispatch fails the link on all five func_ov006_* names.
    unmatched/MgPachinko_StateDispatch.cpp
    unmatched/MgPachinko_Dtor.cpp
    unmatched/MgPachinko_Traps.cpp
    unmatched/MgPachinko_Faces.cpp
    unmatched/MgPachinko_OamRender.cpp
    # Run mg5, lane HISCORE: the class's scoring tick, and the one argument
    # ride-through it owes. func_ov006_020fb7e0 calls the award routine
    # func_ov006_020fbb2c with two arguments where the definition takes three,
    # because the ROM carries the points in r2 out of the ldrh that makes the
    # != 0 test. Same shape and same ruling as unmatched/MgFlower_InitScore.cpp.
    # REPLACES its src TU -- src/func_ov006_020fb7e0.c is out of slice_pch.txt
    # -- so it is not optional on any target that takes the slice, exactly like
    # the state dispatch above.
    unmatched/MgPachinko_Award.cpp
    # Run mg5, lane HSDISP: the minigame HUD's HIGH SCORE drawer and the one
    # 32-bit argument its caller declares as a single byte, which is why the
    # screen read 999999 while the stored word and both getters read zero.
    # REPLACES src/func_ov004_020b19f0.c (out of slice_mg1.txt) and
    # src/func_ov006_020fba48.c (out of slice_pch.txt), so it is not optional
    # on any target that takes either slice, exactly like the award above.
    unmatched/MgHud_HighScore.cpp
    # Run mg5, lane WTIMER: the minigame HUD's SCALED NUMBER drawer, the third
    # of the three ov004 addresses with no delink block and no src TU, and the
    # reason the "Wanted!" countdown never appeared. Same three targets and the
    # same reason as the two curling collision copies above: the faces in
    # hal/scene_mg_faces.cpp now call port_mg_hud_scaled_number_020b2220 by
    # name, so a target that compiles the faces and not this file fails the
    # link on that symbol. Replaces no src TU, because there is none.
    unmatched/MgHud_ScaledNumber.cpp
    # Run mg6, lane LKT: dScMgPachinko2_c (id 0x171, scene 369). Same three
    # targets and the same reason as every file above -- hal/scene_boot.cpp's
    # port_scene_classes[] now carries the 369 row, so every target that
    # compiles scene_boot.cpp references port_mg_pachinko2_spawn and
    # port_scene_fill_pachinko2 by name, and hal/scene_mg.cpp's LKT banner
    # references port_mg_pachinko2_d0, the state counters and the trap.
    #
    # THE STATE DISPATCH IS NOT OPTIONAL EITHER, for the reason the CT1 and PCH
    # comments above give: this class's THREE dispatching TUs are EXCLUDED from
    # slice_lkt.txt and defined only in MgPachinko2_StateDispatch.cpp, so a
    # target that takes the slice and not the dispatch fails the link on
    # func_ov006_020fff84, _0210246c and _02102e8c.
    #
    # THE TRAP FILE IS GONE, run mg7 lane L369. unmatched/MgPachinko2_Traps.cpp
    # existed for exactly one symbol -- func_ov006_0210076c, the per-entry
    # follow-up the fifteen-state dispatcher calls after every dispatch it
    # makes -- and that address is decompiled now and rides slice_lkt.txt as an
    # ordinary src TU. A trap standing beside a real definition is a duplicate
    # symbol rather than a safety net, so the file is deleted, not emptied.
    unmatched/MgPachinko2_StateDispatch.cpp
    unmatched/MgPachinko2_Dtor.cpp
    # The class-vtable name face. src/MgLakituLaunch_Spawn.c writes
    # _ZTV16dScMgPachinko2_c, which no config defines, so the slice does not
    # link without this file's one /alternatename row.
    unmatched/MgPachinko2_Faces.cpp)

# THE DELETING DESTRUCTOR SPELLS ITS OWN TABLE AS THE SHARED PLACEHOLDER `VT`.
# src/func_ov006_020e065c.c is dScMgCurling_c's slot 17 and its body is
# `t[0] = (int)VT;`, but hal/cxx_aliases.cpp binds bare VT to
# _data_ov002_021081e4 -- the ENEMY base table, a whole class wrong for a
# minigame scene object. unmatched/MgPachinko_Dtor.cpp found this on curling's
# seat while repairing the identical defect on pachinko's slot 17 and reported
# it rather than touching another lane's file ("THE SAME DEFECT IS LIVE ON
# CURLING'S SEAT TODAY and this lane did not fix it"). Run mg6, lane VTF, is
# that repair, and it takes the same rename dScMgLuigi_c's slot 17 takes below.
#
# THE ADDRESS IS THE BODY'S OWN LITERAL POOL, disassembled out of
# extracted/overlays/overlay_0006.bin at that overlay's 0x020bfec0 base rather
# than read off a name. The two-word pool sits at the end of the 0x38 body:
#   0x020e0660  E59F1024  ldr r1,[pc,#0x24] -> 0x020e068c
#   0x020e0668  E5841000  str r1,[r4]          the vptr store
#   0x020e068c  0213C304  <- THIS, dScMgCurling_c's own table
#   0x020e0690  020A0EAC  <- the game heap word, loaded BY VALUE at 0x020e0678
# config/arm9/overlays/ov006/relocs.txt agrees on both words
# (from:0x020e068c kind:load to:0x0213c304, from:0x020e0690 to:0x020a0eac), and
# the unmarked D2 sibling func_ov006_020e0638 pools the same 0213C304 at
# 0x020e0658, which confirms the table without going through this body at all.
#
# HEAP IS DELIBERATELY NOT RENAMED. The second pool word is 020A0EAC, exactly
# what cxx_aliases.cpp already binds _HEAP to, so that half is right as it
# stands and only VT was wrong.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020e065c.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=data_ov006_0213c304")

# Run mg5, lane PCH. dScMgPachinko_c's own closure, the second minigame class.
# 65 src TUs in wave 1: 59 ov006 and 6 ov004. The arm9 half is deliberately not
# listed -- port/mg_fanout_costs.txt section 5's /OPT:REF argument -- and the
# waves the linker asks for are appended to slice_pch.txt with the wave they
# arrived in named, which is section 10's method.
#
# port/slice_pch.txt is the derivation: the two-root closure walk, the three
# width checks that keep this table at 36 slots, the `::*` sweep that found
# exactly two pointer-to-member TUs in the class's 623-TU closure, and the
# eight ov006 addresses with no source at all.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_pch.txt" SLICE_PCH_LINES)
set(SLICE_PCH_SOURCES "")
foreach(line IN LISTS SLICE_PCH_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_PCH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Run mg6, lane LKT. dScMgPachinko2_c's own closure, the SIBLING of the class
# above in both name and ROM data adjacency. 64 src TUs: 63 ov006 (62 in wave 1
# plus the one delinks join miss, src/func_ov006_02103ac0.c) and 1 ov004.
# The arm9 half is deliberately not listed -- port/mg_fanout_costs.txt
# section 5's /OPT:REF argument -- and the waves the linker asks for are
# appended to slice_lkt.txt with the wave they arrived in named, which is
# section 10's method.
#
# port/slice_lkt.txt is the derivation: the five-root closure walk, the FOUR
# width checks that keep this table at 36 slots (the fourth being the ROM's own
# relocation count inside the span), the ::* sweep that found exactly three
# pointer-to-member TUs in this class's closure and seven more that the
# framework already owns, and the five ov006 addresses with no source at all.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_lkt.txt" SLICE_LKT_LINES)
set(SLICE_LKT_SOURCES "")
foreach(line IN LISTS SLICE_LKT_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_LKT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Run mg5, lane LUI: dScMgLuigi_c, actor id 0x16e = scene 366, "Wanted!".
#
# ONE VARIABLE FOR THE SLICE AND THE HOST COPY, for the reason the fader and
# NFS seats below give: the two are not separable. Six of the slice's TUs are
# host-copied in unmatched/MgLuigi_StateDispatch.cpp because they dispatch an
# mwcc member pointer, twenty of the slice's TUs are reachable ONLY through
# that file's address switch, and hal/scene_boot.cpp's port_scene_classes row
# names port_mg_luigi_spawn and port_scene_fill_luigi, so every target that
# compiles scene_boot.cpp needs the whole set or the link fails.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_lui.txt" SLICE_LUI_LINES)
set(SLICE_LUI_SOURCES unmatched/MgLuigi_StateDispatch.cpp
                      unmatched/MgLuigi_Faces.cpp)
foreach(line IN LISTS SLICE_LUI_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_LUI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# THE DELETING DESTRUCTOR SPELLS ITS OWN TABLE AS THE SHARED PLACEHOLDER `VT`.
# src/func_ov006_020efc30.c is `t[0] = (int)VT;` and hal/cxx_aliases.cpp binds
# bare VT to _data_ov002_021081e4, the ENEMY base table, with a comment saying
# a second VT-spelling TU cannot join a target that links its one reader. This
# is such a TU, so it is renamed onto its own table the way the four D0 bodies
# further down this file are. The address is the body's OWN literal pool, read
# out of extracted/overlays/overlay_0006.bin: 0x020efc60 holds 0213CF10.
# HEAP is deliberately NOT renamed -- the second pool word at 0x020efc64 is
# 020A0EAC, the game heap cxx_aliases.cpp already binds it to.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020efc30.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=data_ov006_0213cf10")

# Gate SMB: dScMgSmartball_c, actor id 0x178 (scene 376), the SECOND minigame
# class. Run mg5, lane SMB. Spawn symbol MgBingoBallSlotsShot_Spawn, after the
# two menu games one class serves; the ROM's RTTI at 0x0213ec7c reads
# "16dScMgSmartball_c" and every file it loads is /MG/d_2d_mg_smart_ball_*.
#
# 74 src TUs and 41 of them are new. The whole ov004 half and the one ov001
# line are already in slice_mg1.txt and CMake dedupes identical absolute paths,
# so the marginal compile cost is the 41 ov006 lines. That ratio is the
# fan-out's own headline: the framework is paid and a class costs its own
# closure.
#
# THIS CLASS HAS NO POINTER-TO-MEMBER STATE MACHINE, so nothing here needs the
# address-switch treatment MgBase_StateDispatch.cpp and
# MgCurling_StateDispatch.cpp give curling. The three measurements behind that
# claim are in port/slice_smb.txt; the one that matters here is that a
# member-pointer sweep of all 76 TUs the closure reaches hits exactly one file,
# src/func_ov004_020b87e0.cpp, which is the FRAMEWORK's state setter. It stays
# excluded from every slice; run mg5 lane BASESET replaced the MG1 block's trap
# with the host copy unmatched/MgBase_StateSetter.cpp, which this slice
# inherits along with the rest of the framework.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_smb.txt" SLICE_SMB_LINES)
set(SLICE_SMB_SOURCES "")
foreach(line IN LISTS SLICE_SMB_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SMB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# The seat's own two files, on the same three targets every scene seat rides.
# Not optional on any of them: hal/scene_boot.cpp's port_scene_classes table
# carries the 376 row, so every target that compiles scene_boot.cpp references
# port_mg_smartball_spawn and port_scene_fill_smartball by name, and
# hal/scene_mg.cpp's face array references all three trap symbols and the slot
# 18 host copy.
#
# MgSmartball_Slot18.cpp REPLACES src/func_ov006_02118a8c.cpp, which is off the
# slice for that reason. The matched TU declares func_ov006_02115b0c with no
# parameters and calls it with none while src/func_ov006_02115b0c.c:113 defines
# it taking the object; ARM rides r0 through and the host does not, and the
# callee's first act is seven destructor loops off that pointer. Both TUs are
# matched and read-only, so the fix is the PORT_HOST_ABI shape
# port/mg_fanout_costs.txt section 6 names.
# MgSmartball_Faces.cpp carries three hand-ruled /alternatename rows that
# port/tools/facegen.py refused, with each refusal quoted beside the ruling
# that answers it: one namespace-scope Memory::Deallocate mangle, and two ov004
# DATA words that include/decl_common.h spells with func_ names.
set(SCENE_SMB_SOURCES
    unmatched/MgSmartball_Traps.cpp
    unmatched/MgSmartball_Slot18.cpp
    unmatched/MgSmartball_Faces.cpp
    unmatched/MgSmartball_SlotRestore.cpp)

# Run mg5 lane SMBSEAT: TWO SOURCE PROPERTIES THE SUB-OBJECT SEAT NEEDS.
#
# 1. src/_ZN16dScMgSmartball_c6RenderEv.cpp is main's matched __thiscall member
#    and it is force-fed port/unmatched/MgSmartball_HostAbi.h. That header
#    shadows two include/ headers by pre-defining their own guard macros, which
#    is why the src file itself needs no edit: DispObj's two virtuals are
#    redeclared __cdecl so `p->f1()` pushes its receiver like the twelve
#    `(**(VFunc**)o)(o)` calls in src/func_ov006_02118488.c that share the same
#    vtables, and dScMgSmartball_c is redeclared without the `sizeof == 0x629c`
#    assert MSVC cannot meet (it neither reuses the base's tail padding nor
#    skips a vfptr for a base that declares no virtual of its own; measured
#    0x47d0 against the ROM's 0x47c8 for the same prefix). /FI is a FLAG rather
#    than an include-path entry on purpose -- CMake emits <INCLUDES> before
#    <FLAGS>, so a /I here would lose to include/ and the shadow would silently
#    not happen.
#
# 2. src/func_ov006_02110a20.c is the ONE .c among the thirty-seven TUs this
#    lane adds that is really C++ -- a //cpp marker and a top-level
#    `extern "C" {` block, invalid C. All thirty-seven were swept for the
#    marker and for `extern "C"`, `virtual`, `class` and `namespace` rather
#    than this one being found by a compile error and the rest left to luck; it
#    is the only hit among the .c files.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN16dScMgSmartball_c6RenderEv.cpp"
    PROPERTIES COMPILE_OPTIONS
    "/FI${CMAKE_CURRENT_SOURCE_DIR}/unmatched/MgSmartball_HostAbi.h")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_02110a20.c"
    PROPERTIES LANGUAGE CXX)

# Run mg5 lane CCN: THE SECOND SEATED MINIGAME CLASS, dScMgCoin_c
# (Coincentration), actor id 0x17a = scene 378. The ROM's own RTTI at
# 0x0213bf24 reads "11dScMgCoin_c" and its vtable is data_ov006_0213bf50, both
# re-derived from extracted/overlays/overlay_0006.bin rather than inherited.
#
# ONE VARIABLE, and for the reason the fader block below states: the slice's
# ov006 bodies are only reachable through the address switch in
# unmatched/MgCoin_StateDispatch.cpp, and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-378 row, so
# every target that compiles scene_boot.cpp references port_mg_coin_spawn and
# port_scene_fill_coin by name -- the same coupling the curling row has.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under
# them; CMake appends both lists to the same target and dedupes identical
# absolute paths, so nothing is compiled twice. port/slice_ccn.txt says what it
# reaches and why, and records the class's two hard floors.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ccn.txt" SLICE_CCN_LINES)
set(SLICE_CCN_SOURCES unmatched/MgCoin_StateDispatch.cpp
                      unmatched/MgCoin_Faces.cpp)
foreach(line IN LISTS SLICE_CCN_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_CCN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# THE THIRD `VT`-SPELLING DELETING DESTRUCTOR, and the last one live in this
# build. src/func_ov006_020dbe64.c is dScMgCoin_c's slot 17 D0 and its body is
# the same four statements curling's and luigi's are, so bare VT sent it to
# _data_ov002_021081e4, the ov002 Enemy base table, a whole class wrong for a
# minigame scene object.
#
# THE ADDRESS IS THE BODY'S OWN LITERAL POOL, disassembled out of
# extracted/overlays/overlay_0006.bin at that overlay's 0x020bfec0 base:
#   0x020dbe68  E59F1024  ldr r1,[pc,#0x24] -> 0x020dbe94
#   0x020dbe70  E5841000  str r1,[r4]          the vptr store
#   0x020dbe94  0213BF50  <- THIS, dScMgCoin_c's own table
#   0x020dbe98  020A0EAC  <- the game heap word, loaded BY VALUE at 0x020dbe80
# config/arm9/overlays/ov006/relocs.txt agrees on both
# (from:0x020dbe94 kind:load to:0x0213bf50, from:0x020dbe98 to:0x020a0eac); the
# D2 sibling func_ov006_020dbe40 pools the same 0213BF50 at 0x020dbe60; and
# slice_ccn.txt line 29 already named data_ov006_0213bf50 as this class's table
# when the seat was derived. HEAP is right and is not renamed.
#
# FOUND BY THE COFF CENSUS, not by this lane's reading of the alias comment.
# The census is the closure check and it is described at the VT row in
# hal/cxx_aliases.cpp.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020dbe64.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=data_ov006_0213bf50")
# Run mg6 lane SOS: dScMgBomroom_c ("Sort or 'Splode"), actor id 0x172 =
# scene 370. The ROM's own RTTI at 0x0213bb38 reads "14dScMgBomroom_c" and its
# vtable is data_ov006_0213bbb4, both derived from
# extracted/overlays/overlay_0006.bin by that lane rather than inherited from
# the fan-out table.
#
# ONE VARIABLE, for lane CCN's reason: the slice's ov006 bodies are only
# reachable through the address switch in
# unmatched/MgBomroom_StateDispatch.cpp, and that file cannot link without
# them. hal/scene_boot.cpp's port_scene_classes table carries the scene-370
# row, so every target that compiles scene_boot.cpp references
# port_mg_bomroom_spawn and port_scene_fill_bomroom by name -- the same
# coupling every other scene row has, which is why this is not optional on any
# of the three targets.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under
# them; CMake appends both lists to the same target and dedupes identical
# absolute paths, so nothing is compiled twice. port/slice_sos.txt says what it
# reaches, why its ov004 delta is ZERO, and records the class's two former
# floors, both closed by run mg7 lane L370: src/func_ov006_020d7c4c.c
# (byte-matched) and src/func_ov006_020d8408.c (NONMATCHING, logic verified).
# unmatched/MgBomroom_Traps.cpp held the trap for the second and is deleted.
#
# SEVEN HOST COPIES AND NO TRAP LEFT, which is the largest pointer-to-member bill
# any seated class has run up: seven tables, seven dispatchers, thirty-three
# states. Six of the seven dispatchers are findable by a link or by a
# member-pointer source sweep and the seventh is findable by neither -- it
# open-codes the ARM Itanium sequence in plain ints. Its host copy is in the
# same file and hal/scene_mg_bomroom.cpp counts its dispatches separately, so a
# run can say the seat is working rather than the absence of a link error
# implying it.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sos.txt" SLICE_SOS_LINES)
set(SLICE_SOS_SOURCES hal/scene_mg_bomroom.cpp
                      unmatched/MgBomroom_StateDispatch.cpp)

# THE DELETING DESTRUCTOR SPELLS ITS OWN TABLE AS THE SHARED PLACEHOLDER `VT`,
# and this is the failure that links cleanly. src/func_ov006_020d5a78.c is
# `t[0] = (int)VT;` and hal/cxx_aliases.cpp binds bare VT to
# _data_ov002_021081e4, the ov002 ENEMY base table, so with no rename here
# dScMgBomroom_c's D0 writes an ENEMY vtable into the scene object on the way
# down and every slot dispatched after that point comes out of the wrong table.
# Nothing diagnoses it: the alias resolves, the link is clean, and this class's
# D0 did not fire on either of this lane's proof runs, so a census would have
# read zero and said nothing either.
#
# THE ADDRESS IS THE BODY'S OWN LITERAL POOL, read out of
# extracted/overlays/overlay_0006.bin at base 0x020bfec0 rather than taken from
# the vtable derivation: 0x020d5aa8 holds 0213BBB4. Its unmarked D2 sibling
# func_ov006_020d5a54 writes the same word from its own pool at 0x020d5a74 and
# does not need a rename, because it spells the symbol rather than the
# placeholder.
#
# HEAP IS DELIBERATELY NOT RENAMED. The second pool word, at 0x020d5aac, is
# 020A0EAC -- the game heap, which cxx_aliases.cpp already binds bare HEAP to.
#
# Found by the run mg6 coordinator's cross-lane note and confirmed here by the
# check that actually works: a COFF short-name scan for _VT over the built
# objects, which named this TU in all three targets. The same scan names two
# other minigame D0s that are still mis-bound (func_ov006_020dbe64 and
# _020e065c); they belong to other classes and are not this lane's to rename.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020d5a78.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=data_ov006_0213bbb4")
foreach(line IN LISTS SLICE_SOS_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SOS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# run mg6 lane S75: dScMgCurling2_c, actor id 0x177 (scene 375) -- the ADVANCED
# half of the curling pair whose basic half is scene 374, and the seventh ov006
# minigame class the port seats.
#
# ONE TABLE, TWO DEEP. Scene -> dScMgBase_c (ov004 0x020bc0c0) ->
# dScMgCurling2_c (ov006 0x0213c510), 36 slots, checked three ways before a
# word was written (port/slice_s75.txt). No intermediate table, so unlike the
# flower block below there is one face array and no hostgen substitution.
#
# WHAT IT DOES NEED IS THE WHOLE WALL. Five dispatching TUs, seven pointer-to-
# member tables and twenty-five states, covering every one of the three hiding
# places port/mg_fanout_costs.txt section 4 and lane FLW enumerate. All five are
# host-copied in unmatched/MgCurling2_StateDispatch.cpp and all five are
# therefore OUT of slice_s75.txt -- listing both copies would be a duplicate
# symbol, and listing neither would drop the dispatch silently. The slice file
# names the five in its header so the trade is readable from either end.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_s75.txt" SLICE_S75_LINES)
# unmatched/MgCurling2_Traps.cpp IS GONE, run mg7 lane L375. It stood in for
# func_ov006_020e513c and func_ov006_020e5450 while both were decomp gaps, and
# both now have src bodies that slice_s75.txt lists, so keeping the file would
# be two duplicate symbols. Its counters are replaced by the per-slot census in
# MgCurling2_StateDispatch.cpp, which counts at the DISPATCH SITE rather than
# inside a ROM body.
set(SLICE_S75_SOURCES unmatched/MgCurling2_StateDispatch.cpp)
foreach(line IN LISTS SLICE_S75_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_S75_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# The five host-copied dispatchers must stay out of the slice. A slice line that
# came back (a rename, a hand edit, a merge) would define the same symbol twice
# and the link would say so -- but it would say so as a duplicate-symbol wall
# far from its cause, so the assertion is here instead.
foreach(dup IN ITEMS src/func_ov006_020e683c.cpp src/func_ov006_020e6354.cpp
                     src/func_ov006_020e4800.cpp src/func_ov006_020e42b4.cpp
                     src/func_ov006_020e4744.cpp)
    if(dup IN_LIST SLICE_S75_LINES)
        message(FATAL_ERROR "slice_s75.txt lists ${dup}, which "
                "unmatched/MgCurling2_StateDispatch.cpp host-copies. Taking "
                "both is a duplicate symbol; taking the src one puts the mwcc "
                "member-pointer stride back")
    endif()
endforeach()

# THE DELETING DESTRUCTOR SPELLS ITS OWN TABLE AS THE SHARED PLACEHOLDER `VT`,
# and this is the third minigame class to need the rename. src/func_ov006_
# 020e3878.c line 9 is `t[0] = (int)VT;` and hal/cxx_aliases.cpp:1231 binds
# bare VT to _data_ov002_021081e4, the ov002 ENEMY base table. Without the line
# below this class's slot-17 D0 writes the ENEMY vtable into the object on
# teardown.
#
# THE ADDRESS IS THIS BODY'S OWN LITERAL POOL, read out of
# extracted/overlays/overlay_0006.bin at base 0x020bfec0: 0x020e38a8 holds
# 0213C510, which is this class's table. Its unmarked D2 sibling
# func_ov006_020e3854 stores the same word from its own pool at 0x020e3874 and
# is spelled with the symbol rather than the placeholder, so the two halves of
# the destructor pair cross-confirm the binding.
#
# HEAP IS DELIBERATELY NOT RENAMED. The second pool word at 0x020e38ac is
# 020A0EAC, which is the game heap cxx_aliases.cpp:1232 already binds bare HEAP
# to, so the global alias is correct for this TU and a rename would be noise.
#
# AND THE LINK IS NOT A DETECTOR FOR THIS. The wrong binding resolves cleanly,
# and so does a run: slot 17 is the deleting destructor, so a scene that boots
# and ticks never enters it. This lane's 60-frame and 1200-frame runs of scene
# 375 both entered slots 0, 6 and 9 only. The check that works is reading the
# TU and the body's own pool, which is what the paragraph above is.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020e3878.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=data_ov006_0213c510")

# RUN MG8 lane SGX: THE PADDLE DRAWS FROM ANOTHER OVERLAY'S BYTES.
# Tango's lap-2 report on scene 375 is "I cant see the paddle if im suposed to".
# src/func_ov006_020e4b78.c is the paddle draw -- slot 9 Render calls it last,
# it is gated on the cue-visible flag at +0x55b9, and it hands ONE OamAttr
# template to func_ov004_020afdd0 at the cue's own (x,y) less (0x20,8), the
# half-size offset of a 64x16 sprite pair. The template it names is
# data_ov098_0213c4f0. THAT IS THE WRONG OVERLAY.
#
# 0x0213c4f0 is one address with two occupants, exactly the ov002/ov004 shape
# lane A2 fixed above. Both name it and both are right about their own image:
#   config/arm9/overlays/ov006/relocs.txt:3577
#     from:0x020e4bd0 kind:load to:0x0213c4f0 module:overlay(6)
#   config/arm9/overlays/ov098/relocs.txt:143
#     from:0x021388b4 kind:load to:0x0213c4f0 module:overlay(98)
# 0x020e4bd0 is func_ov006_020e4b78's OWN literal pool word (the body is
# 0x020e4b78..0x020e4bd4), so the load is INTERNAL to ov006 and dsd tagged it
# overlay(6) with no ambiguity. The src TU spells the ov098 name, ov098 is
# mounted (PORT_ACTOR_OVERLAYS), so the port links ov098's bytes into a
# dScMgCurling2_c frame -- and ov098 is a castle-grounds actor overlay that can
# never be co-resident with ov006.
#
# THE BYTES ARE NOT THE SAME AND ARE NOT COMPATIBLE, read out of
# extracted/overlays/ at each overlay's own OVT base:
#   ov006 (base 0x020bfec0)  80004000 00001008 80204000 0000100c
#     = attr {0x4000,0x8000,0x1008} and {0x4000,0x8020,0x100c}: two 32x16
#       sprites, 16-colour, palette 1, priority 0, tiles 0x008 and 0x00c, at
#       x=0 and x=0x20 -- a 64x16 paddle, which is what the -0x20 in the draw
#       centres. Scene 374's own bumper is the SAME pair (32x16 tiles 8 and 12,
#       palette 1, priority 0) in its engine-B OAM, measured with
#       SM64DS_PPU_AUDIT, so the shape is corroborated from a second scene.
#   ov098 (base 0x02137be0)  00014000 00014000 00014000 0000f000
#     = ov098's cannon data. As OAM attributes it is a 16x8 entry at x=1 on
#       tile 0 followed by garbage.
# A -D and not an /alternatename, for lane A2's reason: data_ov098_0213c4f0 is
# DEFINED in this binary by the ov098 mount, and an alias with a defined symbol
# on the left is defeated silently. This is a port-side binding fix; the src
# TU's own spelling is a decomp naming defect and is ROUTED, not taken here.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020e4b78.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov098_0213c4f0=data_ov006_0213c4f0")

set(SCENE_MG_CURLING2_SOURCES hal/scene_mg_curling2.cpp)
# Run mg6 lane PPP: THE SEVENTH SEATED MINIGAME CLASS, dScMgPanel_c, actor id
# 0x17c = scene 380 -- the minigame the spawn symbol names
# MgPuzzlePanelPuzzlePanic. The class name is the ROM's and is proved by the
# type_info the word BEFORE the vtable points at: data_ov006_0213e24c[-1] is
# 0x0213dd40, whose name pointer is 0x0213dd84, which reads "12dScMgPanel_c".
#
# ONE VARIABLE, for the reason the CCN block above states: the slice's ov006
# bodies are only reachable through the address switch in
# unmatched/MgPanel_StateDispatch.cpp, and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-380 row, so
# every target that compiles scene_boot.cpp references port_mg_panel_spawn and
# port_scene_fill_panel by name -- the same coupling every minigame row has.
#
# ELEVEN DISPATCHER HOST COPIES, NOT FIVE: this class runs a TWO-LEVEL state
# machine, so unmatched/MgPanel_StateDispatch.cpp carries eleven bodies rather
# than curling's five. Ten of them came with lane PPP's seat; the eleventh is
# func_ov006_02106ca4, which run mg7 lane L380 decompiled and which turned out
# to dispatch data_ov006_02142840 as well as being slot 6 of the Behavior's own
# table.
#
# unmatched/MgPanel_Faces.cpp IS NOW ONLY THE VTABLE FACE. It used to carry a
# counting trap for func_ov006_02106168, the layout picker slot 0 and slot 18
# both call; that body is decompiled too and joins the slice as an ordinary
# line, so nothing in this seat stands in for a ROM body any more.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under
# them; this class's ov004 delta is measured ZERO. CMake appends both lists to
# the same target and dedupes identical absolute paths, so nothing is compiled
# twice. port/slice_ppp.txt says what it reaches and records the class's ONE
# remaining hard floor (func_ov006_021053a8) and two stategen join misses.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ppp.txt" SLICE_PPP_LINES)
set(SLICE_PPP_SOURCES unmatched/MgPanel_StateDispatch.cpp
                      unmatched/MgPanel_Faces.cpp)
foreach(line IN LISTS SLICE_PPP_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_PPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# THE DELETING DESTRUCTOR SPELLS ITS OWN TABLE AS THE SHARED PLACEHOLDER `VT`,
# the same defect the LUI block above renames one TU for. src/
# func_ov006_021042b0.c is `t[0] = (int)VT;` and hal/cxx_aliases.cpp binds bare
# VT to _data_ov002_021081e4, the ENEMY base table. The address below is the
# body's OWN literal pool, read out of extracted/overlays/overlay_0006.bin:
# 0x021042e0 holds 0213E24C. HEAP is deliberately NOT renamed -- the second
# pool word at 0x021042e4 is 020A0EAC, the game heap cxx_aliases.cpp already
# binds it to.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_021042b0.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=data_ov006_0213e24c")

# run mg5 lane FLW: THE SECOND MINIGAME CLASS, and the first with a
# three-deep hierarchy.
#
# dScMgFlower_c, actor id 0x186 (390) -- the "Loves Me...?" petal minigame.
# Scene -> dScMgBase_c (ov004) -> dScMgSingle3DBase_c (ov006 0x0213e448) ->
# dScMgFlower_c (ov006 0x02140140), all three tables 36 slots wide and all
# three checked three ways before a word was written (port/slice_flw.txt).
#
# WHAT IT DOES NOT NEED IS THE INTERESTING PART. This class has NO mwcc
# pointer-to-member state machine: slot 6 is a plain switch, no overlay
# constructor builds a pair table for it, and a `::*` sweep of every TU in the
# slice is clean. So there is no address switch, no dispatcher host copy and no
# stategen output on this lane -- port/mg_fanout_costs.txt section 4's "the
# wall is the same one for all thirty" gains a counterexample.
#
# It rides the SAME three targets as the curling seat and it is not optional on
# any of them: hal/scene_boot.cpp's port_scene_classes table now carries the
# flower row, so every target that compiles scene_boot.cpp references
# port_mg_flower_spawn and port_scene_fill_flower by name.
# ONE TU IN THIS SLICE REACHES A GEOMETRY REGISTER, so it builds from a
# hostgen-emitted copy the way every other MMIO-touching src/ file in this
# tree does -- src/ itself is never edited.
#
# func_ov006_0210a708 is dScMgSingle3DBase_c slot 33, the once-per-boot 3D
# setup every ov006 3D minigame runs, and its last two statements are
#
#     *(vu32 *)0x40004ccu = 0x7fff;        LIGHT_COLOR, light 0 white
#     *(vu32 *)0x40004ccu = 0x40007fff;    LIGHT_COLOR, light 1 white
#
# Compiled straight from src/ those two stores land in the real memory ntr
# maps across the I/O window and stop there: the geometry engine is never
# told, so light 0 keeps colour (0,0,0) and every lit vertex in the scene
# resolves to emission, which every material in this game writes as 0. That
# is the "Yoshi is a black silhouette" report -- measured, before this line,
# as "LIGHT_COLOR commands executed: 0" beside "I/O LATCH
# LIGHT_COLOR(4cc)=40007fff" (SM64DS_MAT_LOG, run mg5 lane YTEX).
#
# The pattern missed it TWICE: hostgen's MMIO type list had no vu32 and the
# `u` suffix on the literal defeated the word boundary after seven hex
# digits. Both are fixed in tools/hostgen.py, and the fix is proven inert
# over the 159 files already in the hostgen set (regenerating all of them
# with the old and new tool gives byte-identical output).
set(FLW_HOSTGEN_SYMS func_ov006_0210a708)
set(FLW_HOSTGEN_GEN "")
set(FLW_HOSTGEN_SKIP "")
foreach(sym IN LISTS FLW_HOSTGEN_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        list(APPEND FLW_HOSTGEN_SKIP "src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        list(APPEND FLW_HOSTGEN_SKIP "src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND FLW_HOSTGEN_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_flw.txt" SLICE_FLW_LINES)
set(SLICE_FLW_SOURCES ${FLW_HOSTGEN_GEN})
foreach(line IN LISTS SLICE_FLW_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # The hostgen'd copy stands in for the src/ one; taking both would be a
    # duplicate symbol, and taking neither would drop the body silently.
    if(line IN_LIST FLW_HOSTGEN_SKIP)
        continue()
    endif()
    list(APPEND SLICE_FLW_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# A slice line that stops matching (a rename, a .c that becomes a .cpp) would
# silently take the src/ copy back and put the defect back with it, so the
# substitution asserts rather than hopes.
list(LENGTH FLW_HOSTGEN_SYMS flw_hg_want)
list(LENGTH FLW_HOSTGEN_GEN flw_hg_have)
if(NOT flw_hg_want EQUAL flw_hg_have)
    message(FATAL_ERROR "slice_flw hostgen substitution incomplete: "
            "${flw_hg_have} of ${flw_hg_want} generated")
endif()
foreach(skip IN LISTS FLW_HOSTGEN_SKIP)
    if(NOT skip IN_LIST SLICE_FLW_LINES)
        message(FATAL_ERROR "slice_flw.txt no longer lists ${skip}; the "
                "hostgen substitution for it is now dead and the geometry "
                "register writes in it would go back to latching silently")
    endif()
endforeach()

set(SCENE_MG_FLOWER_SOURCES
    hal/scene_mg_flower.cpp
    # TWO ARGUMENT RIDE-THROUGHS THIS CLASS OWES, both PORT_HOST_ABI host
    # copies under the coordinator's ruling, both the shape
    # port/mg_fanout_costs.txt section 6 prescribes for its own (b) and (c).
    # func_ov006_0210a6e4 drops the framework's second argument because the
    # ROM rides it through in r1; func_ov004_020ad8b8 drops the pointer the
    # ROM rides through r0 into func_ov004_020adc3c. Each REPLACES its src TU
    # rather than joining it -- both are out of slice_flw.txt -- so they are
    # not optional on any target that compiles the slice.
    unmatched/MgFlower_Slot2.cpp
    unmatched/MgFlower_InitScore.cpp
    # And the class's half of the pointer-to-member wall. It exists after all,
    # in a form section 4's two silent shapes do not cover: a dispatcher that
    # open-codes the Itanium sequence in plain ints. Replaces
    # src/func_ov006_020c3d18.cpp, which is out of the slice.
    unmatched/MgFlower_SubDispatch.cpp
    # Run mg5 lane Y3D: the class's MODEL DRAW, and the reason the minigame
    # showed its 2D background with no flower and no Yoshi. Same local-shadow-
    # class defect unmatched/Door_Render.cpp records on the same two classes:
    # the ROM's vtable BYTE +0x14 is Render, MSVC's sixth word is Virtual18,
    # so the matched TU handed Virtual18 a null matrix, Model::Virtual10 read
    # address 0 and the quarantine froze the whole scene actor on frame 0.
    # Replaces src/func_ov006_020c3bf4.cpp, which is out of the slice.
    unmatched/MgFlower_ModelRender_020c3bf4.cpp)

# Run mg6 lane MEM: dScMgMemory2_c (Memory Master), actor id 0x16b = scene 363,
# and the SECOND three-deep hierarchy the port seats.
#
#   Scene -> dScMgBase_c (ov004 0x020bc0c0) -> dScMgSingle3DBase_c
#         (ov006 0x0213e448) -> dScMgMemory2_c (ov006 0x0213d4d4)
#
# all three tables 36 slots wide and both new ones checked three ways before a
# word was written (port/slice_mem.txt). The middle table is the same object
# dScMgFlower_c and dScMgCup_c sit under; hal/scene_mg_memory2.cpp section 3
# measures why two files defining their own face array over it is safe and what
# decides which one claims it.
#
# ONE VARIABLE, for the reason the flower and fader blocks give: the slice's
# ov006 bodies are only reachable through the address switch in
# unmatched/MgMemory2_StateDispatch.cpp, and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-363 row, so
# every target that compiles scene_boot.cpp references port_mg_memory2_spawn and
# port_scene_fill_memory2 by name.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under them;
# CMake appends every list to the same targets and dedupes identical absolute
# paths, so nothing is compiled twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_mem.txt" SLICE_MEM_LINES)
set(SLICE_MEM_SOURCES hal/scene_mg_memory2.cpp
                      unmatched/MgMemory2_StateDispatch.cpp
                      unmatched/MgMemory2_FieldPmf.cpp
                      # The aliases the link asked for and two __thiscall
                      # method faces. Its named trap for func_ov006_020f5b98
                      # is GONE: run mg7 lane MEMCARDS decompiled that body
                      # (the card draw) into src/func_ov006_020f5b98.c and
                      # slice_mem.txt carries it.
                      unmatched/MgMemory2_Faces.cpp
                      # REPLACES src/func_ov006_020c0a48.c, which is out of the
                      # slice: its three SharedFilePtr file ids are spelled as
                      # three globals that exist in no config, and the ROM
                      # loads 0x205/0x206/0x207 out of its own literal pool.
                      unmatched/MgMemory2_SubObjCtor.cpp
                      # REPLACES src/func_ov006_020c1804.cpp, which is out of
                      # the slice: the local-shadow-class model draw (two call
                      # sites) plus two LIGHT_VECTOR stores that a plain src
                      # build lands in the I/O window. Same pair
                      # unmatched/MgFlower_ModelRender_020c3bf4.cpp carries.
                      unmatched/MgMemory2_ModelRender_020c1804.cpp
                      # REPLACES src/func_ov006_020c06dc.cpp, out of the slice:
                      # the same shadow-class defect one call deeper, at the
                      # slot where the wrong body ALSO `ret 4`s and unbalances
                      # the stack.
                      unmatched/MgMemory2_ShadowSlot_020c06dc.cpp)
foreach(line IN LISTS SLICE_MEM_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_MEM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Run mg9 lane CUP: dScMgCup_c -- the minigame the ROM calls "Tox Box Shuffle"
# -- actor id 0x169 = scene 361, and the THIRD class the port seats under
# dScMgSingle3DBase_c.
#
#   Scene -> dScMgBase_c (ov004 0x020bc0c0) -> dScMgSingle3DBase_c
#         (ov006 0x0213e448) -> dScMgCup_c (ov006 0x0213c154)
#
# all three tables 36 slots wide and both new ones checked three ways before a
# word was written (port/slice_cup.txt section 2). The third check is not a
# formality on THIS class: the word past the end of 0x0213c154 is the mwcc pair
# {0x020e0a24, 0} that src/__sinit_ov006_021304ac.c copies into CURLING's state
# table, so a 37-slot fill would corrupt scene 374. That is the case
# port/mg_fanout_costs.txt section 11 was written from.
#
# The middle table is the same object dScMgFlower_c (390) and dScMgMemory2_c
# (363) sit under. The scene-361 row is APPENDED AFTER BOTH of theirs in
# hal/scene_boot.cpp, so the flower's fill still claims it and both existing
# witnesses are unchanged; hal/scene_mg_cup.cpp section 3 has the measurement
# and the argument for promoting kSingle3DFaces to a seam before a FOURTH class
# under this base is seated.
#
# ONE VARIABLE, for the reason the memory2 and flower blocks give: the slice's
# ov006 bodies are only reachable through the two address switches in
# unmatched/MgCup_StateDispatch.cpp and unmatched/MgShared4f38_SubDispatch.cpp,
# neither file can link without them. hal/scene_boot.cpp's port_scene_classes
# table carries the scene-361 row, so every target that compiles scene_boot.cpp
# references port_mg_cup_spawn and port_scene_fill_cup by name.
#
# THE SLICE IS THE DELTA ONLY, and this one is a smaller delta than its
# predecessor artifact implies: eighteen of port/slice_mga361.txt's sixty-eight
# lines are already compiled in by the flower, memory2 and gate slices.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_cup.txt" SLICE_CUP_LINES)
set(SLICE_CUP_SOURCES hal/scene_mg_cup.cpp
                      # The class's TABLE dispatcher: vtable slot 6 and the
                      # eight states of data_ov006_02141870. REPLACES
                      # src/func_ov006_020e0204.cpp, which is out of the slice.
                      unmatched/MgCup_StateDispatch.cpp
                      # REPLACES src/func_ov006_020e0574.cpp, the factory, out
                      # of the slice: it calls the base constructor
                      # func_ov004_020b2adc with NO argument (correct on ARM,
                      # where r0 already holds the object) and that callee
                      # stores three vtable words through the pointer it never
                      # received. The displacement ruling section 12 grants to
                      # "whichever lane seats 361" is that file's header.
                      unmatched/MgCup_Factory.cpp
                      # REPLACES src/func_ov006_020def80.c, out of the slice:
                      # a MATCHED body that reads an UNINITIALISED local on two
                      # of its three paths. mwccarm put the variable in r1,
                      # which already holds the parameter the ROM's epilogue
                      # uses, so the src is faithful and byte-exact; MSVC gave
                      # it a register holding the object pointer and the first
                      # frame of scene 361 wrote two zero words four gigabytes
                      # out of range. Invisible to the byte gate, to the link
                      # and to a member-pointer sweep -- only a run convicts it.
                      unmatched/MgCup_UninitEpilogue_020def80.cpp
                      # THREE SHARED BODIES OF THE +0x4f38 SUB-OBJECT, host-
                      # copied ONCE and listed by BOTH this block and the BOX
                      # block below. ov006's relocs.txt carries exactly two
                      # arm_calls into each of them, one from dScMgCup_c and
                      # one from dScMgSound_c, so they belong to neither class.
                      # Run mg9 adjudication; the rulings and the ROM evidence
                      # are in each file's header.
                      #   020c2b8c  the FIELD-held pointer-to-member dispatch,
                      #             SIX routed states (lane CUP found four --
                      #             correct for its own closure; 020c22d8 and
                      #             020c26f4 are installed only from
                      #             dScMgSound_c's block and lane BOX routes
                      #             them). REPLACES src/func_ov006_020c2b8c.cpp.
                      unmatched/MgShared4f38_SubDispatch.cpp
                      #   020c29dc  the local-shadow-class model draw at TWO
                      #             call sites (byte +0x14 of the Itanium table
                      #             is Render; the sixth MSVC word is Virtual18
                      #             on BlendModelAnim and nothing on Model),
                      #             plus four LIGHT_COLOR / LIGHT_VECTOR stores
                      #             that a host copy must route through
                      #             NTR_MMIO because tools/hostgen.py never sees
                      #             this file. Both lanes wrote behaviourally
                      #             identical copies and both crash traces are
                      #             in the header. REPLACES
                      #             src/func_ov006_020c29dc.cpp.
                      unmatched/MgShared4f38_ModelRender_020c29dc.cpp
                      #   020c2924  a RECEIVER DROP at a host C++ virtual: a
                      #             __cdecl-typed raw vtable read against a
                      #             __thiscall body, where the stack balances so
                      #             nothing faults and no ABI checker sees it,
                      #             and the garbage answer gates seven stores.
                      #             Found by lane BOX; scene 361 ran the
                      #             DEFECTIVE src TU until this merge. REPLACES
                      #             src/func_ov006_020c2924.cpp, which leaves
                      #             BOTH slice_cup.txt and slice_box.txt.
                      unmatched/MgShared4f38_ShadowFader_020c2924.cpp)
foreach(line IN LISTS SLICE_CUP_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_CUP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Run mg11 lane PGO: dScMgMCarlo2_c, actor id 0x17e (scene 382) -- "Pair-a-Gone
# And On", the ENDLESS-MODE sequel to scene 381's Pair-a-Gone.
#
# THREE TABLES, NOT TWO. Scene -> dScMgBase_c (ov004 0x020bc0c0) ->
# dScMgSingle3DBase_c (ov006 0x0213e448) -> dScMgMCarlo2_c (ov006 0x0213d7e8),
# all three 36 slots, PLUS the element class dMgMCarlo2CardObj_c at
# 0x0213d728, two slots, which the factory writes into the first word of all
# forty card records. Both element slots are live -- Render dispatches slot 0
# and the pair matcher dispatches slot 1 -- so a seat that filled only the scene
# tables would link, boot and jump to a raw DS address on the first rendered
# frame. port/slice_pgo.txt is the derivation.
#
# THE WIDTH IS 36 BY FIVE CHECKS AND THE THIRD ONE IS FOOLED HERE: index 36 of
# 0x0213d7e8 reads 0x020fb4e0, a real code address, because a 25-entry run of
# mwcc {code, 0} pairs starts immediately after the table. Those pairs belong to
# MgBobOmbSquad (0x170) -- their code words are all past this class's block, the
# five .bss tables __sinit_ov006_02131a38 copies them into are read from five
# dispatchers in 0x170's block, and 0x170's own vtable holds 0x020fa75c in slot
# 16, exactly where this class's code ends. A 37-slot fill would have written a
# host thunk over another minigame's first state.
#
# NO HOSTGEN SUBSTITUTION AND NO STATE DISPATCH. This class has no pointer-to-
# member table (no relocation from ov006's .init reaches its code block at all),
# so there is no stategen output; and its slot 0's two PPU writes (0x04000008,
# 0x0400100a) are plain state registers rather than the write-triggered class
# the NTR_MMIO proxy exists for, so it builds from src the way dScMgFlower_c's
# and dScMgMCarlo_c's slot 0 do. The middle base's slot 2 and slot 33
# substitutions are inherited through slice_mem.txt and FLW_HOSTGEN_GEN.
#
# ONE VARIABLE, for the reason the flower and fader blocks give: the slice and
# the .cpp are not separable. hal/scene_boot.cpp's port_scene_classes table now
# carries the scene-382 row, so every target that compiles scene_boot.cpp
# references port_mg_mcarlo2_spawn and port_scene_fill_mcarlo2 by name.
#
# THE SLICE IS THE DELTA ONLY -- this class's own twenty-five bodies. The ov004
# framework, the thirty-five overlay constructors and the arm9 waves come in
# through slice_mg1.txt; the Single3D sub-object family through slice_mem.txt;
# func_ov006_020c1164 and func_ov006_020c16b4 through slice_s381.txt. CMake
# appends every list to the same targets and dedupes identical absolute paths.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_pgo.txt" SLICE_PGO_LINES)
set(SLICE_PGO_SOURCES hal/scene_mg_mcarlo2.cpp)
foreach(line IN LISTS SLICE_PGO_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_PGO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Run mg9 lane MMT: dScMgMemory_c (Memory Match), actor id 0x16a = scene 362,
# and the THIRD seated class under dScMgSingle3DBase_c.
#
#   Scene -> dScMgBase_c (ov004 0x020bc0c0) -> dScMgSingle3DBase_c
#         (ov006 0x0213e448) -> dScMgMemory_c (ov006 0x0213d1b8)
#
# both derived tables 36 slots wide, and 0x16a is one of the twelve rows
# port/mg_fanout_costs.txt section 11 corrected from 37 to 36 -- so the width
# was checked FOUR ways before a word was written (port/slice_mmt.txt), and
# check 3 names the victim a 37th slot would have had: the word at index 36 is
# the code half of an mwcc pair whose body is dScMgMemory2_c's round-end state,
# on a tree where scene 363 is seated and green.
#
# ONE VARIABLE, for the reason the flower, memory2 and fader blocks give: the
# slice's ov006 bodies are only reachable through the address switch in
# unmatched/MgMemory1_StateDispatch.cpp, and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-362 row, so
# every target that compiles scene_boot.cpp references port_mg_memory1_spawn and
# port_scene_fill_memory1 by name.
#
# THE SLICE IS THE DELTA ONLY and this lane's delta is unusually small: its
# ov004 closure adds NOTHING (all 103 functions are already carried or already
# host-copied) and its ov006 delta is this class's own code block plus nothing,
# because the model sub-object at +0x4f38 is the same one run mg6 lane MEM
# already seated for the sibling. CMake appends every list to the same targets
# and dedupes identical absolute paths, so nothing is compiled twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_mmt.txt" SLICE_MMT_LINES)
set(SLICE_MMT_SOURCES hal/scene_mg_memory1.cpp
                      unmatched/MgMemory1_StateDispatch.cpp
                      # The aliases the link asked for. Its named trap for
                      # func_ov006_020f3e68 is GONE: run mg10 lane F362
                      # decompiled that body (the card draw) into
                      # src/func_ov006_020f3e68.c and slice_mmt.txt carries it.
                      unmatched/MgMemory1_Faces.cpp)
foreach(line IN LISTS SLICE_MMT_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_MMT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Run mg9 lane BOX: THE TWELFTH SEATED MINIGAME CLASS, dScMgSound_c, actor id
# 0x16f = scene 367 -- the minigame the spawn symbol names MgBoomBox. The class
# name is the ROM's own and is proved by the type_info the word BEFORE the
# vtable points at: data_ov006_0213f844[-1] is 0x0213f6e4, whose name pointer
# is 0x0213f708, which reads "12dScMgSound_c". That record's base pointer is a
# second type_info reading "19dScMgSingle3DBase_c", so this is the FOURTH class
# under that intermediate base and the THIRD one seated under it.
#
# THE hal FILE IS NAMED FOR THE MINIGAME AND NOT FOR THE CLASS, which is the
# one place this seat departs from the flower/memory2 naming:
# hal/scene_mg_sound.cpp already exists and is the FAMILY's ov005
# sound-and-spawn-param seat, so a second file named for dScMgSound_c would
# read as its sibling and is not.
#
# ONE VARIABLE, for the reason the CCN and PPP blocks above state: the slice's
# ov006 bodies are only reachable through the address switch in
# unmatched/MgSound_StateDispatch.cpp, and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-367 row, so
# every target that compiles scene_boot.cpp references port_mg_boombox_spawn
# and port_scene_fill_boombox by name -- the same coupling every minigame row
# has.
#
# SIXTEEN DISPATCHER HOST COPIES, NOT FIVE AND NOT ELEVEN. Section 14 raised
# the wall from curling's five to dScMgPanel_c's eleven and said the question
# to ask is "how deep is the machine". This one is two deep over FIFTEEN
# tables: vtable slot 6 is a plain switch that calls TWO open-coded
# dispatchers, one of which drives a thirteen-slot table EVERY ONE OF WHOSE
# STATES IS ITSELF A DISPATCHER. Fifty-seven pairs, every adjustment zero.
# unmatched/MgSound_StateDispatch.cpp carries fifteen of the sixteen; the
# sixteenth is the +0x4f38 sub-object's FIELD member pointer, in the same file.
#
# ALL THREE PMF SHAPES IN ONE CLOSURE, which no earlier seat could state: one
# TU is link-visible, twelve are silent behind extern "C" and found only by the
# `::*` sweep, and two open-code the ARM Itanium sequence in plain ints and are
# invisible to BOTH prescribed detectors. Both sweeps were run over the whole
# closure rather than over an LNK1120.
#
# THREE MORE HOST COPIES ARE NOT WALL WORK, and they are one disease in three
# disguises -- a call that reaches the right code with the wrong ABI. None is
# findable by either prescribed detector, and each was found by a different
# instrument, which is the part worth carrying forward:
#   unmatched/MgSound_ShadowSlot35.cpp replaces src/func_ov006_0211ba88.c, a
#     state body that is a delinks join miss AND drops its receiver at a
#     vtable slot 35 call AND approximates the ROM's modulo loop AND writes an
#     int where the ROM writes a byte over a live neighbouring gate AND nests
#     the `n == 0` test where the ROM's is unconditional, so the first of the
#     ten records never gets its flag set. FOUR divergences; its banner claims
#     "Logic verified correct vs ROM" and that file's header is the evidence
#     it is not, and is the ROUTING NOTE that lists all four. Found by READING
#     the body against the ROM; the fourth by the run mg9 reviewer.
#   unmatched/MgShared4f38_ModelRender_020c29dc.cpp -- found by the BATTERY,
#     which renders where this lane's own boots did not.
#   unmatched/MgShared4f38_ShadowFader_020c2924.cpp -- found by a WORD-BY-WORD
#     sweep for `blx Rm` through a register loaded from an object. Nothing
#     else could have: no unresolved symbol, no `::*`, no shadow class in the
#     source to grep for, and every boot clean over it.
# src/ is untouched by all three; the defects are the host's.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under
# them; this class's ov004 delta is TWO lines. CMake appends every list to the
# same targets and dedupes identical absolute paths, so nothing is compiled
# twice. port/slice_box.txt says what it reaches. THE CLASS'S ONE HARD FLOOR IS
# GONE: func_ov006_0211bc8c was a STATE and not a vtable slot, which is why
# section 3's nosrc column read 0 and never saw it, and run mg10 lane F367
# decompiled it -- it is src/func_ov006_0211bc8c.cpp in the slice now.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_box.txt" SLICE_BOX_LINES)
set(SLICE_BOX_SOURCES hal/scene_mg_boombox.cpp
                      unmatched/MgSound_StateDispatch.cpp
                      # REPLACES src/func_ov006_0211ba88.c, which is out of the
                      # slice and is a delinks join miss besides.
                      unmatched/MgSound_ShadowSlot35.cpp
                      # The four ordinary /alternatename rows the first link
                      # wave asked for. No trap and no stand-in body.
                      unmatched/MgSound_Faces.cpp
                      # THE SAME THREE SHARED +0x4f38 BODIES THE CUP BLOCK
                      # ABOVE LISTS, host-copied once and named in both blocks
                      # on purpose: CMake dedupes identical source paths within
                      # a target, so each compiles once, and listing them here
                      # keeps this seat's block honest about what its own
                      # closure needs -- a later run that retires the 361 seat
                      # cannot silently delete 367's dependency. Run mg9
                      # adjudication; the evidence is in each file's header.
                      #   020c2b8c  the FIELD-held pointer-to-member dispatch.
                      #             This lane's six routed states are the
                      #             canonical set: 020c22d8 and 020c26f4 are
                      #             installed only from dScMgSound_c's block,
                      #             and 020c26f4 is LIVE in scene 367.
                      #             REPLACES src/func_ov006_020c2b8c.cpp.
                      unmatched/MgShared4f38_SubDispatch.cpp
                      #   020c29dc  the sub-object's model draw, with the same
                      #             PAIR of defects
                      #             MgMemory2_ModelRender_020c1804.cpp and
                      #             MgFlower_ModelRender_020c3bf4.cpp carry -- a
                      #             local shadow class calling ROM vtable byte
                      #             +0x14 at two sites, and four I/O-window
                      #             stores that need the geometry proxy. Found
                      #             by the BATTERY, which renders; this lane's
                      #             own NO_RENDER boots were clean over it. The
                      #             canonical file keeps this lane's NTR_MMIO
                      #             spelling. REPLACES
                      #             src/func_ov006_020c29dc.cpp.
                      unmatched/MgShared4f38_ModelRender_020c29dc.cpp
                      #   020c2924  a RECEIVER DROP at a host C++ virtual -- a
                      #             __cdecl-typed raw vtable read against a
                      #             __thiscall body, where the stack balances so
                      #             nothing faults and no ABI checker can see
                      #             it. Its own sibling one line earlier spells
                      #             the same call correctly and the host copy
                      #             borrows that spelling. Found by a
                      #             WORD-BY-WORD `blx Rm` sweep, not by a link,
                      #             a `::*` sweep, a source grep or a run.
                      #             REPLACES src/func_ov006_020c2924.cpp.
                      unmatched/MgShared4f38_ShadowFader_020c2924.cpp)
foreach(line IN LISTS SLICE_BOX_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_BOX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# THE STATE RESET SPELLS THE ov004 SINGLETON AS A BARE PLACEHOLDER.
# src/func_ov006_0211c5d0.c reads `g_020beb68`, which include/decl_common.h
# declares as `extern int *g_020beb68` and which exists in NO config -- the
# same per-TU placeholder defect the LUI and PPP blocks above rename VT and
# HEAP for. THE ADDRESS IS THE BODY'S OWN LITERAL POOL, read out of
# extracted/overlays/overlay_0006.bin: 0x0211c6c0 holds 020BEB68, which is
# data_ov004_020beb68, the live dScMgBase_c the framework keeps. The ROM loads
# the POINTER STORED THERE (`ldr r0,[pc,...]` then `ldr r1,[r0]`), which is
# what the src's `int *` spelling means, so the rename is the whole fix.
# ONE SOURCE, ONE SYMBOL: this is the only TU in the tree that names
# g_020beb68, and the -D is scoped to it rather than made repo-global.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_0211c5d0.c"
    PROPERTIES COMPILE_DEFINITIONS "g_020beb68=data_ov004_020beb68")

# run mg9 lane S371: dScMgAmida_c, actor id 0x173 (scene 371) -- THE ONE TRUE
# WIDTH-37 CLASS of the twenty-nine. Slot 35 is this class's own
# func_ov006_020d1170 rather than the family terminator ov004 0x020ad660, and
# slot 36 is a real 37th virtual, func_ov006_020d1188, reached by a load
# relocation and dispatched twenty-five times inside the class's own code.
# port/slice_s371.txt writes out how each of port/mg_fanout_costs.txt section
# 11's three checks answers for a genuine 37.
#
# ONE TABLE, TWO DEEP. Scene -> dScMgBase_c (ov004 0x020bc0c0) -> dScMgAmida_c
# (ov006 0x0213b918). The factory and both destructors each store exactly one
# vtable word, so there is one face array and no hostgen substitution.
#
# NOTHING IS HOST-COPIED FOR THIS CLASS, which is the difference between this
# block and the S75 and PPP blocks above it. The class has no pointer-to-member
# state machine at all -- its overlay constructor builds no pair table, no TU
# declares a member-pointer type, and the only twelve DATA relocations into its
# code block are the SpawnInfo's factory word and the eleven vtable slots. So
# no slice line is excluded and there is no duplicate-symbol assertion to make.
#
# unmatched/MgAmida_Faces.cpp IS NOT A FACE FILE DESPITE THE NAME, and as of
# run mg10 lane WALKER it is not a TRAP file either -- it traps nothing. It held
# the two counting traps for func_ov006_020d27dc (0xe48) and
# func_ov006_020d36a4 (0x4fc); both are decompiled now and both traps are
# deleted, so what is left is the DERIVATION that found the holes. It stays in
# the slice because that derivation -- a delinks join over every callee of every
# override, which is the check that finds a callee-shaped floor when the nosrc
# column reads zero -- is the reusable part, and deleting the file would delete
# it.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_s371.txt" SLICE_S371_LINES)
set(SLICE_S371_SOURCES hal/scene_mg_amida.cpp
                       unmatched/MgAmida_Faces.cpp
                       # REPLACES src/func_ov006_020d3ba0.c, which is out of
                       # the slice: a //cpp-marked TU whose extern "C" block
                       # tentatively defines the arm9 RNG seed. One token.
                       unmatched/MgAmida_Slot3ba0.cpp)
foreach(line IN LISTS SLICE_S371_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_S371_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# NO FLOOR HAS TO STAY OUT OF THIS SLICE ANY MORE, run mg10 lane WALKER, and
# the whole sequencing guard is gone with the last trap it sequenced. It named
# two bodies at run mg9, one after run mg10 lane F371, and none now:
# src/func_ov006_020d27dc.c is a real decompilation in the slice and its trap in
# unmatched/MgAmida_Faces.cpp is deleted, so an assertion naming it would refuse
# the very landing it was written to sequence -- which is exactly the reasoning
# that retired the 020d36a4 rows one wave earlier, applied to the last row.
#
# WHAT THE GUARD WAS FOR is worth keeping in words even though the code is gone,
# because the next class with a trapped callee needs it: a slice line that comes
# back while a trap still defines the same symbol is a duplicate symbol, and the
# link reports that as a wall far from its cause. The rule the two retirements
# share is that the trap and the slice line move IN THE SAME COMMIT.
# And the one host-copied body, for the same reason the S75 block asserts its
# five: a slice line that came back would be a duplicate symbol far from its
# cause, and taking the src one instead would put the arm9 RNG seed's second
# definition back.
if("src/func_ov006_020d3ba0.c" IN_LIST SLICE_S371_LINES)
    message(FATAL_ERROR "slice_s371.txt lists src/func_ov006_020d3ba0.c, which "
            "unmatched/MgAmida_Slot3ba0.cpp host-copies. Taking both is a "
            "duplicate symbol; taking the src one puts a second definition of "
            "the arm9 RNG seed data_0209d4b8 back in an ov006 object")
endif()
# ONE .c IN THIS SLICE IS REALLY C++, run mg10 lane F371, and the whole slice
# was swept for the //cpp marker and for extern "C", virtual, class and
# namespace rather than the file being found by a compile error.
# src/func_ov006_020d36a4.c is the only hit that is IN the slice: it dispatches
# vtable slot 36 through a dummy 37-virtual struct, which is how
# src/func_ov006_020d3ba0.c spells the same dispatch in the same class. That
# file is also //cpp and is host-copied instead, so it is not a second row.
#
# AND IT IS DECLARED extern "C" INSIDE, which is the half hostgen cannot do for
# it: tools/hostgen.py wraps every .c in extern "C" unconditionally, and a
# //cpp file's struct-with-virtuals inside that wrap is not what is wanted. The
# TU spells its own linkage.
#
# AND RUN mg10 LANE WALKER MAKES IT TWO. src/func_ov006_020d27dc.c is //cpp for
# the same reason and spells its own extern "C" for the same reason: it
# dispatches slot 36 three times through the same dummy 37-virtual struct.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020d36a4.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020d27dc.c"
    PROPERTIES LANGUAGE CXX)

# Run mg9 lane S381: dScMgMCarlo_c, the "Pair-a-Gone" card minigame, actor id
# 0x17d = scene 381, and the FOURTH class the port seats under
# dScMgSingle3DBase_c.
#
#   Scene -> dScMgBase_c (ov004 0x020bc0c0) -> dScMgSingle3DBase_c
#         (ov006 0x0213e448) -> dScMgMCarlo_c (ov006 0x0213d664)
#
# both new tables 36 slots wide and checked FOUR ways before a word was written
# (port/slice_s381.txt): span, terminal slot, what the word past the end is,
# and the relocation count inside the span. The ROM gives this id no spawn
# symbol at all, so the whole identity -- class name, hierarchy and player
# title -- is derived from the ROM in that file's header.
#
# WHAT IT DOES NOT NEED IS AGAIN THE INTERESTING PART, and this class makes it
# three: like dScMgFlower_c and unlike dScMgMemory2_c, it has NO mwcc
# pointer-to-member state machine. Slot 6 and slot 19 are plain switches on the
# same s16 at +0x60a8, no overlay constructor builds a pair table for it (zero
# relocations leave ov006's .init code for its cluster), and a `::*` sweep over
# every TU in the slice is clean. So there is no address switch, no dispatcher
# host copy and no stategen output on this lane.
#
# NO HOSTGEN SUBSTITUTION EITHER, and that is a decision rather than an
# omission. Two of this class's own bodies dereference a literal in the
# 0x04xxxxxx window -- src/func_ov006_020f8d08.cpp writes BG0CNT at 0x04000008
# and the sub engine's 0x0400100a -- and both are ordinary PPU control
# registers the display reads back out of the memory ntr maps there. The
# registers that need tools/hostgen.py's NTR_MMIO proxy are the WRITE-TRIGGERED
# ones (the geometry engine, LIGHT_COLOR at 0x040004cc), which is the defect
# run mg5 lane YTEX measured in func_ov006_0210a708 -- and that body comes in
# through FLW_HOSTGEN_GEN above, already substituted. dScMgFlower_c's own slot
# 0 writes the same class of PPU register straight from src/ for the same
# reason. This paragraph exists so the next lane does not read the absence as
# an oversight.
#
# ONE VARIABLE, for the reason the flower and fader blocks give: the slice and
# the .cpp are not separable. hal/scene_boot.cpp's port_scene_classes table now
# carries the scene-381 row, so every target that compiles scene_boot.cpp
# references port_mg_mcarlo_spawn and port_scene_fill_mcarlo by name.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under
# them, and slice_mem.txt carries the whole Single3D sub-object family this
# class shares with dScMgMemory2_c -- including the three host copies that
# family owes (MgMemory2_ModelRender_020c1804.cpp among them, which this
# class's Render reaches). CMake appends every list to the same targets and
# dedupes identical absolute paths, so nothing is compiled twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_s381.txt" SLICE_S381_LINES)
set(SLICE_S381_SOURCES hal/scene_mg_mcarlo.cpp)
foreach(line IN LISTS SLICE_S381_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_S381_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run mg9 lane WIG: dScMgHanachan_c, actor id 0x182 (scene 386) -- the "Which
# Wiggler" minigame.
#
# ONE TABLE, TWO DEEP. Scene -> dScMgBase_c (ov004 0x020bc0c0) ->
# dScMgHanachan_c (ov006 0x0213cab8), 36 slots, checked FIVE ways before a word
# was written (port/slice_wig.txt): the span, the terminal slot, what the word
# past it is, this lane's own addition of the relocation COUNT inside the table
# (exactly 36, with no row at index 36), and run mg9 lane LKY's relocation
# STRIDE (4 for exactly 36 words from 0x0213cab8, then 8 to 0x0213cb4c, the jump
# being the table ending). No intermediate table, so there is one face array and
# no hostgen substitution.
#
# THE WALL IS THE FIELD FORM AND THE LINK IS BLIND TO ALL OF IT. This class has
# NO overlay constructor and NO state table: THIRTY-FIVE {code, 0} pairs sit in
# .data at 0x0213c95c..0x0213ca74, holding fifteen distinct code words, and are
# installed by name into member-pointer FIELDS of two different
# objects. Five TUs dispatch them and not one emits a symbol a link could fail
# on. Four are findable by a `::*` source sweep; the fifth, func_ov006_020eb610,
# open-codes the ARM Itanium sequence in plain ints and is findable by neither
# prescribed detector -- it was found by decoding every word of ov006's text and
# matching `add Rd,Rn,Rm,asr #1` followed by `ands Rm,Rm,#1`. All five are
# host-copied in unmatched/MgWiggler_StateDispatch.cpp and all five are
# therefore OUT of slice_wig.txt: listing both copies would be a duplicate
# symbol, and listing neither would drop the dispatch silently. The assertion
# below is here rather than at the link because a duplicate-symbol wall reports
# far from its cause.
#
# THE SEAT IS NOT OPTIONAL ON ANY OF THE THREE TARGETS. hal/scene_boot.cpp's
# port_scene_classes table carries the scene-386 row, so every target that
# compiles scene_boot.cpp references port_mg_wiggler_spawn and
# port_scene_fill_wiggler by name -- the same coupling every other scene row
# has.
#
# unmatched/MgWiggler_Faces.cpp CARRIES THREE /alternatename ROWS, ONE ARGUMENT
# FACE AND THE TWO NAMED TRAPS, and its own header enumerates all six. The three
# alias rows are the .data pairs src/func_ov006_020eb8f0.cpp declares outside its
# extern "C" block (0x0213c96c, 0x0213c9bc, 0x0213c9c4), alias-safe because that
# TU spells them as two ints; the face is func_0203adec, which is arm9's
# _Z14ApproachLinearRsss under an address-shaped name. The traps are
# func_ov006_020ea914 -- the sole call SITE of which is inside slot 9 Render, at
# 0x020ed0c8, and it is the only arm_call relocation reaching that body anywhere
# in ov006 -- and func_ov006_020ec4dc, wiggler set-up variant 3 of 4. Both traps
# have a config symbol, no delink block and no src file in any module.
#
# NO VT OR HEAP RENAME IS OWED, which is why this block sets no per-source
# COMPILE_DEFINITIONS the way the SOS and S75 blocks above do: both of this
# class's destructors spell data_ov006_0213cab8 by its config name, and a COFF
# short-name scan for _VT and _HEAP over all 180 of this seat's built objects
# finds nothing.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_wig.txt" SLICE_WIG_LINES)
set(SLICE_WIG_SOURCES hal/scene_mg_wiggler.cpp
                      unmatched/MgWiggler_StateDispatch.cpp
                      unmatched/MgWiggler_Faces.cpp)
foreach(line IN LISTS SLICE_WIG_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_WIG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
foreach(dup IN ITEMS src/func_ov006_020ed18c.cpp src/func_ov006_020eb018.cpp
                     src/func_ov006_020eb0c8.cpp src/func_ov006_020eb31c.cpp
                     src/func_ov006_020eb610.cpp)
    if(dup IN_LIST SLICE_WIG_LINES)
        message(FATAL_ERROR "slice_wig.txt lists ${dup}, which "
                "unmatched/MgWiggler_StateDispatch.cpp host-copies. Taking "
                "both is a duplicate symbol; taking the src one puts the mwcc "
                "member-pointer stride back")
    endif()
endforeach()

# Run mg9 lane BOO: dScMgTeresa_c, actor id 0x183 = scene 387, "Hide and Boo
# Seek". The class name is the ROM's own and is proved by the type_info the
# word BEFORE the vtable points at: relocs.txt has from:0x0213fa08 to:
# 0x0213f9b4, and that record's name pointer 0x0213f9c0 reads
# "13dScMgTeresa_c". The spawn symbol MgHideAndBooSeek carries the player
# title and is the weaker witness.
#
# ONE VARIABLE, for the reason the PPP and MEM blocks above state: the slice's
# ov006 bodies are only reachable through the address switches in
# unmatched/MgTeresa_StateDispatch.cpp -- the pair words are mounted DATA
# holding DS addresses, so nothing else in the build names them and /OPT:REF
# would drop all twenty-six -- and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-387 row, so
# every target that compiles scene_boot.cpp references port_mg_booseek_spawn
# and port_scene_fill_booseek and needs both halves.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors (including this class's own,
# __sinit_ov006_02132f68) and the arm9 waves under them; this class's ov004
# delta is measured ZERO and so is its arm9 delta. CMake appends both lists to
# the same target and dedupes identical absolute paths, so nothing is compiled
# twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_boo.txt" SLICE_BOO_LINES)
set(SLICE_BOO_SOURCES hal/scene_mg_booseek.cpp
                      unmatched/MgTeresa_StateDispatch.cpp
                      # REPLACES src/func_ov006_021203fc.c, which is OUT of
                      # slice_boo.txt: vtable slot 0, and MSVC refuses it with
                      # C2036 "'void *': unknown size" on the `f2 += 0;` at
                      # its line 69. hostgen.py's VOIDPP_ARITH rewrite covers
                      # a `*(void**)(x) + n` deref and not a `void*` compound
                      # assignment, and widening a generator six other gates
                      # route their sources through is not a fan-out lane's
                      # change to make. The copy is verbatim minus that one
                      # statement, checked against the ROM's own 235 words.
                      unmatched/MgTeresa_InitResources.cpp
                      # REPLACES src/func_ov004_020b6b40.c and
                      # src/func_ov004_020b6c9c.c, both commented out of
                      # slice_mg1.txt in the same commit. THIS IS THE FAMILY'S
                      # DEFECT AND NOT THIS CLASS'S: both bodies call vtable
                      # slot 19 through a shadow array with a plain C function
                      # pointer, so MSVC cleans eight bytes at the call while
                      # hal/scene_mg.cpp's __fastcall mb_v19 cleans four of
                      # them itself, and the body's own epilogue then returns
                      # to a stack word that is not a return address. It rides
                      # THIS variable rather than slice_mg1's because the two
                      # lists reach the same three targets and this lane
                      # derived it; a later lane may want to move it. Scene
                      # 387 is the first seated scene whose FRAMEWORK state
                      # machine runs at all, which is why nothing had reached
                      # these two before.
                      unmatched/MgBase_ShadowSlot19.cpp)
foreach(line IN LISTS SLICE_BOO_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_BOO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# THE DELETING DESTRUCTOR SPELLS ITS OWN TABLE AS THE SHARED PLACEHOLDER `VT`,
# which is the defect the LUI and PPP blocks above rename one TU each for.
# src/func_ov006_0211cbf4.c is `t[0] = (int)VT;` and hal/cxx_aliases.cpp binds
# bare VT to _data_ov002_021081e4, the ENEMY base table, so without this the
# class's D0 would install another class's vtable on the way down. The address
# below is the body's OWN literal pool, read out of
# extracted/overlays/overlay_0006.bin: 0x0211cc24 holds 0213FA0C.
#
# HEAP IS DELIBERATELY NOT RENAMED. The second pool word at 0x0211cc28 is
# 020A0EAC, which is the game heap hal/cxx_aliases.cpp already binds bare HEAP
# to, so a rename here would be a no-op that later reads as a derivation.
#
# THE SIBLING NEEDS NOTHING. Slot 16, src/func_ov006_0211cbd0.c, spells its
# table as data_ov006_0213fa0c directly rather than as VT, so only one of the
# destructor pair is renamed. A sweep of all 76 slice TUs for bare VT/VT0/VT1/
# VT2/HEAP finds exactly this file plus a COMMENT in
# src/MgHideAndBooSeek_Spawn.c, whose code uses _ZTV13dScMgTeresa_c and is
# served by the alias row in hal/scene_mg_booseek.cpp instead.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_0211cbf4.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=data_ov006_0213fa0c")

# TWO OF THE SEVENTY-ONE .c FILES IN THIS SLICE ARE REALLY C++ -- a //cpp
# marker and a top-level `extern "C"` block, which is invalid C. All 71 were
# SWEPT for the marker and for `extern "C"`, `virtual`, `class`, `namespace`,
# `bool` and `new`/`delete` rather than these two being found by a compile
# error and the other sixty-nine left to luck, which is the sweep the SMB block
# above records doing for its thirty-seven. These are the only hits.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_0211ea70.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_0211f9fc.c"
    PROPERTIES LANGUAGE CXX)

# Run mg9 lane PSY: dScMg3DEsp_c ("Psyche Out!"), actor id 0x185 = scene 389,
# and the THIRD class the port seats under dScMgSingle3DBase_c.
#
#   Scene -> dScMgBase_c (ov004 0x020bc0c0) -> dScMgSingle3DBase_c
#         (ov006 0x0213e448) -> dScMg3DEsp_c (ov006 0x0213c8c4)
#
# all three tables 36 slots wide, and the derived one checked FIVE ways before a
# word was written (port/slice_psy.txt): span, terminal slot, what the word past
# the end is, the relocation COUNT -- this lane's addition, because a vtable
# slot is a relocated code pointer and word 36 carries no relocation at all --
# and the relocation STRIDE, run mg9 lane LKY's check, which needs no config
# symbol: 4 bytes for thirty-five consecutive steps and then twelve.
#
# THE CLASS NAME IS THE ROM'S OWN type_info, reached through the word before the
# vtable: data_ov006_0213c8c4[-1] is 0x0213c7c8, whose name pointer is
# 0x0213c7d4, which reads "12dScMg3DEsp_c".
#
# ONE VARIABLE, for the reason every minigame block above gives: the slice's
# ov006 bodies are only reachable through the address switches in
# unmatched/Mg3DEsp_StateDispatch.cpp, and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-389 row, so
# every target that compiles scene_boot.cpp references port_mg_esp3d_spawn and
# port_scene_fill_esp3d by name.
#
# SEVEN DISPATCHER HOST COPIES, AND THE MACHINE IS THREE LEVELS DEEP. Section
# 14 of port/mg_fanout_costs.txt says the cost question is "how deep is the
# machine"; dScMgPanel_c's answer was two and this class's is three. TWO of the
# seven are the open-coded-int third shape that neither a link nor a `::*` sweep
# can see, and one of those two (func_ov006_020e8830) is reachable only through
# the other, so both prescribed detectors read clean over this class.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors (this class's included) and
# the arm9 waves under them; slice_flw.txt and slice_mem.txt already carry all
# six of the middle base's bodies. CMake appends every list to the same targets
# and dedupes identical absolute paths, so nothing is compiled twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_psy.txt" SLICE_PSY_LINES)
set(SLICE_PSY_SOURCES hal/scene_mg_psycheout.cpp
                      unmatched/Mg3DEsp_StateDispatch.cpp
                      # REPLACES src/func_ov006_020e9cec.c, which is out of
                      # the slice: see the block below this one.
                      unmatched/Mg3DEsp_Cleanup.cpp
                      # REPLACES src/func_ov006_020e7be8.cpp, out of the slice:
                      # the EIGHTH dispatcher, and the one that reads its member
                      # pointer out of an OBJECT FIELD rather than a table. No
                      # link can name that defect -- the type is local to the TU
                      # so no global is emitted -- and the `::*` sweep over the
                      # wave-2 additions is what found it.
                      unmatched/Mg3DEsp_FieldPmf.cpp
                      # The alias rows the link asked for and the five
                      # shadow-class destructors src's two destructors spell.
                      unmatched/Mg3DEsp_Faces.cpp
                      # REPLACES src/func_ov006_020e7b44.cpp, out of the slice:
                      # a shadow-class Render dispatch on a plain ModelAnim,
                      # whose _ZTV9ModelAnim slot 5 is a real Virtual18 and is
                      # deliberately NOT dual-filled. The TU
                      # hal/cxxname_bridge.cpp says trap-by-Virtual18 will name
                      # if it ever appears.
                      unmatched/Mg3DEsp_ModelAnimRender_020e7b44.cpp
                      # REPLACES src/func_ov006_020e777c.cpp, out of the slice:
                      # the same defect one frame down, on the array of THREE
                      # ModelAnims at the sub-object's +0x84. This one crashed
                      # a rendered run; its sibling above was found by reading.
                      unmatched/Mg3DEsp_ModelAnimRender_020e777c.cpp)
foreach(line IN LISTS SLICE_PSY_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_PSY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# ONE .c IN THIS SLICE IS REALLY C++, and all twenty-seven were swept for it
# rather than one being found by a compile error and the rest left to luck.
# src/func_ov006_020e8bd0.c opens with a //cpp marker and declares both its
# callee and itself inside `extern "C"`, which is invalid C. The sweep was for
# the marker and for extern "C", virtual, class and namespace across every .c
# path in port/slice_psy.txt; it is the only hit. Same repair the ov006 smartball
# block above uses for src/func_ov006_02110a20.c.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020e8bd0.c"
    PROPERTIES LANGUAGE CXX)

# NO PER-SOURCE -D ROW FOR THIS CLASS, AND THE REASON IS REUSABLE.
# src/func_ov006_020e9cec.c (slot 3, CleanupResources) releases two
# SharedFilePtrs spelled as the shared placeholders G0 and G1, and
# hal/cxx_aliases.cpp binds bare G0 to _data_020a0eac (the game heap) and bare
# G1 to _data_ov002_0210e05c -- both wrong for this TU, and neither would fail
# a link. The body's OWN literal pool says what they are, read out of
# extracted/overlays/overlay_0006.bin at 0x020e9d14 and 0x020e9d18: 0x02141e9c
# and 0x02141e74, the two SharedFilePtrs src/__sinit_ov006_02130a08.c
# constructs for file ids 0x1ef and 0x202.
#
# THE STANDING REPAIR IS A PER-SOURCE RENAME AND IT CANNOT BE USED HERE. The
# two rows above this block do it (VT and VT0), and it works when the
# placeholder is declared in the TU and the target is not declared anywhere the
# TU can see. G1 and its target are BOTH in include/decl_common.h -- line 396
# is `extern int G1[];` and line 3041 is `extern void*data_ov006_02141e74;` --
# so `-DG1=data_ov006_02141e74` rewrites line 396 into a redefinition of line
# 3041 with a different type of indirection. Measured: error C2372. Host-copied
# in unmatched/Mg3DEsp_Cleanup.cpp instead, and the src TU is out of the slice.
#
# A scan of every TU in port/slice_psy.txt for VT, VT0, HEAP, G0, G1, G2, R0,
# R1 and R2 returns that ONE file. func_020beb68 in src/func_ov006_020e9c20.c
# needs no row: unmatched/MgCoin_Faces.cpp already aliases it onto
# data_ov004_020beb68.

# Lane FDR: the arm9 wipe fader's own static initialiser, and the dWipe_c
# vtable it installs. Rides the SAME three targets as the scene seats above,
# and for a stronger reason than symmetry: hal/fdr_arm9_fader_seat.cpp defines
# data_020926f0 and data_0209f610, and it RUNS src/__sinit_02074f80.c from a
# C++ static initialiser. Leave the file off one of the three and that target's
# data_0209f61c keeps a null vptr while the other two do not, which is the kind
# of split nobody notices until a scene faults on one binary and not the other.
#
# It is one variable rather than two because the .cpp and the slice are not
# separable: the slice's bodies are only reachable through the table the .cpp
# fills, and the .cpp cannot link without them.
# Lane LKY of run mg9: dScMgBSC_c, actor id 0x184, scene 388, "Lucky Stars".
# The twelfth ov006 minigame class and the THIRD under dScMgSingle3DBase_c
# (the flower and Memory Master are the other two), so hal/scene_mg_flower.cpp
# claims the middle table at 0x0213e448 and this seat owns only its own
# derived table at 0x0213fec8. port/slice_lky.txt is the derivation, including
# the four width checks and the reason a 37-slot fill here would write into
# MgSnowballSlalom's live state pair.
#
# ONE VARIABLE, for the reason the flower, memory2 and fader blocks give: the
# slice's ov006 bodies are only reachable through the address switch in
# unmatched/MgBSC_StateDispatch.cpp, and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-388 row, so
# every target that compiles scene_boot.cpp references port_mg_luckystars_spawn
# and port_scene_fill_luckystars by name.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors (both of this class's among
# them) and the arm9 waves under them; CMake appends every list to the same
# targets and dedupes identical absolute paths, so nothing is compiled twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_lky.txt" SLICE_LKY_LINES)
set(SLICE_LKY_SOURCES hal/scene_mg_luckystars.cpp
                      # The one pointer-to-member host copy: vtable SLOT 6.
                      # Its src declares the pair table inside extern "C", so
                      # the link and the compile both read clean while MSVC
                      # strides an eight-byte table by four -- one of the two
                      # SILENT shapes port/mg_fanout_costs.txt section 4 names.
                      # src/func_ov006_021254c0.cpp is out of the slice.
                      unmatched/MgBSC_StateDispatch.cpp
                      # The one /alternatename row the first link asked for:
                      # the class vtable declared at C++ linkage in the factory.
                      unmatched/MgBSC_Faces.cpp)
foreach(line IN LISTS SLICE_LKY_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_LKY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Run mg11 lane BNP: dScMgJump_c, actor id 0x174, scene 372, "Bounce and
# Pounce". The class name is the ROM's own RTTI, reached through the type_info
# the word BEFORE the vtable points at (0x0213cbe0 -> 0x0213cb98 -> 0x0213cba4
# = "11dScMgJump_c"), and the player title is the ROM's own BMG message 552
# reached through the ov005 launch table's ONE row for this id.
# port/slice_bnp.txt is the derivation: five width checks, the fourteen ROM
# adjudications, the seven state pairs and the one floor.
#
# THIS IS THE FIRST OF FOUR CLASSES UNDER dScMgD3DBase_c (0x174, 0x175, 0x180,
# 0x181), and FOUR of the files below are named for the BASE and are reusable by
# the whole family rather than owned by this lane:
#
#   unmatched/MgD3DBase_Slots.cpp            slots 2, 10, 27, 28   (also BNT's)
#   unmatched/MgD3DBase_DeclConflict.cpp     slot 24, the C2733    (also BNT's)
#   unmatched/MgD3DBase_ModelAnimRender.cpp  the ModelAnim draws   (also BNT's)
#   unmatched/MgD3DBase_ObjStateDispatch.cpp the two shared PMF dispatchers
#                                            -- BNP-ONLY, see below
#
# THREE of the four are files lane BNT wrote as well. BNP and BNT hit those
# three independently, from two different scenes, and converged on the same
# filenames and the same symbol names precisely so the two trees collide in ONE
# visible place each instead of producing three duplicate symbols.
#
# THE FOURTH IS BNP-ONLY and the distinction matters at the merge: BNT's tree has
# no MgD3DBase_ObjStateDispatch.cpp and defines none of its symbols, because its
# closure does not reach those two dispatchers. Nothing collides on it.
#
# AND THE THREE CONVERGED FILES ARE NOT BYTE-IDENTICAL, so the merge gate must
# resolve three ADD/ADD conflicts by PICKING ONE COPY -- it does not get a free
# dedupe. Comment-stripped diffs: BNT's Slots carries a vestigial
# `#include <cstdio>` nothing uses; DeclConflict is CODE-IDENTICAL and differs
# only in prose; ModelAnimRender differs in type spelling (BNT forward-declares
# struct OamAttr / Matrix2x2 unqualified, this copy includes common.h and uses
# the struct keyword) over the same two bodies. What lane TTI carries is in
# port/slice_bnp.txt section 9 and is NOT "all four byte-identical": TTI has TWO
# of the four, DeclConflict byte-identical and Slots prose-only-different, with
# ModelAnimRender and ObjStateDispatch absent. Stated there rather than here,
# and pinned to a commit, because an identity claim written inside one of the
# files being compared falsifies itself the moment it is saved.
#
# The dScMgD3DBase_c half of slice_bnp.txt IS safe to list twice: CMake appends
# every list to the same targets and dedupes identical absolute paths.
#
# ONE VARIABLE, for the reason the flower, memory2, LKY and BOO blocks give:
# the slice's ov006 bodies are only reachable through the address switches in
# unmatched/MgBounceAndPounce_StateDispatch.cpp and
# unmatched/MgD3DBase_ObjStateDispatch.cpp -- the state pairs are mounted DATA
# holding DS addresses, so nothing else in the build names them and /OPT:REF
# would drop them -- and those files cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-372 row, so
# every target that compiles scene_boot.cpp references port_mg_jump_spawn and
# port_scene_fill_jump by name.
#
# ONE TU IN THIS SLICE REACHES THE GEOMETRY REGISTERS, so it builds from a
# hostgen-emitted copy the way the flower block's func_ov006_0210a708 and the
# BNT block's func_ov006_020ef2b8 already do -- src/ itself is never edited.
#
# func_ov006_020ee034 is dScMgJump_c::Render, vtable SLOT 9 (the row the slice
# header prints at 020b04ec), so it runs before every drawn frame of scene 372.
# Its second half is this scene's LIGHT SETUP and it is the same six-store
# shape scene 373 carries one class over:
#
#     0x040004c8 = pack(v)            LIGHT_VECTOR, light 0
#     0x040004cc = 0x7fff             LIGHT_COLOR, light 0 white
#     0x040004cc = 0x40007fff         LIGHT_COLOR, light 1 white
#     0x040004c8 = pack(w)|1<<30      LIGHT_VECTOR, light 1
#
# where v and w are both (0,0,-0x0ff8) turned -0x2000 by func_0203cd80, a 45
# degree tilt.  Compiled straight from src/ all four land in the real memory
# ntr maps across the I/O window and stop there: the geometry engine is never
# told.  Both lights keep direction (0,0,0) and colour (0,0,0), and ntr/gx.cpp
# resolves a lit vertex as emission + SUM over the enabled lights of (diffuse *
# lightcol * dot + ambient * lightcol), so a zero light colour kills the
# ambient term too.  The one POLYGON_ATTR this scene submits is 001f8081 --
# light 0 enabled -- and its SPE_EMI is 00000000, so every lit vertex resolves
# to BLACK.
#
# MEASURED in this tree (SM64DS_MAT_LOG=1 SM64DS_SCENE=372), before this line:
#
#     [mat] LIGHT_VECTOR commands executed: 0   LIGHT_COLOR: 0
#     [mat] POLYGON_ATTR 001f8081 lights=1000, SPE_EMI 00000000
#
# Control on the same binary, scene 373, whose light body is already
# substituted: LIGHT_VECTOR 2, LIGHT_COLOR 2.
set(BNP_HOSTGEN_SYMS func_ov006_020ee034)
set(BNP_HOSTGEN_GEN "")
set(BNP_HOSTGEN_SKIP "")
foreach(sym IN LISTS BNP_HOSTGEN_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        list(APPEND BNP_HOSTGEN_SKIP "src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        list(APPEND BNP_HOSTGEN_SKIP "src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND BNP_HOSTGEN_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_bnp.txt" SLICE_BNP_LINES)
set(SLICE_BNP_SOURCES hal/scene_mg_jump.cpp
                      # This class's own pointer-to-member wall: vtable SLOT 6.
                      # src/func_ov006_020ee27c.cpp is OUT of slice_bnp.txt.
                      unmatched/MgBounceAndPounce_StateDispatch.cpp
                      # The four SHARED base-class repairs. See the block above
                      # before adding any of these from another family lane.
                      unmatched/MgD3DBase_Slots.cpp
                      unmatched/MgD3DBase_DeclConflict.cpp
                      unmatched/MgD3DBase_ModelAnimRender.cpp
                      unmatched/MgD3DBase_ObjStateDispatch.cpp
                      ${BNP_HOSTGEN_GEN})
foreach(line IN LISTS SLICE_BNP_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # The hostgen'd copy stands in for the src/ one; taking both would be a
    # duplicate symbol, and taking neither would drop the body silently.
    if(line IN_LIST BNP_HOSTGEN_SKIP)
        continue()
    endif()
    list(APPEND SLICE_BNP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# A slice line that stops matching (a rename, a .c that becomes a .cpp) would
# silently take the src/ copy back and put the defect back with it, so the
# substitution asserts rather than hopes.
list(LENGTH BNP_HOSTGEN_SYMS bnp_hg_want)
list(LENGTH BNP_HOSTGEN_GEN bnp_hg_have)
if(NOT bnp_hg_want EQUAL bnp_hg_have)
    message(FATAL_ERROR "slice_bnp hostgen substitution incomplete: "
            "${bnp_hg_have} of ${bnp_hg_want} generated")
endif()
foreach(skip IN LISTS BNP_HOSTGEN_SKIP)
    if(NOT skip IN_LIST SLICE_BNP_LINES)
        message(FATAL_ERROR "slice_bnp.txt no longer lists ${skip}; the "
                "hostgen substitution for it is now dead and the geometry "
                "register writes in it would go back to latching silently")
    endif()
endforeach()
foreach(dup IN ITEMS src/func_ov006_020ee27c.cpp
                     src/func_ov006_020c4cd8.cpp
                     src/func_ov006_020c7860.cpp
                     src/func_ov006_020c4c54.cpp
                     src/func_ov006_020c7734.cpp
                     src/func_ov006_020e6e78.cpp
                     src/func_ov006_020e6d98.c
                     src/func_ov006_020e6d8c.c
                     src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp
                     src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp)
    if(dup IN_LIST SLICE_BNP_LINES)
        message(FATAL_ERROR "slice_bnp.txt lists ${dup}, which the BNP seat "
                "replaces with its own definition. Taking both is a duplicate "
                "symbol; taking the src one puts back the mwcc member-pointer "
                "stride, the dropped receiver or the C2733 this block exists "
                "to avoid")
    endif()
endforeach()

# Lane BNT of run mg11: dScMgJump2_c, actor id 0x175, scene 373, "Bounce and
# Trounce".  The twenty-second ov006 minigame class and the FIRST under
# dScMgD3DBase_c (data_ov006_0213c62c, which the config misnames
# _ZTV17MgBounceAndPounce and which 0x174, 0x180 and 0x181 also derive from).
# port/slice_bnt.txt is the derivation: the identity, the ov005 row, the
# sibling-attribution ruling, the width checks on both tables and the closure.
#
# ONE VARIABLE, for the reason the flower, memory2, lucky-stars and fader
# blocks give: the slice's ov006 bodies are only reachable through the two
# address switches in unmatched/MgJump2_FieldPmf.cpp and
# unmatched/MgD3DBase_ObjStateDispatch.cpp, and those files cannot link without
# them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-373 row, so
# every target that compiles scene_boot.cpp references port_mg_jump2_spawn and
# port_scene_fill_jump2 by name.
#
# THE SLICE IS THE DELTA ONLY.  slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under
# them; CMake appends every list to the same targets and dedupes identical
# absolute paths, so nothing is compiled twice -- which is also what makes the
# seventeen dScMgD3DBase_c bodies safe to list in all four of this wave's
# sibling slices.
#
# ONE TU IN THIS SLICE REACHES THE GEOMETRY REGISTERS, so it builds from a
# hostgen-emitted copy the way every other MMIO-touching src/ file in this tree
# does -- src/ itself is never edited.  It is the SAME defect the flower block
# above records at func_ov006_0210a708, on a different body.
#
# func_ov006_020ef2b8 is this scene's LIGHT SETUP.  Its only caller is
# src/func_ov006_020ef148.c, dScMgJump2_c vtable slot 9 (Render), which calls
# it as its third statement -- so it runs on EVERY rendered frame, before any
# model is drawn.  Read straight off extracted/overlays/overlay_0006.bin at
# base 0x020bfec0 (0x020ef2b8..0x020ef3bc, pool at 0x020ef3c0) it is six
# literal stores and nothing else:
#
#     0x040004cc = 0x7fff             LIGHT_COLOR, light 0 white
#     0x040004cc = 0x40007fff         LIGHT_COLOR, light 1 white
#     0x040004c8 = pack(v)            LIGHT_VECTOR, light 0
#     0x04000440 = 2                  MTX_MODE, position
#     0x04000454 = 0                  MTX_IDENTITY
#     0x040004c8 = pack(w)|1<<30      LIGHT_VECTOR, light 1
#
# Compiled straight from src/ all six land in the real memory ntr maps across
# the I/O window and stop there: the geometry engine is never told.  Both
# lights keep direction (0,0,0) and colour (0,0,0), and ntr/gx.cpp resolves a
# lit vertex as emission + SUM over the enabled lights of (diffuse * lightcol *
# dot + ambient * lightcol), so a zero light colour kills the ambient term too.
# Every material this scene submits enables light 0 or light 1 (POLYGON_ATTR
# 001f8081 and 001f8082) and every SPE_EMI word it writes is 00000000, so every
# lit vertex in the scene resolves to BLACK.  Mario's model still draws, at
# emission, over the 3D clear colour -- which is the scene-373 report this line
# answers: "a blank screen with glitch textures, and a shadow of mario".  The
# shadow IS the model.
#
# MEASURED in this tree (SM64DS_MAT_LOG=1 SM64DS_SCENE=373), before this line:
#
#     [mat] LIGHT_VECTOR commands executed: 0   LIGHT_COLOR: 0
#     [mat] I/O LATCH  LIGHT_VECTOR(4c8)=6965a400 LIGHT_COLOR(4cc)=40007fff
#
# 0x6965a400 carries bit 30, which is this body's light-1 store: the body RAN
# and its writes arrived nowhere.  Control on the same binary, scene 382, which
# reaches its lights through a body that is already substituted:
#
#     [mat] LIGHT_VECTOR commands executed: 2   LIGHT_COLOR: 2
#
# THE MATRIX PAIR MATTERS TOO and is not incidental.  MTX_MODE and MTX_IDENTITY
# sit BETWEEN the two light vectors because the geometry engine transforms a
# light vector by the current position matrix as it consumes it.  Latching them
# silently would leave light 1 rotated by whatever matrix the previous draw
# left behind even if the two LIGHT_VECTOR writes were fixed on their own, so
# the substitution has to cover the whole body rather than the two registers
# the symptom names.
set(BNT_HOSTGEN_SYMS func_ov006_020ef2b8)
set(BNT_HOSTGEN_GEN "")
set(BNT_HOSTGEN_SKIP "")
foreach(sym IN LISTS BNT_HOSTGEN_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        list(APPEND BNT_HOSTGEN_SKIP "src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        list(APPEND BNT_HOSTGEN_SKIP "src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND BNT_HOSTGEN_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_bnt.txt" SLICE_BNT_LINES)
set(SLICE_BNT_SOURCES hal/scene_mg_jump2.cpp
                      # The class's own pointer-to-member wall: vtable SLOT 6,
                      # dispatching a member pointer held in an OBJECT FIELD at
                      # this+0x5004.  There is no state table and no __sinit
                      # block behind it -- see slice_bnt.txt section 5.
                      # src/func_ov006_020ef3e0.cpp is out of the slice.
                      unmatched/MgJump2_FieldPmf.cpp
                      # The two SUB-OBJECT field dispatchers, fields +0x30 and
                      # +0x3c.  Neither is on a vtable.  src/func_ov006_020c4cd8.cpp
                      # and src/func_ov006_020c7860.cpp are out of the slice.
                      #
                      # THIS LANE WROTE THEM AS unmatched/MgJump2_SubDispatch.cpp
                      # AND LANE BNP WROTE THE SAME TWO SYMBOLS AS
                      # unmatched/MgD3DBase_ObjStateDispatch.cpp.  Different
                      # filenames, so git saw no add/add and the collision only
                      # surfaced as an LNK2005 at the merge.  The two are UNIFIED
                      # into the base-named file below, which carries both lanes'
                      # census APIs; the claim in this block that the pair was
                      # "not shared with the three sibling classes" is what the
                      # merge disproved.  See that file's section 0.
                      unmatched/MgD3DBase_ObjStateDispatch.cpp
                      # dScMgD3DBase_c slot 24, host-copied for the
                      # decl_common.h C2733 run link60 lane MG1 documented for
                      # four dScMgBase_c slots. DELIBERATELY NOT LANE-NAMED:
                      # all four ids under this base need it and all four hit
                      # the same conflict, so the merge keeps exactly one copy.
                      unmatched/MgD3DBase_DeclConflict.cpp
                      # dScMgD3DBase_c slots 2, 10, 27 and 28: four bodies that
                      # each drop an argument the ROM rides through in a
                      # register. Slot 2's dropped RETURN is what put this
                      # class's actor on the cleanup list instead of the
                      # behaviour list on the first run. ALSO NOT LANE-NAMED --
                      # all four ids under this base need all four repairs.
                      unmatched/MgD3DBase_Slots.cpp
                      # The two ModelAnim shadow-render dispatches. ROM slot 5
                      # is MSVC's Virtual18, so the shadow call reached a
                      # two-argument body with one argument and faulted the
                      # RENDERED battery row. Shared with the sibling lanes for
                      # the same reason the file above is.
                      unmatched/MgD3DBase_ModelAnimRender.cpp
                      # The link faces the FIRST link of this seat asked for:
                      # 27 C++-linkage spellings of C-named DS data, one bare
                      # func_-prefixed name for an ov004 bss word, and three
                      # C++ method spellings of Itanium-named bodies already in
                      # the build. All named by a link, none predicted.
                      unmatched/MgJump2_Faces.cpp
                      ${BNT_HOSTGEN_GEN})
foreach(line IN LISTS SLICE_BNT_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # The hostgen'd copy stands in for the src/ one; taking both would be a
    # duplicate symbol, and taking neither would drop the body silently.
    if(line IN_LIST BNT_HOSTGEN_SKIP)
        continue()
    endif()
    list(APPEND SLICE_BNT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# A slice line that stops matching (a rename, a .c that becomes a .cpp) would
# silently take the src/ copy back and put the defect back with it, so the
# substitution asserts rather than hopes.
list(LENGTH BNT_HOSTGEN_SYMS bnt_hg_want)
list(LENGTH BNT_HOSTGEN_GEN bnt_hg_have)
if(NOT bnt_hg_want EQUAL bnt_hg_have)
    message(FATAL_ERROR "slice_bnt hostgen substitution incomplete: "
            "${bnt_hg_have} of ${bnt_hg_want} generated")
endif()
foreach(skip IN LISTS BNT_HOSTGEN_SKIP)
    if(NOT skip IN_LIST SLICE_BNT_LINES)
        message(FATAL_ERROR "slice_bnt.txt no longer lists ${skip}; the "
                "hostgen substitution for it is now dead and the geometry "
                "register writes in it would go back to latching silently")
    endif()
endforeach()

# Lane SNW of run mg11: dScMgSnowball_c, actor id 0x179, scene 377, "Snowball
# Slalom".  THE ONE CLASS OF THIRTY WHOSE VTABLE NO TOOL RESOLVED --
# port/mg_fanout_costs.txt section 3 left its row blank because no load
# relocation inside its factory points at any signature table.  The reason is
# one delegated constructor: MgSnowballSlalom_Spawn allocates and calls
# func_ov006_021295ac, and THAT body writes data_ov006_0213e448 and then
# data_ov006_0214000c.  port/slice_snw.txt carries the derivation, the five
# width checks, the ruling-out of the two alternative tables and the wall.
#
# The FOURTH class under dScMgSingle3DBase_c (the flower, Memory Master and
# Lucky Stars are the other three), so hal/scene_mg_flower.cpp claims the
# middle table at 0x0213e448 and this seat owns only its own derived table at
# 0x0214000c.
#
# ONE VARIABLE, for the reason the flower, memory2, luckystars and fader blocks
# give: the slice's ov006 bodies are only reachable through the address switch
# in unmatched/MgSnowball_StateDispatch.cpp, and that file cannot link without
# them.  hal/scene_boot.cpp's port_scene_classes table carries the scene-377
# row, so every target that compiles scene_boot.cpp references
# port_mg_snowball_spawn and port_scene_fill_snowball by name.
#
# THE HOST COPY IS THREE TUs AND NOT ONE, which is what makes this class
# different from every seat before it.  Its vtable slot 6 -- the slot every
# seat wires by name -- contains no pointer-to-member dispatch at all; it CALLS
# func_ov006_0212a2e0, whose table's slots 1 and 2 are two more dispatchers
# that no arm_call in the overlay reaches.  Reading slot 6 alone prices this
# class's wall at zero.
#
# THE SLICE IS THE DELTA ONLY.  slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors (this class's among them) and
# the arm9 waves under them; CMake appends every list to the same targets and
# dedupes identical absolute paths, so nothing is compiled twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_snw.txt" SLICE_SNW_LINES)
set(SLICE_SNW_SOURCES hal/scene_mg_snowball.cpp
                      # The pointer-to-member host copies: the first-level
                      # dispatcher slot 6 calls, and the two second-level
                      # dispatchers its table holds.  Two of the three declare
                      # their tables inside extern "C" (the SILENT shape: the
                      # link is satisfied and MSVC strides an eight-byte table
                      # by four); the third declares two at C++ linkage (the
                      # LOUD shape).  src/func_ov006_0212a2e0.cpp,
                      # src/func_ov006_0212a224.cpp and
                      # src/func_ov006_02129d94.cpp are out of the slice.
                      unmatched/MgSnowball_StateDispatch.cpp
                      # The one /alternatename row the first link asked for
                      # (this class's vtable declared at C++ linkage in the
                      # constructor).  It used to carry three trapped closure
                      # floors as well; run mg12 lane SNO decompiled and seated
                      # all three through slice_snw.txt section 17, so this
                      # file is now the alias row and its marker only.
                      unmatched/MgSnowball_Faces.cpp
                      # Vtable SLOT 9, Render.  Not a pointer-to-member TU --
                      # the Itanium scan finds no site in it -- but it carries
                      # the OTHER shadow-dispatch shape Door_Render.cpp and
                      # MgFlower_ModelRender_020c3bf4.cpp name: a C function
                      # pointer read out of a model's vtable and called with
                      # the C convention, into a __fastcall thunk.  The vtable
                      # read is correct (the _ZTV5Model dual fill serves it);
                      # the convention is not, and the stack came back four
                      # bytes short.  Found by a run, not by a checker.
                      # src/func_ov006_02127d10.c is out of the slice.
                      unmatched/MgSnowball_ModelRender.cpp)
foreach(line IN LISTS SLICE_SNW_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SNW_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Lane PKR of run mg11: dScMgCard_c, actor id 0x17b, scene 379, "Picture Poker".
# The FOURTH class under dScMgSingle3DBase_c (the flower, Memory Master and
# Lucky Stars are the other three), so hal/scene_mg_flower.cpp claims the middle
# table at 0x0213e448 and this seat owns its own derived table at 0x0213bdb4 --
# plus TWO ELEMENT VTABLES, which no earlier seat in the family has had.
# port/slice_pkr.txt is the derivation, including the five width checks, the
# attribution ruling that keeps dScMgCoin_c's five state tables where they
# belong, and the reason a 37-slot fill here would write into that SEATED
# class's live state pair.
#
# ONE VARIABLE, for the reason the flower, memory2, luckystars and fader blocks
# give: the slice's ov006 bodies are only reachable through the four tables
# hal/scene_mg_card.cpp fills, and that file cannot link without them.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-379 row, so
# every target that compiles scene_boot.cpp references port_mg_card_spawn and
# port_scene_fill_card by name.
#
# THIS CLASS HAS NO POINTER-TO-MEMBER HOST COPY AND THAT IS A MEASUREMENT.
# Its state machine is a 14-arm ARM jump table, not an mwcc member-pointer
# table; the ARM-Itanium encoding scan returns zero over its 37 bodies while the
# same scan over the neighbouring dScMgCoin_c block returns that class's seven
# known sites.  slice_pkr.txt section 6 is the arithmetic.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under them;
# CMake appends every list to the same targets and dedupes identical absolute
# paths, so nothing is compiled twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_pkr.txt" SLICE_PKR_LINES)
set(SLICE_PKR_SOURCES hal/scene_mg_card.cpp
                      # The two /alternatename rows the first link asked for.
                      # No trap and no stand-in body: both are DATA declared at
                      # C++ linkage in a //cpp src TU against the mount's plain
                      # C name.
                      unmatched/MgCard_Faces.cpp)
# NO TRAP. Run mg11 seated this class with ONE floor, func_ov006_020da174
# (0x2ac, the hand sorter), behind unmatched/MgCard_Traps.cpp -- and it was on
# the CRITICAL PATH: the only caller of src/func_ov006_020d99ec.c, which is the
# only writer of a card's slide target, with state 10 waiting for every card to
# arrive. Run mg12 lane SRT decompiled it (byte-identical at 2004/b56) and the
# trap file is gone; src/func_ov006_020da174.c is an ordinary line in
# slice_pkr.txt and the delink block for 0x020da174..0x020da420 is enrolled.
# slice_pkr.txt section 7 is the retirement record, including why that block is
# NOT marked `complete`.
foreach(line IN LISTS SLICE_PKR_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_PKR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Lane TTI of run mg11: dScMgTrampoline_c, actor id 0x180, scene 384,
# "Trampoline Time".  The twenty-second ov006 minigame class and the FIRST
# under dScMgD3DBase_c (data_ov006_0213c62c), a middle base four classes share
# -- 0x174, 0x175, 0x180 and 0x181 -- so this seat claims that table rather
# than finding it already hosted.  port/slice_tti.txt is the derivation,
# including the five width checks, the ELEMENT vtable at data_ov006_0213b2c4
# and the reason a 37-slot fill here would write into dScMgTrampoline2_c's
# live pause flag.
#
# ONE VARIABLE, for the reason the flower, memory2, luckystars and fader blocks
# give: the slice's ov006 bodies are only reachable through the address switch
# in unmatched/MgTrampolineTime_StateDispatch.cpp, and that file cannot link
# without them.  hal/scene_boot.cpp's port_scene_classes table carries the
# scene-384 row, so every target that compiles scene_boot.cpp references
# port_mg_trampoline_spawn and port_scene_fill_trampoline by name.
#
# THE SLICE IS THE DELTA ONLY.  slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under
# them; CMake appends every list to the same targets and dedupes identical
# absolute paths, so nothing is compiled twice.
#
# ================= THE TRAMPOLINE PAIR'S SHARED LIGHT BODY =================
#
# func_ov006_020d09e0 IS LISTED IN BOTH slice_tti.txt AND slice_tte.txt, and it
# is one of the geometry-register bodies both scenes need substituted.  That
# makes it the one shape the per-slice hostgen blocks above cannot express, so
# it gets ONE arrangement here rather than a copy in each block.
#
# WHY A PER-SLICE COPY WOULD NOT WORK, precisely, because the failure is not
# the obvious one.  Both blocks would emit to the SAME output path --
# ${CMAKE_BINARY_DIR}/host-src/src/func_ov006_020d09e0.cpp is derived from the
# symbol, not from the slice -- so the second add_custom_command(OUTPUT ...)
# is a hard CMake error ("multiple rules generate"), not a duplicate symbol.
# The duplicate-symbol version is the NEXT failure, the one that appears if
# somebody works around the first by giving the two copies different output
# paths: two TUs, one body, one link error.  And a third arrangement --
# hostgen in one slice, plain src/ line in the other -- links fine, takes the
# src/ copy, and puts the defect silently back for whichever scene lost the
# race.  All three are avoided by emitting once and letting BOTH slices point
# at the single output; CMake dedupes the identical absolute path when the two
# lists are appended to the same target.
#
# WHAT THE BODY IS.  func_ov006_020d09e0 is the trampoline family's shared
# light setup, called by scene 385's Render (func_ov006_021231ac, below) and
# reached in scene 384 through its own slice.  It is
#
#     0x04000440 = 2                  MTX_MODE, position
#     0x040004c8 = pack(s)|1<<30      LIGHT_VECTOR, light 1
#     0x040004cc = 0x40007fff         LIGHT_COLOR, light 1 white
#
# around a func_020553a4 call and the four-element draw walk over
# data_ov006_02140990.  THE MTX_MODE STORE IS PART OF THE BODY and not
# incidental: the geometry engine transforms a light vector by the current
# position matrix as it consumes it, so latching MTX_MODE silently would leave
# light 1 rotated by whatever matrix the previous draw left behind even if the
# LIGHT_VECTOR write alone were fixed.  The substitution covers the whole body
# for the reason the BNT block gives for its MTX_MODE/MTX_IDENTITY pair.
# func_ov006_020cf2fc JOINS IT FOR THE SAME REASON, run mg13 lane TRAMP.  It is
# THE MESH DRAW -- the body 020d09e0's loop calls once per record whose +0x328
# gate the stroke installer set -- and it is listed in BOTH slices too, so the
# emit-once arrangement above is the only one that expresses it.  Thirty-two raw
# MMIO writes: MTX_MODE and MTX_SCALE, TEXIMAGE_PARAM and PLTT_BASE, POLYGON_ATTR,
# DIF_AMB and SPE_EMI, then BEGIN_VTXS / TEXCOORD / NORMAL / VTX_16 / END_VTXS
# through two double loops.  Compiled from src/ every one of them lands in the
# mapped I/O window and stops there, which is a trampoline that is built, gated
# on, walked, and invisible -- exactly the shape Tango reported.
set(TT_SHARED_HOSTGEN_SYMS func_ov006_020d09e0 func_ov006_020cf2fc)
set(TT_SHARED_HOSTGEN_GEN "")
set(TT_SHARED_HOSTGEN_SKIP "")
foreach(sym IN LISTS TT_SHARED_HOSTGEN_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        list(APPEND TT_SHARED_HOSTGEN_SKIP "src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        list(APPEND TT_SHARED_HOSTGEN_SKIP "src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND TT_SHARED_HOSTGEN_GEN "${sym_out}")
endforeach()

# TTI's own geometry-register body, on top of the shared one above.
#
# func_ov006_020cd270 is scene 384's light setup: two LIGHT_VECTOR stores,
# light 0 from (0,0,-0x0ff8) turned 0x2000 by func_0203cd80 and then 0 by
# func_0203ccd4, light 1 from the same vector turned -0x2000, before the
# data_ov006_02140550 draw walk.  Compiled from src/ both land in the mapped
# I/O window and stop there; the scene's one POLYGON_ATTR is 001f8081 (light 0
# enabled) over SPE_EMI 00000000, so every lit vertex resolves to black.
#
# MEASURED (SM64DS_MAT_LOG=1 SM64DS_SCENE=384), before these lines:
#     [mat] LIGHT_VECTOR commands executed: 0   LIGHT_COLOR: 0
set(TTI_HOSTGEN_SYMS func_ov006_020cd270)
set(TTI_HOSTGEN_GEN "")
set(TTI_HOSTGEN_SKIP "")
foreach(sym IN LISTS TTI_HOSTGEN_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        list(APPEND TTI_HOSTGEN_SKIP "src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        list(APPEND TTI_HOSTGEN_SKIP "src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND TTI_HOSTGEN_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_tti.txt" SLICE_TTI_LINES)
set(SLICE_TTI_SOURCES hal/scene_mg_trampoline.cpp
                      # The one pointer-to-member host copy: vtable SLOT 6.
                      # This class's member pointer is a FIELD at this+0x5004,
                      # not a table, so src spells it as a local struct with a
                      # PMF member -- no external symbol for a `::*` sweep to
                      # find and nothing for the linker to complain about,
                      # while MSVC's member pointer is four bytes where the
                      # ROM's field is eight.  The third of the three silent
                      # shapes port/mg_fanout_costs.txt section 4 names.
                      # src/func_ov006_021214f8.cpp is out of the slice.
                      unmatched/MgTrampolineTime_StateDispatch.cpp
                      # dScMgD3DBase_c's SHARED framework-slot repairs, taken
                      # VERBATIM off mg11-bnp so the family merge dedupes
                      # instead of colliding: slots 2, 10, 27 and 28 in
                      # MgD3DBase_Slots.cpp and the slot-24 decl_common C2733
                      # in MgD3DBase_DeclConflict.cpp. Four classes derive from
                      # that base and three other lanes are seating them in
                      # this run; the four src TUs they replace stay out of
                      # every slice.
                      unmatched/MgD3DBase_Slots.cpp
                      unmatched/MgD3DBase_DeclConflict.cpp
                      # vtable SLOT 34, the brush. src does arithmetic on a
                      # void* -- a GNU/mwcc extension MSVC refuses with C2036
                      # -- so the body is re-spelled with one cast under its
                      # OWN name and src/func_ov006_02120da8.c is out of the
                      # slice. Not an ABI repair: the behaviour is identical.
                      unmatched/MgTrampolineTime_Brush.cpp
                      # the name-spelling repairs (32 C++-linkage references to
                      # C-named mount symbols, three bare names resolved from
                      # their literal pools) and one hosted arm9 BSS global
                      unmatched/MgTrampolineTime_Faces.cpp
                      # the class's undecompiled bodies, count-and-return. Run
                      # mg12 lane TRM retired the stylus hit test out of this
                      # file and lane INST retired the 0x800 installer the same
                      # way (src/func_ov006_020d0c38.c and
                      # src/func_ov006_020d01e0.c are slice lines in BOTH
                      # trampoline slices now); only 020cf2fc is still trapped
                      # here, and both lanes' counter APIs are unchanged -- the
                      # two retired slots report a fixed 0.
                      unmatched/MgTrampolineTime_Floors.cpp
                      # The trampoline RECORD READOUT that replaced the retired
                      # hit-test want count as the stroke evidence. Listed ONCE,
                      # here, and read by BOTH scenes' censuses -- every target
                      # that takes SLICE_TTE_SOURCES takes SLICE_TTI_SOURCES
                      # too, so one definition serves 384 and 385 alike and a
                      # second listing would only risk the duplicate the mg11
                      # merge already paid for.
                      hal/trampoline_records.cpp
                      # the SECOND level of the wall: dMgTrmpln3DMario_c, the
                      # four element records the factory builds at this+0x500c,
                      # has a field-held member pointer of its own at
                      # element+0x64 and one dispatcher over 21 bodies. Its src
                      # open-codes the pair correctly and would still have
                      # jumped to a DS address; MSVC refuses it anyway (C2761).
                      unmatched/MgTrmpln3DMario_StateDispatch.cpp
                      # TWO CROSS-MODULE MISBINDINGS: src calls the ov007 DATA
                      # symbol that shares a DS address with the ov006 FUNCTION
                      # the ROM's relocation names. Harmless on hardware (the
                      # two overlays are never co-resident), a hard crash in a
                      # port that mounts both. This was the only thing stopping
                      # scene 384 from booting.
                      unmatched/MgTrmpln3DMario_ModuleFix.cpp
                      # the element's per-frame model draw: the local-shadow
                      # slot-numbering defect Door_Render.cpp and the flower's
                      # ModelRender file already carry, one class over
                      unmatched/MgTrmpln3DMario_ModelRender.cpp
                      # This slice's own light body plus the one it shares with
                      # slice_tte. The shared path is listed by BOTH slices on
                      # purpose -- CMake dedupes the identical absolute path,
                      # and pointing only one slice at it would leave the other
                      # taking the src/ copy back.
                      ${TTI_HOSTGEN_GEN}
                      ${TT_SHARED_HOSTGEN_GEN})
foreach(line IN LISTS SLICE_TTI_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # The hostgen'd copy stands in for the src/ one; taking both would be a
    # duplicate symbol, and taking neither would drop the body silently. Both
    # sets are consulted because the shared body is a slice_tti line too.
    if(line IN_LIST TTI_HOSTGEN_SKIP OR line IN_LIST TT_SHARED_HOSTGEN_SKIP)
        continue()
    endif()
    list(APPEND SLICE_TTI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# A slice line that stops matching (a rename, a .c that becomes a .cpp) would
# silently take the src/ copy back and put the defect back with it, so the
# substitution asserts rather than hopes.
list(LENGTH TTI_HOSTGEN_SYMS tti_hg_want)
list(LENGTH TTI_HOSTGEN_GEN tti_hg_have)
if(NOT tti_hg_want EQUAL tti_hg_have)
    message(FATAL_ERROR "slice_tti hostgen substitution incomplete: "
            "${tti_hg_have} of ${tti_hg_want} generated")
endif()
foreach(skip IN LISTS TTI_HOSTGEN_SKIP TT_SHARED_HOSTGEN_SKIP)
    if(NOT skip IN_LIST SLICE_TTI_LINES)
        message(FATAL_ERROR "slice_tti.txt no longer lists ${skip}; the "
                "hostgen substitution for it is now dead and the geometry "
                "register writes in it would go back to latching silently")
    endif()
endforeach()

# FIVE OF THIS SLICE'S 98 .c FILES ARE REALLY C++ -- a //cpp marker and a
# top-level `extern "C"` block, which is invalid C. ALL 98 were SWEPT for the
# marker and for `extern "C"`, `virtual`, `class`, `namespace`, `bool`,
# `new`/`delete` and `::` rather than any one of the five being found by a
# compile error and the rest left to luck, which is the sweep the BOO and SMB
# blocks above record doing for theirs. The slice is 143 lines: 98 .c and
# 45 .cpp.
#
# THE `::` ARM OF THE SWEEP: 12 of the 98 contain `::` somewhere, and 11 of the
# 12 have it only in a recovered-name COMMENT ("dScMgTrampoline_c::Behavior --
# recovered from vtable slot identity"), checked line by line against a
# comment-stripping match. The twelfth, src/func_ov006_020cb1a8.c, has it in
# CODE (`Sound::PlayBank2_2D`, `ModelAnim::SetAnim`) and is in the list below.
#
# THIS COMMENT SAID "All 49" AND "Eleven files" UNDER A HEADING SAYING FIVE.
# Both figures were true of the slice as it stood when the block was first
# written, at 49 .c files and one CXX row, and neither was re-measured as four
# closure waves grew the slice to 98. A count in a comment is a measurement
# with no gate under it; the numbers above were re-taken against the committed
# slice.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/MgTrampolineTime_Spawn.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020c9c8c.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020cb1a8.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020cb528.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020cc198.c"
    PROPERTIES LANGUAGE CXX)

# ---- run mg15 lane MENU: dScMiniGm_c, actor id 5, the MINIGAME SELECTION
# ---- MENU (ov005) ---------------------------------------------------------
#
# The scene every minigame row above is reached FROM on the DS, and the one the
# port's harness has always skipped: SM64DS_SCENE=<id> boots an ov006 minigame
# directly, so dScMiniGm_c had never run a frame in this tree.
# port/slice_mgm.txt carries the derivation; hal/scene_mg_menu.cpp is the seat.
#
# THIS SLICE IS THE WHOLE OVERLAY, and that is a statement about how small
# ov005 is rather than an appetite: .text is 0x020bfec0..0x020c2250, 0x2390
# bytes, TWENTY-SIX functions, and all twenty-six have a src/ TU. Twenty-three
# are marked complete in config/arm9/overlays/ov005/delinks.txt. A partial
# slice would save nothing and would leave the class's own vtable bodies split
# across the seam.
#
# THE FIVE arm9 TUs AT THE END OF THE SLICE ARE THE MEASURED GAP, not a guess
# at one. The 26 TUs' entire external closure -- 94 names -- was checked
# against build/port/walk_window.map, the shipped window's own link.
# EIGHTY-FIVE already resolved. Nine did not: these five functions (each with a
# matched src/ TU, all five /OPT:REF-stripped today because nothing referenced
# them), two arm9 .data words that joined port/tools/romdata.py's NAMED list,
# and two arm9 .bss words that joined hal/auto_bss.cpp. Nine names is the
# entire distance between "the port has never touched this scene" and "its code
# links", which is why this is one slice and not the multi-lane wave the ov006
# families needed.
#
# THREE OF THE SLICE'S 27 .c FILES ARE REALLY C++ -- a //cpp marker and a
# top-level `extern "C" {` block, invalid C. ALL 27 were SWEPT for the marker
# and for `extern "C"`, `virtual`, `class`, `namespace`, `bool`, `new`/`delete`
# and `::` over comment-stripped text, the sweep the TTI block above records,
# rather than three being found by a compile error and the rest left to luck.
# The sweep found exactly these three and no `::` in code anywhere. The slice
# is 30 lines: 27 .c and 3 .cpp.
#
# ONE of the nine gap names is NOT a line of this slice: func_020233f4 is
# already in port/slice_gate29.txt, and every target that takes SLICE_MGM
# takes that one too. Listing it twice gave MSVC two object rules for one file
# (gate 29 compiles it as C++) and the link refused with LNK2005.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_mgm.txt" SLICE_MGM_LINES)
set(SLICE_MGM_SOURCES hal/scene_mg_menu.cpp)
foreach(line IN LISTS SLICE_MGM_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_MGM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov005_020c0140.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov005_020c0378.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov005_020c0878.c"
    PROPERTIES LANGUAGE CXX)


# ---- VS wiring lane: dScEntry_c, scene id 6 -- the VS / wireless entry menu
# ---- (ov075) ---------------------------------------------------------------
#
# port/slice_vs.txt is BOTH the derivation record and this manifest: 138 TUs
# (135 delinked + 3 honestly-NONMATCHING), the scene-5 template applied, with
# the differences that overlay demanded -- three static initialisers run by
# the seat's gate, a generated .data code-pointer patch
# (hal/vs_data_patch.inc), and FOUR contested-name renames below.
# hal/scene_vs_menu.cpp is the seat; the ov075 data mount joins
# PORT_ACTOR_OVERLAYS beside ov005's.
#
# THE RENAMES ARE THE SHARED-WINDOW RACE, slice_vs.txt section 6: ov063/065
# and ov075 share a load window, and four ov075 spellings are live in OTHER
# overlays' TUs meaning those overlays' storage
# (hal/actor_classes_ov065.cpp's and actor_classes_bowserpuzzle.cpp's
# alternatenames, which go INERT the moment a real definition appears). So
# the mount excludes the four names, and the two ov075 TUs that carry them
# compile with -D renames onto seat-owned symbols. The src/ files are
# untouched -- the rename exists only in this build's compile of them.
# (the PORT_ACTOR_OVERLAYS append lives beside ov005's, BEFORE the ovdata
# generation loop reads the list)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_vs.txt" SLICE_VS_LINES)
set(SLICE_VS_SOURCES hal/scene_vs_menu.cpp
                     # the three PMF dispatch sites, host copies; their
                     # matched src TUs are out of the slice (see the
                     # manifest's dispatch note and the file's banner)
                     unmatched/VS_EntryDispatch.cpp
                     # PathLift's path-follow body with ARM division
                     # semantics; substitutes slice_w6d's src line (the skip
                     # in that loop names it)
                     unmatched/VS_PathDivGuard.cpp)
foreach(line IN LISTS SLICE_VS_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_VS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# The language sweep found TWO .c files in this slice carrying C++ and this row
# marked both (slice_vs.txt s3). It marks ONE now: run rel0215 lane prop16 took
# main's src/func_ov075_02116128.cpp by address, and a .cpp needs no marking --
# CMake reads the language off the extension. Leaving the dead .c path in the
# list would have been silent (set_source_files_properties on a path no target
# compiles does nothing), which is why it goes rather than stays.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov075_021184b0.c"
    PROPERTIES LANGUAGE CXX)
# WorkElevator's TUs spell their vtable func_ov075_0211478c; ov075's own body
# at that window address compiles under a port name and the generated patch
# points the one .data record that dispatches it (0x0211c618) at the rename.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov075_0211478c.c"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov075_0211478c=port_vs_body_0211478c")
# Snufit's SharedFilePtrs and BowserPuzzleManager's record keep their
# ov065/ov064 bindings; ov075's own registrations land on seat storage.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/__sinit_ov075_0211bb00.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov075_0211c800=port_vs_data_0211c800;data_ov075_0211d608=port_vs_bss_0211d608;data_ov075_0211d650=port_vs_bss_0211d650")


# Lane TTE of run mg11: dScMgTrampoline2_c, actor id 0x181, scene 385,
# "Trampoline Terror". The twenty-second ov006 minigame class and the FIRST
# under dScMgD3DBase_c -- the middle base config/arm9/overlays/ov006/symbols.txt
# calls _ZTV17MgBounceAndPounce (0x174's spawn name) and the ROM's own RTTI at
# 0x0213c5c8 calls dScMgD3DBase_c. Four classes hold that table and run mg11
# seats the other three concurrently, so on this branch this seat's fill claims
# it and after the merge the 0x174 row does; both readings are printed by the
# census rather than assumed. port/slice_tte.txt is the derivation, including
# the five width checks and the reason a 37-slot fill here would write into
# scene 388's first live state pair.
#
# ONE VARIABLE, for the reason the flower, memory2 and luckystars blocks give:
# the slice's ov006 bodies are only reachable through the field switch in
# unmatched/MgTrampolineTerror_StateDispatch.cpp, and that file cannot link
# without them. hal/scene_boot.cpp's port_scene_classes table carries the
# scene-385 row, so every target that compiles scene_boot.cpp references
# port_mg_trampoline2_spawn and port_scene_fill_trampoline2 by name.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt already carries the ov004
# framework, the thirty-five overlay constructors and the arm9 waves under
# them; CMake appends every list to the same targets and dedupes identical
# absolute paths, so nothing is compiled twice.
#
# TWO GEOMETRY-REGISTER BODIES OF ITS OWN, on top of func_ov006_020d09e0 which
# it shares with slice_tti and which is emitted once by the TT_SHARED block
# above -- see the hazard note there for why it is not a copy in each slice.
#
# func_ov006_021231ac is dScMgTrampoline2_c::Render, so it runs before every
# drawn frame of scene 385, and it is BOTH a light setup and the caller of the
# other two.  Its own stores are LIGHT_VECTOR light 0 from (0,0,-0x0ff8) turned
# -0x4000 by func_0203cd80, then LIGHT_COLOR 0x7fff and 0x40007fff; and when
# the field at +0x7ba8 is zero it goes on to call func_ov006_020caadc and
# func_ov006_020d09e0, which are the other two.  All three have to be
# substituted together: leaving any one on the src/ path leaves the register it
# writes latched, and because they run in sequence over the same two lights the
# surviving raw body would simply be the last writer of a dead word.
#
# func_ov006_020caadc is the second: light 0 from (0,0,-0x0ff8) turned -0x2000
# and then -0x3000, light 1 from the same vector turned -0x2000, then
# LIGHT_COLOR 0x7fff and 0x40007fff, before the data_ov006_02140554 walk.
#
# MEASURED (SM64DS_MAT_LOG=1 SM64DS_SCENE=385), before these lines:
#     [mat] LIGHT_VECTOR commands executed: 0   LIGHT_COLOR: 0
set(TTE_HOSTGEN_SYMS func_ov006_020caadc func_ov006_021231ac)
set(TTE_HOSTGEN_GEN "")
set(TTE_HOSTGEN_SKIP "")
foreach(sym IN LISTS TTE_HOSTGEN_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        list(APPEND TTE_HOSTGEN_SKIP "src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        list(APPEND TTE_HOSTGEN_SKIP "src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND TTE_HOSTGEN_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_tte.txt" SLICE_TTE_LINES)
set(SLICE_TTE_SOURCES hal/scene_mg_trampoline2.cpp
                      # The one pointer-to-member host copy: vtable SLOT 6.
                      # This class holds its member pointer in a FIELD at
                      # +0x5004 rather than in a table, which is the worse of
                      # the two shapes port/mg_fanout_costs.txt section 4 names.
                      # src/func_ov006_02123340.cpp is out of the slice.
                      unmatched/MgTrampolineTerror_StateDispatch.cpp
                      # The alias rows the six link waves asked for, the two
                      # method faces an /alternatename cannot do, and the three
                      # bodiless floors this closure traps.
                      unmatched/MgTrampolineTerror_Faces.cpp
                      # The FACTORY, host-copied because the matched-side TU
                      # swaps func_020733a8's ctor and dtor arguments at all six
                      # of its call sites -- the ROM's own pool words are in the
                      # file's header. src/MgTrampolineTerror_Spawn.c is out of
                      # the slice.
                      unmatched/MgTrampolineTerror_Factory.cpp
                      # The Trampoline-Mario element records' field-held member
                      # pointer, open-coded in plain ints by its src TU -- the
                      # third shape a link and a `::*` sweep are both blind to,
                      # convicted by a run. src/func_ov006_020c8f20.cpp is out
                      # of the slice.
                      unmatched/MgTrampolineTerror_SubDispatch.cpp
                      # The 3D-Mario records' member pointer at offset 0, the
                      # `::*` shape a sweep does find. Two dispatch sites, one
                      # switch. src/func_ov006_020cea2c.cpp and
                      # src/func_ov006_020cf758.cpp are out of the slice.
                      unmatched/MgTrampolineTerror_MarioDispatch.cpp
                      # dScMgD3DBase_c's slots 10, 27 and 28, whose srcs drop
                      # the receiver an ARM register carried. NOT lane-scoped:
                      # the four ids under that base share the three bodies and
                      # more than one mg11 lane lands this same filename, which
                      # is how the merge dedupes it.
                      unmatched/MgD3DBase_Slots.cpp
                      # dScMgD3DBase_c slot 24: decl_common.h declares it
                      # (void*) and the TU defines it (char*), C2733 to MSVC.
                      # The canonical shared host copy, adopted verbatim from
                      # lane BNP so the merge dedupes.
                      unmatched/MgD3DBase_DeclConflict.cpp
                      # The family's two mwcc virtual-shadow dispatches onto
                      # ModelAnim's ROM slot 5. Same non-lane-scoped naming
                      # rule as the file above.
                      unmatched/MgD3DBase_ModelAnimRender.cpp
                      # This slice's own two light bodies plus the one it
                      # shares with slice_tti. The shared path is listed by
                      # BOTH slices on purpose -- CMake dedupes the identical
                      # absolute path, and pointing only one slice at it would
                      # leave the other taking the src/ copy back.
                      ${TTE_HOSTGEN_GEN}
                      ${TT_SHARED_HOSTGEN_GEN})
foreach(line IN LISTS SLICE_TTE_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # The hostgen'd copy stands in for the src/ one; taking both would be a
    # duplicate symbol, and taking neither would drop the body silently. Both
    # sets are consulted because the shared body is a slice_tte line too.
    if(line IN_LIST TTE_HOSTGEN_SKIP OR line IN_LIST TT_SHARED_HOSTGEN_SKIP)
        continue()
    endif()
    list(APPEND SLICE_TTE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# A slice line that stops matching (a rename, a .c that becomes a .cpp) would
# silently take the src/ copy back and put the defect back with it, so the
# substitution asserts rather than hopes.
list(LENGTH TTE_HOSTGEN_SYMS tte_hg_want)
list(LENGTH TTE_HOSTGEN_GEN tte_hg_have)
if(NOT tte_hg_want EQUAL tte_hg_have)
    message(FATAL_ERROR "slice_tte hostgen substitution incomplete: "
            "${tte_hg_have} of ${tte_hg_want} generated")
endif()
foreach(skip IN LISTS TTE_HOSTGEN_SKIP TT_SHARED_HOSTGEN_SKIP)
    if(NOT skip IN_LIST SLICE_TTE_LINES)
        message(FATAL_ERROR "slice_tte.txt no longer lists ${skip}; the "
                "hostgen substitution for it is now dead and the geometry "
                "register writes in it would go back to latching silently")
    endif()
endforeach()
# THE SHARED BODY'S OWN GATE, and it is the one this arrangement exists for.
# The two per-slice loops above each assert that their OWN slice still lists
# the shared line, so between them they already cover both files -- but only
# while both blocks keep consulting TT_SHARED_HOSTGEN_SKIP. This asserts the
# sharing itself: the body must be a line in BOTH slices, because that is the
# condition that makes one emitted TU the right answer. If a future slice edit
# drops it from one of the two, the single generated copy is still compiled
# into the target and still correct, but the arrangement is no longer shared
# and the comment above it is no longer true -- and a shared block nobody
# re-reads is how the src/ copy comes back.
foreach(skip IN LISTS TT_SHARED_HOSTGEN_SKIP)
    if(NOT skip IN_LIST SLICE_TTI_LINES OR NOT skip IN_LIST SLICE_TTE_LINES)
        message(FATAL_ERROR "${skip} is the slice_tti/slice_tte SHARED hostgen "
                "body but is no longer listed by both slices; re-read the "
                "TT_SHARED block before changing it")
    endif()
endforeach()

# FOUR .c FILES IN THIS SLICE ARE REALLY C++ -- a //cpp marker and a top-level
# `extern "C" {` block each, two of them also a `virtual`, none of which is
# valid C. Every .c line in the slice
# was SWEPT with comments stripped for the marker and for `extern "C"`,
# `virtual`, `class`, `namespace`, `bool`, `new`, `delete`, `::` and `template`
# rather than this one being found by a compile error and the rest left to
# luck. It is the only hit: the files that match `::` or `class` match them
# only inside the naming pass's own comments, which is why the sweep strips
# comments first. (src/MgTrampolineTerror_Spawn.c was the second hit until the
# factory host copy took its place; it is no longer a slice line.)
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020c9098.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020c9c8c.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020cb528.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020cc198.c"
    PROPERTIES LANGUAGE CXX)

# AN AMBIGUOUS-MODULE ADDRESS, and the -D rename run mg9 lane S364 established
# for the same shape. 0x020cd72c is func_ov006_020cd72c, a 0x18-byte FUNCTION
# in ov006's symbol table (line 281) -- and it is ALSO data_ov007_020cd72c,
# kind data, in ov007's (line 839). src/func_ov006_020cd6f4.c declares its
# callee with the ov007 DATA name (`extern void data_ov007_020cd72c(int *c);`)
# and calls it. On the DS that is harmless: ov007 is not resident while ov006
# is, so the address is ov006's code either way. On the port BOTH mounts exist,
# the ov007 data face wins the name, and the call jumps into mounted ROM DATA.
# It faulted exactly there on this lane's second boot of scene 385 --
# FAULT c0000005 at +0x008f05e0 inside func_ov006_020cd6f4, one frame under
# the element constructor the factory runs. The rename points the reference at
# ov006's function; src/ and include/ stay untouched, and
# src/func_ov006_020cd72c.c is a slice line so the real body is linked.
# src/func_ov006_020cd12c.c names the same symbol and is NOT in this slice; a
# lane that pulls it in needs the same row.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020cd6f4.c"
    PROPERTIES COMPILE_DEFINITIONS "data_ov007_020cd72c=func_ov006_020cd72c")

# THE ONE BARE VT EXTERN IN THIS SLICE, and it is the run law's per-source
# rename onto this lane's own pool word. Every one of the slice's 204 TUs was
# swept (comments stripped) for VT / VT0 / VT1 / VT2 / HEAP / G0 / G1 / G2 /
# R0 / R1 / R2; there are four hits and three of them are LOCAL TYPE NAMES
# (`typedef struct { int x, y; } G2;` in src/func_ov006_020cc8c8.c and the two
# `struct G2 { int w[2]; }` in _020cc9fc / _020ccd78), which is the reason the
# sweep is read rather than counted. The real one is
# src/_ZN6Player29TryExitCharacterDoorWithIntroEv.cpp -- the Trampoline-Mario
# element CONSTRUCTOR, misnamed by the naming pass and correctly flagged in its
# own header ("The vptr it stores is the invented symbol VT0, which carries no
# address -- a separate problem, left alone here"). VT0 is DEFINED in this link
# as zeroed storage, so the constructor was writing a pointer-to-zeros into the
# element's word 0 and the first virtual dispatch through it called address 0.
# That is exactly what the fourth boot of scene 385 did:
#   FAULT c0000005 at +0xffc00000 accessing 00000000
#     func_ov006_020c9c8c   (o->slot4() on the element)
#     func_ov006_020ca310 / tte_sub_try / func_ov006_020c8f20
# The real table is data_ov006_0213b2e0 -- the ROM's own `str r1,[r4]` at
# 0x020ca7a0 loads it from the pool at 0x020ca7b4 -- and its five words are
# filled with host thunks by hal/scene_mg_trampoline2.cpp.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN6Player29TryExitCharacterDoorWithIntroEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS "VT0=data_ov006_0213b2e0")


file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_fdr.txt" SLICE_FDR_LINES)
set(FDR_FADER_SOURCES hal/fdr_arm9_fader_seat.cpp)
foreach(line IN LISTS SLICE_FDR_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND FDR_FADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Run link60 Stage 4, lane NFS: the NitroSDK open-BY-NAME file system.
#
# hal/fs_names.cpp registers the ROM's "rom" archive by calling the ROM's own
# func_0205cc80, which is what lets func_0205d714 resolve a path at all. It
# carries three host faces (FS_Init, the archive read function and the archive
# command proc -- all three are unmapped memory or the card) and hosts the six
# FS globals FS_Init owns.
#
# ONE VARIABLE, THREE TARGETS, for exactly the reason the fader seat above
# gives: the .cpp and the slice are not separable (the slice's three TUs are
# only reachable through this file, and this file cannot link without them),
# and a target that gets one but not the other has an unregistered archive
# while the others do not.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_nfs.txt" SLICE_NFS_LINES)
set(NFS_NAMES_SOURCES hal/fs_names.cpp)
foreach(line IN LISTS SLICE_NFS_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND NFS_NAMES_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

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
    unmatched/ChainChomp_Spawn_hostcopy.cpp
    # ...and for the eleventh, the widest one yet: KING_BOB_OMB keeps the
    # ADDRESS of a two-PMF record rather than an index, so both his setter and
    # his Behavior do pointer arithmetic on a type MSVC doubles
    unmatched/KingBobOmb_States.cpp
    # r0-passthrough overlay readers: func_ov084_02129cf4 (gate 32),
    # func_ov084_0212f204 (gate 70), func_ov094_02136024 (gate 194) and
    # func_ov102_02149078 (gate 203) each call Actor::ClosestPlayer() with no
    # argument (this rode ARM r0). Host copies pass each body's own receiver; src
    # lines commented out in their slice gates. Grouped here because all four ship
    # on the same three targets this GATE32 host set is added to.
    unmatched/Actor_ClosestPlayer_OverlayReaders.cpp)

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
# of bytes. unmatched/func_02008550_hostcopy.cpp is the second table's
# consumer, host-copied for the same mwcc-PMF reason func_0200cae4 is.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ptrsweep.txt" SLICE_PTR_LINES)
set(SLICE_PTR_SOURCES hal/ptr_tables.cpp unmatched/func_02008550_hostcopy.cpp)
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
    # disagreement port/unmatched/NumStars_hostcopy.cpp already carries the
    # read half of. port/unmatched/CollectStar_hostcopy.cpp takes the course
    # id as s8.
    if(src MATCHES "/HitDeathPlane\\.c$"
       OR src MATCHES "/func_0203d974\\.c$"
       OR src MATCHES "/CollectStar\\.c$"
    # ...and func_02049018, the 3D sound distance function, which drives the
    # DS square-root unit through raw cast-derefs and so has to go through
    # hostgen's MMIO transform (GATE35_SYMS) rather than compile as it stands.
       OR src MATCHES "/func_02049018\\.c$")
        continue()
    endif()
    # run mg15 lane MP1: THE DS SLEEP/WAKE PAIR AND ITS SCHEDULER.
    # src/OS_SleepThread.c's first act is data_020a612c->node, and
    # hal/cxx_aliases.cpp hosts data_020a612c as zero -- so the ROM body is a
    # null dereference on its first call, and port/nfs_names_map.txt:402 has
    # said so since the NitroFS seat. MEASURED by build target
    # mp_sleepwake_rom: 0xC0000005, through the real veneer. It is not
    # theoretical either -- src/func_02018e68.c (slice_mg1) reaches it on the
    # file-read failure path, so today the error report faults before it can
    # print. Past the deref, src/func_02057f54.c reschedules through
    # ARMProcessorMode/ARMSaveContext/ARMRestoreContext, none of which has a
    # host body and none of which has an honest one to write.
    # hal/os_thread.cpp reproduces the queue-word half of the contract exactly
    # and replaces the ARM context switch with a bounded host pump. It is
    # appended through SLICE_COMMS_SOURCES below, which every consumer of
    # SLICE10_CAM_SOURCES also takes -- checked: the three add_executable
    # lines that use this variable are the three that use that one.
    if(src MATCHES "/OS_SleepThread\\.c$"
       OR src MATCHES "/OS_WakeupThread\\.c$"
       OR src MATCHES "/func_02057f54\\.c$")
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

# run mg15 lane MP1: THE RADIO SEAM AND THE COMMS CLOSURE.
#
# hal/comms_seam.cpp hosts the eight ROM addresses where the game stops being
# a game and starts being a radio (the WM SDK, and under it arm7.bin, which
# this repo does not decompile and a PC port does not want). Everything ABOVE
# those eight stays the ROM's, verbatim -- that is the whole point, and it is
# what keeps the port portable back to hardware. The transport contract a
# launcher mod implements is in hal/comms_seam.h; nothing here is a network.
#
# hal/os_thread.cpp is the DS sleep/wake pair, hosted, replacing the three
# gate-10 rows filtered out above.
#
# slice_comms.txt is the ROM's four-player fan-out, linked but not yet driven.
# run mg16 lane MP2: hal/comms_loopback.cpp is the first REAL transport written
# against the seam's contract -- UDP on 127.0.0.1, two copies of the game on one
# machine as DS parent and child. hal/comms_lockstep.cpp drives the eight faces
# in src/func_0203ea5c.c's own order, because that TU is still in no slice and
# an invented order would prove nothing about the contract. BOTH ARE INERT
# WITHOUT SM64DS_COMMS_ROLE: nothing installs, and the seam answers solo.
set(SLICE_COMMS_SOURCES hal/comms_seam.cpp hal/os_thread.cpp
                        hal/comms_loopback.cpp)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_comms.txt" SLICE_COMMS_LINES)
foreach(line IN LISTS SLICE_COMMS_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_COMMS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run mg16 lane MP3: the REAL conductor, src/func_0203ea5c.c, and the dispatcher
# that switches to it. slice_mp3.txt says what is in it and why the seam line did
# not move to accommodate it. hal/comms_conductor.cpp is the host side: the DS
# globals the conductor needs that nothing had hosted, the one WM face that stays
# below the seam, the DS key register the dispatcher reads, and the pump that
# finally makes the contract's `poll` entry true.
#
# APPENDED TO SLICE_COMMS_SOURCES ON PURPOSE rather than given a list of its own:
# every consumer of the comms slice needs the conductor too, and three separate
# target lists each having to remember a new variable is exactly the shape
# FINDING 5 of the TITLE lane got bitten by (take one side of a generator-list
# conflict and a lane's TUs silently leave the link, which links fine and runs
# wrong).
list(APPEND SLICE_COMMS_SOURCES hal/comms_conductor.cpp)
# run mg16 lane MP4: the host-authoritative state-sync layer. Port-side
# only -- nothing in src/ knows it exists -- and OFF unless SM64DS_SYNC=1,
# so linking it changes no behaviour. See runs/mg16/status/MP4-DESIGN.md.
list(APPEND SLICE_COMMS_SOURCES hal/comms_sync.cpp)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_mp3.txt" SLICE_MP3_LINES)
foreach(line IN LISTS SLICE_MP3_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_COMMS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

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
    unmatched/func_0204335c_hostcopy.cpp
    unmatched/func_02043288_hostcopy.cpp
    unmatched/func_0204322c_hostcopy.cpp
    unmatched/func_02043fdc_hostcopy.cpp
    # ...and a fourth, dormant one: LoadEntranceObjects ends in
    # StartEntranceFaderWipe() spelled (void), with the wipe type riding in r0
    # from the `< 0` test right above it. Every entrance the port mounts takes
    # that return, so the call never fires today -- see the file header for
    # what it did and did not fix. Commented out of slice_gate14.txt in
    # exchange.
    unmatched/LoadEntranceObjects.cpp
    unmatched/func_02016ff4_hostcopy.cpp
    # unmatched/ActorBase_AfterCleanupResources.cpp RETIRED -- the matched
    # source hosts itself now, through hostgen's ROLE_DATA map. See
    # HOSTABI_RETIRE_SYMS.
    unmatched/func_ov002_020c6f3c.cpp
    # gate 16: two more mwcc pointer-to-member dispatches, both in actor
    # classes the registry carries. MSVC forms a PMF over an INCOMPLETE class
    # as the four-word general representation, which doubles the stride of the
    # table it indexes; see each file's header.
    unmatched/OneUpMushroom_Behavior.cpp
    unmatched/NumStars_hostcopy.cpp
    # gate 35: the WRITE half of the same s8/int argument-width disagreement
    unmatched/CollectStar_hostcopy.cpp
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
    unmatched/ChangeArea_hostcopy.c
    unmatched/func_0200cae4_hostcopy.cpp
    unmatched/func_0200ca50_hostcopy.cpp
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

# Gate 36: the real Stage::InitResources, to replace port_stage_a_boot.
#
# SLICE36_SOURCES IS INERT. It is parsed here and added to NO target, so these
# 33 lines compile nothing and link nothing. The comment that stood here said
# "Linked but NOT yet called", which was false in its first half: nothing is
# linked. The list is kept because it is the gate's inventory and because the
# measurement below is worth having written down, not because it is wired.
#
# WHY IT STAYS THAT WAY, measured in wave 4 rather than argued. Adding
# ${SLICE36_SOURCES} to walk_window and building fails the link with 43
# distinct unresolved externals and ZERO duplicate definitions -- so the
# blocker is not a clash with the host boot, it is that the slice does not
# close. The 43 sort into three groups and none of them is a line in a file:
#
#   22  storage the port hosts nowhere: data_020755e0/5f0/600/6f0/720/769/76a,
#       data_0208ecf4, data_0208ee00, data_02092130, data_02092778,
#       data_0209d4a8, data_0209f278, data_0209f290, data_0209f300,
#       data_0209f304, data_020758c8, data_020a0f04, data_ov001_020ad620,
#       data_ov001_020ad624, _ZTV9FaderWipe, and LoadOrUnloadObjectOverlays
#   15  matched TUs that exist in src/ and are not on this slice at all:
#       GetControllerMode, LoadOverlay, _ZN5ColorD1Ev, the five GX::SetBankFor*
#       setters, func_02018908, func_02018934, func_020190b8, func_02023408,
#       func_0203d81c, func_02053e1c, func_02053e34
#    6  ON THIS SLICE ALREADY and still unresolved: _Z17LoadLevelOverlaysi,
#       ShadowModel::CleanAll, Stage::LoadGraphics2D, Stage::LoadFog,
#       Stage::LoadTextureTransformers, FaderWipe::LoadAndSetFile
#
# That last group is what decides it. Those six .cpp files define the real C++
# METHOD -- src/_ZN5Stage7LoadFogEv.cpp compiles `void Stage::LoadFog()`, which
# MSVC mangles ?LoadFog@Stage@@... -- while Stage::InitResources calls the
# ROM's C name __ZN5Stage7LoadFogEv. Listing the file does not produce the
# symbol its caller wants; a hand-written face does, the way
# hal/method_faces.cpp already carries its own. So the slice head's "all 45
# direct callees are matched, nothing here is written by hand, this slice is
# the wiring" does not survive a build: six faces have to be written, 22
# globals hosted and 15 more callees enrolled before the boot swap it describes
# is reachable at all. Wiring it in today only breaks the build, which is why
# the honest move is to leave it inert and say so.
#
# ShadowModel::CleanAll is the one line here that IS doing work today, and it
# does it from slice_w1l3.txt instead -- see the block in that file.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate36.txt" SLICE36_LINES)
set(SLICE36_SOURCES "")
foreach(line IN LISTS SLICE36_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE36_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Stage 4, lane SD0: gate 36 WIRED, which is what the block above says nobody
# had done. port/slice_s4_slot0.txt is SLICE36_SOURCES minus src/LoadArchive.c
# (the one real LNK2005, against hal/scene_boot.cpp's PORT_HOST_ABI host body),
# minus src/UnloadArchive.c and src/_Z17LoadLevelOverlaysi.cpp (host bodies in
# hal/stage_slot0.cpp instead), plus the three matched callees the closure
# names and no active slice carries.
#
# THE 43 THE BLOCK ABOVE MEASURED IS NOW ZERO, and none of the three groups it
# sorted them into survived as written:
#   the 22 unhosted globals   six are romdata rows (data_020755e0/5f0/600,
#       data_02092778 and the two 0x9a sound columns), one is
#       hal/ptr_tables.cpp table 4 (data_020756f0, twelve ov002 pointers), one
#       is an /alternatename (data_02075720 onto VS_STAR_SPAWN_ORDERS), two are
#       4-byte BSS words in hal/stage_slot0.cpp, and the rest closed on their
#       own as the port grew -- data_0208ecf4 and LoadOrUnloadObjectOverlays
#       through hal/scene_boot.cpp, _ZTV9FaderWipe and the two ov001 words
#       through lane SL0.
#   the 15 unsliced callees   twelve closed as other slices took them; three
#       are on this slice.
#   the 6 already-on-the-slice  five are C-name faces (four in
#       hal/stage_slot0.cpp, FaderWipe::LoadAndSetFile in hal/fader_wipes.cpp),
#       and _Z17LoadLevelOverlaysi is a host body.
# The block above is kept as written because the measurement in it is the
# reason this one could be short.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_s4_slot0.txt" SLICE_S4_SLOT0_LINES)
set(SLICE_S4_SLOT0_SOURCES ${SCN3D_HOSTGEN_GEN})
foreach(line IN LISTS SLICE_S4_SLOT0_LINES)
    if(line IN_LIST SCN3D_HOSTGEN_SKIP)
        continue()
    endif()
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_S4_SLOT0_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
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
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/ShutterBob_Spawn_hostcopy.cpp")

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

# run linkw wave 7, lane w7b: THE MR. I FAMILY -- MR_I (262) and BIG_MR_I (263),
# which share one class and one vtable, plus MR_I_PROJECTILE (264), the shot
# MrI's state 1 fires through Actor::Spawn(0x108). ov071 was already mounted
# per-symbol by gate 176, so this lane adds no overlay: rows, two fills, the
# state seat, and four host copies (the two unmatched per-frame state MAINs,
# the two PMF dispatchers, plus the slot-5 Render and two D1 thunks).
# See slice_w7b.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w7b.txt" SLICE_W7B_LINES)
set(SLICE_W7B_SOURCES "")
foreach(line IN LISTS SLICE_W7B_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W7B_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Two MR_I_PROJECTILE TUs need hostgen, both mwcc-vs-MSVC syntax quirks the
# byte-verified tree must not be edited for (the gate-32 / gate-64 treatment).
# func_ov071_02121b08 is C++ inside a .c -- `struct Callback {}`, an empty
# struct, which MSVC will not compile as C (C2016); hostgen emits it as .cpp
# with the symbols wrapped back to C linkage.
# func_ov071_02121ba4 is the decl_common.h HEADER_SHADOW case: that header
# declares it `(void*)` inside its extern "C" block while the TU defines it
# `(char*)` -- one register on the ROM, C2733 to MSVC. This lane adds the
# HEADER_SHADOW table entry in tools/hostgen.py.
set(GATE_W7B_SYMS func_ov071_02121b08 func_ov071_02121ba4)
set(GATE_W7B_GEN "")
foreach(sym IN LISTS GATE_W7B_SYMS)
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
    list(APPEND GATE_W7B_GEN "${sym_out}")
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

# ---- run linkw wave 9, lane w9-reg: TILTING_PLATFORM_LLL (id 72, ov064) ----
# The largest single class in the wave-8 skip census: fourteen instances, x10 on
# level 44 and x4 on level 48. ov064 is already mounted twice (gates 177/178), so
# this adds no overlay mount -- only the class's own five matched TUs, its four
# extra mount symbols in port/ov064_syms.txt, and the fill in
# hal/actor_classes_ov064_w9.cpp. Its ov002 dependencies (the shared TILTING
# Behavior/Render pair and the base Init/Cleanup pair the two veneers tail-call)
# were all confirmed present in walk_window.map BEFORE the slice was written.
# See port/slice_w9a.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w9a.txt" SLICEW9A_LINES)
set(SLICEW9A_SOURCES "")
foreach(line IN LISTS SLICEW9A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICEW9A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# ---- run linkw wave 9, lane w9-reg, slice B: three ov084 shared-table siblings
# GOOMBA_LARGE (202) x9, FIRE_PIRANHA_PLANT_BIG (251) x5 and
# FIRE_PIRANHA_PLANT_SMALL (252) x2 -- sixteen instances for three TUs. Both ROM
# tables they use are already host arrays filled by an already-registered id, and
# all three SpawnInfo records are already in port/ov084_syms.txt, so this adds no
# mount and no fill. See port/slice_w9b.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w9b.txt" SLICEW9B_LINES)
set(SLICEW9B_SOURCES "")
foreach(line IN LISTS SLICEW9B_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICEW9B_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# ---- run linkw wave 9, lane w9-reg, slice C: three ov091 sliding-platform
# siblings sharing the table the registered id 55 already fills. Four instances
# for three TUs; no mount and no fill added. See port/slice_w9c.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w9c.txt" SLICEW9C_LINES)
set(SLICEW9C_SOURCES "")
foreach(line IN LISTS SLICEW9C_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICEW9C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# ---- run linkw wave 9, lane w9-reg, slice D: three ov095 seesaw siblings
# sharing the table the registered id 39 already fills. Four instances for three
# TUs; three records join port/ov095_syms.txt at the 0x1c stride that file
# already measured, and no fill or vtable is added. See port/slice_w9d.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w9d.txt" SLICEW9D_LINES)
set(SLICEW9D_SOURCES "")
foreach(line IN LISTS SLICEW9D_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICEW9D_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
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

# Gate 203: the QUESTION_BLOCK / EXCLAMATION_BLOCK's five HIT slots -- the
# vtable tail that ENTERS state 1, which gate 180 then runs. Gate 23 left
# slots 21/22/24/27/28 on ac_trapNN because their ov102 bodies were in no
# slice, so hitting the block raised a fault the quarantine net caught and the
# actor froze instead of opening. Slot 28 (0x021496a4) is OnHitFromUnderneath,
# the jump-into-it-from-below hit that is how the box is meant to be opened.
# See slice_gate203.txt and the QUESTION_BLOCK section in hal/actor_classes.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate203.txt" SLICE203_LINES)
set(SLICE203_SOURCES "")
foreach(line IN LISTS SLICE203_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE203_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 204 (run linkw wave C, lane cat-ov002): the ov002 DELETING DESTRUCTORS.
# Slot 17 on three seated ov002 classes -- PushBlock, HealingHeart and the Cap
# -- was a trap or a hand-spelled copy of the chain, while each class's own
# matched D0 body sat in src/ with nothing referencing it. The seats are in
# hal/actor_classes_bob_world.cpp and hal/actor_classes_ov002g200.cpp; the
# derivation, the RTTI-name aliases and why this is not a behaviour change are
# in slice_gate204.txt. Coin's D0 needed no line: it has been on
# slice_gate33.txt all along and only wanted the reference edge.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate204.txt" SLICE204_LINES)
set(SLICE204_SOURCES "")
foreach(line IN LISTS SLICE204_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE204_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 205 (run link60, lane A1): ov085's WALL_SIGN (183) and TOAD (185), the
# overlay's last two screened classes, nineteen placed instances across levels
# 2, 4, 5 and 50. ov085 has been mounted per symbol since gate 18 and all six
# of its sinits are already called, so the slice is code only. THREE host
# bodies ride beside it: both classes' slot 6 is not decompiled at all
# (unmatched/Ov085_Behaviors.cpp, HOST REIMPLEMENTATIONS written from the ROM),
# and Toad's two PMF dispatchers are the gate-16 case
# (unmatched/Ov085_Toad_StateSeat.cpp). Toad's D1 is held out and spelled in
# the fill, the SlideDecorationSilverStar shape. See slice_gate205.txt and
# hal/actor_classes_ov085.cpp.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate205.txt" SLICE205_LINES)
set(SLICE205_SOURCES "")
foreach(line IN LISTS SLICE205_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE205_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# Gate 208 (run link60, lane A2): MONTY_MOLE_ROCK (311), ov080's second enemy
# class after gate 174's mole. The mole's throw state spawns 0x137, the port
# declined it and the ROM-faithful thrower stored through the NULL, so
# SM64DS_LEVEL=13 SM64DS_SPAWN_ACTOR=310 faulted at +0x98 on the first throw.
# The whole cast is matched and every TU is on the slice -- no host copy, no
# state seat, no thunk standing in for a body, the first ov080 class needing
# none of those. The 31-slot width is re-derived with vtspan on this tree and
# slot 9's wave-19 slot-5 shadow is adjudicated SAFE from how the class
# constructs its +0x110 member (a plain Model, and _ZTV5Model[5] is
# dual-filled) -- the opposite ruling from the mole's own ModelAnim Render one
# class over. The fill is hal/actor_classes_montymolerock.cpp; the derivation,
# the vtspan routes and the seven friendly-name renames are in
# slice_gate208.txt.

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate208.txt" SLICE208_LINES)
set(SLICE208_SOURCES "")
foreach(line IN LISTS SLICE208_LINES)
    string(STRIP "${line}" line)
    if(line STREQUAL "" OR line MATCHES "^#")
        continue()
    endif()
    list(APPEND SLICE208_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# The rock's Behavior is the only TU in the tree that spells seven arm9/ov002
# functions by recovered friendly names that exist nowhere in the binary. Each
# target is pinned by the reloc out of that body's own range (ov080 relocs,
# Behavior = 0x0212459c..0x0212478c) and named from config/arm9/symbols.txt,
# never from the friendly name. A -D and not /alternatename directives on
# purpose: these are single global names of the G0/G1 kind, and an alias LHS
# another overlay's recovery pass reuses for a different address would
# misroute silently the day that overlay lands. A -D cannot be defeated.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN13MontyMoleRock8BehaviorEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "Actor_MakeVanishLuigiWork=_ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn;Actor_FindWithID=_ZN5Actor10FindWithIDEj;Player_Hurt=_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj;Actor_UpdatePos=_ZN5Actor9UpdatePosEP12CylinderClsn;Enemy_UpdateWMClsn=_ZN5Enemy12UpdateWMClsnER12WithMeshClsnj;CylinderClsn_Clear=_ZN12CylinderClsn5ClearEv;CylinderClsn_Update=_ZN12CylinderClsn6UpdateEv")
# Gate 208's arrival cost, and it is the R3 ov071/ov073 shape from the OTHER
# side: seating the rock DEFINES _ZN13MontyMoleRockD0Ev, which silently defeats
# hal/bob_enemy_bridges.cpp's /alternatename with that LHS. King Bob-omb's two
# throw states call ov078 0x02123804 -- his own throw body, relocs 0x02123ac0
# and 0x02123c28 both module:overlay(78), unambiguous -- and dsd named that
# same address for ov080 too, so their C spells it with the rock's destructor
# name. Without this they would have called the rock's destructor.
# tools/alternatename_guard.py failed the build the first time the seat linked;
# the dead directive is deleted there and the routing lands here, where a -D
# cannot be defeated.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov078_02123aa0.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov078_02123c20.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZN13MontyMoleRockD0Ev=func_ov078_02123804")

# Gate 206 (run link60, lane A2): DIAMOND_LIFT (135), ov043's first seated
# class. Six of level 35's eight skipped instances, and the only one of the
# overlay's three placed classes with a clean factory. The 32-slot width is
# vtspan-confirmed on every route, slot 9's slot-5 shadow is adjudicated SAFE
# from construction (a plain Model at +0xd4, and _ZTV5Model[5] is dual-filled),
# and one sinit runs rather than four. The fill and the mount bring-up are
# hal/actor_classes_ov043.cpp; the derivation and the four re-verified traps
# are in slice_gate206.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate206.txt" SLICE206_LINES)
set(SLICE206_SOURCES "")
foreach(line IN LISTS SLICE206_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE206_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# TRAP 1, and it is the live one. src/func_ov043_0211123c.c (id 135's
# CleanupResources) releases ov043's own two bss SharedFilePtrs through the
# shared role-name placeholders G0 and G1, and hal/cxx_aliases.cpp has already
# bound _G0 to _data_020a0eac (the GAME HEAP pointer) and _G1 to
# _data_ov002_0210e05c (SignPost's KCL). Neither would fail at link and the
# body would have released the game heap pointer -- the CannonHatch/MetalNet
# shape, the PATH_LIFT case exactly. The two targets are read off this body's
# own literal pool: 0x02111278 -> 0x021125e8 and 0x0211127c -> 0x021125e0,
# both module:overlay(43), each feeding a SharedFilePtr::Release at 0x02111260
# and 0x02111268. The rename goes to private names this lane owns, not
# straight to the mount names, because include/decl_common.h:396 declares
# `extern int G1[];` and MSVC rejects the redeclaration; the two private names
# bind onto the mounted cells in hal/actor_classes_ov043.cpp.
# src/func_ov043_021111e4.c IS DELIBERATELY NOT HERE. Its G0 is a DIFFERENT
# object -- the game heap it hands Memory::Deallocate, literal at 0x02111238
# relocating to 0x020a0eac module:main -- which is what cxx_aliases.cpp already
# binds. Renaming it would break a binding that is right today.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov043_0211123c.c"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_ov043_ukishima_file0;G1=port_ov043_ukishima_file1")
# TRAP 3: src/func_ov043_02111320.c (InitResources) spells ov043's own two bss
# cells with ov047 names. ov043 and ov047 are distinct objects sharing the load
# window and dsd raced the name; the load sites are the same two addresses the
# sinit initialises, both module:overlay(43). Neither ov047 name resolves in
# port/ today, so this is a hard link error rather than a silent miscall. Safe
# as a straight -D to the mount names: decl_common.h declares
# data_ov047_021125e0/e8 as `extern int [];` and declares no data_ov043_
# 021125e0/e8 at all, so there is nothing to conflict with.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov043_02111320.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov047_021125e8=data_ov043_021125e8;data_ov047_021125e0=data_ov043_021125e0")
# TRAP 4: src/func_ov043_021111e4.c (the deleting destructor) writes VT0 then
# VT1 and both are placeholders. Its own literal pool names them:
# 0x02111230 -> 0x021122b8 module:overlay(43), the class's own table, which is
# the host array; 0x02111234 -> 0x0210ae38 module:overlay(2), the Platform base
# table, which hal/actor_classes.cpp:583 already defines and fills. Renamed to
# the two real hosted names, never to a placeholder. Its sibling
# src/func_ov043_021111a0.c (the D1) writes the SAME pair BY NAME and needs no
# rename at all.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov043_021111e4.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=_ZTV19daObjKm1_Ukishima_c;VT1=_ZTV10dBgActor_c")

# Gate 207 (run link60, lane A2): four more ov002 slot-17 seats -- VirtualDoor,
# PoppingLavaBubbles, AmbientSoundEffects and MugenBgm. Gate 204's convention
# applied rather than extended, with all three of its conditions re-run against
# this build: the ROM word at table+0x44 is each class's own D0 read as raw
# bytes out of the overlay image, each body is flat C with no inferred-stub
# marker and a closed callee set, and the binary-wide [reg+44h] dispatch count
# is still zero with the slot-16 positive control still landing in
# ActorBase::AfterCleanupResources. Each table is installed by exactly one
# factory plus its own D1/D0, which is the condition BrickBlock fails. The
# derivation, the raw-byte table and the sweep output are in slice_gate207.txt.
# run rel0215 wave 1 (lane cast-ov026): ov026's whole cast, level 18 (Dire Dire
# Docks). Five classes, 49 of the overlay's 54 matched TUs. Per-symbol ov026
# mount (port/ov026_syms.txt), fill hal/actor_classes_ov026.cpp. The five TUs
# NOT in this slice are host-copied and named at the bottom of
# port/slice_ov026cast.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov026cast.txt" SLICE_OV026_LINES)
set(SLICE_OV026_SOURCES "")
foreach(line IN LISTS SLICE_OV026_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV026_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# ---- lane cast-ov026: the per-source renames -------------------------------
#
# A. THE SHARED ROLE-NAME PLACEHOLDERS. A comment-stripped sweep of all 54
# ov026 TUs for VT/VT0/VT1/VT2/G0/G1/G2/R0/R1/R2/HEAP returns exactly five
# files. hal/cxx_aliases.cpp binds bare G0 -> data_020a0eac (the GAME HEAP) and
# bare G1 -> data_ov002_0210e05c (SignPost's KCL) for the whole link, so an
# unrenamed placeholder here does not fail a link -- it releases the wrong
# storage, the CannonHatch/MetalNet/PathLift shape. Every target below is the
# address that TU's OWN ROM body loads, read out of the literal pool of
# extracted/overlays/overlay_0026.bin:
#
#   func_ov026_021111e0 (89's D0)   pool 0211122c/1230: VT0=0x02113ae0, G0=0x020a0eac
#   func_ov026_0211170c (91's D0)   pool 02111758/5c/60: VT0=0x02113ba4,
#                                     VT1=0x0210ae38 (_ZTV8Platform), G0=0x020a0eac
#   func_ov026_02111764 (91 Cleanup) pool 021117a0/a4: G0=0x02113ebc, G1=0x02113eb4
#   func_ov026_021118fc (90's D0)   pool 02111948/4c/50: VT0=0x02113c6c,
#                                     VT1=0x0210ae38, G0=0x020a0eac
#   func_ov026_02111954 (90 Cleanup) pool 02111990/94: G0=0x02113ee4, G1=0x02113edc
#
# VT0 goes to the HOST ARRAY in each case (all five vtables are excluded from
# the mount). VT1 is spelled _ZTV10dBgActor_c to match what the sibling D1s
# (func_ov026_021116c8.c, func_ov026_021118b8.c) already write BY NAME;
# hal/actor_classes.cpp:591 defines that array and dual-fills it with
# _ZTV8Platform.
# The four SharedFilePtr targets and the polelift's collision vector go through
# PRIVATE NAMES this lane owns, bound in hal/actor_classes_ov026.cpp with a
# C-name /alternatename, because include/decl_common.h already declares
# data_ov026_02113ebc/eb4/edc/ee4 as `char[]` and data_ov026_02113a9c as
# `Vector3` while the TUs declare their placeholders as `extern int G0[];` and
# a local `struct V3` -- a direct -D is the C2371/C2372 redeclaration the ov006
# Mg3DEsp block measured. Each private LHS is undefined everywhere else, so
# those aliases cannot be defeated.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov026_021111e0.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=_ZTV17daObjWlPolelift_c;G0=data_020a0eac")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov026_0211170c.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=_ZTV21daObjWlKoopaShutter_c;VT1=_ZTV10dBgActor_c;G0=data_020a0eac")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov026_021118fc.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=_ZTV18daObjWlSubmarine_c;VT1=_ZTV10dBgActor_c;G0=data_020a0eac")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov026_02111764.c"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_ov026_shutter_bmd;G1=port_ov026_shutter_kcl")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov026_02111954.c"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_ov026_submarine_bmd;G1=port_ov026_submarine_kcl")
#
# B. THE SECOND SPELLING OF A HOST-ARRAY VTABLE. Three of the five tables are
# reached under two names by ov026's own sources, and only one host array can
# exist per table because the object's vptr is one word. A -D rather than an
# /alternatename on purpose: a -D cannot be defeated by a later mount (the
# ov071/ov073 R3 failure), and the R5 precedent points the rename AT the host
# array for exactly this case.
#   BowserShutter_Spawn.c writes data_ov026_02113ba4, its D1 writes
#     _ZTV21daObjWlKoopaShutter_c -- same address 0x02113ba4.
#   Submarine_Spawn.c writes data_ov026_02113c6c, its D1 writes
#     _ZTV18daObjWlSubmarine_c -- same address 0x02113c6c.
#   _ZN9SubmarineD1Ev.c and Whirlpool_Spawn.c write _ZTV9Submarine, the D0
#     writes _ZTV18daWater_Tatumaki_c -- same address 0x02113d54, and the
#     _ZTV9Submarine row is the WRONG label (that table's own vtable[-1]
#     typeinfo name string reads 18daWater_Tatumaki_c), so the rename runs
#     toward the right name.
#   _ZN12WaterSuctionD0Ev.c writes _ZTV17daWater_Suikomi_c, the D1 and the
#     Spawn write _ZTV12WaterSuction -- same address 0x02113e24, and here BOTH
#     labels are correct spellings of one class, so the rename is only picking
#     the majority spelling.
# include/decl_common.h declares every one of these as `extern int NAME[];`, so
# each rename produces a duplicate IDENTICAL declaration, which is legal.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/BowserShutter_Spawn.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov026_02113ba4=_ZTV21daObjWlKoopaShutter_c")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/Submarine_Spawn.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov026_02113c6c=_ZTV18daObjWlSubmarine_c")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9SubmarineD1Ev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/Whirlpool_Spawn.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV9Submarine=_ZTV18daWater_Tatumaki_c")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN12WaterSuctionD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV17daWater_Suikomi_c=_ZTV12WaterSuction")
#
# C. THE WRONG-OVERLAY CROSSINGS. ov026 shares the level load window at
# 0x021111a0 with ov014/ov015/ov016/ov020/ov021/ov022/ov029/ov032/ov034/ov036/
# ov045/ov053 and dsd raced the name on four references inside ov026's own
# sources. Each target is ov026's own symbol at the identical address.
#
#   src/func_ov026_021122b0.c calls __sinit_ov045_02112280 where ov026's own
#   func_ov026_02112280 (0x02112280, kind:function, size 0x30) is meant. THIS
#   ONE IS A LIVE HAZARD, not a synonym: ___sinit_ov045_02112280 is ALREADY
#   DEFINED in walk_window by ov013_syms.c.obj (measured in the 0.2.15 baseline
#   map, 0004:000cc184) -- a mounted DATA object at the shared address. Left
#   alone, WaterSuction's state-0 tick would have called into mounted data, and
#   an /alternatename would have been defeated silently. A -D cannot be.
#
#   src/__sinit_ov026_02112d68.c writes data_ov036_02113f58 where ov026's own
#   bss cell data_ov026_02113f58 is meant -- that sinit is ov026's own .init
#   code and the cell it builds is the one WaterSuction::InitResources passes to
#   func_ov026_021122d4. ov036's 0x02113f58 is a real .data symbol of ov036's,
#   which is exactly why this must not be an alias.
#
#   src/func_ov026_02111598.cpp reads data_ov032_02113a9c (ov026's own
#   0x02113a9c, the polelift's collision half-extents) and calls
#   func_ov053_021112a4 (ov026's own func_ov026_021112a4, the matrix/scale
#   helper immediately above it in the same class block). The first goes
#   through the private name in block A because decl_common.h's own
#   data_ov026_02113a9c declaration would collide; the second is renamed
#   directly -- decl_common.h:1566 declares func_ov053_021112a4 and does not
#   declare func_ov026_021112a4, so there is nothing to conflict with.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov026_021122b0.c"
    PROPERTIES COMPILE_DEFINITIONS
    "__sinit_ov045_02112280=func_ov026_02112280")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/__sinit_ov026_02112d68.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov036_02113f58=data_ov026_02113f58")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov026_02111598.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov032_02113a9c=port_ov026_polelift_clsn;func_ov053_021112a4=func_ov026_021112a4")

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate207.txt" SLICE207_LINES)
set(SLICE207_SOURCES "")
foreach(line IN LISTS SLICE207_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE207_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# All four spell their own table with the shared role name VT, which
# hal/cxx_aliases.cpp binds globally to _data_ov002_021081e4 -- a fifth class's
# table. That is the CannonHatch/MetalNet shape and it would not fail at link,
# so each file is renamed to its own table. The targets come from each body's
# OWN literal pool, read out of extracted/overlays/overlay_0002.bin:
#   0x020b0a04 -> 0x021086b4   0x020b6e5c -> 0x021094a0
#   0x020f19e0 -> 0x0210b4c8   0x020f1c18 -> 0x0210b584
# HEAP is deliberately NOT renamed: the second literal in all four is
# 0x020a0eac, the game heap, which is what cxx_aliases.cpp already binds it to.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN11VirtualDoorD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=_ZTV11VirtualDoor")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN18PoppingLavaBubblesD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=_ZTV18PoppingLavaBubbles")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN19AmbientSoundEffectsD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=_ZTV19AmbientSoundEffects")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN8MugenBgmD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "VT=_ZTV8MugenBgm")

# ---- THE GAPLESS PER-ENTRY CORRECTION: the five G-adding OAM routers ---------
#
# Read THE ROUTER HOOK at the foot of port/hal/screen_gap.cpp before this. The
# gapless minigame mod zeroes the framework's screen-gap word, which takes G out
# of every submission these five bodies make to the top engine, and the fix has
# to put it back for THOSE submissions and for no others -- the score rows and
# the top screen's artwork are placed in screen space and were never displaced.
# Nothing in the finished OAM tells the two apart, so the routed set is recorded
# at the call, and this is the only place a call can be got at without editing
# src/.
#
# TWO RENAMES PER TU, and both are the R-series remedy this file already uses a
# dozen times over:
#
#   <router>=<router>__rom   the TU defines its body under a private name, and
#                            hal/screen_gap.cpp defines the real name as a
#                            wrapper that captures the world row and calls it.
#                            Every caller in the program links the wrapper.
#   OAM::Render=port_...     the TU declares its OAM::Render overload extern "C"
#                            by mangled name, so renaming that name redirects
#                            THAT TU's calls -- and only that TU's -- to the
#                            shim in hal/screen_gap.cpp. The overload's own
#                            definition, and every other caller of it in the
#                            program, is untouched.
#
# EACH FILE IS ON EXACTLY ONE SLICE and those slices ride smoke_player,
# walk_window and walk_window_hires, all three of which link hal/screen_gap.cpp,
# so the shims and wrappers are present wherever the renamed TUs are. Checked
# rather than assumed: the five appear once each across every port/slice_*.txt.
#
# WHICH OVERLOAD EACH TU CALLS, off the TU's own declaration:
#   func_ov004_020aff38   ..._Fix12IiEi        8 args   -> port_oam_render_routed_8
#   func_ov004_020afdd0   ..._Fix12IiES3_ii   10 args   -> port_oam_render_routed_10
#   func_ov004_020b023c   ...P9Matrix2x2       7 args   -> port_oam_render_routed_mtx
#   func_ov004_020b0380   ...P9Matrix2x2       7 args   -> port_oam_render_routed_mtx
#   RenderOamBothScreens  ...P9Matrix2x2       7 args   -> port_oam_render_routed_mtx
# func_ov004_020afdd0 also calls OAM::RenderSub for the bottom engine, which is
# a different symbol, is not renamed, and needs no correction: the bottom
# engine's y is the world row itself in every arm.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov004_020aff38.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov004_020aff38=func_ov004_020aff38__rom;_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi=port_oam_render_routed_8")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov004_020afdd0.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov004_020afdd0=func_ov004_020afdd0__rom;_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii=port_oam_render_routed_10")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov004_020b023c.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov004_020b023c=func_ov004_020b023c__rom;_ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2=port_oam_render_routed_mtx")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov004_020b0380.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov004_020b0380=func_ov004_020b0380__rom;_ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2=port_oam_render_routed_mtx")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/RenderOamBothScreens.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "RenderOamBothScreens=RenderOamBothScreens__rom;_ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2=port_oam_render_routed_mtx")

# Lane w8-shadows (run linkw): the SHADOWS bucket. One matched TU by name --
# TextureTransformer::Prepare, whose header declares it STATIC, which is what
# makes the matched body callable through the C-name face that used to
# duplicate it. The lane's other two seats need no line here: they were already
# in slice_gate16.txt and only wanted a reference edge. See slice_w8shadows.txt
# for the full triage of all twelve shadows and the derivation of the one that
# is deliberately still open.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w8shadows.txt" SLICE_W8SHADOWS_LINES)
set(SLICE_W8SHADOWS_SOURCES "")
foreach(line IN LISTS SLICE_W8SHADOWS_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W8SHADOWS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
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

# Gate 202: CLOCK_HAND_SHORT (292) + CLOCK_HAND_LONG (293) + CLOCK_PENDULUM
# (294), ov013 (level 5, the 2F clock-painting furniture). Per-symbol ov013
# mount (port/ov013_syms.txt). The fill is hal/actor_classes_ov013.cpp; the
# four D1/D0 TUs are host thunks there, everything else rides from src.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate202.txt" SLICE202_LINES)
set(SLICE202_SOURCES "")
foreach(line IN LISTS SLICE202_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE202_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run rel0215 wave 1 (lane cast-ov077): HEAVE_HO (238) + SPINY (260) +
# LAKITU (265), ov077. Fresh per-symbol ov077 mount (port/ov077_syms.txt). The
# fill and the PMF state seat are hal/actor_classes_ov077.cpp; the twelve
# host-copied TUs (three ModelAnim slot-5 Renders, six PMF dispatch sites, two
# real-C++ destructors MSVC cannot name, one implicit-r1 caller) are the three
# unmatched/*_HostSites.cpp files, each refusal itemised in
# port/slice_ov077.txt; unmatched/Lakitu_ShadowFaces.cpp carries the slice's
# measured closure gap, which was eight naming mismatches and no missing body.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov077.txt" SLICE_OV077_LINES)
set(SLICE_OV077_SOURCES "")
foreach(line IN LISTS SLICE_OV077_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV077_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run rel0215 wave 1 (lane cast-ov030): ROLLING_LOG_TTM (102), UKIKI_CAGE (103),
# UKIKI_THIEF (267) and UKIKI_STAR (268) -- ov030, Tall Tall Mountain (level 22).
# Fresh per-symbol ov030 mount (port/ov030_syms.txt) beside the existing whole
# mount. The three vtable fills, the eleven-cell PMF state seat, the loud face
# for the overlay's ONE missing body and the two RollingLogTtm method faces are
# in hal/actor_classes_ov030.cpp; the three host copies carry the two
# pointer-to-member dispatch sites and the ModelAnim slot-3 shadow. The gate-64
# shape: the hal file and the host copies ride in the slice's own SOURCES list,
# so the target lists take one token.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov030cast.txt" SLICE_OV030CAST_LINES)
set(SLICE_OV030CAST_SOURCES "")
foreach(line IN LISTS SLICE_OV030CAST_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV030CAST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE_OV030CAST_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/actor_classes_ov030.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ukiki_StateEnter.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ukiki_StateTick.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Ukiki_Behavior.cpp")

# lane cast-ov030, TRAP: THREE ov030 TUs SPELL A VTABLE WITH THE WRONG NAME AND
# TWO SPELL IT WITH A PLACEHOLDER. Each rename is read off that TU's OWN literal
# pool in config/arm9/overlays/ov030/relocs.txt, the ov043 trap-4 discipline
# (line 8300 above), never guessed from the placeholder's usual meaning.
#
#   src/RollingLogTtm_Spawn.c   stores TWO tables, in this ROM order:
#       from:0x02111680 -> 0x02128338  data_ov080_02128338, an INTERMEDIATE base
#                                      table that lives in ov080 (level 22 loads
#                                      ov080, not ov078, so that is the live
#                                      spelling of `module:overlays(79,80)`);
#                                      already mounted by port/ov080_syms.txt
#       from:0x02111684 -> 0x02115a48  the class's OWN table, the host array
#     and src spells them `_ZTV15daObjHmMaruta_c` then `VT1` -- the FIRST name is
#     on the WRONG store. VT1 is renamed to _ZTV9UkikiCage rather than straight
#     to _ZTV15daObjHmMaruta_c because a macro whose body is another macro is
#     rescanned and both stores would collapse onto the same array.
#     _ZTV9UkikiCage is config's own (mis-attributed) name for 0x02115a48 and
#     hal/actor_classes_ov030.cpp aliases it onto the one host array with
#     /alternatename, so this is ONE storage under two names -- the
#     _ZTV10dBgActor_c / _ZTV8Platform shape at hal/actor_classes.cpp:591.
#
#   src/_ZN9UkikiCageD1Ev.c and src/_ZN9UkikiCageD0Ev.c (BOTH are really
#   RollingLogTtm's destructors) store THREE tables, ROM order:
#       0x02115a48  the class's own      -- spelled _ZTV15daObjHmMaruta_c, RIGHT
#       0x02128338  ov080's intermediate -- spelled _ZTV10dBgActor_c,      WRONG
#       0x0210ae38  Platform's base      -- spelled VT2, a placeholder
#     0x0210ae38 is config's _ZTV8Platform, which hal/actor_classes.cpp already
#     defines and fills (it fills _ZTV10dBgActor_c and _ZTV8Platform with the
#     same contents for exactly this reason), so VT2 goes straight to it.
#     Neither replacement is itself a macro here, so no rescan collision.
#     D0 additionally spells the heap `G0`; its pool word is 0x0211160c ->
#     0x020a0eac, the same global _ZN13RollingLogTtmD0Ev.c names outright.
#
#   src/func_ov030_021111ec.c (UkikiCage's D0) spells all three by placeholder;
#     its pool is 0x02111240 -> 0x02115974, 0x02111244 -> 0x0210ae38 and
#     0x02111248 -> 0x020a0eac, so VT0/VT1/G0 map to the class's own table,
#     Platform's base and the heap. Its sibling src/func_ov030_021111a0.c (the
#     D1) spells the SAME pair BY NAME and correctly, and needs no rename.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/RollingLogTtm_Spawn.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV15daObjHmMaruta_c=data_ov080_02128338;VT1=_ZTV9UkikiCage")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9UkikiCageD1Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV10dBgActor_c=data_ov080_02128338;VT2=_ZTV8Platform")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9UkikiCageD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV10dBgActor_c=data_ov080_02128338;VT2=_ZTV8Platform;G0=data_020a0eac")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov030_021111ec.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=_ZTV13daObjHmBskt_c;VT1=_ZTV8Platform;G0=data_020a0eac")
# lane cast-ov030, the SECOND rename shape: src/func_ov030_02112ff8.c calls
# three arm9 helpers by their func_ADDRESS PLACEHOLDER while config names all
# three and the port already links them under those real names. Measured off
# the first link's own unresolved list, then read back out of
# config/arm9/symbols.txt rather than guessed from the call site:
#   0x0203cfdc  line 1644  Vec3_Dist
#   0x0200f97c  line  234  _ZN5Actor13SpawnSoundObjEj
#   0x02015024  line  500  _ZN12CylinderClsn5ClearEv
# All three are already in walk_window.map on the BASE tree as _Vec3_Dist,
# __ZN5Actor13SpawnSoundObjEj and __ZN12CylinderClsn5ClearEv, so this is a
# spelling bridge onto bodies that already link and adds no TU to the count.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov030_02112ff8.c"
    PROPERTIES COMPILE_DEFINITIONS
    "func_0203cfdc=Vec3_Dist;func_0200f97c=_ZN5Actor13SpawnSoundObjEj;func_02015024=_ZN12CylinderClsn5ClearEv")
# lane cast-ov030, THE G0/G1 TRAP, LIVE, and this row is the whole reason
# port/tools/alias_audit.py exists (its own docstring opens on this shape).
# src/func_ov030_0211130c.c is UkikiCage's CleanupResources and it Releases TWO
# SharedFilePtrs, spelled G0 and G1. hal/cxx_aliases.cpp binds those names ONCE
# for the whole link -- _G0 to _data_020a0eac (the GAME HEAP POINTER, line 1272)
# and _G1 to _data_ov002_0210e05c (an ov002 SharedFilePtr, line 1721) -- so
# without this row that function would have called SharedFilePtr::Release on the
# game heap pointer and on another overlay's object every time the cage tore
# down. It fails at NO link, shows in NO map, and no byte gate can see it.
# WHAT THE TU ACTUALLY MEANS, decoded from its own literal pool in the shipped
# image rather than from the placeholder's usual meaning:
#   0x0211132c  ldr r0,[pc,#0x14] -> 0x02111348 = 0x02115c88  = G0
#   0x02111334  ldr r0,[pc,#0x10] -> 0x0211134c = 0x02115c80  = G1
# Both are ov030's own .bss SharedFilePtrs and both are mounted by
# port/ov030_syms.txt. A comment-stripped sweep of all 67 slice TUs for
# G0/G1/G2/VT/VT0/VT1/VT2/HEAP/R0/R1/R2 finds placeholders in exactly five
# files, and all five are bound per source by the rows above and this one.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov030_0211130c.c"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=data_ov030_02115c88;G1=data_ov030_02115c80")



# ============================================================================
# RUN REL0215 WAVE 1, LANE CAST-SWEEP1 -- the small-cast sweep.
#
# Six level overlays whose whole cast is one or two classes with a tiny vtable
# set: ov035 (27), ov024 (16), ov033 (25), ov023 (15), ov017 (9), ov058 (50).
# Each lands as its own commit; this region grows by one block per overlay and
# is the ONLY place in this file the lane appends more than once. The three
# target source lists take ${SWEEP1_SOURCES} exactly once, so a second overlay
# costs the coordinator nothing outside this region.
#
# Every overlay here is already whole-mounted in PORT_LEVEL_OVERLAYS, so each
# per-symbol mount is the SECOND mount of the same overlay -- the
# ov012/ov013/ov025/ov045/ov052/ov056 dual shape. --whole emits
# port_ovNNN_image/_patch/_at and nothing by name; --from-list --pack emits the
# named arrays plus port_ovNNN_syms_patch. Every mount bring-up runs from the
# lane's own hal/actor_classes_ovNNN.cpp behind a DSSTATE-bracketed done-guard
# (the ov025/ov045/ov052/ov056/ov073 lane-ownership pattern), so this lane
# appends NOTHING to hal/actor_overlays.cpp.
set(SWEEP1_SOURCES "")

# ---- 1/6: ov035, level 27 (Tick Tock Clock) --------------------------------
# ROTATING_CLOCK_HAND (119) x2, SPINNING_PLATFORM (120) x13, ROTATING_COG_BIG
# (121) x2, ROTATING_COG_SMALL (122) x2 -- nineteen of level 27's 57 skipped
# instances. All twenty of ov035's TUs, no host copy, no refusal. Two 32-slot
# Platform tables hosted in hal/actor_classes_ov035.cpp; ov035 is ALREADY in
# PORT_ACTOR_OVERLAYS (wave 18 landed the list for the ov065 Ttc cluster's
# cross-overlay reads) and this lane adds the cast's own symbols to that list
# and, for the first time, a caller for port_ov035_syms_patch(). See
# port/ov035_syms.txt's second header and port/slice_sweep1_ov035.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep1_ov035.txt" SLICE_SWEEP1_OV035_LINES)
set(SLICE_SWEEP1_OV035_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP1_OV035_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP1_OV035_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# THE TWO SHARED-WINDOW ALIAS RACES (T6). ov017/ov023/ov035/ov045/ov055/ov056
# all link at 0x021111a0 and the decomp was written against whichever dsd
# export won the naming race, so two ov035 TUs spell ov035's own bytes with a
# sibling overlay's name. data_ov045_02112cb0 IS DEFINED in this link
# (port/ov045_syms.txt:123 mounts it), so an /alternatename with it on the left
# would be defeated silently -- the R3 ov071/ov073 failure -- and left alone
# the ov035 sinit would construct a SharedFilePtr over LEVEL 37's mounted data.
# A -D is the remedy T6 prescribes. The second is routed the same way so the
# pair cannot drift apart.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/__sinit_ov035_02111fc0.c"
    PROPERTIES COMPILE_DEFINITIONS "data_ov045_02112cb0=data_ov035_02112cb0")
# ...and this one goes to a PRIVATE name rather than straight to
# data_ov035_02112c68, for the ov100 path-lift reason: decl_common.h declares
# BOTH spellings, `extern char data_ov056_02112c68[]` at line 262 and
# `extern int data_ov035_02112c68[]` at line 1149, so a direct -D makes 262
# redeclare 1149 with a different element type. C2371, measured on the first
# build of this seat, not predicted. hal/actor_classes_ov035.cpp binds it.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN16RotatingCogSmall16CleanupResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS "data_ov056_02112c68=port_ov035_rcs_clsn")
# G0/G1 in id 120's CleanupResources are the two SharedFilePtrs at 0x02112cb0
# and 0x02112cb8 (pool order read off the ROM body at 0x0211192c/0x02111930).
# They go to PRIVATE names this lane owns rather than straight to
# data_ov035_02112cb0/b8, because that TU includes decl_common.h, which
# already declares both of those as `extern char` scalars -- a direct -D is a
# C2040 redeclaration, the ov100 path-lift trap. hal/actor_classes_ov035.cpp
# binds each private name onto the mounted object, and both LHS are undefined
# everywhere so neither alias can be defeated.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN17RotatingClockHand16CleanupResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_ov035_sp_file0;G1=port_ov035_sp_file1")
# G0 in the two deleting destructors is the GAME HEAP, 0x020a0eac -- the same
# name meaning something else in a different TU, which is why every one of
# these is per-source. Read off the ROM bodies' own literal pools
# (0x02111238 and 0x02111794).
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN16RotatingCogSmallD0Ev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN17RotatingClockHandD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "G0=data_020a0eac")
list(APPEND SWEEP1_SOURCES hal/actor_classes_ov035.cpp ${SLICE_SWEEP1_OV035_SOURCES})

# ---- 2/6: ov024, level 16 (Shifting Sand Land) -----------------------------
# PYRAMID_TOP (85) x1 and PYRAMID_TAG (86) x4 -- five of level 16's nineteen
# skipped instances. SIXTEEN of the overlay's seventeen TUs; the seventeenth,
# src/_ZN10PyramidTagD1Ev.cpp, is the //cpp real-destructor shape MSVC cannot
# deliver (it would call ??1Actor@@UAE@XZ and ??1MovingCylinderClsn@@QAE@XZ as
# thiscall against cdecl C bodies) and is transcribed from the ROM in
# hal/actor_classes_ov024.cpp -- the gate-199/200 treatment ov013 used on
# ClockPaintingHandShort's D1. Two host vtables of DIFFERENT lengths, 31 and
# 32. See port/ov024_syms.txt and port/slice_sweep1_ov024.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep1_ov024.txt" SLICE_SWEEP1_OV024_LINES)
set(SLICE_SWEEP1_OV024_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP1_OV024_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP1_OV024_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# G0 in id 85's deleting destructor is the GAME HEAP, 0x020a0eac, read off that
# body's own literal pool at 0x02111240. No alias race in this overlay: every
# ov024 symbol its TUs name is spelled with ov024's own prefix.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10PyramidTopD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "G0=data_020a0eac")
list(APPEND SWEEP1_SOURCES hal/actor_classes_ov024.cpp ${SLICE_SWEEP1_OV024_SOURCES})

# ---- 3/6: ov033, level 25 (Tiny-Huge Island, small side) -------------------
# TINY_COVER (104) x1 and TINY_WATER (105) x1 -- two of level 25's three
# skipped instances. All seventeen of the overlay's TUs, no host copy and no
# refusal. Two 32-slot Platform tables, and dsd's one vtable label is on the
# wrong one. See port/ov033_syms.txt and port/slice_sweep1_ov033.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep1_ov033.txt" SLICE_SWEEP1_OV033_LINES)
set(SLICE_SWEEP1_OV033_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP1_OV033_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP1_OV033_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# id 104's deleting destructor: VT0 is the OWN table 0x0211237c, VT1 is
# Platform's base 0x0210ae38, G0 is the game heap -- pool order read off the
# ROM body at 0x02111230/0x02111234/0x02111238. Private names because VT0/VT1
# mean different objects in the id-105 pair below.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov033_021111e4.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=port_ov033_tc_vt_own;VT1=port_ov033_vt_base;G0=data_020a0eac")
# id 105's D1/D0: THE RESOLVER'S TWO LABELS ARE INVERTED. The source writes
# _ZTV10dBgActor_c first and VT1 second; the ROM writes the OWN table first
# (0x021113e4 stores the word at pool 0x02111418 = 0x02112440) and Platform's
# base second (0x021113f4 stores pool 0x0211141c = 0x0210ae38). Left alone the
# destructor would install the base table before the TextureTransformer
# teardown and leave the OWN table installed after Actor::D2 -- exactly
# inverted, and invisible to both the byte gate and the linkage count. A direct
# -D onto _ZTV10dBgActor_c is impossible anyway: that name is a live host array
# for every Platform in the port.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9TinyCoverD1Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV10dBgActor_c=port_ov033_tw_vt_own;VT1=port_ov033_vt_base")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9TinyCoverD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV10dBgActor_c=port_ov033_tw_vt_own;VT1=port_ov033_vt_base;G0=data_020a0eac")
# THE ALIAS RACE (T6). id 104's InitResources hands the collider a CLPS block
# it spells data_ov035_02111bfc -- OV035's name for the same shared-window
# address -- and ov035's 0x02111bfc IS MOUNTED (port/ov035_syms.txt, wave 18's
# seeds), so the name resolves and the collider would take level 27's data.
# decl_common.h declares the ov035 spelling and not the ov033 one, so this
# rename is direct and needs no private name.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov033_02111310.cpp"
    PROPERTIES COMPILE_DEFINITIONS "data_ov035_02111bfc=data_ov033_02111bfc")
list(APPEND SWEEP1_SOURCES hal/actor_classes_ov033.cpp ${SLICE_SWEEP1_OV033_SOURCES})

# ---- 4/6: ov023, level 15 (Lethal Lava Land's volcano) ---------------------
# SQUASHER (84) x1 -- and it is the WHOLE of level 15's skip list, so this seat
# takes that level to zero skipped classes. All ten of the overlay's TUs, no
# host copy and no refusal. One 32-slot Platform table; the dsd landing
# OVER-reads it by seven slots. See port/ov023_syms.txt and
# port/slice_sweep1_ov023.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep1_ov023.txt" SLICE_SWEEP1_OV023_LINES)
set(SLICE_SWEEP1_OV023_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP1_OV023_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP1_OV023_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# The destructors' two vtable stores are LABELLED BACKWARDS by the resolver:
# the source writes _ZTV10dBgActor_c first and VT1 second, the ROM writes the
# OWN table first (pool 0x021111e4 = 0x02111fe4) and Platform's base second
# (0x021111e8 = 0x0210ae38). Same inversion ov033's id-105 pair carries. A
# direct -D onto _ZTV10dBgActor_c is impossible: it is a live host array for
# every Platform in the port.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN8SquasherD1Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV10dBgActor_c=port_ov023_vt_own;VT1=port_ov023_vt_base")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN8SquasherD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV10dBgActor_c=port_ov023_vt_own;VT1=port_ov023_vt_base;G0=data_020a0eac")
# CleanupResources releases the model file then the collision one, in that
# order (pool 0x02111380 then 0x02111384). Private names because that TU
# includes decl_common.h, which already declares 0x02112088 as a scalar.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN8Squasher16CleanupResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_ov023_sq_file0;G1=port_ov023_sq_file1")
# TWO LIVE ALIAS RACES IN ONE BODY (T6), both pinned to the ROM:
#   the call at 0x021116a4 relocates to 0x02111308 module ov023 -- the source
#     calls it func_ov026_02111308, and lane W1-A is seating ov026 in this same
#     wave, so that name is about to become DEFINED and the call would land in
#     another level's overlay. A direct -D collides with decl_common.h's own
#     func_ov023_02111308(char*) declaration, so it goes to a private name.
#   pool 0x0211171c holds 0x02112080, ov023's collision SharedFilePtr -- the
#     source spells it &_ZN32FloatOnWaterPlatformWdwRectangleD1Ev, an OV029
#     FUNCTION name at ov023's data address. Left alone MeshCollider::LoadFile
#     takes the address of a destructor as a file pointer.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN8Squasher13InitResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov026_02111308=port_ov023_xform;_ZN32FloatOnWaterPlatformWdwRectangleD1Ev=port_ov023_clsn_file")
list(APPEND SWEEP1_SOURCES hal/actor_classes_ov023.cpp ${SLICE_SWEEP1_OV023_SOURCES})

# ---- 5/6: ov017, level 9 (Jolly Roger Bay's sunken ship) -------------------
# SHIP_WATER (62) x1 -- one of level 9's two skipped instances, and a blocker
# hal/level_boot.cpp's own level-9 block named and deferred. All eight of the
# overlay's TUs, no host copy and no refusal. One 32-slot Platform table; dsd's
# landing reads TEN. See port/ov017_syms.txt and port/slice_sweep1_ov017.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep1_ov017.txt" SLICE_SWEEP1_OV017_LINES)
set(SLICE_SWEEP1_OV017_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP1_OV017_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP1_OV017_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# THREE ALIAS RACES IN ONE FUNCTION (T6), every one pinned to the ROM's own
# literal pool at 0x02111470..0x0211147c, over which the relocations resolve
# ALL FOUR of InitResources's objects to OV017 symbols:
#     pool 0x02111474 -> 0x02111a60  spelled data_ov056_02111a60
#     pool 0x02111478 -> 0x02111c80  spelled &_ZN16FloatingFloorBfsD0Ev
#     pool 0x0211147c -> 0x02111a94  spelled data_ov055_02111a94
# THE MIDDLE ONE IS LIVE ON THIS TIP: src/_ZN16FloatingFloorBfsD0Ev.c is in
# port/slice_w1l4.txt and the symbol is in build/port/walk_window.map, so
# without this rename MeshCollider::LoadFile is handed the ADDRESS OF AN OV045
# DESTRUCTOR as a file pointer -- resolving cleanly, failing nowhere at link.
# The other two are undefined today and latent, but ov056 IS mounted and a
# later lane adding either name turns them live silently. The TU includes no
# headers at all, so all three renames are direct.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9ShipWater13InitResourcesEv.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov056_02111a60=data_ov017_02111a60;_ZN16FloatingFloorBfsD0Ev=data_ov017_02111c80;data_ov055_02111a94=data_ov017_02111a94")
# G0 in the deleting destructor is the game heap. Note this overlay's D1/D0 get
# their TWO VTABLE STORES IN THE RIGHT ORDER, unlike ov023's and ov033's: pool
# 0x021111e4 holds the own table and 0x021111e8 Platform's base, which is the
# order the source writes them, so neither store needs renaming.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9ShipWaterD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "G0=data_020a0eac")
# CleanupResources releases the model file then the collision one (pool
# 0x02111288 then 0x0211128c). Private names because that TU includes
# decl_common.h, which already declares both.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9ShipWater16CleanupResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=port_ov017_sw_file0;G1=port_ov017_sw_file1")
list(APPEND SWEEP1_SOURCES hal/actor_classes_ov017.cpp ${SLICE_SWEEP1_OV017_SOURCES})

# ---- 6/6: ov058, level 50 (the Rec Room) -----------------------------------
# REC_ROOM_CUPBOARD (182) x1 -- and it is the WHOLE of level 50's skip list, so
# this seat takes that level to zero skipped, the second in this lane after
# level 15. All six of the overlay's TUs, no host copy, no refusal and NOT ONE
# per-source -D: no placeholder reaches code, no TU spells a sibling's name,
# and dsd's vtable label is on the table its own factory installs. One 31-slot
# ACTOR table (the ov024 PYRAMID_TAG shape, not a Platform one); dsd's landing
# reads TWO. See port/ov058_syms.txt and port/slice_sweep1_ov058.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep1_ov058.txt" SLICE_SWEEP1_OV058_LINES)
set(SLICE_SWEEP1_OV058_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP1_OV058_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP1_OV058_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SWEEP1_SOURCES hal/actor_classes_ov058.cpp ${SLICE_SWEEP1_OV058_SOURCES}
    unmatched/RecRoomCupboard_ActorDtorSeam.cpp)
# ---- end lane cast-sweep1 --------------------------------------------------



# run rel0215 wave 1 (lane cast-ov036): ov036's Rainbow Cruise cast --
# SWINGING_PLATFORM (127), ROTATING_PLATFORM_RR (129), SHIP_WING (125),
# DONUT_BLOCK (126), ARMED_ROTATING_PLATFORM (132), TRICKY_TRIANGLES (123) and
# FLYING_CARPET (130), level 28. Per-symbol ov036 mount (port/ov036_syms.txt).
# The fill is hal/actor_classes_ov036.cpp. Two TUs are NOT in the slice:
# _ZN18RotatingPlatformRrD1Ev.cpp (SHIP_WING's D1, an MSVC-synthesised
# destructor over a local shadow class that provides no C name -- the chain is
# spelled as sw_d1 in the fill) and func_ov036_02112378.cpp (FLYING_CARPET's
# Render, the ModelAnim slot-5 collision -- host copy in
# unmatched/FlyingCarpet_Render.cpp). The last four lines of the slice are the
# MEASURED external closure: four ov002 bodies TRICKY_TRIANGLES needs, taken
# off the map rather than guessed.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov036cast.txt" SLICE_OV036CAST_LINES)
set(SLICE_OV036CAST_SOURCES "")
foreach(line IN LISTS SLICE_OV036CAST_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV036CAST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run rel0215 wave 2 (lane cast-ov066): EYEROK (176, 10daIwante_c), ov066, the
# SSL pyramid boss. Fresh per-symbol ov066 mount (port/ov066_syms.txt). The
# 32-slot vtable fill, the NINETEEN-cell pointer-to-member state seat and the
# three slot traps are in hal/actor_classes_ov066.cpp; four TUs are host-copied
# (the PMF enter half, the PMF tick half inside Behavior, the BlendModelAnim
# slot-5 Render and the receiver-dropping ClosestPlayer reader), each refusal
# itemised in port/slice_ov066.txt. The gate-64 / cast-ov030 shape: the hal file
# and the four host copies ride in the slice's own SOURCES list, so the three
# target lists take ONE token.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov066.txt" SLICE_OV066_LINES)
set(SLICE_OV066_SOURCES "")
foreach(line IN LISTS SLICE_OV066_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV066_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE_OV066_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/actor_classes_ov066.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Eyerok_StateEnter.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Eyerok_Behavior.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Eyerok_Render.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Eyerok_ClosestPlayerRead.cpp")
foreach(dup IN ITEMS src/_ZN6Eyerok8BehaviorEv.cpp src/_ZN6Eyerok6RenderEv.cpp
                     src/func_ov066_02119454.cpp src/func_ov066_02119398.cpp)
    if(dup IN_LIST SLICE_OV066_LINES)
        message(FATAL_ERROR
            "slice_ov066.txt lists ${dup}, which port/unmatched/ host-copies. "
            "Both would define the same symbol (LNK2005). Remove the slice line.")
    endif()
endforeach()

# ---- lane cast-ov066: THE SIX ALIASES THIS MOUNT DEFEATS --------------------
# THE TREE PREDICTED THIS LANE IN WRITING. port/build-port.cmd:92 and
# port/tools/alternatename_guard.py:20 both say "EyerokD0 and the data_ov075
# aliases flip the same way if their overlays land". SIX live /alternatename
# directives carried an ov066 name as their LHS, and defining that name defeats
# the alias SILENTLY -- no link error, a wrong object at run time, the
# Coffin/Spindrift shape. All six are removed from their hal files and replaced
# by per-source -D renames on the FOREIGN CALLER, which is the remedy
# hal/actor_classes_koopa_chuckya.cpp's own note already prescribes for exactly
# this case (the w5b_review.md R2 func_ov065_02117994 precedent).
#
# Every one was checked BOTH ways before the conversion: the caller TU is on a
# live slice, and the -D target is defined in the link. A -D onto an undefined
# name would have turned a silent bug into a link error, which is better, but
# neither is what this lane is for.
#
# (1) src/func_ov065_021162c0.c (slice_w5b) -- a Snufit state body. It calls
#     0x02115f84 and the reloc `from:0x02116314 kind:arm_call to:0x02115f84
#     module:overlay(65)` puts that body in ov065's OWN image as
#     func_ov065_02115f84, size 0x6c. Eyerok's D0 at the same address is size
#     0xb8 -- a DIFFERENT body, and the sizes alone settle it. Undefeated this
#     calls a destructor on a live Snufit.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov065_021162c0.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZN6EyerokD0Ev=func_ov065_02115f84")
# (2) src/func_ov062_02117acc.c (slice_gate182) -- a Chuckya/Koopa body whose
#     callees dsd named under ov066 because it saw that overlay first. Both
#     targets are ov062's own and both are sliced by gate 182.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov062_02117acc.c"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov066_02118cdc=func_ov062_02118cdc;func_ov066_02118a50=func_ov062_02118a50")
# (3) src/func_ov060_02111c68.c (slice_w5e) and (4) src/func_ov060_02112350.c
#     (slice_w5e) -- three of ov060's OWN bss cells at addresses ov066's mount
#     now names as Eyerok PMF source pairs (port/ov060_syms.txt lines 128/131/
#     135 carry the ov060 storage). Undefeated, ov060 would read Eyerok's state
#     table.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov060_02111c68.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov066_0211ac20=data_ov060_0211ac20;data_ov066_0211ac68=data_ov060_0211ac68")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov060_02112350.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov066_0211acd0=data_ov060_0211acd0")

# ---- lane cast-ov066: THREE MIS-SPELLED NAMES IN ONE OWN TU -----------------
# src/func_ov066_021166c8.cpp (InitResources' resource half) spells three
# symbols nothing in the tree defines. All three are settled off that TU's OWN
# literal pool in config/arm9/overlays/ov066/relocs.txt, disassembled out of
# extracted/overlays/overlay_0066.bin, never by name -- the ov036/ov043 trap-4
# discipline.
#
#   func_02112cc8            pool 0x02116890 -> 0x02112cc8   an ov025 CLPS_Block
#   func_02112c88            pool 0x021168a0 -> 0x02112c88   an ov025 CLPS_Block
#   func_02112cd8_updatepos  pool 0x021168a8 -> 0x02039348   NOT 0x02112cd8
#
# The first two are collision-parameter blocks handed to
# MovingMeshCollider::SetFile. Their addresses fall in the LEVEL window and
# dsd's module column names eleven level overlays, every one `ambiguous`; ov066
# is loaded on level 17 alone and level 17's overlay is ov025, so ov025's are
# the ones the ROM means. Both joined port/ov025_syms.txt with this lane (one
# of them pinned to 0x20, see that file). This is the ov046 arena-CLPS shape
# (hal/actor_classes_ov060.cpp:201) with the alias written as a -D instead.
#
# THE THIRD NAME IS SIMPLY WRONG. There is NO reloc to 0x02112cd8 anywhere in
# ov066. The pool word func_020393d4's second argument is loaded from holds
# 0x02039348, module:main, which config/arm9/symbols.txt names
# MeshColliderBase::UpdatePosWithTransform -- an arm9 body, and the callback
# the collider genuinely wants. The recovered name invented an overlay address
# out of the "updatepos" role. src/ is not edited; the rename fixes the one
# call site.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov066_021166c8.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_02112cc8=data_ov025_02112cc8;func_02112c88=data_ov025_02112c88;func_02112cd8_updatepos=_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_")
# THREE MORE OF THE SAME BLOCKS, in five more TUs, MEASURED off the FIRST
# link's own unresolved list rather than predicted: _func_02112c08,
# _func_02112ca8 and _func_02112d48 came back undefined from
# _ZN6Eyerok13InitResourcesEv.cpp.obj, func_ov066_02116db0.c.obj,
# func_ov066_021175e8.c.obj, func_ov066_02117bf0.c.obj and
# func_ov066_02118188.c.obj. Every one is another ov025 CLPS_Block handed to
# MovingMeshCollider::SetFile, settled the same way -- the TU's own pool word,
# then the level-17 roster. 0x02112d48 is the sharpest of the five: ov060
# claims that address with a real FUNCTION (func_ov060_02112d48, size 0x94)
# and ov060 is NOT on level 17's roster, so the CLPS block is what is resident
# when Eyerok runs. The four .c TUs declare them with C linkage and the one
# .cpp declares them inside its extern "C" block, so all five come out
# undecorated and the rename lands directly on the mount's C name.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN6Eyerok13InitResourcesEv.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov066_02116db0.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov066_021175e8.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov066_02117bf0.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov066_02118188.c"
    PROPERTIES COMPILE_DEFINITIONS
    "func_02112c08=data_ov025_02112c08;func_02112ca8=data_ov025_02112ca8;func_02112d48=data_ov025_02112d48")

# ---- lane cast-ov036: SIX PER-SOURCE PLACEHOLDER AND CROSS-NAME BINDINGS ----
# Every one is settled from the TU's OWN literal pool in
# config/arm9/overlays/ov036/relocs.txt, disassembled out of
# extracted/overlays/overlay_0036.bin, never by name. VT0/VT1/VT2 are the
# dangerous half: all three are DEFINED in this link (actor_vtables.cpp:407,
# `int VT0[20]` and its siblings), so an unrenamed store lands silently on a
# twenty-word shared dummy instead of failing at link -- the CMakeLists:2600
# class. G1 is the safe half (no cdecl row, so it would be a hard link error),
# and bare G0 in a .c resolves through `/alternatename:_G0=_data_020a0eac`,
# which is RIGHT for the three deleting destructors whose pools hold the game
# heap and WRONG for the one Cleanup whose pool holds its own two files.
#
# (1) src/func_ov036_021111e4.c -- SWINGING_PLATFORM's D0. Pool, in store order:
#     0x02111230 -> 0x02113a98 (its own table, host storage) and
#     0x02111234 -> 0x0210ae38 (_ZTV8Platform, the dBgActor_c base, already
#     host-filled by hal_fill_platform_vtable). Its G0 is NOT renamed: the third
#     pool word 0x02111238 -> 0x020a0eac module:main is the game heap
#     Memory::Deallocate takes, which is exactly what the cdecl row binds.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov036_021111e4.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=data_ov036_02113a98;VT1=_ZTV8Platform")
# (2) src/func_ov036_02111284.c -- SWINGING_PLATFORM's CleanupResources.
#     Releases two SharedFilePtrs as G0/G1, and its own pool names them:
#     0x021112c0 -> 0x02114028 then 0x021112c4 -> 0x02114020, both
#     module:overlay(36), both in this lane's own mount. Unrenamed, G0 would
#     Release the game heap and G1 would not link. Renamed straight to the mount
#     names rather than to private ones: include/decl_common.h declares neither
#     data_ov036_02114020 nor data_ov036_02114028, so there is nothing to
#     conflict with (the ov043 TRAP-3 reading; the ov100 C2040 needed private
#     names only because the target was already declared as a scalar).
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov036_02111284.c"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=data_ov036_02114028;G1=data_ov036_02114020")
# (3) src/func_ov036_02111444.c -- ROTATING_PLATFORM_RR's D1, three vptr stores
#     back to back with no call between them. Pool in store order:
#     0x02111488 -> 0x02113b74 (its own table), 0x0211148c -> 0x021091d4
#     (daObjKaitendai_c, the ov002 INTERMEDIATE base, host storage in
#     hal/actor_classes_ov036.cpp) and 0x02111490 -> 0x0210ae38 (_ZTV8Platform).
#     The middle one is the trap: the TU spells it _ZTV10dBgActor_c, the
#     196-speller placeholder the port defines AS the Platform base table, so
#     unrenamed this body would store 0x0210ae38's host array where the ROM
#     stores the Kaitendai one. Dead in this body, wrong everywhere it is read.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov036_02111444.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV19daObjRc_Kaitendai_c=data_ov036_02113b74;_ZTV10dBgActor_c=data_ov002_021091d4;VT2=_ZTV8Platform")
# (4) src/func_ov036_02111494.c -- ROTATING_PLATFORM_RR's D0, the same three
#     stores as its D1 twin and the same pool order (0x021114e8/ec/f0), with the
#     game-heap word at 0x021114f4 for G0, which is left on the cdecl row.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov036_02111494.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=data_ov036_02113b74;VT1=data_ov002_021091d4;VT2=_ZTV8Platform")
# (5) src/RotatingPlatformRr_Spawn.c -- THE ONE MIS-BOUND NAME INSIDE A TU.
#     The ROM stores the BASE table first and the derived one second, which is
#     the correct constructor order:
#       0211155c ldr r1,[pc,#0x14] -> 0x02111578 -> 0x021091d4 daObjKaitendai_c
#       02111560 ldr r0,[pc,#0x14] -> 0x0211157c -> 0x02113b74 its own table
#       02111564 str r1,[r4]   02111568 str r0,[r4]
#     but the recovered source calls the FIRST store _ZTV19daObjRc_Kaitendai_c
#     (the derived class's own name on the base object) and the second VT1. Both
#     spellings are bound here to the address that TU's own pool carries, so the
#     store order and both values stay exactly the ROM's. Neither definition
#     expands to the other, so there is no macro rescan.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/RotatingPlatformRr_Spawn.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV19daObjRc_Kaitendai_c=data_ov002_021091d4;VT1=data_ov036_02113b74")
# (6) THE THREE ROM-NAME SPELLINGS OF A TABLE THIS LANE ALREADY HOSTS UNDER ITS
#     config NAME. Each pair is the SAME address under two labels dsd emitted,
#     and the port needs one storage, so the ROM spelling is folded onto the
#     config one the fill declares. Confirmed per TU against its own pool:
#       _ZN18RotatingPlatformRrD0Ev.c  0x021115ec -> 0x02113c38 == _ZTV18RotatingPlatformRr
#       _ZN8ShipWingD1Ev.c             0x02111980 -> 0x02113cf8 == _ZTV8ShipWing
#       _ZN8ShipWingD0Ev.c             0x021119dc -> 0x02113cf8 == _ZTV8ShipWing
#       _ZN10DonutBlockD1Ev.c          0x02111d50 -> 0x02113dcc == _ZTV10DonutBlock
#       _ZN10DonutBlockD0Ev.c          0x02111da4 -> 0x02113dcc == _ZTV10DonutBlock
#       func_ov036_021111a0.c          0x02111230 -> 0x02113a98 == data_ov036_02113a98
#     The last one goes the OTHER way -- onto the config name rather than onto
#     the _ZTV one -- because config names 0x02113a98 only data_ov036_02113a98,
#     and vtspan --sweep can only size an array whose name it can look up.
#     The _ZTV10dBgActor_c in the four ShipWing/DonutBlock destructors is NOT
#     renamed: their pools hold 0x0210ae38, which is what the port's own
#     _ZTV10dBgActor_c array already is (hal/actor_classes.cpp fills it and
#     _ZTV8Platform with identical contents). Only the ROTATING_PLATFORM_RR pair
#     above spells that name for a different table.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN18RotatingPlatformRrD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "_ZTV14daObjRc_Hane_c=_ZTV18RotatingPlatformRr")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN8ShipWingD1Ev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN8ShipWingD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "_ZTV16daObjRc_Tikuwa_c=_ZTV8ShipWing")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10DonutBlockD1Ev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10DonutBlockD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "_ZTV18daObjRc_Guruguru_c=_ZTV10DonutBlock")
#     SwingingPlatform_Spawn.c needs NO rename: it already spells
#     data_ov036_02113a98, which is the canonical host array name (see the
#     vtspan note in hal/actor_classes_ov036.cpp -- config names 0x02113a98
#     only that way, so the host-only _ZTV16daObjRcBuranko_c spelling would
#     have gone unsized). src/func_ov036_021111a0.c, SWINGING_PLATFORM's D1,
#     is the one TU that spells the host-only name; its pool at 0x02111230 is
#     0x02113a98, module:overlay(36).
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov036_021111a0.c"
    PROPERTIES COMPILE_DEFINITIONS "_ZTV16daObjRcBuranko_c=data_ov036_02113a98")
# (7) AN ALIAS THIS MOUNT DEFEATS, AND THE ONE LIVE READER IT HAD.
# ov018, ov022, ov036 and ov056 share the level-overlay base 0x021111a0, and
# ov036's dsd export won the naming race at 0x021140d4, so
# src/_ZN21FloatingFloorLllSmall13InitResourcesEv.c (ov022's
# FLOATING_FLOOR_LLL_SMALL, id 76, level 14, LINKED via
# port/slice_w8packs.txt:51) reaches ov022's OWN six-word file/CLPS table under
# ov036's spelling. hal/actor_classes_ov022.cpp routed that with
# `/alternatename:_data_ov036_021140d4=_data_ov022_021140d4`, and an
# /alternatename is defeated the moment its LHS is DEFINED -- which is exactly
# what this lane's mount now does (data_ov036_021140d4 is one of
# TRICKY_TRIANGLES' ten SharedFilePtrs, built by __sinit_ov036_0211279c).
# Unfixed, FLOATING_FLOOR_LLL_SMALL on level 14 would load its models out of
# Rainbow Cruise's cells: no link error, no byte-gate signal, wrong object at
# run time -- the Coffin/Spindrift shape, caught here by
# port/tools/alternatename_guard.py's post-link map check.
# The guard's own remedy, and the wave-5 R1/R2 recipe: the reader gets a
# per-source -D onto the name it actually means, and the now-inert directive is
# retired at its own site rather than left reading as live routing.
# FOUR MORE TUs spell an ov036 name for another overlay's object at the shared
# base -- src/__sinit_ov025_021129dc.c (02113a98), src/func_ov025_02111384.cpp
# (02113a60), src/__sinit_ov026_02112d68.c (02113f58) and
# src/__sinit_ov027_02112df8.cpp (02113cb4/cc0/ccc). NONE of the four is in any
# slice or in any map, so nothing races today; they are ov025's, ov026's and
# ov027's names to fix when those lanes land, and alternatename_guard will say
# so at the link if one arrives.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN21FloatingFloorLllSmall13InitResourcesEv.c"
    PROPERTIES COMPILE_DEFINITIONS "data_ov036_021140d4=data_ov022_021140d4")

# run rel0215 wave 2 (lane cast-ov090): SKEETER (230) + MANTA_RAY (226) +
# CHEEP_CHEEP (227) + SHARK (225), ov090, the water enemy pack levels 18 and 21
# both name. Fresh per-symbol ov090 mount (port/ov090_syms.txt). The four
# vtable fills and the sixteen-record PMF state seat are
# hal/actor_classes_ov090.cpp; the twelve host-copied TUs (four ModelAnim
# slot-5 Renders, four PMF state setters, four Behaviors whose tick half
# dispatches the same way, two of which additionally construct a PathPtr as a
# real C++ object) are the four unmatched/*_HostSites.cpp files, each refusal
# itemised in port/slice_ov090.txt. The gate-64 shape: the hal file and the
# host copies ride in the slice's own SOURCES list, so the target lists take
# one token. ov090 has NO missing and NO NONMATCHING body -- 70 of 70 present
# -- so there is no loud L2 face in this pack.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov090.txt" SLICE_OV090_LINES)
set(SLICE_OV090_SOURCES "")
foreach(line IN LISTS SLICE_OV090_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV090_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE_OV090_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/actor_classes_ov090.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Skeeter_HostSites.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/MantaRay_HostSites.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/CheepCheep_HostSites.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Shark_HostSites.cpp")

# ---- lane cast-ov090: THREE CROSS-OVERLAY NAME BINDINGS, ALL BY ADDRESS ----
# Every one is settled from the crossing TU's OWN relocation in
# config/arm9/overlays/<ov>/relocs.txt -- the `module:` column, read against
# the raw bytes in extracted/overlays/ -- never from a dsd label. Two point
# outward from ov090 and one points inward at it. All three are -D renames
# rather than /alternatename for the same reason the ov036 lane's (7) is: this
# lane's mount DEFINES the names an alias would need to leave undefined.
#
# (a) src/func_ov090_02131608.cpp spells func_ov091_02134498 for ov090's OWN
#     data_ov090_02134498. ov090's own pool reloc is
#     `from:0x02131644 kind:load to:0x02134498 module:overlay(90)`. 0x02134498
#     is a SharedFilePtr in ov090's .bss that __sinit_ov090_02133ce8 constructs
#     with file id 0x3a3, and the TU reads its second word as a FILE POINTER
#     and hands it to ModelAnim::SetAnim. ov091's .text runs to 0x021344e8, so
#     the ov091 name is a real FUNCTION there and src/func_ov091_02134498.c
#     exists -- left alone this links clean and calls the wrong thing.
#
# (b) src/func_ov090_02133830.c spells func_ov091_02133710 for ov090's OWN
#     func_ov090_02133710. ov090's own pool reloc is
#     `from:0x02133874 kind:load to:0x02133710 module:overlay(90)`, and
#     0x02133710 sits inside SHARK's .text block (0x0213367c..0x02133ca0) --
#     as does 0x02133830 itself, which is Shark's own body. src/
#     func_ov091_02133710.cpp exists, so this too links clean and runs the
#     wrong body on any level that loads both overlays.
#
# (c) AN ALIAS THIS MOUNT DEFEATS, AND ITS ONE LIVE READER -- the ov036 (7)
#     shape exactly. src/func_ov006_020d5e5c.c declares
#     `extern int data_ov090_021343b0[]` for what is ov006's own minigame table
#     (`from:0x020d5eb4 kind:load to:0x021343b0 module:overlay(6)`, and
#     0x020d5eb4 is that TU's own literal pool). In ov090 the same address is
#     SHARK's typeinfo NAME STRING, "9daShark_c".
#     port/unmatched/MgBomroom_Faces.cpp routed that with
#       /alternatename:_data_ov090_021343b0=_data_ov006_021343b0
#     whose own header recorded that its LHS is "DEFINED NOWHERE ELSE IN THE
#     LINK". THIS MOUNT DEFINES IT, so the alias goes inert and mg6's
#     dScMgBomroom_c would read Shark's name string as an OAM table -- no link
#     error, no byte-gate signal, wrong object at run time. The routing is the
#     -D below, the guard's own remedy, and it survives the LHS becoming
#     defined the way the alias could not.
#
#     WHAT THE RETIRED SEAT FILE SAID, folded here because the file is gone.
#     port/unmatched/MgBomroom_Faces.cpp was deleted in run rel0215 (lane
#     cleanup-w2): the ov090 lane took its one directive out and left 68 lines
#     of comment compiling to an empty object, and prose belongs beside the
#     routing that is live rather than in a translation unit that emits
#     nothing. Three things it recorded that are not obvious from the -D:
#       * AN ADDRESS ALONE DOES NOT NAME A SYMBOL. 0x021343b0 is a live symbol
#         in BOTH config trees -- ov090/symbols.txt:108 data_ov090_021343b0 and
#         ov006/symbols.txt:2381 data_ov006_021343b0 -- because the two
#         overlays share an address window. Only the relocation's module field
#         decides, which is why every one of these three notes quotes it. It is
#         the finding lane CCN's closure walker hit when resolving by address
#         reported the overlay window four times over.
#       * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN. src/func_ov006_020d5e5c.c
#         should declare data_ov006_021343b0; whether it still builds
#         byte-identically under mwccarm is a byte-gated-tree question. src/ is
#         not edited from here, so the -D is the whole port-side answer.
#       * ov090 IS NOT CO-RESIDENT WITH A MINIGAME SCENE ANYWAY. The resident
#         set for scene 370 is main + itcm + ov001 + ov004 + ov006, which is
#         what makes this mount define the ov006 name and nothing define the
#         ov090 one.
#     Same retirement, same reason, as lane cast-ov036's on
#     hal/actor_classes_ov022.cpp's data_ov036_021140d4 alias one wave earlier.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov090_02131608.cpp"
    PROPERTIES COMPILE_DEFINITIONS "func_ov091_02134498=data_ov090_02134498")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov090_02133830.c"
    PROPERTIES COMPILE_DEFINITIONS "func_ov091_02133710=func_ov090_02133710")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_020d5e5c.c"
    PROPERTIES COMPILE_DEFINITIONS "data_ov090_021343b0=data_ov006_021343b0")


# run rel0215 wave 2 (lane cast-ov027): ov027's Snowman's Land cast --
# SLIDING_ICE_SPAWNER (92), SLIDING_ICE (93), CHILL_BULLY (217), DA_PG_DFDR
# (258) and SNOWMAN_BREATH (275), level 19. Per-symbol ov027 mount
# (port/ov027_syms.txt). The fill is hal/actor_classes_ov027.cpp. Two TUs are
# NOT in the slice: src/func_ov027_02111d38.cpp and src/func_ov027_02111cfc.cpp,
# DA_PG_DFDR's two pointer-to-member dispatch sites -- host-copied in
# unmatched/DaPgDfdr_StateDispatch.cpp, originals byte-locked in src/. There is
# NO external closure line: every arm9/ov002/ov062/ov064 body this closure
# reaches was already linked on the base tip, checked against walk_window.map
# before the first configure and confirmed by the first link's own unresolved
# list.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov027cast.txt" SLICE_OV027CAST_LINES)
set(SLICE_OV027CAST_SOURCES "")
foreach(line IN LISTS SLICE_OV027CAST_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV027CAST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# ---- lane cast-ov027: NINE WRONG-OVERLAY NAMES AND SIX PLACEHOLDER BINDINGS -
# Every one is settled from the TU's OWN literal pool or its own call reloc in
# config/arm9/overlays/ov027/relocs.txt, disassembled out of
# extracted/overlays/overlay_0027.bin, never by name.
#
# ov009/ov014/ov015/ov016/ov018/ov020/ov021/ov022/ov024/ov025/ov026/ov027/
# ov029/ov030/ov032/ov033/ov034/ov035/ov036/ov043/ov045/ov053/ov056 all share
# the level-overlay load window at base 0x021111a0, and dsd's export won the
# naming race at nine addresses that are ov027's OWN. The task sheet flagged
# two of them. A sweep of all 53 TUs against every overlay's symbol table found
# SEVEN MORE SPELLINGS, and five of the nine are SILENT today -- the name is
# already DEFINED in this link, so the reference binds to another overlay's
# object with no link error and no byte-gate signal (the Coffin/Spindrift
# shape, T6). Status measured against build/port/walk_window.map on the base
# tip, not assumed:
#
#   spelling                      really            defined now?  failure mode
#   data_ov036_02113cb4/cc0/ccc   ov027 bss nodes   YES ov036     SILENT
#   data_ov035_02112ca4           ov027 rodata      YES ov035     SILENT
#   func_ov030_02113be8           ov027 bss ptr     YES ov030     SILENT, worst
#   data_ov043_02112424           ov027 CODE        YES ov043     SILENT
#   RotatingPlatformRr_SpawnInfo  ov027 vtable      YES ov036     SILENT
#   data_ov056_02112158           ov027 CODE        no            hard link error
#   data_ov033_0211233c           ov027 CODE        no            hard link error
#   data_ov053_021123b0           ov027 CODE        no            hard link error
#
# (1) src/__sinit_ov027_02112df8.cpp -- THE TASK SHEET'S FIRST TRAP. Three
#     names, all ov027's own destructor-chain nodes. Its own literal pool, in
#     source order, is at 0x02112f08..0x02112f5c and the three rows are:
#       from:0x02112f30 kind:load to:0x02113cb4 module:overlay(27)
#       from:0x02112f3c kind:load to:0x02113cc0 module:overlay(27)
#       from:0x02112f4c kind:load to:0x02113ccc module:overlay(27)
#     config/arm9/overlays/ov027/symbols.txt lists all three as ov027 bss.
#     ov036's mount landed in wave 1 and port/ov036_syms.txt:294-295 DEFINES
#     all three, so unrouted this sinit registers DA_PG_DFDR's three Anim-file
#     destructor nodes into Rainbow Cruise's mounted cells.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/__sinit_ov027_02112df8.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov036_02113cb4=data_ov027_02113cb4;data_ov036_02113cc0=data_ov027_02113cc0;data_ov036_02113ccc=data_ov027_02113ccc")
# (2) src/func_ov027_02111d8c.c -- THE TASK SHEET'S SECOND TRAP. DA_PG_DFDR's
#     CleanupResources, whose three-iteration SharedFilePtr::Release loop reads
#     data[0..2]. Both of this TU's loads of that address say overlay(27):
#       from:0x02111df0 kind:load to:0x02112ca4 module:overlay(27)
#       from:0x02112050 kind:load to:0x02112ca4 module:overlay(27)
#     and 0x02112ca4's own three words relocate to 0x02113c84/0x02113c74/
#     0x02113c8c, the three Anim SharedFilePtrs sinit 02112df8 builds -- so the
#     table is unmistakably this class's. port/ov035_syms.txt:295 DEFINES
#     data_ov035_02112ca4:0xc (ov035's id-119 collision chain node), so
#     unrouted this TU Releases Tick Tock Clock's chain node three times.
#     include/decl_common.h:3079 already declares data_ov027_02112ca4 as
#     void*[3], which is the right type and the right width.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov027_02111d8c.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov035_02112ca4=data_ov027_02112ca4")
# (3) SLIDING_ICE's TWO RESOURCE METHODS, and this is the worst of the nine.
#     Both spell func_ov030_02113be8 for ov027's own model SharedFilePtr, and
#     ov030's 0x02113be8 is a 0x138-byte FUNCTION that wave 1 linked
#     (walk_window.map: _func_ov030_02113be8 in func_ov030_02113be8.c.obj).
#     InitResources passes it to Model::LoadFile and reads [1] back out;
#     CleanupResources calls SharedFilePtr::Release on it. Unrouted, the port
#     WRITES the loaded BMD_File pointer into Tall Tall Mountain's code. Each
#     TU's own pool names the address:
#       from:0x02111554 kind:load to:0x02113be8 module:overlay(27)   (Init)
#       from:0x021112ac kind:load to:0x02113be8 module:overlay(27)   (Cleanup)
#     and __sinit_ov027_02112cb0 constructs 0x02113be8 with asset 1713.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10SlidingIce13InitResourcesEv.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10SlidingIce16CleanupResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov030_02113be8=data_ov027_02113be8")
# (4) src/_ZN13SnowmanBreath6RenderEv.cpp -- a CALL, not a load. The body loops
#     `data_ov043_02112424(p)` fifty times over the object's fifty sub-elements
#     at +0xd4 stride 0x60. The reloc is a call and it names this overlay:
#       from:0x0211277c kind:arm_call to:0x02112424 module:overlay(27)
#     0x02112424 is func_ov027_02112424, inside ov027's own .text. ov043's
#     mount DEFINES data_ov043_02112424 (port/ov043_syms.txt:153), so unrouted
#     this Render executes ov043 romdata BYTES as code, fifty times a frame.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN13SnowmanBreath6RenderEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov043_02112424=func_ov027_02112424")
# (5) SNOWMAN_BREATH's TWO DESTRUCTORS, two wrong names each. Both spell
#     RotatingPlatformRr_SpawnInfo for the VPTR STORE -- and that is ov036's id
#     129 record, at 0x02113b50, which is exactly where ov027 parks
#     SnowmanBreath's own vtable. Wave 1 mounted it, so unrouted these two
#     destructors write the address of Rainbow Cruise's SpawnInfo record into
#     the object's vptr and every virtual after that reads a spawn record as a
#     table. Both also spell data_ov056_02112158 for the per-element destructor
#     they hand __destroy_arr, which is ov027's own func_ov027_02112158 (that
#     one is undefined today, so it would at least fail loudly). Pools:
#       D1  from:0x021120fc load -> 0x02113b50 module:overlay(27)
#           from:0x02112100 load -> 0x02112158 module:overlay(27)
#       D0  from:0x0211214c load -> 0x02113b50 module:overlay(27)
#           from:0x02112150 load -> 0x02112158 module:overlay(27)
#     SnowmanBreath_Spawn's own pool holds the same two words and spells them
#     correctly (_ZTV13SnowmanBreath and func_ov027_02112158), which is the
#     third read that settles both.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN13SnowmanBreathD1Ev.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN13SnowmanBreathD0Ev.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "RotatingPlatformRr_SpawnInfo=_ZTV13SnowmanBreath;data_ov056_02112158=func_ov027_02112158")
# (6) src/func_ov027_02112480.cpp -- two calls, two wrong overlays, both really
#     ov027's own bodies. Neither name is defined in the link today, so this
#     pair fails loudly rather than silently; routed anyway, by address:
#       from:0x021124a0 kind:arm_call to:0x021123b0 module:overlay(27)
#       from:0x021124a8 kind:arm_call to:0x0211233c module:overlay(27)
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov027_02112480.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov033_0211233c=func_ov027_0211233c;data_ov053_021123b0=func_ov027_021123b0")
# ---- the placeholder half: VT0/VT1 and the host-only _ZTV spellings ---------
# VT0/VT1 are the dangerous ones: both are DEFINED in this link
# (hal/actor_classes.cpp, `int VT0[20]` and its sibling), so an unrenamed store
# lands silently on a twenty-word shared dummy instead of failing at link.
# Every VT0/VT1 in this overlay's 53 TUs is bound below; the sweep found four
# TUs using them and no others. G0 is NOT renamed anywhere here: all three
# bodies that spell it (SlidingIce's D0, CHILL_BULLY's D0, DA_PG_DFDR's D0)
# carry 0x020a0eac in their pools, which is the game heap Memory::Deallocate
# takes and exactly what `/alternatename:_G0=_data_020a0eac`
# (hal/cxx_aliases.cpp:1272) resolves it to. _ZTV10dBgActor_c is NOT renamed
# either: both pools that spell it hold 0x0210ae38, which is what the port
# already defines that name as.
#
# (7) SLIDING_ICE's D1 and D0 spell _ZTV17daObjSlIceBlock_c (the ROM RTTI name)
#     for the table config calls _ZTV10SlidingIce. Same address under two
#     labels; the port needs one storage, so the ROM spelling folds onto the
#     config one the fill declares. Both pools, in store order (disassembled:
#     `ldr r2 <- 02113824; ldr r1 <- 0210ae38; str r2,[r4]; str r1,[r4]`):
#       D1  0x021111dc -> 0x02113824      0x021111e0 -> 0x0210ae38
#       D0  0x02111230 -> 0x02113824      0x02111234 -> 0x0210ae38
#                                         0x02111238 -> 0x020a0eac (G0, left)
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10SlidingIceD1Ev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10SlidingIceD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV17daObjSlIceBlock_c=_ZTV10SlidingIce")
# (8) CHILL_BULLY's D1: _ZTV12daIDonketu_c is its OWN table and VT1 is ov064's
#     Bully base. Pool in store order, disassembled at 0x021115c8:
#       ldr r2 <- @02111610 -> 0x02113930   (own table)
#       ldr r1 <- @02111614 -> 0x0211b768   (ov064 Bully/BigBully base, 37)
#       str r2,[r4] ; str r1,[r4]
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov027_021115c4.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV12daIDonketu_c=data_ov027_02113930;VT1=data_ov064_0211b768")
# (9) CHILL_BULLY's D0: the same two stores in the same order, plus the heap.
#       0x02111674 -> 0x02113930   0x02111678 -> 0x0211b768
#       0x0211167c -> 0x020a0eac (G0, left on the cdecl row)
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov027_02111618.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=data_ov027_02113930;VT1=data_ov064_0211b768")
# (10) src/ChillBully_Spawn.c -- THE ONE MIS-BOUND PAIR INSIDE A TU, and it is
#      the exact inverse of (8). The ROM stores the BASE table first and the
#      derived one last, which is the correct constructor shape, but the
#      recovered source calls the FIRST store _ZTV12daIDonketu_c (the derived
#      class's own name, on the base object) and the LAST one VT1. Settled by
#      DISASSEMBLY, not by pool order -- the pool is laid out the other way
#      round from the store order, which is exactly the trap:
#        02111880  bl 020aed98              Enemy::C2
#        02111884  ldr r1,[pc,#52] -> @021118c0 -> 0211b768
#        0211188c  str r1,[r4,#0]           FIRST store  = the ov064 BASE
#        02111890  bl 02016958              ModelAnim::C1     at +0x110
#        02111898  bl 02037430              WithMeshClsn::C1  at +0x174
#        021118a0  bl 020149c8              MovingCylinderClsn::C1 at +0x33c
#        021118a8  bl 02016068              ShadowModel::C1   at +0x370
#        021118ac  ldr r0,[pc,#16] -> @021118c4 -> 02113930
#        021118b0  str r0,[r4,#0]           SECOND store = its OWN table
#      So the SAME spelling means the opposite table in this TU and in (8),
#      which is why an /alternatename could never have routed this pair and the
#      binding has to be per source. Neither definition expands to the other,
#      so there is no macro rescan.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/ChillBully_Spawn.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV12daIDonketu_c=data_ov064_0211b768;VT1=data_ov027_02113930")
# (11) DA_PG_DFDR's D1 and its factory both spell _ZTV10daPgDfdr_c for the
#      table config calls data_ov027_02113a90. Pools, disassembled:
#        D1       0x0211191c -> 0x02113a90   then 0x02111920 -> 0x0210ae38
#                 (store order: own table, four member teardowns, then dBgActor)
#        factory  0x021120c0 -> 0x02113a90
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov027_021118c8.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov027_0211207c.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV10daPgDfdr_c=data_ov027_02113a90")
# (12) DA_PG_DFDR's D0: the same two stores as its D1 twin, same order, plus
#      the heap. 0x02111988 -> 0x02113a90, 0x0211198c -> 0x0210ae38,
#      0x02111990 -> 0x020a0eac (G0, left).
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov027_02111924.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=data_ov027_02113a90;VT1=_ZTV8Platform")


# run rel0215 wave 2 (lane w2-ov074): GOOMBOSS (198) + EXPLOSION_GOOMBA (199),
# ov074, level 45 (Mario's key arena). Fresh per-symbol ov074 mount
# (port/ov074_syms.txt). The fill, the nine-cell PMF state seat and the three
# loud faces for the overlay's three missing bodies are
# hal/actor_classes_ov074.cpp; the four host-copied TUs (two ModelAnim slot-5
# Renders, two PMF dispatch sites) are unmatched/Goomboss_HostSites.cpp and
# unmatched/Goomboss_StateDispatch.cpp, each refusal itemised in
# port/slice_ov074.txt. The gate-64 shape: the hal file and the host copies
# ride in the slice's own SOURCES list, so the target lists take one token.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov074.txt" SLICE_OV074_LINES)
set(SLICE_OV074_SOURCES "")
foreach(line IN LISTS SLICE_OV074_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV074_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SLICE_OV074_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/hal/actor_classes_ov074.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Goomboss_HostSites.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Goomboss_StateDispatch.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/unmatched/Goomboss_InitResources.cpp")

# lane w2-ov074: THE TWO .c FILES IN THIS SLICE THAT ARE REALLY C++.
# The comment-stripped token sweep (`extern "C"`, virtual, class, namespace,
# bool, new, delete, template, true, false, ::) plus the //cpp marker check was
# run over all the .c lines rather than waiting for a compile error. A third
# file hit the sweep and is a HOST COPY rather than a slice line
# (src/_ZN8Goomboss13InitResourcesEv.c -- MSVC C2561, see
# unmatched/Goomboss_InitResources.cpp). A fourth matched and is NOT C++:
# src/func_ov074_02120d74.c matches `false` because it DEFINES it
# (`#define false 0`), which is the false positive that sweep has to tolerate
# to stay cheap -- and compiling it as C++ would break on the macro.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov074_0211f5b8.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov074_02121800.c"
    PROPERTIES LANGUAGE CXX)

# lane w2-ov074: THE ENEMY CONSTRUCTOR, one of the three plain C names off the
# first link's own unresolved list. Both factories call `func_020aed98(p)` --
# ov074's relocation reads `to:0x020aed98 module:overlays(2,7)`, ambiguous, and
# the class settles it: _ZTV8Goomboss's vtable[-1] typeinfo names base
# 0x021081c0, whose name string reads "12dEnemyBase_c". The port's spelling of
# that constructor is _ZN5EnemyC2Ev (hal/cxx_aliases.cpp:1434, and the ov073
# lane's port_factory_chief_chilly calls it under that name).
# BOTH FACTORIES NAME THE RECEIVER, so the ChainChomp / ChiefChilly r0
# ride-through problem (port/hal/bob_enemy_bridges.cpp:308) does not apply here
# and neither factory needs a host copy -- a rename is enough.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/Goomboss_Spawn.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/ExplosionGoomba_Spawn.cpp"
    PROPERTIES COMPILE_DEFINITIONS "func_020aed98=_ZN5EnemyC2Ev")

# lane w2-ov074: TWO ALIASES THIS MOUNT DEFEATS, AND BOTH HAVE A LIVE READER.
# ov070, ov071, ov073 and ov074 share the load window at base 0x0211f000, and
# two /alternatename pragmas in the tree have an ov074 NAME as their LHS. An
# /alternatename is defeated the moment its LHS is DEFINED, which is exactly
# what this lane's mount and slice now do. The wave-5 R1/R2 recipe, the same
# one the ov036 block above applies: the reader gets a per-source -D onto the
# name it actually means, and the now-inert directive is retired at its own
# site rather than left reading as live routing. Unfixed, neither would fail at
# link and neither would show in the byte gate -- the Coffin/Spindrift shape,
# caught by port/tools/alternatename_guard.py's post-link map check.
#
# (4a) src/func_ov071_021209c8.cpp spells its texture-sequence SharedFilePtr
#      `data_ov074_02123038`. ov071's own relocation from 0x02120a1c is
#      module:overlay(71), so it means ov071's bss -- the ruling
#      hal/actor_classes_ov071.cpp already recorded, unchanged. ov074's own
#      relocations cite the same address three times as module:overlay(74) and
#      its sinit Constructs it with file id 0x38f, so this mount defines the
#      name for real. The TU includes no headers, so the rename is direct.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov071_021209c8.cpp"
    PROPERTIES COMPILE_DEFINITIONS "data_ov074_02123038=data_ov071_02123038")
# (4b) src/_ZN3Amp16CleanupResourcesEv.c reads
#      `struct SharedFilePtr *func_ov074_021222e0[2]` -- ov070's own
#      two-pointer file table at the same window address -- under the ov074
#      FUNCTION spelling. hal/cxx_aliases.cpp's own comment predicted this
#      collision word for word. ov074's function there is real (0xdc bytes) and
#      Goomboss::Render calls it, so the name has to resolve to CODE now.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN3Amp16CleanupResourcesEv.c"
    PROPERTIES COMPILE_DEFINITIONS "func_ov074_021222e0=data_ov070_021222e0")


# ============================================================================
# RUN REL0215 WAVE 2, LANE CAST-SWEEP2 -- the four-overlay cast sweep.
#
# ov032 (24), ov034 (26), ov047 (39), ov055 (47), each landing as its own
# commit. This region grows by one block per overlay and is the ONLY place in
# this file the lane appends more than once. The three target source lists take
# ${SWEEP2_SOURCES} exactly once, so a second overlay costs the coordinator
# nothing outside this region -- the wave-1 cast-sweep1 shape, chosen for the
# same reason.
set(SWEEP2_SOURCES "")

# ---- 1/4: ov032, level 24 (Tiny-Huge Island, big side) ---------------------
# HUGE_COVER (106) x1, HUGE_WATER (107) x1 and BUBBA (228) x2 -- four of level
# 24's seven skipped instances. Thirty-eight of the overlay's forty-one TUs;
# the other three are BUBBA's PMF state-enter setter, its Behavior and its
# ModelAnim slot-5 Render, all three host-copied below. Two 32-slot Platform
# tables and one 31-slot Enemy table, and dsd's only vtable label sits twice on
# the wrong one. See port/ov032_syms.txt and port/slice_sweep2_ov032.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep2_ov032.txt" SLICE_SWEEP2_OV032_LINES)
set(SLICE_SWEEP2_OV032_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP2_OV032_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP2_OV032_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# id 106's deleting destructor: VT0 is the OWN table 0x021138e0, VT1 is
# Platform's base 0x0210ae38 and G0 is the game heap -- pool order read off the
# ROM body at 0x02112538/0x0211253c/0x02112540, and the ROM stores r2 (own)
# BEFORE r1 (base), which is the order the source already writes. Checked, not
# assumed: ov033's twin pair one overlay along had the two labels INVERTED.
# VT0/VT1 are DEFINED in this link (actor_vtables.cpp `int VT0[20]` and
# siblings), so an unrenamed store would land silently on a twenty-word shared
# dummy rather than failing at link -- the ov036 (1) reading.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov032_021124ec.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=data_ov032_021138e0;VT1=_ZTV10dBgActor_c;G0=data_020a0eac")
# id 107's deleting destructor spells both its tables by name and needs only the
# heap: its pool at 0x02112740 is 0x020a0eac module:main. Its D1 twin needs
# nothing at all.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9HugeCoverD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS "G0=data_020a0eac")
# id 107's CleanupResources releases two SharedFilePtrs as G0/G1, and its own
# pool names them: 0x02112780 -> 0x02113afc then 0x02112784 -> 0x02113af4, both
# module:overlay(32), both in this lane's own mount. Unrenamed, G0 resolves
# through `/alternatename:_G0=_data_020a0eac` and would Release the GAME HEAP,
# and G1 would not link. Renamed straight to the mount names: decl_common.h
# declares neither, so there is nothing to conflict with (the ov036 (2)
# reading).
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN9HugeCover16CleanupResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "G0=data_ov032_02113afc;G1=data_ov032_02113af4")
# THE WRONG-NAME AT THE SHARED WINDOW (T6). BUBBA's state-2 tick calls a
# function it spells func_ov047_02111254 -- OV047's name for the same
# shared-window address -- and OV047 IS SEATED BY THIS SAME LANE two commits
# later, so the name would resolve to Bowser-in-the-Sky's body. relocs.txt
# resolves the ROM's own arm_call at 0x02111d84 to 0x02111254
# module:overlay(32), which is func_ov032_02111254. decl_common.h declares
# neither spelling, so the rename is direct and needs no private name.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov032_02111d7c.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_ov047_02111254=func_ov032_02111254")
list(APPEND SWEEP2_SOURCES hal/actor_classes_ov032.cpp ${SLICE_SWEEP2_OV032_SOURCES}
    unmatched/Bubba_StateEnter.cpp unmatched/Bubba_Behavior.cpp
    unmatched/Bubba_Render.cpp unmatched/Bubba_ChaseGate.cpp)

# ---- 2/4: ov034, level 26 (Tiny-Huge Island's cave) ------------------------
# WIGGLER (248) x1 -- level 26's WHOLE skip list, so this takes the level to
# zero skipped. Thirty-two of the overlay's thirty-six TUs: one has no matched
# body at all (_ZN7Wiggler8BehaviorEv, faced loudly) and three are host copies
# (the two pointer-to-member state dispatch sites and the five-ModelAnim
# slot-5 Render). One 31-slot Enemy table, and no identity shift. See
# port/ov034_syms.txt and port/slice_sweep2_ov034.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep2_ov034.txt" SLICE_SWEEP2_OV034_LINES)
set(SLICE_SWEEP2_OV034_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP2_OV034_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP2_OV034_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# THE WRONG-NAME AT THE SHARED WINDOW (T6), the same rename twice. State 9
# DAMAGE_WAIT's tick and state 6 DAMAGETALK's tick both CALL a function they
# spell data_ov052_021125b8 -- an OV052 name, and a DATA-kind one, for this
# overlay's own state setter. The ROM's own arm_calls at 0x021115b0,
# 0x021119ec and 0x02111a00 all resolve to 0x021125b8 module:overlay(34) =
# func_ov034_021125b8. OV052 IS MOUNTED (PORT_ACTOR_OVERLAYS), so the ov052
# spelling resolves and both bodies would call into ov052's DATA -- no link
# error, no byte-gate signal, a jump into a data array at run time. Renamed
# straight to the ov034 spelling: decl_common.h declares neither name.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov034_02111588.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov034_021119ac.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov052_021125b8=func_ov034_021125b8")
# AN ALIAS THIS MOUNT DEFEATS, AND ITS ONE LIVE READER (the ov036 (7) shape).
# ov014, ov015, ov021, ov022 and ov034 share the level-overlay base 0x021111a0,
# and ov034's dsd export won the naming race at 0x02114538, so
# src/_ZN14KnockDownPlank13InitResourcesEv.cpp:30 (ov015's KNOCK_DOWN_PLANK,
# LINKED via port/slice_gate59.txt:93) reaches OV015's OWN collider-file table
# under ov034's spelling. hal/actor_classes_wf.cpp routed that with
# `/alternatename:_data_ov034_02114538=_data_ov015_02114538`, and an
# /alternatename is defeated the moment its LHS is DEFINED -- which is exactly
# what this lane's mount now does: data_ov034_02114538 is the Wiggler's
# ELEVEN-ENTRY STATE TABLE. Unfixed, KNOCK_DOWN_PLANK on level 7 would load its
# collider file out of the Wiggler's state machine: no link error, no byte-gate
# signal, wrong object at run time -- the Coffin/Spindrift shape, caught here by
# port/tools/alternatename_guard.py's post-link map check, which is where this
# lane found it. The now-inert directive is retired at its own site.
# THE SECOND DEFEATED ALIAS NEEDED NO REPLACEMENT:
# `/alternatename:_data_ov034_021147ec=__ZTV10ChainChomp` in
# hal/bob_enemy_bridges.cpp had NO reader left -- src/_ZN10ChainChompD1Ev.c and
# src/_ZN10ChainChompD0Ev.cpp both spell _ZTV10ChainChomp directly today -- so
# it is deleted rather than re-routed.
# A PRIVATE NAME, not a direct rename: include/decl_common.h declares BOTH
# spellings, at line 224 `extern char data_ov015_02114538[]` and at line 1146
# `extern int data_ov034_02114538[]`, so a direct -D makes one identifier carry
# two basic types in one TU and MSVC refuses it (C2371). That is the ov100 C2040
# reading. The private name is declared and never defined, and
# hal/actor_classes_ov034.cpp aliases it onto the mounted ov015 symbol.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN14KnockDownPlank13InitResourcesEv.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov034_02114538=port_ov015_kdp_clsn_files")
list(APPEND SWEEP2_SOURCES hal/actor_classes_ov034.cpp ${SLICE_SWEEP2_OV034_SOURCES}
    unmatched/Wiggler_StateEnter.cpp unmatched/Wiggler_StateTick.cpp
    unmatched/Wiggler_Render.cpp)

# ---- 3/4: ov047, level 39 (Bowser in the Sky) ------------------------------
# RICKSHAW_BS (152) x3, STAIRS_BS (153) x1 and ROTATING_PLATFORM_BS (156) x3 --
# seven of level 39's nine skipped instances -- plus RICKSHAW_PLATFORM_BS (151),
# which is not placed on this level and is registered because both attribution
# routes name it. ALL TWENTY-FOUR of the overlay's TUs, no host copy, no face
# and no refusal, plus EIGHT ov002 bodies that are the measured external
# closure. Four 32-slot Platform tables, and dsd's two vtable names are each
# attached ONE TABLE LATE. See port/ov047_syms.txt and
# port/slice_sweep2_ov047.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep2_ov047.txt" SLICE_SWEEP2_OV047_LINES)
set(SLICE_SWEEP2_OV047_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP2_OV047_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP2_OV047_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
# NINE PER-SOURCE BINDINGS, and every one is settled from the TU's OWN literal
# pool in config/arm9/overlays/ov047/relocs.txt, disassembled out of
# extracted/overlays/overlay_0047.bin, never by name.
#
# THE RECOVERED SOURCE USES THE RTTI-DERIVED VTABLE NAMES AND GETS THE CLASS
# RIGHT EVERY TIME -- but in the three factories it gets the ORDER backwards.
# The ROM stores the ov002 INTERMEDIATE BASE first and the class's OWN table
# second, which is the correct constructor order:
#   02111298 ldr r1,[pc,#0x14] -> 0x021112b4 -> 0x02109320   the base
#   0211129c ldr r0,[pc,#0x14] -> 0x021112b8 -> 0x021122a0   its own table
#   021112a0 str r1,[r4]   021112a4 str r0,[r4]
# while src/RickshawBs_Spawn.c writes `p[0] = _ZTV21daObjKm3_Kurumajiku_c;`
# FIRST and `p[0] = VT1;` second. So in the three factories the RTTI name means
# the BASE and VT1 means the own table; in the six destructors the same RTTI
# name means the OWN table. Both readings are bound per source. Left alone the
# factories would install the abstract intermediate LAST, and every one of these
# actors would dispatch InitResources through a table whose slot 0 is ZERO in
# the ROM -- a null call on the first frame, invisible to both the byte gate and
# the linkage count.
#
# _ZTV10dBgActor_c IS THE OTHER TRAP, the ov036 (3) one: three destructors spell
# their ov002 INTERMEDIATE base that way, and the port defines _ZTV10dBgActor_c
# as the PLATFORM base (0x0210ae38). Unrenamed those bodies would store the
# Platform table where the ROM stores the intermediate.
# id 153's five TUs need no rename at all: they spell _ZTV18RickshawPlatformBs,
# data_ov002_02108d94 and _ZTV8Platform, which are the three addresses the ROM
# writes, in the ROM's own order.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov047_021111a0.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV21daObjKm3_Kurumajiku_c=data_ov047_021122a0;_ZTV10dBgActor_c=data_ov002_02109320;VT2=_ZTV8Platform")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov047_021111f0.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=data_ov047_021122a0;VT1=data_ov002_02109320;VT2=_ZTV8Platform;G0=data_020a0eac")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/RickshawBs_Spawn.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV21daObjKm3_Kurumajiku_c=data_ov002_02109320;VT1=data_ov047_021122a0")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10RickshawBsD1Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV20daObjKm3_Kaitendai_c=_ZTV10RickshawBs;_ZTV10dBgActor_c=data_ov002_021091d4;VT2=_ZTV8Platform")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN10RickshawBsD0Ev.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV20daObjKm3_Kaitendai_c=_ZTV10RickshawBs;_ZTV10dBgActor_c=data_ov002_021091d4;VT2=_ZTV8Platform;G0=data_020a0eac")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov047_021113bc.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV20daObjKm3_Kaitendai_c=data_ov002_021091d4;VT1=_ZTV10RickshawBs")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov047_021113f8.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV17daObjKm3_Kuruma_c=data_ov047_0211244c;_ZTV10dBgActor_c=data_ov002_02109278;VT2=_ZTV8Platform")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov047_02111448.c"
    PROPERTIES COMPILE_DEFINITIONS
    "VT0=data_ov047_0211244c;VT1=data_ov002_02109278;VT2=_ZTV8Platform;G0=data_020a0eac")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/RickshawPlatformBs_Spawn.c"
    PROPERTIES COMPILE_DEFINITIONS
    "_ZTV17daObjKm3_Kuruma_c=data_ov002_02109278;VT1=data_ov047_0211244c")
# FOUR WRONG-NAMES AT THE SHARED WINDOW (T6), all in one sinit. STAIRS_BS's file
# constructor spells FOUR of its own .bss symbols with an OV052 tag, and ov052
# IS MOUNTED (PORT_ACTOR_OVERLAYS), so all four resolve: the sinit would
# construct three SharedFilePtrs and register one destructor chain node in
# MARIO'S KEY COURSE's storage instead of its own. The ROM's own pool words at
# 0x02111958, 0x02111970, 0x0211197c and 0x0211196c all resolve to
# module:overlay(47). decl_common.h declares none of the eight names, so the
# renames are direct.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/__sinit_ov047_0211181c.c"
    PROPERTIES COMPILE_DEFINITIONS
    "data_ov052_02112680=data_ov047_02112680;data_ov052_02112688=data_ov047_02112688;data_ov052_02112690=data_ov047_02112690;data_ov052_021126a8=data_ov047_021126a8")
list(APPEND SWEEP2_SOURCES hal/actor_classes_ov047.cpp ${SLICE_SWEEP2_OV047_SOURCES})

# ---- 4/4: ov055, level 47 (Luigi's key arena) ------------------------------
# MIRROR_LUIGI (195) x1 -- level 47's WHOLE skip list, so this takes the level
# to zero skipped. Ten of the overlay's thirteen TUs: two are the
# pointer-to-member dispatch sites and one is the //cpp real-destructor shape,
# hand-spelled in the seat. One 31-slot ACTOR table (not Platform), no identity
# shift, and the ONLY overlay in this lane that needs no per-source -D at all.
# See port/ov055_syms.txt and port/slice_sweep2_ov055.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_sweep2_ov055.txt" SLICE_SWEEP2_OV055_LINES)
set(SLICE_SWEEP2_OV055_SOURCES "")
foreach(line IN LISTS SLICE_SWEEP2_OV055_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_SWEEP2_OV055_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(APPEND SWEEP2_SOURCES hal/actor_classes_ov055.cpp ${SLICE_SWEEP2_OV055_SOURCES}
    unmatched/MirrorLuigi_StateEnter.cpp unmatched/MirrorLuigi_Behavior.cpp)
# ---- end lane cast-sweep2 --------------------------------------------------

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

# HOST-ABI RETIREMENT: Player::InitResources, the host copy that existed only
# because hostgen could not read one asm block, now that it can.
#
# The file said so in its own header, and it was the vacuous-pass shape rather
# than a link failure: the hand copy compiled, linked and ran, and only a
# reader comparing it line by line with the matched source could tell whether
# it had drifted.
#
# The match note explains the block -- it is there to force mwccarm's CLEAR
# codegen shape (5 x 4 strb post-inc), not to compute anything, and what it
# computes is a 20-byte zero over a fresh allocation. hostgen's ASM_EXCISION
# substitutes the C semantics, per-symbol and exact-text, so the day the
# source moves the build stops rather than silently substituting C for
# instructions nobody re-read.
#
# --extern-data because the TU defines a dozen data_<hex> latches at file
# scope that hal/level_boot.cpp and the dsstate section own.
#
# ActorBase::AfterCleanupResources, the OTHER file this retirement was written
# for, turned out not to need a block of its own: gate 9 was already hostgen'ing
# that symbol for the actor smokes, just without --extern-data. See GATE9_SYMS.
set(HOSTABI_RETIRE_SYMS _ZN6Player13InitResourcesEv)
set(HOSTABI_RETIRE_GEN "")
foreach(sym IN LISTS HOSTABI_RETIRE_SYMS)
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
    list(APPEND HOSTABI_RETIRE_GEN "${sym_out}")
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
    func_ov002_020ec410
    # Lane lk4 seat 3: Heap::ResizeToFit is a virtual shadow class inside a
    # .c -- the same case again.
    _ZN4Heap11ResizeToFitEv)
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

# run linkw wave C, lane cat-2d, slice 2d1. One TU: src/func_ov004_020adc4c.c,
# the only ov004 body a LINKED arm9 caller reaches (src/LoadFont.cpp's
# data_0209d698 == 2 arm calls it twice). It reads one ov004 BSS word,
# data_ov004_020beb60, and hal/oam2d_ov004_bss.cpp used to host that word by
# hand -- which is what retired the PORT_HOST_ABI printf stand-in for it in
# hal/sub_screen.cpp. The ov004 mount now hosts the word itself, from the same
# ROM span and with the same four zero bytes, so the hand-hosted file is gone
# (run link60 lane s2-m46). The slice still ships on exactly the three targets
# that link hal/sub_screen.cpp, and all three carry the mount. Why the slice is
# one line and not 276: port/ov004_ov007_2d_map.txt.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_2d1.txt" SLICE_2D1_LINES)
set(SLICE_2D1_SOURCES "")
foreach(line IN LISTS SLICE_2D1_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_2D1_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# run mg9 lane S364: dScMgSlot1_c, actor id 0x16c = scene 364. The ROM gives
# this id no Mg*_SpawnInfo symbol and no naming import has ever named it, so
# the class name is the ROM's own RTTI, reached through the type_info the word
# BEFORE the vtable points at: data_ov006_0213eb40[-1] is 0x0213e5a0, whose
# name pointer 0x0213e5bc reads "12dScMgSlot1_c".
#
# THE WIDTH IS THE REASON THIS BLOCK IS WORTH READING. port/mg_fanout_costs.txt
# section 11 names 0x16c by hand as the second class whose phantom slot 36 is
# LIVE DATA BELONGING TO A SHIPPED GAME: index 36 of this table is
# MgBingoBallSlotsShot_SpawnInfo's factory word, and scene 376 is seated in
# this tree. hal/scene_mg_slot1.cpp fills 36 and port/slice_s364.txt section 3
# carries all four checks -- span, terminal slot, what the word is, and the
# relocation count.
#
# ONE VARIABLE FOR THE SLICE, for the reason the CCN and PPP blocks state.
# hal/scene_boot.cpp's port_scene_classes table carries the scene-364 row, so
# every target that compiles scene_boot.cpp references port_mg_slot1_spawn and
# port_scene_fill_slot1 by name -- the same coupling every minigame row has.
#
# NO STATE DISPATCHER HOST COPY AND NO stategen OUTPUT. This class has no mwcc
# pointer-to-member state machine: its own machine is a fourteen-arm jump-table
# switch, no overlay constructor builds a pair table for it, and a `::*` sweep
# over every TU in the slice is clean. That is the second counterexample to
# section 4's "the wall is the same one for all thirty", after the flower.
#
# ONE TU BUILDS FROM A HOSTGEN COPY. src/func_ov006_0210d1fc.cpp is slot 0,
# InitResources, and it does thirty-seven literal-address accesses in the
# 0x04xxxxxx window -- both engines' DISPCNT and BGxCNT at 0x0400000c /
# 0x0400000e / 0x04001008 / 0x0400100a / 0x0400100c / 0x0400100e, BLDCNT at
# 0x04000050 and the sub-engine blend at 0x04001050. Compiled straight from
# src/ those land in the memory ntr maps across the I/O window and stop there.
# src/ itself is never edited; the hostgen copy stands in for it and the
# substitution asserts rather than hopes, exactly as the FLW block's does.
set(S364_HOSTGEN_SYMS func_ov006_0210d1fc)
set(S364_HOSTGEN_GEN "")
set(S364_HOSTGEN_SKIP "")
foreach(sym IN LISTS S364_HOSTGEN_SYMS)
    if(EXISTS "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.c")
        list(APPEND S364_HOSTGEN_SKIP "src/${sym}.c")
    else()
        set(sym_src "${PORT_REPO_ROOT_ABS}/src/${sym}.cpp")
        list(APPEND S364_HOSTGEN_SKIP "src/${sym}.cpp")
    endif()
    set(sym_out "${CMAKE_BINARY_DIR}/host-src/src/${sym}.cpp")
    add_custom_command(OUTPUT "${sym_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
                --out "${CMAKE_BINARY_DIR}/host-src" ${sym}
        DEPENDS "${sym_src}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/hostgen.py"
        COMMENT "hostgen ${sym}")
    list(APPEND S364_HOSTGEN_GEN "${sym_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_s364.txt" SLICE_S364_LINES)
set(SLICE_S364_SOURCES ${S364_HOSTGEN_GEN})
foreach(line IN LISTS SLICE_S364_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    # The hostgen'd copy stands in for the src/ one; taking both would be a
    # duplicate symbol, and taking neither would drop the body silently.
    if(line IN_LIST S364_HOSTGEN_SKIP)
        continue()
    endif()
    list(APPEND SLICE_S364_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
list(LENGTH S364_HOSTGEN_SYMS s364_hg_want)
list(LENGTH S364_HOSTGEN_GEN s364_hg_have)
if(NOT s364_hg_want EQUAL s364_hg_have)
    message(FATAL_ERROR "slice_s364 hostgen substitution incomplete: "
            "${s364_hg_have} of ${s364_hg_want} generated")
endif()
foreach(skip IN LISTS S364_HOSTGEN_SKIP)
    if(NOT skip IN_LIST SLICE_S364_LINES)
        message(FATAL_ERROR "slice_s364.txt no longer lists ${skip}; the "
                "hostgen substitution for it is now dead and the display "
                "control writes in it would go back to latching silently")
    endif()
endforeach()

# TWO NAME-SPELLING DEFECTS, BOTH SECTION 6's FOURTH KIND, BOTH RESOLVED OFF
# THE ROM RATHER THAN OFF THE NAME. The first link wave asked for seven
# externals and all seven are decomp-side spellings of DATA symbols under a
# func_ prefix. Per-source -D and not /alternatename, for lane A2's reason: the
# right-hand symbols are DEFINED in this binary by the ov004 mount, and an
# alias with a defined symbol on the left is defeated silently.
#
# (a) THE SIX ov004 WINDOW WORDS slot 0 writes -- 0x80, 0x40, 0x80, 0x40, 0x80,
#     0x40 into 0x020bc88c, _020bc860, _020bc878, _020bc890, _020bc8b8 and
#     _020bc8b4. include/decl_common.h declares all six as `extern int
#     func_020bcXXX;`, config names every one of them data_ov004_020bcXXX at
#     four bytes each, and port/ov004_syms.txt already mounts the whole run.
#     port/mg_fanout_costs.txt section 12 records dScMgCup_c's InitResources
#     writing "the six ov004 window words" as well, so this rename is the
#     family's and not this class's.
set_source_files_properties(
    "${CMAKE_BINARY_DIR}/host-src/src/func_ov006_0210d1fc.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_020bc860=data_ov004_020bc860;func_020bc878=data_ov004_020bc878;func_020bc88c=data_ov004_020bc88c;func_020bc890=data_ov004_020bc890;func_020bc8b4=data_ov004_020bc8b4;func_020bc8b8=data_ov004_020bc8b8")

# (b) THE SUB-OBJECT'S BASE VTABLE, and this one had to be ADJUDICATED BETWEEN
#     TWO OVERLAYS rather than looked up. The factory and both destructors
#     store 0x020ad494 into this+0x4660 and then store the betIcon table over
#     it -- a base-then-derived vptr pair. 0x020ad494 is `ambiguous` in config:
#     ov000 and ov001 share the base 0x020aa420 and both name a symbol there.
#     hal/scene_mg_faces.cpp already hosts the ov000 reading, a 24-byte
#     filename string "thum_shinkei_ncg.bin", AND THAT IS THE WRONG MODULE FOR
#     THIS CLASS. The ROM decides it:
#
#       overlay_0001.bin at 0x020ad478 is a type_info record {0x0209a774,
#       0x020ad480} and 0x020ad480 reads "9dThIcon_c";
#       0x020ad490 holds 0x020ad478, i.e. that type_info, which is the word
#       before a vtable;
#       0x020ad494 and 0x020ad498 hold 0x020ab550 and 0x020ab54c, a two-slot
#       table -- and src/func_ov006_0210c410.c, the DERIVED betIcon slot 0,
#       calls func_ov001_020ab550 directly, which is the base calling
#       convention showing through;
#       overlay_0000.bin at the same address is the middle of the string
#       "...ogezo/togezo.bmd", which is not a type_info and not a table.
#
#     So dScMgSlot1_c::betIcon_c derives from dThIcon_c in ov001. The ov001
#     per-symbol mount covers 0x020ab800..0x020abb00 only (port/ov001_syms.txt
#     says why), so the table is HOSTED in hal/scene_mg_slot1.cpp with faces
#     for its two ROM bodies rather than mounted, which is the same treatment
#     and the same reasoning hal/scene_mg_faces.cpp gives for the ov000
#     reading of this very address. Expanding the ov001 mount instead would be
#     the wrong trade for two words and would put a raw DS code address in the
#     table anyway.
foreach(s364_thicon_tu
        "${PORT_REPO_ROOT_ABS}/src/func_ov006_0210a8c0.c"
        "${PORT_REPO_ROOT_ABS}/src/func_ov006_0210a900.cpp"
        "${PORT_REPO_ROOT_ABS}/src/func_ov006_0210d6b8.cpp")
    set_source_files_properties("${s364_thicon_tu}"
        PROPERTIES COMPILE_DEFINITIONS
        "func_020ad494=data_ov001_020ad494")
endforeach()

set(SCENE_MG_SLOT1_SOURCES hal/scene_mg_slot1.cpp)

# Run mg11 lane MUG: dScMgSlot3_c, the "Mix-a-Mug" slot machine, actor id
# 0x16d = scene 365, and the NINTH class the port seats under
# dScMgSingle3DBase_c (eight at base; port/slice_mug.txt has the sweep that
# counts them).
#
#   Scene -> dScMgBase_c (ov004 0x020bc0c0) -> dScMgSingle3DBase_c
#         (ov006 0x0213e448) -> dScMgSlot3_c (ov006 0x0213eaa8)
#
# both new tables 36 slots wide and checked FIVE ways before a word was written
# (port/slice_mug.txt): the span, the terminal slot, what the words past the
# end are, the relocation count inside the span, and the relocation stride.
# THE SPAN READS 38 AND THE WIDTH IS 36, which port/mg_fanout_costs.txt line
# 1287 already resolved; what the slice adds is the mechanism, and it names a
# live victim -- the two extra words are the {0, &typeinfo} RTTI HEADER of
# dScMgSlot1_c's vtable, so a 38-slot fill here would write a host thunk over
# scene 364's typeinfo pointer. The ROM gives this id no spawn symbol at all,
# so the whole identity -- class name, hierarchy and player title -- is derived
# from the ROM in that file's header.
#
# THE WALL IS THE ORDINARY TABLE FORM AND IT IS PAID ONCE. Eight states in
# data_ov006_02142bdc, filled by __sinit_ov006_02132894 and dispatched from
# vtable slot 6 at stride eight with arity 0. unmatched/MgSlot3_StateDispatch.cpp
# host-copies func_ov006_0210bcb0 and routes the dispatch through an address
# switch, so src/func_ov006_0210bcb0.cpp is OUT of slice_mug.txt: listing both
# copies would be a duplicate symbol, and listing neither would drop the whole
# state machine silently. A word-by-word decode of the class's code block finds
# exactly THREE Itanium-sequence instructions and all three are in that one
# body, so there is no second dispatcher and no open-coded third shape.
#
# THE ONE BODILESS STATE IS RETIRED (run mg12, lane MAM). func_ov006_0210adac
# (slot 2, 0x1b8, the face-evaluation step after the third reel stops) had a
# config symbol, no delink block and no src in any module, so mg11 left it out
# of the slice and invented no definition for it. It is now byte-matched:
# src/func_ov006_0210adac.c is in slice_mug.txt, the delink block covers
# 0x0210adac..0x0210af64, and unmatched/MgSlot3_StateDispatch.cpp calls it. The
# per-slot census still prints the former floor counter so an mg11 log and an
# mg12 log compare field for field.
#
# NO HOSTGEN SUBSTITUTION, for the reason the S381 block above gives: the two
# bodies in this class that dereference a 0x04xxxxxx literal
# (src/func_ov006_0210bdb0.cpp's BG0CNT/BG1CNT/BG2CNT/BG3CNT writes and
# src/func_ov006_0210aa60.cpp's BG1CNT read-modify-writes) are ordinary PPU
# control registers the display reads back out of the memory ntr maps there.
# The write-TRIGGERED registers this class touches are all inside
# func_ov006_0210a708, which comes in through FLW_HOSTGEN_GEN already
# substituted.
#
# ONE VARIABLE, for the reason the flower and S381 blocks give: the slice and
# the .cpp are not separable. hal/scene_boot.cpp's port_scene_classes table
# carries the scene-365 row, so every target that compiles scene_boot.cpp
# references port_mg_slot3_spawn and port_scene_fill_slot3 by name.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_mug.txt" SLICE_MUG_LINES)
set(SLICE_MUG_SOURCES hal/scene_mg_slot3.cpp
                      unmatched/MgSlot3_StateDispatch.cpp
                      unmatched/MgSlot3_SubObjCtor.cpp
                      unmatched/MgSlot3_SubObjUpdate_020c1f4c.cpp)
foreach(line IN LISTS SLICE_MUG_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_MUG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# THE SIX ov004 WINDOW WORDS AGAIN, and this is the second class to write them.
# src/func_ov006_0210bdb0.cpp (slot 0 InitResources) assigns eight ov004 words
# spelled with a func_ prefix: include/decl_common.h declares all eight as
# `extern int func_020bcXXX;` while config names each of them
# data_ov004_020bcXXX, and port/ov004_syms.txt mounts the whole run. Per-source
# -D and not /alternatename, for the reason the S364 block below the SLOT1
# sources gives: the right-hand symbols are DEFINED in this binary by the ov004
# mount. Six of the eight are the same run dScMgSlot1_c's InitResources and
# dScMgCup_c's write; the other two (0x020bc864 and 0x020bc888) already have
# /alternatename rows in unmatched/MgLuigi_Faces.cpp and
# unmatched/MgSmartball_Faces.cpp, and are renamed here too rather than left to
# those -- one mechanism per TU is what makes "this TU spells eight ov004 data
# words under a func_ prefix" a checkable sentence. Those two alias rows keep
# their own callers and are untouched.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov006_0210bdb0.cpp"
    PROPERTIES COMPILE_DEFINITIONS
    "func_020bc860=data_ov004_020bc860;func_020bc864=data_ov004_020bc864;func_020bc878=data_ov004_020bc878;func_020bc888=data_ov004_020bc888;func_020bc88c=data_ov004_020bc88c;func_020bc890=data_ov004_020bc890;func_020bc8b4=data_ov004_020bc8b4;func_020bc8b8=data_ov004_020bc8b8")

# Run mg11 lane RLT: dScMgRoulette_c, actor id 0x17f = scene 383, "Mushroom
# Roulette", and the FIFTH class the port seats under dScMgSingle3DBase_c.
#
#   Scene -> dScMgBase_c (ov004 0x020bc0c0) -> dScMgSingle3DBase_c
#         (ov006 0x0213e448) -> dScMgRoulette_c (ov006 0x0213e39c)
#
# both new tables 36 slots wide and checked FOUR ways before a word was written
# (port/slice_rlt.txt): span, terminal slot, what the word past the end is, and
# the relocation count inside the span with no row at index 36. The class name
# is the ROM's own type_info, "15dScMgRoulette_c" at 0x0213e30c, reached through
# the relocation the word before the vtable carries.
#
# THREE HOST COPIES, AND EACH IS A DIFFERENT DEFECT:
#
#   MgRoulette_Factory.cpp        REPLACES src/MgMushroomRoulette_Spawn.cpp. The
#     SECOND AND LAST factory in the family that calls func_ov004_020b2adc with
#     no argument -- port/unmatched/MgCup_Factory.cpp names this exact TU as the
#     other one. On ARM r0 still holds the object after `movs r4,r0`; on the
#     host the callee reads [esp+4] and writes three vtable words through it.
#     The displacement ruling is port/mg_fanout_costs.txt section 12's, spent
#     here for the other id that paragraph names.
#   MgRoulette_WheelDispatch.cpp  REPLACES src/func_ov006_02107db8.cpp, which
#     carries TWO defects in eleven lines: the class's ONE mwcc member-pointer
#     dispatch (pair at offset 0 of the wheel object at +0x530c, universe of
#     exactly two code words bounded from the relocation set) and a local
#     shadow-class call of ROM vtable slot 3 on a HOST Model.
#   MgRoulette_ModelRender_02107d80.cpp  REPLACES src/func_ov006_02107d80.cpp,
#     the same shadow defect at ROM slot 5, twice, on the DRAW path. A separate
#     file because the two failures look nothing alike and only one of them is
#     what a rendered frame depends on.
#
# All three src TUs are therefore OUT of port/slice_rlt.txt; listing both copies
# would be an LNK2005 and listing neither would drop the body silently. The
# assertion below is here rather than at the link because a duplicate-symbol
# wall reports far from its cause.
#
# NO LANGUAGE CXX ROW AND NO PER-SOURCE -D ROW, and both are sweeps rather than
# absences. All 41 TUs in the class's code block were scanned for the //cpp
# marker and for extern "C", virtual, class and namespace before the first
# build: the six hits are all already .cpp files (02107920, 02107a6c, 02107d80,
# 02107db8, 0210a194 and the factory), so CMake compiles each as C++ on its
# extension and no .c path in this slice is really C++. A scan of every source
# this seat adds for the bare placeholders VT, VT0, HEAP, G0, G1, G2, R0, R1 and
# R2 returns nothing.
#
# NO HOSTGEN SUBSTITUTION EITHER, and it is a decision rather than an omission,
# the same one the S381 block above records. The one body of this class's chain
# that writes a WRITE-TRIGGERED register is func_ov006_0210a708 (slot 33,
# LIGHT_COLOR at 0x040004cc), and that comes in already substituted through
# FLW_HOSTGEN_GEN. Slot 0's own writes -- 0x0400100c, 0x04001008 and 0x04000008
# -- are ordinary PPU control registers the display reads back out of the memory
# ntr maps there.
#
# ONE VARIABLE, for the reason the flower, fader and S381 blocks give: the slice
# and the .cpp are not separable. hal/scene_boot.cpp's port_scene_classes table
# now carries the scene-383 row, so every target that compiles scene_boot.cpp
# references port_mg_roulette_spawn and port_scene_fill_roulette by name.
#
# THE SLICE IS THE DELTA ONLY. slice_mg1.txt carries the ov004 framework, the
# thirty-five overlay constructors and the arm9 waves under them; slice_mem.txt
# carries the +0x4f38 model sub-object family this class shares with
# dScMgMemory2_c (including MgMemory2_FieldPmf.cpp, which routes the field
# member pointer this class's slot 6 dispatches every frame); slice_flw.txt and
# slice_mga361.txt carry the eight dScMgSingle3DBase_c bodies. CMake appends
# every list to the same targets and dedupes identical absolute paths, so
# nothing is compiled twice.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_rlt.txt" SLICE_RLT_LINES)
set(SLICE_RLT_SOURCES hal/scene_mg_roulette.cpp
                      unmatched/MgRoulette_Factory.cpp
                      unmatched/MgRoulette_WheelDispatch.cpp
                      unmatched/MgRoulette_ModelRender_02107d80.cpp)
foreach(line IN LISTS SLICE_RLT_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_RLT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
foreach(dup IN ITEMS src/MgMushroomRoulette_Spawn.cpp src/func_ov006_02107db8.cpp
                     src/func_ov006_02107d80.cpp)
    if(dup IN_LIST SLICE_RLT_LINES)
        message(FATAL_ERROR
            "slice_rlt.txt lists ${dup}, which port/unmatched/ host-copies. "
            "Both would define the same symbol (LNK2005). Remove the slice line.")
    endif()
endforeach()

# run rel0215 wave 2 (lane cast-lvl16pair, sub-job 1 of 2): TOX_BOX (309),
# ov092. Fresh per-symbol ov092 mount (port/ov092_syms.txt). The 32-slot
# Platform fill, the nine-state PMF seat and the two method faces are
# hal/actor_classes_ov092.cpp; the ONE host-copied TU (the pointer-to-member
# dispatch site in ToxBox::Behavior, where MSVC's unknown-inheritance form is
# 16 bytes against the ROM's 8 and the array stride breaks with it) is
# unmatched/ToxBox_HostSites.cpp, the refusal itemised in port/slice_ov092.txt.
# ToxBox::Render is deliberately NOT refused: its six-virtual local shadow is
# over a plain Model at +0xd4, which cxxname_bridge.cpp:511 dual-fills, not
# over a ModelAnim -- the ov017 case, not the ov077 one.
# unmatched/ToxBox_ShadowFaces.cpp carries the slice's MEASURED closure gap,
# taken off the first link's own unresolved list rather than guessed: ONE
# symbol, ?GetNode@PathPtr@@QBEHAAUVector3@@I@Z, a naming mismatch against the
# already-linked flat _ZNK7PathPtr7GetNodeER7Vector3j, not a missing body.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov092.txt" SLICE_OV092_LINES)
set(SLICE_OV092_SOURCES "")
foreach(line IN LISTS SLICE_OV092_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV092_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
if("src/_ZN6ToxBox8BehaviorEv.cpp" IN_LIST SLICE_OV092_LINES)
    message(FATAL_ERROR
        "slice_ov092.txt lists src/_ZN6ToxBox8BehaviorEv.cpp, which "
        "port/unmatched/ToxBox_HostSites.cpp host-copies. Both would define "
        "_ZN6ToxBox8BehaviorEv (LNK2005). Remove the slice line.")
endif()

# run rel0215 wave 2 (lane cast-lvl16pair, sub-job 2 of 2): POKEY (240) +
# POKEY_SEGMENT (241, the same class) + TORNADO (308), ov096. Fresh per-symbol
# ov096 mount (port/ov096_syms.txt). The two 31-slot fills, the six-state PMF
# seat, the two RTTI-spelling bridges and the seven method faces are
# hal/actor_classes_ov096.cpp; the FIVE host-copied TUs (two real-C++
# destructors MSVC cannot name, the two pointer-to-member dispatch halves, and
# TORNADO's ModelAnim slot-5 Render) are the two unmatched/*_HostSites.cpp
# files, each refusal itemised in port/slice_ov096.txt.
# POKEY's Render is deliberately NOT refused even though it is the same
# six-virtual shadow shape: it dispatches on a plain Model at +0xd4, which
# cxxname_bridge.cpp:511 dual-fills, while TORNADO's is on a ModelAnim at
# +0x2c4, which cannot be. Two Renders in one overlay, one refused and one not.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_ov096.txt" SLICE_OV096_LINES)
set(SLICE_OV096_SOURCES "")
foreach(line IN LISTS SLICE_OV096_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_OV096_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()
foreach(dup IN ITEMS src/_ZN5PokeyD1Ev.cpp src/_ZN7TornadoD1Ev.cpp
                     src/_ZN7Tornado6RenderEv.cpp src/func_ov096_021368b4.cpp
                     src/func_ov096_021368f0.cpp src/func_ov096_02135e2c.cpp
                     src/_ZN7Tornado8BehaviorEv.cpp src/func_ov096_02137088.cpp
                     src/_ZN7Tornado13InitResourcesEv.cpp)
    if(dup IN_LIST SLICE_OV096_LINES)
        message(FATAL_ERROR
            "slice_ov096.txt lists ${dup}, which port/unmatched/ host-copies. "
            "Both would define the same symbol (LNK2005). Remove the slice line.")
    endif()
endforeach()
# src/func_ov096_021372c0.c is a .c FILE THAT CARRIES C++: it opens with //cpp
# and wraps its declarations in `extern "C" { ... }`, which as C is
# `error C2059: syntax error: 'string'` on line 2. It is the ONLY .c file in
# either of this lane's two overlays that does -- found by a comment-stripped
# token sweep that includes `extern "C"` and the //cpp marker, the form ov077's
# slice specifies and which a narrower first sweep in this lane missed. It
# needs no host copy, only the language: unlike src/func_ov096_02137088.cpp it
# includes no headers at all, so compiling it as C++ cannot reach the
# decl_common.h redeclaration that forced that one out of the slice.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_ov096_021372c0.c"
    PROPERTIES LANGUAGE CXX)

# The camera slice is on both binaries. The gate-13 State objects carry real
# DS function addresses, and Player::ChangeState already reaches
# func_0200d81c -> Camera::ChangeState on every state change, so the smoke
# needs the host dispatcher too or the first ChangeState calls a mwcc
# member-function pointer.
add_executable(smoke_player tests/smoke_player.cpp "${OV002DATA_C}" ${LEVEL_OV_DATA}
    # hal/sub_screen.cpp is in this target's list below, and since the screen
    # gap landed it asks hal/screen_gap.cpp for the stacked layout -- which in
    # turn asks hal/host_settings.cpp for the player's four gap keys. Neither
    # of those was in this harness before: it never read settings.json and
    # never composed a stacked image. They are here to close the link, and a
    # harness with no settings.json beside it gets every default.
    hal/screen_gap.cpp hal/host_settings.cpp hal/gap_art.cpp hal/gap_continuity.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/meshcolliderbase_vtable.cpp hal/meshcollider_dtor_seat.cpp hal/lk1_eh_vmax_seat.cpp hal/lk2_platform_dtor_seat.cpp hal/model_dtor_seat.cpp hal/lk4_eh_dtor_seat.cpp hal/lk4_solidheap_seat.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    ${SLICE_W8SHADOWS_SOURCES}
    # hal/megachar_stub.cpp is NOT here any more (lane w8-shadows). Its
    # `return 0` stand-in for Platform::UpdateKillByMegaChar is now compiled
    # only into the three narrow gate-9 harnesses above, which cannot build the
    # real closure. In this target the ROM's C name is the face at the bottom
    # of hal/wf_enemy_bridges.cpp and the body is the matched TU that has been
    # sitting in slice_gate16.txt line 178 all along, dropped by /OPT:REF for
    # want of a reference edge. Removing the stub from here is what makes the
    # matched body reachable; leaving it would be a duplicate definition.
    hal/cxxname_bridge.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/door_ring_faces.cpp hal/fader_wipes.cpp "${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/fader/_ZN9FaderWipe14LoadAndSetFileEt.cpp" hal/level_boot.cpp hal/stage_mods.cpp hal/scene_boot.cpp hal/title_doodle_probe.cpp hal/scene_actor_faces.cpp unmatched/Scene_AfterCleanupResources.cpp ${SLICE_SCENE1_SOURCES} ${SLICE_OV007_SOURCES} ${SC1_INTERIM_SOURCES} ${SLICE_MG1_SOURCES} ${SCENE_MG_SOURCES} ${SLICE_LUI_SOURCES} ${SLICE_PCH_SOURCES} ${SLICE_SMB_SOURCES} ${SCENE_SMB_SOURCES} ${SLICE_CCN_SOURCES} ${SLICE_PPP_SOURCES} ${SLICE_FLW_SOURCES} ${SCENE_MG_FLOWER_SOURCES} ${SLICE_MEM_SOURCES} ${SLICE_S381_SOURCES} ${SLICE_MMT_SOURCES} ${SLICE_LKT_SOURCES} ${SLICE_SOS_SOURCES} ${SLICE_S75_SOURCES} ${SCENE_MG_CURLING2_SOURCES} ${SLICE_PSY_SOURCES} ${SLICE_LKY_SOURCES} ${SLICE_SNW_SOURCES} ${SLICE_TTE_SOURCES} ${SLICE_BOO_SOURCES} ${SLICE_WIG_SOURCES} ${SLICE_S371_SOURCES} ${SLICE_BOX_SOURCES} ${SLICE_S364_SOURCES} ${SCENE_MG_SLOT1_SOURCES} ${SLICE_CUP_SOURCES} ${SLICE_MUG_SOURCES} ${SLICE_BNP_SOURCES} ${SLICE_BNT_SOURCES} ${SLICE_PKR_SOURCES} ${SLICE_PGO_SOURCES} ${SLICE_RLT_SOURCES} ${SLICE_TTI_SOURCES} ${SLICE_MGM_SOURCES} ${SLICE_VS_SOURCES} ${FDR_FADER_SOURCES} ${NFS_NAMES_SOURCES}
    # run link60 lane L2: hal/w8a_stage_storage.cpp is pure zero arm9 BSS and it
    # owns data_0209d4a8, which dScDSMT_c's InitResources and CleanupResources
    # both touch. walk_window already had it; smoke_player and walk_window_hires
    # did not, so it joins all three rather than the symbol being defined a
    # second time next to the one that already exists.
    hal/w8a_stage_storage.cpp hal/cannon_probe.cpp hal/message_boot.cpp hal/message_gen_text.cpp hal/message_compositor.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_classes_bob_world.cpp hal/actor_faces_bob.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/sub_screen.cpp ${SLICE_2D1_SOURCES} hal/sub_actors.cpp hal/stage_bridges.cpp "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN5Stage16OnPendingDestroyEv.cpp" hal/level_change.cpp hal/title_entry.cpp hal/star_flow.cpp hal/oam_lists.cpp hal/blend_vtable.cpp hal/particle_vtable.cpp hal/particle_bridges.cpp unmatched/Particle_RideThroughs.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE_PTR_SOURCES}
    # Stage 4, lane SD0: slot 0's closure. On all THREE targets rather than
    # walk_window alone, and not by preference: all three compile
    # hal/stage_bridges.cpp, which is where the Stage vtable and its slot-0
    # occupant live, so a face or a body that only one of them carried would
    # fail the other two at link. hal/stage_slot0.cpp holds the three
    # PORT_HOST_ABI bodies, the four C-name faces, two 4-byte arm9 BSS words
    # and the data_02075720 alias; the slice holds the ROM's own TUs.
    hal/stage_slot0.cpp ${SLICE_S4_SLOT0_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE_COMMS_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE_W1L1_SOURCES} ${SLICE_W1L2_SOURCES} ${SLICE_W1L3_SOURCES}
    ${SLICE_W1L4_SOURCES} ${SLICE_W1L5_SOURCES} ${SLICE_W5D_SOURCES} ${SLICE_W5E_SOURCES}
    ${SLICE_W6A_SOURCES}
    ${SLICE_W9HARVEST_SOURCES}
    ${SLICE_W5C_SOURCES} ${SQRTUNIT_GEN} ${OV70GEN}
    ${SLICE_W5B_SOURCES} ${SLICE_W5A_SOURCES} ${SLICE_W6D_SOURCES}
    ${SLICE_W7C_SOURCES}
    hal/actor_classes_ov022.cpp ${SLICE_W8PACKS_SOURCES}
    hal/actor_classes_ov031_w10.cpp ${SLICE_W10A_SOURCES}
    ${SLICE_W10B_SOURCES}
    ${SLICE_W11_SOURCES}
    hal/actor_classes_ov073.cpp unmatched/Ov073_State.cpp
    unmatched/Ov073_Render.cpp
    ${SLICE_W12_SOURCES}
    hal/actor_classes_ov052.cpp ${SLICE_W12B_SOURCES}
    hal/actor_classes_ov064_w12.cpp unmatched/Ov064_Clam.cpp
    ${SLICE_W12C_SOURCES}
    hal/actor_classes_ov056.cpp ${SLICE_W15_SOURCES}
    hal/actor_classes_ov002_w15.cpp ${SLICE_W15B_SOURCES}
    hal/actor_classes_ov025.cpp ${SLICE_W15C_SOURCES}
    hal/actor_classes_ov022_w16.cpp ${SLICE_W16A_SOURCES}
    hal/actor_classes_ov002_w17.cpp ${SLICE_W17A_SOURCES}
    ${SLICE_W18A_SOURCES} hal/ttc_level_data_seat.cpp
    unmatched/Ov070_PmfDispatch.cpp
    unmatched/FlyGuy_InitResources.cpp
    unmatched/W19_Slot5_Renders.cpp
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES} ${SLICE25_SOURCES} ${SLICE26_SOURCES}
    ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE33_SOURCES} ${GATE6_GEN} ${SLICE31_SOURCES} ${SLICE35_SOURCES} ${SLICEINTRO_SOURCES}
    ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE32_SOURCES}
    ${GATE32_HOST_SOURCES} ${GATE32_GEN} ${GATE6_GEN}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN} ${GATE35_GEN} ${GATE29_GEN} ${T2_DIV_GEN} ${TI2_MOD_GEN} ${SQ_DIST_GEN} ${TITEX_GX_GEN} ${TIDEP_RESET_GEN}
    ${GATE9_GEN} ${HOSTABI_RETIRE_GEN}
    ${SLICE40_SOURCES} ${SLICE41_SOURCES} ${SLICE42_SOURCES}
    hal/actor_classes_painting.cpp ${SLICE50_SOURCES} hal/actor_classes_wf.cpp
    unmatched/Cap_StateDispatch.cpp unmatched/ExclamationSwitch_StateDispatch.cpp ${SLICE51_SOURCES} ${SLICE59_SOURCES} ${SLICE78_SOURCES} ${SLICE79_SOURCES} ${SLICE82_SOURCES} ${SLICE83_SOURCES} ${SLICE64_SOURCES} ${GATE64_GEN} ${SLICE89_SOURCES} ${SLICE102_SOURCES} ${SLICE103_SOURCES} ${SLICE128_SOURCES} ${SLICE121_SOURCES} ${SLICE122_SOURCES} ${SLICE123_SOURCES}
    hal/actor_classes_ccm.cpp ${SLICE142_SOURCES} ${SLICE190_SOURCES} ${SLICE191_SOURCES} unmatched/SoundObject_Behavior.cpp unmatched/OneUpLogo_InitResources.cpp unmatched/MotherPenguin_InitResources.cpp unmatched/MotherPenguin_AfterClsnSeat.cpp unmatched/MotherPenguin_Behavior.cpp hal/actor_classes_ov081.cpp ${SLICE192_SOURCES} unmatched/MrBlizzard_StateDispatch.cpp unmatched/MrBlizzard_Behavior.cpp hal/actor_classes_ov072.cpp ${SLICE193_SOURCES} unmatched/Snowman_InitResources.cpp unmatched/BabyPenguin_StateEnter.cpp unmatched/BabyPenguin_InitResources.cpp unmatched/BabyPenguin_LandTransition.cpp unmatched/Snowman_StateDispatch.cpp unmatched/Ov072_MethodFaces.cpp hal/actor_classes_ov094.cpp ${SLICE194_SOURCES} unmatched/HootTheOwl_StateDispatch.cpp hal/actor_classes_ov012.cpp ${SLICE199_SOURCES} hal/actor_classes_ov002g200.cpp ${SLICE200_SOURCES} unmatched/PushBlock_StateDispatch.cpp hal/actor_classes_ov013.cpp ${SLICE202_SOURCES} hal/actor_classes_ov077.cpp ${SLICE_OV077_SOURCES} unmatched/Lakitu_HostSites.cpp unmatched/Spiny_HostSites.cpp unmatched/HeaveHo_HostSites.cpp unmatched/Lakitu_ShadowFaces.cpp ${SLICE_OV030CAST_SOURCES} hal/actor_classes_ov092.cpp ${SLICE_OV092_SOURCES} unmatched/ToxBox_HostSites.cpp unmatched/ToxBox_ShadowFaces.cpp hal/actor_classes_ov096.cpp ${SLICE_OV096_SOURCES} unmatched/Pokey_HostSites.cpp unmatched/Tornado_HostSites.cpp
    # run rel0215 wave 1 (lane cast-ov026): ov026's whole cast, level 18.
    hal/actor_classes_ov026.cpp ${SLICE_OV026_SOURCES} unmatched/Whirlpool_Render.cpp unmatched/Whirlpool_Behavior.cpp unmatched/Whirlpool_StateDispatch.cpp unmatched/WaterSuction_Behavior.cpp unmatched/WaterSuction_StateDispatch.cpp
    hal/actor_classes_ov036.cpp ${SLICE_OV036CAST_SOURCES} unmatched/FlyingCarpet_Render.cpp
    # run rel0215 wave 2 (lane cast-ov090): ov090's whole four-class water cast.
    ${SLICE_OV090_SOURCES}
    # run rel0215 wave 2 (lane cast-ov066): EYEROK (176), ov066, level 17.
    ${SLICE_OV066_SOURCES}
    hal/actor_classes_ov027.cpp ${SLICE_OV027CAST_SOURCES} unmatched/DaPgDfdr_StateDispatch.cpp unmatched/DaPgDfdr_Render.cpp unmatched/Player_BlowAway.cpp
    ${SLICE_OV074_SOURCES}
    hal/actor_classes_bbh.cpp ${SLICE149_SOURCES}
    hal/actor_classes_hmc.cpp ${SLICE172_SOURCES} unmatched/Crate_StateDispatch.cpp
    hal/actor_classes_updownlift.cpp ${SLICE173_SOURCES} unmatched/UpDownLiftBbh_Behavior.cpp
    hal/actor_classes_montymole.cpp ${SLICE174_SOURCES} unmatched/MontyMole_StateDispatch.cpp unmatched/MontyMole_1Up.cpp
    hal/actor_classes_flame.cpp ${SLICE175_SOURCES}
    hal/actor_classes_flamethrower.cpp ${SLICE_W6F_SOURCES} unmatched/Flamethrower_Behavior.c
    ${SLICE_W8FACES_SOURCES}
    hal/actor_classes_scuttlebug.cpp ${SLICE176_SOURCES} unmatched/Scuttlebug_StateDispatch.cpp
    hal/actor_classes_ov071.cpp ${SLICE_W7B_SOURCES} ${GATE_W7B_GEN} unmatched/MrI_StateDispatch.cpp unmatched/MrI_StateMains.cpp unmatched/MrI_Render.cpp
    hal/actor_classes_ov064.cpp ${SLICE177_SOURCES} unmatched/Bully_AfterClsn.cpp
    hal/actor_classes_ov064_gate178.cpp ${SLICE178_SOURCES} unmatched/Ov064Gate178_States.cpp
    hal/actor_classes_ov064_w9.cpp ${SLICEW9A_SOURCES} ${SLICEW9B_SOURCES} ${SLICEW9C_SOURCES} ${SLICEW9D_SOURCES}
    hal/actor_classes_jrb.cpp ${SLICE188_SOURCES} unmatched/Jrb_Renders.cpp
    unmatched/Unagi_StateDispatch.cpp unmatched/Unagi_BodyChain.cpp unmatched/Jrb_Aliases.cpp
    hal/actor_classes_bowserpuzzle.cpp ${SLICE179_SOURCES} unmatched/BowserPuzzle_StateDispatch.cpp unmatched/BowserPuzzle_Render.cpp
    hal/actor_classes_koopa_chuckya.cpp hal/koopa_chuckya_faces.cpp ${SLICE182_SOURCES} unmatched/KoopaChuckya_StateDispatch.cpp unmatched/Koopa_Render.cpp
    ${SLICE180_SOURCES} ${SLICE203_SOURCES} ${SLICE204_SOURCES}
    hal/actor_classes_ov085.cpp ${SLICE205_SOURCES}
    unmatched/Ov085_Behaviors.cpp unmatched/Ov085_Toad_StateSeat.cpp
    hal/actor_classes_montymolerock.cpp ${SLICE208_SOURCES}
    hal/actor_classes_ov043.cpp ${SLICE206_SOURCES} ${SLICE207_SOURCES}
    hal/actor_classes_l7.cpp ${SLICE_L7_SOURCES} ${GATE_L7_GEN} unmatched/PiranhaPlant_States.cpp
    unmatched/WaterBomb_StateDispatch.cpp unmatched/WaterBomb_State0.cpp
    unmatched/KnockDownPlank_Behavior.cpp unmatched/FallBlockWf_InitVeneer.cpp
    unmatched/StarMarker_D1.cpp
    # run rel0215 wave 1 (lane cast-sweep1): the six small-cast overlays
    ${SWEEP1_SOURCES}
    # run rel0215 wave 2 (lane cast-sweep2): the four-overlay cast sweep
    ${SWEEP2_SOURCES}
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

# run linkw wave 8 (lane w8-a): THE STAGE CLASS CLOSURE, first landed piece.
#
# Eight matched Stage bodies plus the one callee they drag (nine TUs), the
# class's own arm9 BSS (hal/w8a_stage_storage.cpp) and the C-name faces that
# reach them (hal/w8a_stage_faces.cpp). There are no /include: link roots here
# and NOTHING REACHES THESE YET: every one of the nine is /OPT:REF-stripped and
# every alias LHS is unused, which is why this lands at the linkage floor. The
# slice head records, per held-out TU, the measured blocker that keeps the other
# twenty-one out; the faces file records that the reach does not exist, because
# the ROM reaches all of these through _ZTV5Stage, which hal/stage_bridges.cpp
# still trap-fills. Seating that table is what turns this groundwork live.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_w8a.txt" SLICE_W8A_LINES)
set(SLICE_W8A_SOURCES "")
foreach(line IN LISTS SLICE_W8A_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE_W8A_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

# Gate 12: the interactive window (same stack, windowed loop).
#
# tests/io_pressure.cpp is the negative control for ntr/io.cpp's fixed-address
# reservation: it takes 0x02000000 before the game asks for it, so the failure a
# player hit can be reproduced on demand against THIS binary rather than against
# a special build of it. Inert unless SM64DS_IO_PRESSURE_AT is set in the
# environment, and nothing in the tree sets it.
add_executable(walk_window tests/walk_window.cpp tests/io_pressure.cpp "${OV002DATA_C}" ${LEVEL_OV_DATA}
    # dialogue pipeline (2026-08-08): the box pump (Stage::UpdateMessage's
    # dialogue arm), the engine-A 2D-over-3D compositor, and the headless probe.
    hal/message_pump.cpp hal/message_compositor.cpp hal/message_probe.cpp hal/message_gen_text.cpp hal/input_probe.cpp
    # the launcher's settings.json, read by the game (camera turn direction,
    # and the four screen-gap keys hal/screen_gap.cpp reads)
    hal/host_settings.cpp hal/screen_gap.cpp
    # run lvled lane B: the editor control channel on 127.0.0.1:7355, the link
    # the Studio's editor talks to. Compiled in unconditionally but INERT unless
    # SM64DS_EDITOR_CHANNEL=1 -- no thread, no socket, no listener without it.
    # It is on BOTH targets that compile tests/walk_window.cpp (walk_window and
    # walk_window_hires), because the frame loop's drain call lives in that
    # shared file and a target without this source fails the link.
    hal/editor_channel.cpp
    # run lvled lane g3: live skybox switching for the editor's `skyset` verb.
    # Same story as the channel and the same flag: compiled in unconditionally,
    # INERT until editor_channel_init calls port_editor_sky_arm(), which it
    # does only under SM64DS_EDITOR_CHANNEL=1. Unarmed it runs no line,
    # allocates nothing and writes no game state, which is what lets the
    # battery -- which runs unarmed -- stand as the proof that it changed
    # nothing. On both walk_window targets for the channel's reason.
    hal/editor_sky.cpp
    # the Mods panel's file-layer half: installs hal/fs.cpp's mod hooks
    # (LovesMeCharacter). Game-shaped targets only; every smoke keeps the
    # null hooks and stock file serving.
    hal/fs_mods.cpp
    # SM64DS_STAGE_GEOM: a level's stage model and collision served out of an
    # external folder. CHAINS onto the hook above rather than replacing it --
    # its installer sits in .CRT$XCV so it runs after every ordinary one -- so
    # it goes beside hal/fs_mods.cpp on the same targets and nowhere else.
    hal/stage_geom.cpp
    # the scene's hand-drawn gap band and the band's per-scene continuity
    # table, both asked for at the same latch hal/screen_gap.cpp does
    hal/gap_art.cpp hal/gap_continuity.cpp
    # in-memory save state (lane lk6): F8/F9 snapshot and restore of the hosted
    # DS arena plus the out-of-arena game globals.
    hal/lk6_savestate.cpp hal/lk7_persist.cpp hal/dsstate_seg.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/meshcolliderbase_vtable.cpp hal/meshcollider_dtor_seat.cpp hal/lk1_eh_vmax_seat.cpp hal/lk2_platform_dtor_seat.cpp hal/model_dtor_seat.cpp hal/lk4_eh_dtor_seat.cpp hal/lk4_solidheap_seat.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    ${SLICE_W8SHADOWS_SOURCES}
    # hal/megachar_stub.cpp is NOT here any more (lane w8-shadows). Its
    # `return 0` stand-in for Platform::UpdateKillByMegaChar is now compiled
    # only into the three narrow gate-9 harnesses above, which cannot build the
    # real closure. In this target the ROM's C name is the face at the bottom
    # of hal/wf_enemy_bridges.cpp and the body is the matched TU that has been
    # sitting in slice_gate16.txt line 178 all along, dropped by /OPT:REF for
    # want of a reference edge. Removing the stub from here is what makes the
    # matched body reachable; leaving it would be a duplicate definition.
    hal/cxxname_bridge.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/door_ring_faces.cpp hal/fader_wipes.cpp "${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/fader/_ZN9FaderWipe14LoadAndSetFileEt.cpp" hal/level_boot.cpp hal/stage_mods.cpp hal/scene_boot.cpp hal/title_doodle_probe.cpp hal/scene_actor_faces.cpp unmatched/Scene_AfterCleanupResources.cpp ${SLICE_SCENE1_SOURCES} ${SLICE_OV007_SOURCES} ${SC1_INTERIM_SOURCES} ${SLICE_MG1_SOURCES} ${SCENE_MG_SOURCES} ${SLICE_LUI_SOURCES} ${SLICE_PCH_SOURCES} ${SLICE_SMB_SOURCES} ${SCENE_SMB_SOURCES} ${SLICE_CCN_SOURCES} ${SLICE_PPP_SOURCES} ${SLICE_FLW_SOURCES} ${SCENE_MG_FLOWER_SOURCES} ${SLICE_MEM_SOURCES} ${SLICE_S381_SOURCES} ${SLICE_MMT_SOURCES} ${SLICE_LKT_SOURCES} ${SLICE_SOS_SOURCES} ${SLICE_S75_SOURCES} ${SCENE_MG_CURLING2_SOURCES} ${SLICE_PSY_SOURCES} ${SLICE_LKY_SOURCES} ${SLICE_SNW_SOURCES} ${SLICE_TTE_SOURCES} ${SLICE_BOO_SOURCES} ${SLICE_WIG_SOURCES} ${SLICE_S371_SOURCES} ${SLICE_BOX_SOURCES} ${SLICE_S364_SOURCES} ${SCENE_MG_SLOT1_SOURCES} ${SLICE_CUP_SOURCES} ${SLICE_MUG_SOURCES} ${SLICE_BNP_SOURCES} ${SLICE_BNT_SOURCES} ${SLICE_PKR_SOURCES} ${SLICE_PGO_SOURCES} ${SLICE_RLT_SOURCES} ${SLICE_TTI_SOURCES} ${SLICE_MGM_SOURCES} ${SLICE_VS_SOURCES} ${FDR_FADER_SOURCES} ${NFS_NAMES_SOURCES}
    # run link60 lane L2: hal/w8a_stage_storage.cpp is pure zero arm9 BSS and it
    # owns data_0209d4a8, which dScDSMT_c's InitResources and CleanupResources
    # both touch. walk_window already had it; smoke_player and walk_window_hires
    # did not, so it joins all three rather than the symbol being defined a
    # second time next to the one that already exists.
    hal/w8a_stage_storage.cpp hal/cannon_probe.cpp hal/message_boot.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_classes_bob_world.cpp hal/actor_faces_bob.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/sub_actors.cpp hal/sub_screen.cpp ${SLICE_2D1_SOURCES} hal/stage_bridges.cpp hal/level_change.cpp hal/title_entry.cpp hal/star_flow.cpp hal/oam_lists.cpp hal/blend_vtable.cpp hal/particle_vtable.cpp hal/particle_bridges.cpp unmatched/Particle_RideThroughs.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE_PTR_SOURCES}
    # Stage 4, lane SD0: slot 0's closure. On all THREE targets rather than
    # walk_window alone, and not by preference: all three compile
    # hal/stage_bridges.cpp, which is where the Stage vtable and its slot-0
    # occupant live, so a face or a body that only one of them carried would
    # fail the other two at link. hal/stage_slot0.cpp holds the three
    # PORT_HOST_ABI bodies, the four C-name faces, two 4-byte arm9 BSS words
    # and the data_02075720 alias; the slice holds the ROM's own TUs.
    hal/stage_slot0.cpp ${SLICE_S4_SLOT0_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE_COMMS_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE_W1L1_SOURCES} ${SLICE_W1L2_SOURCES} ${SLICE_W1L3_SOURCES}
    ${SLICE_W1L4_SOURCES} ${SLICE_W1L5_SOURCES} ${SLICE_W5D_SOURCES} ${SLICE_W5E_SOURCES}
    ${SLICE_W6A_SOURCES}
    ${SLICE_W9HARVEST_SOURCES}
    ${SLICE_W5C_SOURCES} ${SQRTUNIT_GEN} ${OV70GEN}
    ${SLICE_W5B_SOURCES} ${SLICE_W5A_SOURCES} ${SLICE_W6D_SOURCES}
    ${SLICE_W7C_SOURCES}
    hal/actor_classes_ov022.cpp ${SLICE_W8PACKS_SOURCES}
    hal/actor_classes_ov031_w10.cpp ${SLICE_W10A_SOURCES}
    ${SLICE_W10B_SOURCES}
    ${SLICE_W11_SOURCES}
    hal/actor_classes_ov073.cpp unmatched/Ov073_State.cpp
    unmatched/Ov073_Render.cpp
    ${SLICE_W12_SOURCES}
    hal/actor_classes_ov052.cpp ${SLICE_W12B_SOURCES}
    hal/actor_classes_ov064_w12.cpp unmatched/Ov064_Clam.cpp
    ${SLICE_W12C_SOURCES}
    hal/actor_classes_ov056.cpp ${SLICE_W15_SOURCES}
    hal/actor_classes_ov002_w15.cpp ${SLICE_W15B_SOURCES}
    hal/actor_classes_ov025.cpp ${SLICE_W15C_SOURCES}
    hal/actor_classes_ov022_w16.cpp ${SLICE_W16A_SOURCES}
    hal/actor_classes_ov002_w17.cpp ${SLICE_W17A_SOURCES}
    ${SLICE_W18A_SOURCES} hal/ttc_level_data_seat.cpp
    unmatched/Ov070_PmfDispatch.cpp
    unmatched/FlyGuy_InitResources.cpp
    unmatched/W19_Slot5_Renders.cpp
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES}
    ${SLICE33_SOURCES} ${GATE6_GEN}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES} ${SLICE31_SOURCES} ${GATE6_GEN} ${SLICEINTRO_SOURCES}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES} ${SLICE35_SOURCES} ${GATE6_GEN}
    ${SLICE32_SOURCES} ${GATE32_HOST_SOURCES} ${GATE32_GEN} ${GATE6_GEN}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN} ${GATE35_GEN} ${GATE29_GEN} ${T2_DIV_GEN} ${TI2_MOD_GEN} ${SQ_DIST_GEN} ${TITEX_GX_GEN} ${TIDEP_RESET_GEN}
    ${GATE9_GEN} ${HOSTABI_RETIRE_GEN}
    ${SLICE40_SOURCES} ${SLICE41_SOURCES} ${SLICE42_SOURCES}
    hal/actor_classes_painting.cpp ${SLICE50_SOURCES} hal/actor_classes_wf.cpp
    unmatched/Cap_StateDispatch.cpp unmatched/ExclamationSwitch_StateDispatch.cpp ${SLICE51_SOURCES} ${SLICE59_SOURCES} ${SLICE78_SOURCES} ${SLICE79_SOURCES} ${SLICE82_SOURCES} ${SLICE83_SOURCES} ${SLICE64_SOURCES} ${GATE64_GEN} ${SLICE89_SOURCES} ${SLICE102_SOURCES} ${SLICE103_SOURCES} ${SLICE128_SOURCES} ${SLICE121_SOURCES} ${SLICE122_SOURCES} ${SLICE123_SOURCES}
    hal/actor_classes_ccm.cpp ${SLICE142_SOURCES} ${SLICE190_SOURCES} ${SLICE191_SOURCES} unmatched/SoundObject_Behavior.cpp unmatched/OneUpLogo_InitResources.cpp unmatched/MotherPenguin_InitResources.cpp unmatched/MotherPenguin_AfterClsnSeat.cpp unmatched/MotherPenguin_Behavior.cpp hal/actor_classes_ov081.cpp ${SLICE192_SOURCES} unmatched/MrBlizzard_StateDispatch.cpp unmatched/MrBlizzard_Behavior.cpp hal/actor_classes_ov072.cpp ${SLICE193_SOURCES} unmatched/Snowman_InitResources.cpp unmatched/BabyPenguin_StateEnter.cpp unmatched/BabyPenguin_InitResources.cpp unmatched/BabyPenguin_LandTransition.cpp unmatched/Snowman_StateDispatch.cpp unmatched/Ov072_MethodFaces.cpp hal/actor_classes_ov094.cpp ${SLICE194_SOURCES} unmatched/HootTheOwl_StateDispatch.cpp hal/actor_classes_ov012.cpp ${SLICE199_SOURCES} hal/actor_classes_ov002g200.cpp ${SLICE200_SOURCES} unmatched/PushBlock_StateDispatch.cpp hal/actor_classes_ov013.cpp ${SLICE202_SOURCES} hal/actor_classes_ov077.cpp ${SLICE_OV077_SOURCES} unmatched/Lakitu_HostSites.cpp unmatched/Spiny_HostSites.cpp unmatched/HeaveHo_HostSites.cpp unmatched/Lakitu_ShadowFaces.cpp ${SLICE_OV030CAST_SOURCES} hal/actor_classes_ov092.cpp ${SLICE_OV092_SOURCES} unmatched/ToxBox_HostSites.cpp unmatched/ToxBox_ShadowFaces.cpp hal/actor_classes_ov096.cpp ${SLICE_OV096_SOURCES} unmatched/Pokey_HostSites.cpp unmatched/Tornado_HostSites.cpp
    # run rel0215 wave 1 (lane cast-ov026): ov026's whole cast, level 18.
    hal/actor_classes_ov026.cpp ${SLICE_OV026_SOURCES} unmatched/Whirlpool_Render.cpp unmatched/Whirlpool_Behavior.cpp unmatched/Whirlpool_StateDispatch.cpp unmatched/WaterSuction_Behavior.cpp unmatched/WaterSuction_StateDispatch.cpp
    hal/actor_classes_ov036.cpp ${SLICE_OV036CAST_SOURCES} unmatched/FlyingCarpet_Render.cpp
    # run rel0215 wave 2 (lane cast-ov090): ov090's whole four-class water cast.
    ${SLICE_OV090_SOURCES}
    # run rel0215 wave 2 (lane cast-ov066): EYEROK (176), ov066, level 17.
    ${SLICE_OV066_SOURCES}
    hal/actor_classes_ov027.cpp ${SLICE_OV027CAST_SOURCES} unmatched/DaPgDfdr_StateDispatch.cpp unmatched/DaPgDfdr_Render.cpp unmatched/Player_BlowAway.cpp
    ${SLICE_OV074_SOURCES}
    hal/actor_classes_bbh.cpp ${SLICE149_SOURCES}
    hal/actor_classes_hmc.cpp ${SLICE172_SOURCES} unmatched/Crate_StateDispatch.cpp
    hal/actor_classes_updownlift.cpp ${SLICE173_SOURCES} unmatched/UpDownLiftBbh_Behavior.cpp
    hal/actor_classes_montymole.cpp ${SLICE174_SOURCES} unmatched/MontyMole_StateDispatch.cpp unmatched/MontyMole_1Up.cpp
    hal/actor_classes_flame.cpp ${SLICE175_SOURCES}
    hal/actor_classes_flamethrower.cpp ${SLICE_W6F_SOURCES} unmatched/Flamethrower_Behavior.c
    ${SLICE_W8FACES_SOURCES}
    hal/actor_classes_scuttlebug.cpp ${SLICE176_SOURCES} unmatched/Scuttlebug_StateDispatch.cpp
    hal/actor_classes_ov071.cpp ${SLICE_W7B_SOURCES} ${GATE_W7B_GEN} unmatched/MrI_StateDispatch.cpp unmatched/MrI_StateMains.cpp unmatched/MrI_Render.cpp
    hal/actor_classes_ov064.cpp ${SLICE177_SOURCES} unmatched/Bully_AfterClsn.cpp
    hal/actor_classes_ov064_gate178.cpp ${SLICE178_SOURCES} unmatched/Ov064Gate178_States.cpp
    hal/actor_classes_ov064_w9.cpp ${SLICEW9A_SOURCES} ${SLICEW9B_SOURCES} ${SLICEW9C_SOURCES} ${SLICEW9D_SOURCES}
    hal/actor_classes_jrb.cpp ${SLICE188_SOURCES} unmatched/Jrb_Renders.cpp
    unmatched/Unagi_StateDispatch.cpp unmatched/Unagi_BodyChain.cpp unmatched/Jrb_Aliases.cpp
    hal/actor_classes_bowserpuzzle.cpp ${SLICE179_SOURCES} unmatched/BowserPuzzle_StateDispatch.cpp unmatched/BowserPuzzle_Render.cpp
    hal/actor_classes_koopa_chuckya.cpp hal/koopa_chuckya_faces.cpp ${SLICE182_SOURCES} unmatched/KoopaChuckya_StateDispatch.cpp unmatched/Koopa_Render.cpp
    ${SLICE180_SOURCES} ${SLICE203_SOURCES} ${SLICE204_SOURCES}
    hal/actor_classes_ov085.cpp ${SLICE205_SOURCES}
    unmatched/Ov085_Behaviors.cpp unmatched/Ov085_Toad_StateSeat.cpp
    hal/actor_classes_montymolerock.cpp ${SLICE208_SOURCES}
    hal/actor_classes_ov043.cpp ${SLICE206_SOURCES} ${SLICE207_SOURCES}
    hal/actor_classes_l7.cpp ${SLICE_L7_SOURCES} ${GATE_L7_GEN} unmatched/PiranhaPlant_States.cpp
    unmatched/WaterBomb_StateDispatch.cpp unmatched/WaterBomb_State0.cpp
    unmatched/KnockDownPlank_Behavior.cpp unmatched/FallBlockWf_InitVeneer.cpp
    unmatched/StarMarker_D1.cpp
    # run rel0215 wave 1 (lane cast-sweep1): the six small-cast overlays
    ${SWEEP1_SOURCES}
    # run rel0215 wave 2 (lane cast-sweep2): the four-overlay cast sweep
    ${SWEEP2_SOURCES}
    # _ZTV5Stage SLOT 12 (run link60, lane L4). src/_ZN5Stage16OnPendingDestroyEv.cpp
    # is named on smoke_player and walk_window_hires above, because those two
    # targets compile hal/stage_bridges.cpp -- which seats the slot -- and do
    # NOT carry SLICE_W8A_SOURCES, which is how walk_window gets the same TU.
    # Naming it here as well would put the object in this link twice.
    #
    # It has to be seated on all three: the seat in hal/stage_bridges.cpp runs
    # Scene::BeforeBehavior on the Stage every frame, and that function reaches
    # ActorBase::MarkForDestruction, which calls slot 12 through the vptr.
    #
    # THE FADER TU IS NAMED THE SAME WAY (run link60, lane SL0).
    # src/engine/fader/_ZN9FaderWipe14LoadAndSetFileEt.cpp is named beside
    # hal/fader_wipes.cpp on all THREE targets that compile that file --
    # smoke_player, walk_window and walk_window_hires -- rather than put on a
    # slice, because it is not optional for any of them: hal/fader_wipes.cpp
    # now defines the C-name face _ZN9FaderWipe14LoadAndSetFileEt, and that
    # face calls the method this TU is the only definition of. Face and body
    # link together or the target does not link at all. Measured, not
    # predicted: naming the face without the body failed all three links with
    # LNK2019 on ?LoadAndSetFile@FaderWipe@@QAEXG@Z.
    # run linkw wave 8 (lane w8-a): the Stage class closure, first piece.
    hal/w8a_stage_storage.cpp hal/w8a_stage_faces.cpp ${SLICE_W8A_SOURCES}
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
    # hal/asset_root_refuse.cpp: the shipping build has NO PORT_REPO_ROOT
    # fallback for assets. It is in this block and only this block, so the
    # developer build keeps the fallback every lane in this tree relies on.
    # Run link60 lane KIT1; port/kit_pipeline.txt is the write-up.
    target_sources(walk_window PRIVATE "${ROMDATA_DISPATCH_C}"
                   hal/romdata_loader.cpp hal/asset_root_refuse.cpp)
    target_compile_definitions(walk_window PRIVATE PORT_ROM_CLEAN)
    add_custom_target(romblob_ready_ww DEPENDS "${ROMDATA_BIN}"
        "${CMAKE_BINARY_DIR}/assets/romblob_verified.stamp"
        "${ROMDATA_RECIPE}")
    add_dependencies(walk_window romblob_ready_ww)
endif()
if(MSVC)
    target_compile_options(walk_window PRIVATE /wd4311 /wd4312 /wd4068 /wd4576)
    # No ASLR for the shipped window: a disk save state (hal/lk7_persist
    # .cpp) stores absolute pointers into the exe image -- every actor's
    # vtable pointer -- and they only relocate if the image loads at the
    # SAME base on the next launch. ASLR rebases once per boot, which
    # would refuse every state written before a reboot. The header still
    # checks the base, so if anything rebases the exe anyway the state is
    # refused with a message rather than loaded onto dangling pointers.
    target_link_options(walk_window PRIVATE /DYNAMICBASE:NO)
endif()

# The save state captures the whole .dsstate section (hal/dsstate_seg.h). Fail
# the build if a hosted DS global landed OUTSIDE it: that symbol would not roll
# back on a restore, which is exactly the class of bug that shipped in 0.2.0
# (the ANIM_PTRS table crashing Player::SetAnim on a null BCA_File*). The map
# the guard reads is the one /MAP already writes next to the exe.
add_custom_command(TARGET walk_window POST_BUILD
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/dsstate_guard.py"
            "$<TARGET_FILE_DIR:walk_window>/walk_window.map"
    VERBATIM
    COMMENT "dsstate_guard: hosted DS globals all inside .dsstate")

# hires clone for screenshot sessions (4x resolution, ~1/16 the fps)
add_executable(walk_window_hires tests/walk_window.cpp "${OV002DATA_C}" ${LEVEL_OV_DATA}
    # dialogue pipeline (2026-08-08): pump, engine-A compositor, headless probe.
    hal/message_pump.cpp hal/message_compositor.cpp hal/message_probe.cpp hal/message_gen_text.cpp hal/input_probe.cpp
    # the launcher's settings.json, read by the game (camera turn direction,
    # and the four screen-gap keys hal/screen_gap.cpp reads)
    hal/host_settings.cpp hal/screen_gap.cpp
    # run lvled lane B: the editor control channel on 127.0.0.1:7355, the link
    # the Studio's editor talks to. Compiled in unconditionally but INERT unless
    # SM64DS_EDITOR_CHANNEL=1 -- no thread, no socket, no listener without it.
    # It is on BOTH targets that compile tests/walk_window.cpp (walk_window and
    # walk_window_hires), because the frame loop's drain call lives in that
    # shared file and a target without this source fails the link.
    hal/editor_channel.cpp
    # run lvled lane g3: live skybox switching for the editor's `skyset` verb.
    # Same story as the channel and the same flag: compiled in unconditionally,
    # INERT until editor_channel_init calls port_editor_sky_arm(), which it
    # does only under SM64DS_EDITOR_CHANNEL=1. Unarmed it runs no line,
    # allocates nothing and writes no game state, which is what lets the
    # battery -- which runs unarmed -- stand as the proof that it changed
    # nothing. On both walk_window targets for the channel's reason.
    hal/editor_sky.cpp
    # the Mods panel's file-layer half: installs hal/fs.cpp's mod hooks
    # (LovesMeCharacter). Game-shaped targets only; every smoke keeps the
    # null hooks and stock file serving.
    hal/fs_mods.cpp
    # SM64DS_STAGE_GEOM: a level's stage model and collision served out of an
    # external folder. CHAINS onto the hook above rather than replacing it --
    # its installer sits in .CRT$XCV so it runs after every ordinary one -- so
    # it goes beside hal/fs_mods.cpp on the same targets and nowhere else.
    hal/stage_geom.cpp
    # the scene's hand-drawn gap band and the band's per-scene continuity
    # table, both asked for at the same latch hal/screen_gap.cpp does
    hal/gap_art.cpp hal/gap_continuity.cpp
    # in-memory save state (lane lk6): F8/F9 snapshot and restore of the hosted
    # DS arena plus the out-of-arena game globals.
    hal/lk6_savestate.cpp hal/lk7_persist.cpp hal/dsstate_seg.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/meshcolliderbase_vtable.cpp hal/meshcollider_dtor_seat.cpp hal/lk1_eh_vmax_seat.cpp hal/lk2_platform_dtor_seat.cpp hal/model_dtor_seat.cpp hal/lk4_eh_dtor_seat.cpp hal/lk4_solidheap_seat.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    ${SLICE_W8SHADOWS_SOURCES}
    # hal/megachar_stub.cpp is NOT here any more (lane w8-shadows). Its
    # `return 0` stand-in for Platform::UpdateKillByMegaChar is now compiled
    # only into the three narrow gate-9 harnesses above, which cannot build the
    # real closure. In this target the ROM's C name is the face at the bottom
    # of hal/wf_enemy_bridges.cpp and the body is the matched TU that has been
    # sitting in slice_gate16.txt line 178 all along, dropped by /OPT:REF for
    # want of a reference edge. Removing the stub from here is what makes the
    # matched body reachable; leaving it would be a duplicate definition.
    hal/cxxname_bridge.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/door_ring_faces.cpp hal/fader_wipes.cpp "${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/fader/_ZN9FaderWipe14LoadAndSetFileEt.cpp" hal/level_boot.cpp hal/stage_mods.cpp hal/scene_boot.cpp hal/title_doodle_probe.cpp hal/scene_actor_faces.cpp unmatched/Scene_AfterCleanupResources.cpp ${SLICE_SCENE1_SOURCES} ${SLICE_OV007_SOURCES} ${SC1_INTERIM_SOURCES} ${SLICE_MG1_SOURCES} ${SCENE_MG_SOURCES} ${SLICE_LUI_SOURCES} ${SLICE_PCH_SOURCES} ${SLICE_SMB_SOURCES} ${SCENE_SMB_SOURCES} ${SLICE_CCN_SOURCES} ${SLICE_PPP_SOURCES} ${SLICE_FLW_SOURCES} ${SCENE_MG_FLOWER_SOURCES} ${SLICE_MEM_SOURCES} ${SLICE_S381_SOURCES} ${SLICE_MMT_SOURCES} ${SLICE_LKT_SOURCES} ${SLICE_SOS_SOURCES} ${SLICE_S75_SOURCES} ${SCENE_MG_CURLING2_SOURCES} ${SLICE_PSY_SOURCES} ${SLICE_LKY_SOURCES} ${SLICE_SNW_SOURCES} ${SLICE_TTE_SOURCES} ${SLICE_BOO_SOURCES} ${SLICE_WIG_SOURCES} ${SLICE_S371_SOURCES} ${SLICE_BOX_SOURCES} ${SLICE_S364_SOURCES} ${SCENE_MG_SLOT1_SOURCES} ${SLICE_CUP_SOURCES} ${SLICE_MUG_SOURCES} ${SLICE_BNP_SOURCES} ${SLICE_BNT_SOURCES} ${SLICE_PKR_SOURCES} ${SLICE_PGO_SOURCES} ${SLICE_RLT_SOURCES} ${SLICE_TTI_SOURCES} ${SLICE_MGM_SOURCES} ${SLICE_VS_SOURCES} ${FDR_FADER_SOURCES} ${NFS_NAMES_SOURCES}
    # run link60 lane L2: hal/w8a_stage_storage.cpp is pure zero arm9 BSS and it
    # owns data_0209d4a8, which dScDSMT_c's InitResources and CleanupResources
    # both touch. walk_window already had it; smoke_player and walk_window_hires
    # did not, so it joins all three rather than the symbol being defined a
    # second time next to the one that already exists.
    hal/w8a_stage_storage.cpp hal/cannon_probe.cpp hal/message_boot.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_classes_bob_world.cpp hal/actor_faces_bob.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/sub_actors.cpp hal/sub_screen.cpp ${SLICE_2D1_SOURCES} hal/stage_bridges.cpp "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN5Stage16OnPendingDestroyEv.cpp" hal/level_change.cpp hal/title_entry.cpp hal/star_flow.cpp hal/oam_lists.cpp hal/blend_vtable.cpp hal/particle_vtable.cpp hal/particle_bridges.cpp unmatched/Particle_RideThroughs.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE_PTR_SOURCES}
    # Stage 4, lane SD0: slot 0's closure. On all THREE targets rather than
    # walk_window alone, and not by preference: all three compile
    # hal/stage_bridges.cpp, which is where the Stage vtable and its slot-0
    # occupant live, so a face or a body that only one of them carried would
    # fail the other two at link. hal/stage_slot0.cpp holds the three
    # PORT_HOST_ABI bodies, the four C-name faces, two 4-byte arm9 BSS words
    # and the data_02075720 alias; the slice holds the ROM's own TUs.
    hal/stage_slot0.cpp ${SLICE_S4_SLOT0_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE_COMMS_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE_W1L1_SOURCES} ${SLICE_W1L2_SOURCES} ${SLICE_W1L3_SOURCES}
    ${SLICE_W1L4_SOURCES} ${SLICE_W1L5_SOURCES} ${SLICE_W5D_SOURCES} ${SLICE_W5E_SOURCES}
    ${SLICE_W6A_SOURCES}
    ${SLICE_W9HARVEST_SOURCES}
    ${SLICE_W5C_SOURCES} ${SQRTUNIT_GEN} ${OV70GEN}
    ${SLICE_W5B_SOURCES} ${SLICE_W5A_SOURCES} ${SLICE_W6D_SOURCES}
    ${SLICE_W7C_SOURCES}
    hal/actor_classes_ov022.cpp ${SLICE_W8PACKS_SOURCES}
    hal/actor_classes_ov031_w10.cpp ${SLICE_W10A_SOURCES}
    ${SLICE_W10B_SOURCES}
    ${SLICE_W11_SOURCES}
    hal/actor_classes_ov073.cpp unmatched/Ov073_State.cpp
    unmatched/Ov073_Render.cpp
    ${SLICE_W12_SOURCES}
    hal/actor_classes_ov052.cpp ${SLICE_W12B_SOURCES}
    hal/actor_classes_ov064_w12.cpp unmatched/Ov064_Clam.cpp
    ${SLICE_W12C_SOURCES}
    hal/actor_classes_ov056.cpp ${SLICE_W15_SOURCES}
    hal/actor_classes_ov002_w15.cpp ${SLICE_W15B_SOURCES}
    hal/actor_classes_ov025.cpp ${SLICE_W15C_SOURCES}
    hal/actor_classes_ov022_w16.cpp ${SLICE_W16A_SOURCES}
    hal/actor_classes_ov002_w17.cpp ${SLICE_W17A_SOURCES}
    ${SLICE_W18A_SOURCES} hal/ttc_level_data_seat.cpp
    unmatched/Ov070_PmfDispatch.cpp
    unmatched/FlyGuy_InitResources.cpp
    unmatched/W19_Slot5_Renders.cpp
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES}
    ${SLICE33_SOURCES} ${GATE6_GEN}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES} ${SLICE31_SOURCES} ${GATE6_GEN} ${SLICEINTRO_SOURCES}
    ${SLICE25_SOURCES} ${SLICE27_SOURCES} ${SLICE28_SOURCES} ${SLICE26_SOURCES} ${SLICE35_SOURCES} ${GATE6_GEN}
    ${SLICE32_SOURCES} ${GATE32_HOST_SOURCES} ${GATE32_GEN} ${GATE6_GEN}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN} ${GATE35_GEN} ${GATE29_GEN} ${T2_DIV_GEN} ${TI2_MOD_GEN} ${SQ_DIST_GEN} ${TITEX_GX_GEN} ${TIDEP_RESET_GEN}
    ${GATE9_GEN} ${HOSTABI_RETIRE_GEN}
    ${SLICE40_SOURCES} ${SLICE41_SOURCES} ${SLICE42_SOURCES}
    hal/actor_classes_painting.cpp ${SLICE50_SOURCES} hal/actor_classes_wf.cpp
    unmatched/Cap_StateDispatch.cpp unmatched/ExclamationSwitch_StateDispatch.cpp ${SLICE51_SOURCES} ${SLICE59_SOURCES} ${SLICE78_SOURCES} ${SLICE79_SOURCES} ${SLICE82_SOURCES} ${SLICE83_SOURCES} ${SLICE64_SOURCES} ${GATE64_GEN} ${SLICE89_SOURCES} ${SLICE102_SOURCES} ${SLICE103_SOURCES} ${SLICE128_SOURCES} ${SLICE121_SOURCES} ${SLICE122_SOURCES} ${SLICE123_SOURCES}
    hal/actor_classes_ccm.cpp ${SLICE142_SOURCES} ${SLICE190_SOURCES} ${SLICE191_SOURCES} unmatched/SoundObject_Behavior.cpp unmatched/OneUpLogo_InitResources.cpp unmatched/MotherPenguin_InitResources.cpp unmatched/MotherPenguin_AfterClsnSeat.cpp unmatched/MotherPenguin_Behavior.cpp hal/actor_classes_ov081.cpp ${SLICE192_SOURCES} unmatched/MrBlizzard_StateDispatch.cpp unmatched/MrBlizzard_Behavior.cpp hal/actor_classes_ov072.cpp ${SLICE193_SOURCES} unmatched/Snowman_InitResources.cpp unmatched/BabyPenguin_StateEnter.cpp unmatched/BabyPenguin_InitResources.cpp unmatched/BabyPenguin_LandTransition.cpp unmatched/Snowman_StateDispatch.cpp unmatched/Ov072_MethodFaces.cpp hal/actor_classes_ov094.cpp ${SLICE194_SOURCES} unmatched/HootTheOwl_StateDispatch.cpp hal/actor_classes_ov012.cpp ${SLICE199_SOURCES} hal/actor_classes_ov002g200.cpp ${SLICE200_SOURCES} unmatched/PushBlock_StateDispatch.cpp hal/actor_classes_ov013.cpp ${SLICE202_SOURCES} hal/actor_classes_ov077.cpp ${SLICE_OV077_SOURCES} unmatched/Lakitu_HostSites.cpp unmatched/Spiny_HostSites.cpp unmatched/HeaveHo_HostSites.cpp unmatched/Lakitu_ShadowFaces.cpp ${SLICE_OV030CAST_SOURCES} hal/actor_classes_ov092.cpp ${SLICE_OV092_SOURCES} unmatched/ToxBox_HostSites.cpp unmatched/ToxBox_ShadowFaces.cpp hal/actor_classes_ov096.cpp ${SLICE_OV096_SOURCES} unmatched/Pokey_HostSites.cpp unmatched/Tornado_HostSites.cpp
    # run rel0215 wave 1 (lane cast-ov026): ov026's whole cast, level 18.
    hal/actor_classes_ov026.cpp ${SLICE_OV026_SOURCES} unmatched/Whirlpool_Render.cpp unmatched/Whirlpool_Behavior.cpp unmatched/Whirlpool_StateDispatch.cpp unmatched/WaterSuction_Behavior.cpp unmatched/WaterSuction_StateDispatch.cpp
    hal/actor_classes_ov036.cpp ${SLICE_OV036CAST_SOURCES} unmatched/FlyingCarpet_Render.cpp
    # run rel0215 wave 2 (lane cast-ov090): ov090's whole four-class water cast.
    ${SLICE_OV090_SOURCES}
    # run rel0215 wave 2 (lane cast-ov066): EYEROK (176), ov066, level 17.
    ${SLICE_OV066_SOURCES}
    hal/actor_classes_ov027.cpp ${SLICE_OV027CAST_SOURCES} unmatched/DaPgDfdr_StateDispatch.cpp unmatched/DaPgDfdr_Render.cpp unmatched/Player_BlowAway.cpp
    ${SLICE_OV074_SOURCES}
    hal/actor_classes_bbh.cpp ${SLICE149_SOURCES}
    hal/actor_classes_hmc.cpp ${SLICE172_SOURCES} unmatched/Crate_StateDispatch.cpp
    hal/actor_classes_updownlift.cpp ${SLICE173_SOURCES} unmatched/UpDownLiftBbh_Behavior.cpp
    hal/actor_classes_montymole.cpp ${SLICE174_SOURCES} unmatched/MontyMole_StateDispatch.cpp unmatched/MontyMole_1Up.cpp
    hal/actor_classes_flame.cpp ${SLICE175_SOURCES}
    hal/actor_classes_flamethrower.cpp ${SLICE_W6F_SOURCES} unmatched/Flamethrower_Behavior.c
    ${SLICE_W8FACES_SOURCES}
    hal/actor_classes_scuttlebug.cpp ${SLICE176_SOURCES} unmatched/Scuttlebug_StateDispatch.cpp
    hal/actor_classes_ov071.cpp ${SLICE_W7B_SOURCES} ${GATE_W7B_GEN} unmatched/MrI_StateDispatch.cpp unmatched/MrI_StateMains.cpp unmatched/MrI_Render.cpp
    hal/actor_classes_ov064.cpp ${SLICE177_SOURCES} unmatched/Bully_AfterClsn.cpp
    hal/actor_classes_ov064_gate178.cpp ${SLICE178_SOURCES} unmatched/Ov064Gate178_States.cpp
    hal/actor_classes_ov064_w9.cpp ${SLICEW9A_SOURCES} ${SLICEW9B_SOURCES} ${SLICEW9C_SOURCES} ${SLICEW9D_SOURCES}
    hal/actor_classes_jrb.cpp ${SLICE188_SOURCES} unmatched/Jrb_Renders.cpp
    unmatched/Unagi_StateDispatch.cpp unmatched/Unagi_BodyChain.cpp unmatched/Jrb_Aliases.cpp
    hal/actor_classes_bowserpuzzle.cpp ${SLICE179_SOURCES} unmatched/BowserPuzzle_StateDispatch.cpp unmatched/BowserPuzzle_Render.cpp
    hal/actor_classes_koopa_chuckya.cpp hal/koopa_chuckya_faces.cpp ${SLICE182_SOURCES} unmatched/KoopaChuckya_StateDispatch.cpp unmatched/Koopa_Render.cpp
    ${SLICE180_SOURCES} ${SLICE203_SOURCES} ${SLICE204_SOURCES}
    hal/actor_classes_ov085.cpp ${SLICE205_SOURCES}
    unmatched/Ov085_Behaviors.cpp unmatched/Ov085_Toad_StateSeat.cpp
    hal/actor_classes_montymolerock.cpp ${SLICE208_SOURCES}
    hal/actor_classes_ov043.cpp ${SLICE206_SOURCES} ${SLICE207_SOURCES}
    hal/actor_classes_l7.cpp ${SLICE_L7_SOURCES} ${GATE_L7_GEN} unmatched/PiranhaPlant_States.cpp
    unmatched/WaterBomb_StateDispatch.cpp unmatched/WaterBomb_State0.cpp
    unmatched/KnockDownPlank_Behavior.cpp unmatched/FallBlockWf_InitVeneer.cpp
    unmatched/StarMarker_D1.cpp
    # run rel0215 wave 1 (lane cast-sweep1): the six small-cast overlays
    ${SWEEP1_SOURCES}
    # run rel0215 wave 2 (lane cast-sweep2): the four-overlay cast sweep
    ${SWEEP2_SOURCES}
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
                   hal/romdata_loader.cpp hal/asset_root_refuse.cpp)
    target_compile_definitions(walk_window_hires PRIVATE PORT_ROM_CLEAN)
    add_dependencies(walk_window_hires romblob_ready_ww)
endif()
if(MSVC)
    target_compile_options(walk_window_hires PRIVATE /wd4311 /wd4312 /wd4068 /wd4576)
    # No ASLR for the shipped window: a disk save state (hal/lk7_persist
    # .cpp) stores absolute pointers into the exe image -- every actor's
    # vtable pointer -- and they only relocate if the image loads at the
    # SAME base on the next launch. ASLR rebases once per boot, which
    # would refuse every state written before a reboot. The header still
    # checks the base, so if anything rebases the exe anyway the state is
    # refused with a message rather than loaded onto dangling pointers.
    target_link_options(walk_window_hires PRIVATE /DYNAMICBASE:NO)
endif()


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

# ---------------------------------------------------------------------------
# THE ABI DEFECT DETECTORS
#
# port/tools/run_checks.py fires the receiver / calling-convention / vtable
# shape detectors and fails on any red, and on any VACUOUS pass (a checker
# that parsed nothing must not read as green):
#
#   aliascheck            an /alternatename that crosses the receiver boundary
#   vtspan --sweep        a host vtable array shorter than its ROM span
#   vtspan --fills        a fill loop that leaves a ROM slot unwritten
#   aritycheck            a declaration and a definition of one extern "C"
#                         symbol that disagree about how many parameters it
#                         takes (census, plus ratchets on the receiver shape
#                         and the plain-name func_* shape)
#   abicheck              a vtable thunk whose emitted `ret` breaks its slot's
#                         frame contract (needs this build's hal objects)
#
# TWO TARGETS, AND NEITHER IS IN ALL. That is deliberate. The port has no CI,
# so a target wired into ALL is the only thing that would run these for other
# people -- and turning a currently-green build red for every contributor is a
# decision for the port's owner, not for the lane that recovered the tools.
# Both are one command away:
#
#   ninja -C build/port port_checks_nobuild    the four no-build checkers
#   ninja -C build/port port_checks            + abicheck over THIS build
#
# port_checks_nobuild depends on nothing, so it runs even when the port does
# not fully link. port_checks depends on smoke_actor (which compiles
# actor_vtables.cpp and the other vtable-fill HAL objects), disassembles the
# build's hal objects with /disasm:nobytes, and runs the whole gate.
#
# THE DISASSEMBLY GOES IN THE BUILD DIRECTORY, never a fixed absolute path.
# The recovered wiring wrote to a shared C:/tmp/sm64ds-abisweep/_disasm, so
# two lanes running the suite at once would have written each other's input
# and each measured the other's build -- green, and meaningless.
find_program(PORT_PYTHON NAMES python python3 py)
if(PORT_PYTHON)
    add_custom_target(port_checks_nobuild
        COMMAND "${PORT_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/run_checks.py"
                "${PORT_REPO_ROOT_ABS}"
        WORKING_DIRECTORY "${PORT_REPO_ROOT_ABS}"
        COMMENT "port ABI checks (no build): aliascheck + vtspan + aritycheck"
        USES_TERMINAL VERBATIM)

    add_custom_target(port_abi_prove
        COMMAND "${PORT_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/abi_prove.py"
                "${PORT_REPO_ROOT_ABS}"
                --disasm-dir "${CMAKE_BINARY_DIR}/_abicheck_disasm"
        WORKING_DIRECTORY "${PORT_REPO_ROOT_ABS}"
        COMMENT "prove each ABI checker green here and red on a re-broken copy"
        USES_TERMINAL VERBATIM)

    if(MSVC)
        # gen_disasm.cmd emits ONLY /disasm:nobytes. Plain /disasm makes
        # abicheck parse zero stores; abicheck refuses a zero-fill run and
        # run_checks refuses it again, but generating the wrong format is the
        # mistake that causes it, so the generator never produces anything
        # else.
        add_custom_target(port_checks
            COMMAND "${CMAKE_COMMAND}" -E echo
                    "== generating /disasm:nobytes for abicheck =="
            COMMAND cmd /c "${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_disasm.cmd"
                    "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/_abicheck_disasm"
            COMMAND "${PORT_PYTHON}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/tools/run_checks.py"
                    "${PORT_REPO_ROOT_ABS}"
                    --disasm-dir "${CMAKE_BINARY_DIR}/_abicheck_disasm"
            DEPENDS smoke_actor
            WORKING_DIRECTORY "${PORT_REPO_ROOT_ABS}"
            COMMENT "port ABI checks (full): + abicheck over this build"
            USES_TERMINAL VERBATIM)
    endif()
else()
    message(WARNING "python not found: the port ABI check targets were not "
                    "created")
endif()

# Windows identity resources: version info + icon for the game-shaped exes a
# player can meet (the bundle ships walk_window; hires is its sibling build of
# the same sources). Resources land in .rsrc and move nothing in .text, so the
# map-reading guards are untouched. The smokes stay bare on purpose: they are
# dev tools, not shipped artifacts. Version numbers live in res/walk_window.rc
# and are bumped by the release lane next to version.json.
if(WIN32)
    enable_language(RC)
    target_sources(walk_window PRIVATE res/walk_window.rc)
    target_sources(walk_window_hires PRIVATE res/walk_window.rc)
endif()

# ===========================================================================
# run mg15 lane MP1: THE SLEEP/WAKE PROBE.
#
# runs/mg15/out/MP/FRONTIER.md section 10.2 names the ROM wireless lockstep's
# OS_SleepThread as the largest unknown in the whole radio-seam plan. These two
# targets are the experiment it asked for, and they are a matched pair on ONE
# source file: same probe, two thread implementations.
#
#   mp_sleepwake_rom  links the ROM bodies port/slice_gate10.txt already puts
#                     in every shipped binary (lines 340/347/349) and calls the
#                     sleep through the real veneer under a structured-exception
#                     guard. It MEASURES port/nfs_names_map.txt:402's claim that
#                     "OS_SleepThread would fault in this port anyway".
#   mp_sleepwake      links port/hal/os_thread.cpp instead.
#
# Both link src/func_02042778.c and src/func_0201a4d0.c AS THEMSELVES, so the
# call really is func_02042778 -> func_0201a4d0 -> OS_SleepThread(data_0209d4fc)
# and not a transcription of it. Console-only, no window, no BMP.
add_executable(mp_sleepwake tests/mp_sleepwake.cpp
    hal/os_thread.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_02042778.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_0201a4d0.c")
target_include_directories(mp_sleepwake PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/hal"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(mp_sleepwake PRIVATE ntr)
target_compile_definitions(mp_sleepwake PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(mp_sleepwake PRIVATE /wd4311 /wd4312 /wd4068)
endif()

add_executable(mp_sleepwake_rom tests/mp_sleepwake.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/OS_SleepThread.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/OS_WakeupThread.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_02057f54.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_02042778.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_0201a4d0.c")
target_include_directories(mp_sleepwake_rom PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/hal"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(mp_sleepwake_rom PRIVATE ntr)
target_compile_definitions(mp_sleepwake_rom PRIVATE SM64DS_PLATFORM_PC
    MP_PROBE_ROM_THREADS
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(mp_sleepwake_rom PRIVATE /wd4311 /wd4312 /wd4068 /EHa)
endif()

# run mg15 lane MP1: THE SEAM PROBE. The seam's solo answers and the ROM's
# four-slot fan-out, exercised with the ROM's own code (src/func_0203e0ac.c,
# src/func_0203bb60.c, src/func_0203bc7c.c and the two slot-index accessors)
# and the hosted seam faces. Console-only, no window, no BMP.
add_executable(mp_comms_seam tests/mp_comms_seam.cpp
    hal/comms_seam.cpp hal/os_thread.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_0203e0ac.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_0203bb60.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_0203bc7c.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_0203dabc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/func_0203dae4.c")
target_include_directories(mp_comms_seam PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/hal"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(mp_comms_seam PRIVATE ntr)
target_compile_definitions(mp_comms_seam PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(mp_comms_seam PRIVATE /wd4311 /wd4312 /wd4068)
endif()
