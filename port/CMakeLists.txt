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
    ntr/io.cpp ntr/gx.cpp ntr/texture.cpp ntr/ppu.cpp ntr/lz77.cpp
    ntr/rt.cpp ntr/runtime.cpp ntr/fs.cpp ntr/bmd.cpp)
target_include_directories(ntr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ntr/include")
if(MSVC)
    target_compile_options(ntr PRIVATE /wd4311 /wd4312)
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

add_executable(smoke_clsn tests/smoke_clsn.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/cstd_div.c
    hal/clsn_vtable.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE8_SOURCES} ${GATE4A_GEN} ${GATE4B_GEN} ${GATE8_GEN} "${ROMDATA_C}")
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

add_executable(smoke_modelanim tests/smoke_modelanim.cpp
    hal/heap_globals.cpp hal/os_arena.cpp hal/heap_vtable.cpp hal/fs.cpp
    hal/model_host.cpp hal/gx_upload_bridge.cpp hal/anim_bridge.cpp
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES} ${SLICE4B_SOURCES}
    ${SLICE7_SOURCES} ${GATE4A_GEN} ${GATE4B_GEN} "${ROMDATA_C}")
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
    ${SLICE2_SOURCES} ${SLICE3A_SOURCES} ${SLICE3B_SOURCES})
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
    ${SLICE7_SOURCES} ${SLICE8_SOURCES} ${SLICE9_SOURCES}
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
