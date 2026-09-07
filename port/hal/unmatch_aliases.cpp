// run link100 lane UNMATCH, batch 2: THE TWO NAMES THE PAINTING'S
// InitResources ASKS FOR.
//
// src/func_ov080_02126ca0.cpp (daPicGate_c::InitResources) declares its two
// arrays at namespace scope in C++ and NOT inside extern "C":
//
//     extern Disp data_ov080_02128628[];
//     extern int  data_0209caa0[];
//
// so MSVC mangles both references, while the mount publishes each under C
// linkage. Its own EXTRN list, measured under the port's own flags on
// 2026-09-07:
//
//     EXTRN ?data_ov080_02128628@@3PAUDisp@@A:BYTE
//     EXTRN ?data_0209caa0@@3PAHA:BYTE
//
// The reference and the definition are the same address and the same bytes;
// only the decoration differs, and src/ is byte-locked against mwccarm, so the
// port does what hal/pmf3_aliases.cpp already does for the same class of
// mismatch. Read that file's header for the guard argument: the LHS is a
// C++-mangled name that only this matched TU ever spells, nothing in the tree
// defines it, and the healthy shape alternatename_guard.py looks for -- LHS and
// RHS in the map at the SAME address -- is what these produce.
//
// WHY THE DISPATCH BEHIND THEM IS SAFE. The Painting's twelve state records
// are seated by port_painting_states_seat() in hal/actor_overlays.cpp, which
// verifies the mount holds the ROM's own function address, aborts on a nonzero
// adjustment word, and rewrites the function word with a host body -- and as of
// this lane it seats a __fastcall FACE rather than the bare cdecl body,
// because the Painting's three dispatchers are the one member-pointer family in
// the port that MSVC compiles as `call eax` instead of a tail jump. See the
// block beside g_painting_states.

#pragma comment(linker, "/alternatename:?data_ov080_02128628@@3PAUDisp@@A=_data_ov080_02128628")
#pragma comment(linker, "/alternatename:?data_0209caa0@@3PAHA=_data_0209caa0")

/* An object with nothing but directives is a legal linker input and its
   .drectve section is read like any other's, but this file also carries one
   real symbol so the object is never mistaken for empty by a tool that reads
   the map rather than the objs. It is called from nowhere on purpose. */
extern "C" int port_unmatch_alias_count(void) { return 2; }
