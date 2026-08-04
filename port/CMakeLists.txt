cmake_minimum_required(VERSION 3.16)
project(sm64ds-port C CXX)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MAP")
add_compile_options(/Oy-)

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
set(ROMDATA_C "${CMAKE_BINARY_DIR}/host-src/romdata.c")
add_custom_command(OUTPUT "${ROMDATA_C}"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/romdata.py" "${ROMDATA_C}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/romdata.py"
    COMMENT "romdata from extracted/arm9_dec.bin")

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

# Two Actor entry points are C++ inside a .c, but a different shape from the
# hostgen cases below: each is a real METHOD against Actor.h, so hostgen's
# wrap is wrong for them -- it would put the whole file, Actor.h include and
# all, inside extern "C". They compile as C++ instead, and
# hal/method_faces.cpp carries the C-linkage faces their callers spell. Both
# were plain C free functions until main rewrote them as methods.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN5Actor13SpawnSoundObjEj.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/_ZN5Actor24KillAndTrackInDeathTableEv.c"
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
set(GATE10_SYMS func_ov002_020dec70 func_ov002_020d708c
    engine/fader/StartExitFaderWipe func_ov002_020c897c
    engine/fader/StartEntranceFaderWipe)
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

set(OV002DATA_C "${CMAKE_BINARY_DIR}/host-src/ov002_data.c")
add_custom_command(OUTPUT "${OV002DATA_C}"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ov002
            "${OV002DATA_C}" --from-list "${CMAKE_CURRENT_SOURCE_DIR}/ov002_syms.txt"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/ov002_syms.txt"
    COMMENT "ovdata ov002")

# ov009 is the castle-grounds LEVEL overlay, and a level overlay cannot be
# mounted symbol by symbol: its LVL_Overlay hangs nine object tables off one
# base and the loaders walk them by count, straight across whatever symbol
# boundaries dsd happened to name. --whole emits the entire image as one
# array and rebases its internal pointers from the reloc table.
set(OV009DATA_C "${CMAKE_BINARY_DIR}/host-src/ov009_data.c")
add_custom_command(OUTPUT "${OV009DATA_C}"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ov009
            "${OV009DATA_C}" --whole
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py"
    COMMENT "ovdata ov009 (whole image)")

# Gate 17: the level overlay's own actors. ov009 is mounted WHOLE for the
# loaders; its actor code reaches its SharedFilePtrs, CLPS blocks and spawn
# tables BY NAME, so those symbols are mounted a second time per symbol.
# --pack keeps the runs that are really one array contiguous -- ov009's water
# spawn points are three seven-element Vector3 arrays that dsd split into
# twenty-one symbols and func_ov009_02111b1c walks by index.
set(OV009SYMS_C "${CMAKE_BINARY_DIR}/host-src/ov009_syms.c")
add_custom_command(OUTPUT "${OV009SYMS_C}"
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ov009
            "${OV009SYMS_C}" --from-list
            "${CMAKE_CURRENT_SOURCE_DIR}/ov009_syms.txt" --pack
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/ov009_syms.txt"
    COMMENT "ovdata ov009 (per symbol, packed)")

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
set(PORT_ACTOR_OVERLAYS ov085 ov098 ov100 ov089 ov102)
set(ACTOR_OV_SYMS "")
foreach(ovn IN LISTS PORT_ACTOR_OVERLAYS)
    set(ovn_out "${CMAKE_BINARY_DIR}/host-src/${ovn}_syms.c")
    add_custom_command(OUTPUT "${ovn_out}"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ${ovn}
                "${ovn_out}" --from-list
                "${CMAKE_CURRENT_SOURCE_DIR}/${ovn}_syms.txt" --pack
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py"
                "${CMAKE_CURRENT_SOURCE_DIR}/${ovn}_syms.txt"
        COMMENT "ovdata ${ovn} (per symbol, packed)")
    list(APPEND ACTOR_OV_SYMS "${ovn_out}")
endforeach()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/slice_gate18.txt" SLICE18_LINES)
set(SLICE18_SOURCES "")
foreach(line IN LISTS SLICE18_LINES)
    string(STRIP "${line}" line)
    if(line MATCHES "^#" OR line STREQUAL "")
        continue()
    endif()
    list(APPEND SLICE18_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
endforeach()

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

# gate 10 minus the two files the camera build replaces
set(SLICE10_CAM_SOURCES "")
foreach(src IN LISTS SLICE10_SOURCES)
    if(src MATCHES "/ChangeArea\\.c$" OR src MATCHES "/func_0200cae4\\.cpp$")
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
       OR src MATCHES "/_ZN5Sound4PlayEjjRK7Vector3\\.c$")
        continue()
    endif()
    list(APPEND SLICE10_CAM_SOURCES "${src}")
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
    unmatched/func_0204335c.cpp
    unmatched/func_02043288.cpp
    unmatched/func_0204322c.cpp
    unmatched/func_02043fdc.cpp
    unmatched/func_02016ff4.cpp
    unmatched/ActorBase_AfterCleanupResources.cpp
    unmatched/func_ov002_020c6f3c.cpp
    # gate 16: two more mwcc pointer-to-member dispatches, both in actor
    # classes the registry carries. MSVC forms a PMF over an INCOMPLETE class
    # as the four-word general representation, which doubles the stride of the
    # table it indexes; see each file's header.
    unmatched/OneUpMushroom_Behavior.cpp
    unmatched/NumStars.cpp
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
    # gate 22: the sixth, plus the Model slot the dual fill cannot serve
    unmatched/Door_Behavior.cpp
    unmatched/Door_Render.cpp
    # gate 23: the seventh, and the last on this level
    unmatched/QuestionBlock_States.cpp
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
    list(APPEND SLICE14_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../${line}")
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
    func_ov002_020aefb8)
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

