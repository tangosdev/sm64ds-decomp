// PORT_HOST_ABI. dScMgSound_c's link faces. Run mg9, lane BOX.
// actor id 0x16f = scene 367, the "Boom Box" minigame.
//
// FOUR ALIASES AND NOTHING ELSE. This class stands in for no ROM body: its one
// floor (func_ov006_0211bc8c) is a STATE and is reported by the address switch
// in unmatched/MgSound_StateDispatch.cpp rather than trapped by a fake symbol,
// and its one broken state body is host-copied in
// unmatched/MgSound_ShadowSlot35.cpp. So there is no counting stub here and
// nothing in this seat is a guess.
//
// ---- WHY THESE FOUR ------------------------------------------------------
//
// All four are port/mg_fanout_costs.txt section 4's ORDINARY /alternatename
// case: a src TU declares a symbol the ov006 mount already defines under its
// plain C name, but declares it at C++ LINKAGE, so MSVC mangles the reference
// and the plain definition cannot satisfy it. An alias is correct because
// nothing about the symbol changes -- same address, same storage, no calling
// convention involved. Section 4's rulebook corollary about member pointers
// does not apply: none of these four is a pair.
//
//   ?_ZTV12dScMgSound_c@@3HA     src/actors/dScMgSound_c.cpp
//                                 `extern int _ZTV12dScMgSound_c;` and takes
//                                 its ADDRESS to store as the vptr
//   ?_ZTV12dScMgSound_c@@3PAXA   src/actors/dScMgSound_c.cpp (slot 16, D2)
//                                 `extern void* _ZTV12dScMgSound_c;`, the
//                                 same symbol at a different C++ type
//   ?data_ov006_0212ef7c@@3PAHA   src/actors/dScMgSound_c.cpp
//   ?data_ov006_0212ef8c@@3PAHA   src/actors/dScMgSound_c.cpp
//                                 the two int tables that body indexes by the
//                                 spacing byte at +0x5627
//
// ONE SYMBOL, TWO SPELLINGS, TWO ALIASES, and that is not redundancy: MSVC
// encodes the TYPE into the decorated name, so `extern int` and
// `extern void*` produce two different undefined symbols for one address and
// each needs its own row. hal/scene_mg_flower.cpp and
// unmatched/MgMemory2_Faces.cpp already carry exactly this pair for the
// intermediate base's table (?_ZTV19dScMgSingle3DBase_c@@3HA and @@3PAXA), which is
// why 0x0213e448 did NOT appear in this class's first link wave even though
// src/actors/dScMgSound_c.cpp declares it the same way.
//
// ---- THE RIGHT-HAND SIDES ARE THE LINKER'S OWN ANSWER --------------------
//
// Each was not chosen, it was READ. The wave-1 LNK2019 for every one of the
// four printed
//
//     Hint on symbols that are defined and could potentially match:
//       _data_ov006_0213f844          (and _0212ef7c, _0212ef8c)
//
// which is the mount's definition, one per row and no ambiguity in any of the
// four. port/tools/facegen.py was NOT used to generate this file and the
// reason is section 10's tool finding 3 rather than a preference: the link
// that produces a wall TRUNCATES walk_window.map to zero bytes, and the map is
// facegen's universe, so the natural order -- link, read the wall, run facegen
// -- cannot work twice running on one tree. A lane that wants facegen here
// should copy the map aside BEFORE the probe link.

/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEAD RHS and an UNREFERENCED left hand side: nothing in the build defines _data_ov006_0213f844, and nothing references ?_ZTV12dScMgSound_c@@3HA, so the row can never fire and nothing wants it to. */
// #pragma comment(linker, "/alternatename:?_ZTV12dScMgSound_c@@3HA=_data_ov006_0213f844")
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEAD RHS and an UNREFERENCED left hand side: nothing in the build defines _data_ov006_0213f844, and nothing references ?_ZTV12dScMgSound_c@@3PAXA, so the row can never fire and nothing wants it to. */
// #pragma comment(linker, "/alternatename:?_ZTV12dScMgSound_c@@3PAXA=_data_ov006_0213f844")
#pragma comment(linker, "/alternatename:?data_ov006_0212ef7c@@3PAHA=_data_ov006_0212ef7c")
#pragma comment(linker, "/alternatename:?data_ov006_0212ef8c@@3PAHA=_data_ov006_0212ef8c")
