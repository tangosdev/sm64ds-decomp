// PORT_HOST_ABI. dScMgSnowball_c's vtable face and its three closure floors.
// Run mg11, lane SNW. Actor id 0x179 = scene 377, "Snowball Slalom".
//
// ---- 1. ONE ALIAS, AND IT IS THE ORDINARY KIND ---------------------------
//
// src/func_ov006_021295ac.cpp -- this class's constructor, the body that
// resolves the vtable section 3 of port/mg_fanout_costs.txt left blank -- is a
// //cpp TU and declares both tables as
//
//     extern void* data_ov006_0213e448;
//     extern void* data_ov006_0214000c;
//
// so MSVC mangles the references as ?data_ov006_0213e448@@3PAXA and
// ?data_ov006_0214000c@@3PAXA -- C++-linkage void pointers -- while the ov006
// mount (port/tools/ovdata.py's emission) defines the plain C names. That is
// the "C-named symbols declared at C++ linkage" case port/mg_fanout_costs.txt
// section 4 counts twenty-two of, and an alias is the whole fix: both sides
// are the same object at the same address and nothing about the type or the
// calling convention differs.
//
// ONLY ONE OF THE TWO IS HERE, and the reason is worth a line because it looks
// like an omission. The first link of this seat asked for
// ?data_ov006_0214000c@@3PAXA and NOT for the middle base's, because an
// earlier seat under dScMgSingle3DBase_c already stands an alias for
// ?data_ov006_0213e448@@3PAXA. An /alternatename with the same left and right
// sides declared twice is deduplicated by the linker, so a duplicate would be
// harmless -- but a row nothing asks for is a row nobody can tell is doing
// anything, so this file carries the one the link named.
//
// IT IS CHECKED THE OTHER WAY ROUND RATHER THAN TAKEN ON TRUST.
// port/tools/alternatename_guard.py runs POST-LINK over the real map on every
// build and fails if an /alternatename left-hand side is also a defined
// symbol, which is the failure mode that would make this line silently
// useless.
//
// facegen was NOT run to generate this, for the reason
// port/mg_fanout_costs.txt section 10 records: a failed link truncates
// walk_window.map to zero bytes, and that map is the universe facegen
// classifies against, so it refuses after a probe link. The refusal is correct
// and the row is hand-written instead.

#pragma comment(linker, "/alternatename:?data_ov006_0214000c@@3PAXA=_data_ov006_0214000c")

// ---- 2. THE THREE CLOSURE FLOORS ARE RETIRED (run mg12, lane SNO) --------
//
// Run mg11 lane SNW trapped three bodies here with count-and-return stubs,
// because no src TU existed for any of the three addresses anywhere in the
// tree.  All three are decompiled now and seated through port/slice_snw.txt
// section 17, so the stubs and their counter accessor are GONE from this file
// rather than left beside the real bodies where both could define the symbol:
//
//   func_ov006_02125f68   0x9e0  -> src/func_ov006_02125f68.c
//   func_ov006_02126ee4   0xacc  -> src/func_ov006_02126ee4.c
//   func_ov006_02126b4c   0x398  -> src/func_ov006_02126b4c.c
//
// All three are honest NONMATCHING seats and each carries its measured
// residual in its own banner; slice_snw.txt section 17 carries the summary and
// the mechanical "register names only" proof for two of them.
//
// WHAT THE TRAPS COST WHILE THEY STOOD, now that it can be stated from the
// decompiled bodies rather than guessed: the layout generator writes the whole
// course tile grid at +0x4f38 and seeds the snowball and scenery tables at
// +0xac58/+0xacd8 and +0xb358/+0xb5d8, the tile writer paints those rows into
// both screens' BG2, and the obstacle pass is what pushes the ball out of
// geometry, reflects its velocity off a contact normal and applies drag.  With
// the three trapped, the course had no tiles, no rocks and no walls.
//
// WHAT THEY NEVER COST, and this is the correction to the open question
// slice_snw.txt section 16 left standing: NONE of them advances the course.
// func_ov006_02125f68 references the scroll word +0xab6c exactly once in 617
// instructions and it is a READ (ldr r2,[r0,#0xb6c] at 0x02126388, zero stores
// anywhere in the body).  The advance is slot 6's own already-matched body at
// 0x0212870c.  See runs/mg12/out/SNO/EVIDENCE_ROLLS.txt.

/* This symbol exists so a reader grepping for who owns the alias above finds a
   definition rather than only a comment. */
extern "C" const char port_mg_snowball_faces_marker[] =
    "dScMgSnowball_c faces and floors, run mg11 lane SNW";
