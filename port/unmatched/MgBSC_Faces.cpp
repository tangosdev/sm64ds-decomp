// PORT_HOST_ABI. dScMgBSC_c's vtable face. Run mg9, lane LKY.
// actor id 0x184 = scene 388, "Lucky Stars".
//
// ---- ONE ALIAS, AND IT IS THE ORDINARY KIND ------------------------------
//
// src/MgLuckyStars_Spawn.cpp is a //cpp TU and declares the class's vtable as
//
//     extern int data_ov006_0213fec8;
//
// so MSVC mangles the reference as ?data_ov006_0213fec8@@3HA -- a C++-linkage
// int -- while the ov006 mount (port/tools/ovdata.py's emission) defines the
// plain C name _data_ov006_0213fec8. That is the "C-named symbols declared at
// C++ linkage" case port/mg_fanout_costs.txt section 4 counts twenty-two of,
// and hal/scene_boot.cpp section 1 already carries twenty-three of for the star
// select. An alias is the whole fix: both sides are the same object at the same
// address and nothing about the type or the calling convention differs.
//
// THE FIRST LINK OF THIS SEAT ASKED FOR EXACTLY THREE SYMBOLS AND THIS IS THE
// ONLY ONE THAT IS A FACE. The other two were ordinary slice lines the closure
// walk had classified as already-covered -- src/func_ov004_020b5c18.cpp, which
// func_ov004_020b5ed0 calls, and src/func_ov004_020b6324.c, which slot 21 tail-
// jumps to. Both are now in port/slice_lky.txt. Section 14's "budget ONE wave
// plus its own alias row" held: one wave, one alias.
//
// facegen COULD NOT BE RUN TO GENERATE THIS AND THAT IS THE TOOL BEHAVING.
// port/mg_fanout_costs.txt section 10's third tool finding is that a failed
// link TRUNCATES walk_window.map to zero bytes, which is the universe facegen
// classifies against. The probe link that produced this symbol did exactly
// that, and facegen refused with "walk_window.map is ZERO BYTES -- a failed
// link truncates the map, and measuring against it calls every symbol
// unresolved". The refusal is correct and the row is hand-written instead. It
// is checked the other way round rather than taken on trust:
// port/tools/alternatename_guard.py runs POST-LINK over the real map on every
// build and fails if an /alternatename left-hand side is also a defined symbol,
// which is the failure mode that would make this line silently useless.

#pragma comment(linker, "/alternatename:?data_ov006_0213fec8@@3HA=_data_ov006_0213fec8")

// ---- A SECOND ALIAS, AND IT IS ov004's RATHER THAN THIS CLASS'S ----------
//
// The wave that closed the three above asked for one more, and the shape is
// identical: src/func_ov004_020b5c18.cpp is a //cpp TU declaring
// `extern int data_ov004_020bfa14;`, so the reference mangles as
// ?data_ov004_020bfa14@@3HA against the mount's plain _data_ov004_020bfa14.
// link.exe even prints the hint itself ("symbols that are defined and could
// potentially match: _data_ov004_020bfa14").
//
// IT IS HERE BECAUSE THIS SEAT IS THE FIRST TO REACH THAT BODY, not because it
// belongs to dScMgBSC_c. func_ov004_020b5ed0 -- which this class's state slots
// 9 and 10 (src/func_ov006_02124ae4.c) call -- calls func_ov004_020b5c18, and
// neither was in any wired slice before this lane. A later lane that reaches
// the same pair will find the alias already standing; an /alternatename with
// the same left and right sides declared twice is deduplicated by the linker,
// so a duplicate is harmless, and port/tools/alternatename_guard.py is the
// check that the row is doing something rather than being shadowed by a real
// definition.
#pragma comment(linker, "/alternatename:?data_ov004_020bfa14@@3HA=_data_ov004_020bfa14")

/* MSVC emits nothing for a TU that is only pragmas, and an empty object still
   carries the linker directive. This symbol exists so a reader grepping for
   who owns the alias finds a definition rather than only a comment. */
extern "C" const char port_mg_bsc_faces_marker[] = "dScMgBSC_c faces, run mg9 lane LKY";
