// PORT_HOST_ABI. dScMgCoin_c's own faces and its one named trap.
// Run mg5, lane CCN.
//
// Everything here was named by a LINK of the seat, not predicted. Wave 1
// produced exactly five unresolved externals across walk_window, walk_window_
// hires and smoke_player, identically on all three:
//
//     _func_020beb68                        3 objects  name-spelling, alias
//     _func_020127a4                        3 objects  a matched TU nobody
//                                                      listed -- a SLICE line,
//                                                      not a face
//     ?data_ov006_0212e3d0@@3PAHA           1 object   C++-linkage spelling
//     ?data_ov006_0212e3e8@@3PAHA           1 object   C++-linkage spelling
//     _func_ov006_020dbe9c                  1 object   NO SOURCE -- the trap
//
// This file carries four of the five. The fifth is src/func_020127a4.c, which
// is a matched arm9 TU with its own file that simply had no caller in the
// build before this seat; it is a line in port/slice_ccn.txt. That is the same
// category error port/mg_fanout_costs.txt section 10 records facegen making
// over _ZN6Memory8AllocateEji: a name being unresolved does not make it a
// face.
//
// ---- 1. THE TWO NAME-SPELLING ALIASES --------------------------------------
//
// func_020beb68 IS ov004's data_ov004_020beb68 UNDER A NAME THAT EXISTS IN NO
// CONFIG, and it is the same defect class as src/func_ov006_020e3578.c's bare
// `func_020adc74`, which port/mg_fanout_costs.txt section 6 records and which
// hal/scene_mg_faces.cpp line 232 already aliases. Three TUs in this seat
// spell it: src/func_ov006_020dcd74.c, src/func_ov006_020de1d4.c and (through
// the host copy of src/func_ov006_020de26c.cpp) this lane's
// unmatched/MgCoin_StateDispatch.cpp, which keeps the src spelling on purpose
// so the reference is not silently dropped.
//
// THE RESOLUTION IS NOT A GUESS. data_ov004_020beb68 is dScMgBase_c's own
// "current minigame scene" pointer -- src/func_ov004_020b2adc.c, the base
// constructor, writes `data_ov004_020beb68 = self` -- and every one of the
// three consumers uses it the same way: `g != 0 ? *(int *)(g + 0xa8) : 0`,
// which is the scene object's +0xa8 word that slot 0 (InitResources) sets to
// zero and slot 18 (the state reset) sets again. The reloc module for the
// address is ov004 and ov004 is the co-resident answer.
//
// data_ov006_0212e3d0 and _0212e3e8 are ordinary ov006 data symbols. They are
// unresolved only in their C++ spelling, because src/func_ov006_020dd880.c
// declares them ABOVE its `extern "C"` block and that file compiles as C++
// (it is //cpp-marked and dispatches through a thirty-six-virtual dummy
// struct, so port/CMakeLists.txt gives it LANGUAGE CXX). The ov006 mount
// defines both under the plain C name, which is what the linker's own "Hint on
// symbols that are defined and could potentially match" line says.
//
// ---- 2. THE ONE TRAP, AND IT IS A DECOMP FLOOR -----------------------------
//
// func_ov006_020dbe9c is the sixth of the ten calls vtable slot 9 (Render)
// makes. config/arm9/overlays/ov006/symbols.txt names it and sizes it 0xe0,
// and there is NO delink block covering it in
// config/arm9/overlays/ov006/delinks.txt and NO src file in either extension
// -- both checked rather than assumed. So dScMgCoin_c cannot render everything
// it renders, however the seat is written.
//
// It gets a trap rather than a transcription for the reason
// hal/scene_mg_faces.cpp's three ov004 traps do: a body that returns 0 and
// says so is honest, and a plausible one is the guess
// port/tools/inferred_stub_guard exists to refuse. Transcribing it from the
// ROM is a follow-up of the shape lane CT1 did for curling's 0x020e1854, and
// it is deliberately not this lane's, because that lane's own banner is the
// rule: the host body would have to be named port_* rather than
// func_ov006_020dbe9c, and nothing here would then satisfy the src caller.
//
// THIS CLASS'S SECOND FLOOR IS NOT HERE. func_ov006_020dd0e0 is a STATE with
// no body, and it is handled at the dispatch site in
// unmatched/MgCoin_StateDispatch.cpp, where the report can say which state was
// wanted. It has no symbol here on purpose: no host code names that address
// except the switch, so a trap would be a definition nobody calls.

#include <cstdio>

/* func_020beb68 -> data_ov004_020beb68 (bss, ov004). */
#pragma comment(linker, "/alternatename:_func_020beb68=_data_ov004_020beb68")

/* the two C++-linkage spellings of ov006 data the mount defines as C names */
#pragma comment(linker, "/alternatename:?data_ov006_0212e3d0@@3PAHA=_data_ov006_0212e3d0")
#pragma comment(linker, "/alternatename:?data_ov006_0212e3e8@@3PAHA=_data_ov006_0212e3e8")

static unsigned g_coin_trap_hits;

extern "C" unsigned port_mg_coin_trap_hits(void) { return g_coin_trap_hits; }

/* THE UNMATCHED RENDER CALLEE. Returns without doing anything and says so
   once. Declared with one argument because src/func_ov006_020de63c.c's call
   passes the scene object; a trap that took none would leave the caller's
   push in place, which is harmless on cdecl but reads as though the ROM
   passed nothing. */
extern "C" void func_ov006_020dbe9c(void *)
{
    static int said;
    ++g_coin_trap_hits;
    if (said)
        return;
    said = 1;
    std::fprintf(stderr, "  [scene] UNMATCHED ov006 body entered: "
                 "func_ov006_020dbe9c (0xe0, no delink block and no src; the "
                 "sixth call of dScMgCoin_c::Render). Returns without doing "
                 "anything. port/unmatched/MgCoin_Faces.cpp\n");
    std::fflush(stderr);
}