# The camera slice is on both binaries. The gate-13 State objects carry real
# DS function addresses, and Player::ChangeState already reaches
# func_0200d81c -> Camera::ChangeState on every state change, so the smoke
# needs the host dispatcher too or the first ChangeState calls a mwcc
# member-function pointer.
add_executable(smoke_player tests/smoke_player.cpp "${OV002DATA_C}" "${OV009DATA_C}"
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/fader_wipes.cpp hal/level_boot.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/stage_bridges.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES} ${SLICE26_SOURCES}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN}
    "${ROMDATA_C}" "${OV009SYMS_C}" ${ACTOR_OV_SYMS})
target_include_directories(smoke_player PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(smoke_player PRIVATE ntr)
target_compile_definitions(smoke_player PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(smoke_player PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# Gate 12: the interactive window (same stack, windowed loop).
add_executable(walk_window tests/walk_window.cpp "${OV002DATA_C}" "${OV009DATA_C}"
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/fader_wipes.cpp hal/level_boot.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/sub_screen.cpp hal/stage_bridges.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES}
    ${SLICE25_SOURCES} ${SLICE26_SOURCES} ${GATE6_GEN}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN}
    "${ROMDATA_C}" "${OV009SYMS_C}" ${ACTOR_OV_SYMS})
target_include_directories(walk_window PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
# 2x (512x384, window ZOOM 2): the software raster is fill-bound; the
# hires 1024x768 tier burns ~40ms/frame on the level alone, native
# 256x192 reads too chunky. 2x is ~12ms -> 60fps with frame pacing.
target_link_libraries(walk_window PRIVATE ntr_2x)
target_compile_definitions(walk_window PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(walk_window PRIVATE /wd4311 /wd4312 /wd4068)
endif()

# hires clone for screenshot sessions (4x resolution, ~1/16 the fps)
add_executable(walk_window_hires tests/walk_window.cpp "${OV002DATA_C}" "${OV009DATA_C}"
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp hal/actor_vtables.cpp hal/anim_bridge.cpp
    hal/cxxname_bridge.cpp hal/megachar_stub.cpp hal/player_bridges.cpp hal/cxx_aliases.cpp hal/auto_bss.cpp hal/reverse_bridges.cpp hal/method_faces.cpp hal/fader_wipes.cpp hal/level_boot.cpp hal/actor_registry.cpp hal/actor_classes.cpp hal/actor_class_faces.cpp hal/mmc_vtable.cpp hal/ov009_boot.cpp hal/actor_overlays.cpp hal/sub_screen.cpp hal/stage_bridges.cpp
    ${CAMERA_HOST_SOURCES} ${LEVELBOOT_HOST_SOURCES} ${SOUND_HOST_SOURCES}
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_CAM_SOURCES}
    ${SLICE10_CAM_SOURCES} ${SLICE13_SOURCES} ${SLICE14_SOURCES}
    ${SLICE15_SOURCES} ${SLICE16_SOURCES} ${SLICE17_SOURCES}
    ${SLICE18_SOURCES} ${SLICE19_SOURCES} ${SLICE20_SOURCES}
    ${SLICE21_SOURCES} ${SLICE22_SOURCES} ${SLICE23_SOURCES}
    ${SLICE25_SOURCES} ${SLICE26_SOURCES} ${GATE6_GEN}
    ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} ${GATE10_GEN}
    ${GATE13_GEN} ${GATE14_GEN} ${GATE15_GEN} ${GATE16_GEN}
    "${ROMDATA_C}" "${OV009SYMS_C}" ${ACTOR_OV_SYMS})
target_include_directories(walk_window_hires PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(walk_window_hires PRIVATE ntr_hires)
target_compile_definitions(walk_window_hires PRIVATE SM64DS_PLATFORM_PC
    PORT_REPO_ROOT="${PORT_REPO_ROOT_ABS}")
if(MSVC)
    target_compile_options(walk_window_hires PRIVATE /wd4311 /wd4312 /wd4068)
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
