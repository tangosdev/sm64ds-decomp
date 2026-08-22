// PORT_HOST_ABI. dScMgPachinko2_c's ONE NAME-SPELLING FACE. Actor id 0x171,
// scene 369. Run mg6, lane LKT.
//
// It is not a body and it does not change a calling convention. It is the
// ordinary /alternatename kind: a reference spelled one way and a definition
// that exists under another name, in the same link. It lives in this lane's
// own file rather than in hal/scene_mg_faces.cpp so the seat is additive, the
// shape unmatched/MgPachinko_Faces.cpp and MgLuigi_Faces.cpp established.
//
// THE LHS IS UNDEFINED EVERYWHERE ELSE IN THE BUILD, which is the condition
// port/tools/alternatename_guard.py watches: a defined LHS defeats the alias
// silently. It needs no row in tools/alternatename_baseline.txt, because that
// file is the allowlist of DELIBERATELY defeated pairs and this one is not
// defeated.
//
// ---- THE CLASS VTABLE SYMBOL ---------------------------------------------
//
// src/MgLakituLaunch_Spawn.c ends with `p[0] = (int)_ZTV16dScMgPachinko2_c;`
// and nothing in this tree defines that symbol -- the table is mounted ov006
// data under its address name, data_ov006_0213dbbc. This is the same shape
// hal/scene_mg_faces.cpp already carries for curling
// (__ZTV14dScMgCurling_c = _data_ov006_0213c304) and MgPachinko_Faces.cpp for
// the sibling class, and it is a REAL identity claim rather than bookkeeping.
// The ROM ties the mangled name to the address TWICE:
//
//   1. The factory at 0x02104258 loads 0x0213dbbc out of its own literal pool,
//      and that is its only ov006 load --
//      config/arm9/overlays/ov006/relocs.txt
//          from:0x02104288 kind:load to:0x0213dbbc module:overlay(6)
//   2. The RTTI string "16dScMgPachinko2_c" sits at 0x0213db58 in
//      extracted/overlays/overlay_0006.bin, 0x64 bytes before the table, which
//      is the same RTTI-then-table adjacency the sibling has at 0x0213d960 /
//      0x0213d9cc.
//
// It is NOT a per-TU placeholder guess of the VT0/VT1/VT2 kind. The factory
// spells the Itanium vtable name in full, and port_scene_fill_pachinko2 has
// already replaced every one of that table's thirty-six words with a host
// thunk by the time any object of this class exists.
//
// ---- AND WHY THERE IS ONLY ONE ROW ---------------------------------------
//
// The sibling class needed a second: src/func_ov006_020fb97c.c spells
// MultiStore16 as bare func_0205a448, a name that exists nowhere. This class's
// closure has no such row -- the link over the whole 63-line slice returned
// exactly one unresolved external, this one -- and that is a measurement
// rather than an assumption: the wave-2 link log is
// runs/mg6/out/LKT/build_seat1.log.

#pragma comment(linker, "/alternatename:__ZTV16dScMgPachinko2_c=_data_ov006_0213dbbc")

/* MSVC will not emit an object for a translation unit that defines nothing,
   and a dropped object takes its linker directives with it. */
extern "C" int port_mg_pachinko2_faces_anchor(void) { return 0x171; }
