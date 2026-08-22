// PORT_HOST_ABI. dScMgPanel_c's one unmatched-body trap.
// Run mg6, lane PPP. actor id 0x17c = scene 380.
//
// ---- func_ov006_02106168: A DECOMP HOLE ON THE INIT PATH ------------------
//
// 0x02106168, 0x238 bytes. config/arm9/overlays/ov006/symbols.txt NAMES it,
// which is exactly why a name-shaped check would have thought it decompiled;
// config/arm9/overlays/ov006/delinks.txt covers it with NO block, and no
// src/func_ov006_02106168.c or .cpp exists in either extension. So it is the
// func_ov006_020e1854 class of floor -- a decomp gap, not a port one -- and it
// is NOT one of this class's twenty-seven states.
//
// IT IS WORSE PLACED THAN A STATE WOULD BE. Two vtable slots call it directly,
// both adjudicated out of the ROM by this lane:
//
//   slot 0  InitResources  0x0210729c  mov r0,r4 ; bl #0x2106168
//   slot 18 state reset    0x0210729c  (the same call, in the shared tail)
//
// and its own two literals are the pair of three-entry tables the class picks
// its board layout from:
//
//   relocs.txt  from:0x0210638c kind:load to:0x0213dd4c module:overlay(6)
//   relocs.txt  from:0x02106390 kind:load to:0x0213dd58 module:overlay(6)
//
//   data_ov006_0213dd4c = { 0x0213df10, 0x0213e10c, 0x0213e1b4 }
//   data_ov006_0213dd58 = { 0x0213df30, 0x0213e140, 0x0213e1fc }
//
// read out of extracted/overlays/overlay_0006.bin at base 0x020bfec0. Which of
// the two it takes is decided by a virtual call on SLOT 35 at 0x021061e8
// (`ldr r1,[r0]; ldr r1,[r1,#0x8c]; blx r1`), the same mode gate
// func_ov006_021057f0 uses. That is the mechanism behind this class carrying
// two player-facing titles on one actor id, and it is recorded rather than
// resolved: nothing here says which mode is "Puzzle Panel" and which is
// "Puzzle Panic".
//
// THE TRAP REPORTS ITSELF AND SETS NO STATE. Standing in for a 0x238-byte
// layout builder is exactly the guess port/tools/inferred_stub_guard exists to
// refuse, and this one writes per-panel position words the render slot reads,
// so a plausible stand-in would draw a plausible-looking wrong board. The
// count is printed by hal/scene_mg.cpp's port_scene_mg_panel_hits.
//
// RETIRING IT is a decomp job and not a port one: the block wants delinking in
// config/arm9/overlays/ov006/delinks.txt and a src TU. Nothing in this lane
// can do either without leaving the port.

// ---- 1. THE CLASS VTABLE'S ITANIUM NAME -----------------------------------
//
// src/MgPuzzlePanelPuzzlePanic_Spawn.c writes the class vtable into the new
// object as `_ZTV12dScMgPanel_c`, which is the ROM's own class name -- the
// type_info the word before the table points at reads "12dScMgPanel_c" -- and
// is not a config symbol name, so it needs a face onto the mounted table. The
// address is settled by the ROM twice over:
//   config/arm9/overlays/ov006/relocs.txt
//   from:0x02107888 kind:load to:0x0213e24c module:overlay(6)
// and 0x02107888 is inside MgPuzzlePanelPuzzlePanic_Spawn (0x02107858, 0x34
// bytes); and slot 17's own literal pool at 0x021042e0 stores the same word
// into the object, as does slot 16's at 0x021042e4's sibling. This is exactly
// the shape hal/scene_mg_faces.cpp section 2 carries for _ZTV14dScMgCurling_c
// and MgLuigi_Faces.cpp section 1 for _ZTV12dScMgLuigi_c.
//
// IT IS THE ONLY SYMBOL THE FIRST LINK WAVE ASKED FOR, which is worth
// recording against port/mg_fanout_costs.txt section 10's eight-wave shape:
// the whole framework half of that account is already paid, so this class's
// closure converged in ONE wave plus this row.
#pragma comment(linker, "/alternatename:__ZTV12dScMgPanel_c=_data_ov006_0213e24c")

// ---- 2. THE TRAP ----------------------------------------------------------

#include <cstdio>

static unsigned g_panel_02106168_hits;

extern "C" void func_ov006_02106168(void *c)
{
    (void)c;
    if (++g_panel_02106168_hits == 1) {
        std::fprintf(stderr,
                     "  [scene] dScMgPanel_c TRAP: func_ov006_02106168 has no "
                     "delink block and no src file. The panel layout is not "
                     "built. (Reported once; the count is in the run "
                     "report.)\n");
        std::fflush(stderr);
    }
}

extern "C" unsigned port_mg_panel_02106168_hits(void)
{
    return g_panel_02106168_hits;
}
