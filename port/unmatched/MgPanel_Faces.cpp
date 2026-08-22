// PORT_HOST_ABI. dScMgPanel_c's vtable face. Run mg6 lane PPP seated the class;
// run mg7 lane L380 retired the one trap this file used to carry.
// actor id 0x17c = scene 380.
//
// ---- THE TRAP IS GONE, AND WHAT IT WAS HIDING IS WORTH KEEPING -------------
//
// This file used to define func_ov006_02106168 as a counting stub, because the
// body had a config symbol, no delink block in
// config/arm9/overlays/ov006/delinks.txt and no src file in either extension.
// src/func_ov006_02106168.cpp now exists and byte-matches at mwccarm 1.2/sp2p3
// (135 instructions + 7 pool words = 142 = 0x238/4), so the body is an
// ordinary line in port/slice_ppp.txt and nothing here stands in for it.
//
// WHAT IT DOES, now that it can be read rather than guessed at. Two vtable
// slots call it directly, both adjudicated out of the ROM by lane PPP and both
// re-checked here:
//
//   slot 0  InitResources  func_ov006_021073b0, bl #0x2106168 at 0x021077a4
//   slot 18 state reset    func_ov006_021071fc, bl #0x2106168 at 0x0210729c
//
// and it is the sole writer of the two panel arrays func_ov006_021067a4 zeroes
// just before it runs: the CURRENT face at c+0x4f1e+i and the TARGET face at
// c+0x4f42+i. It also writes every panel's fixed-point position (c+0x4cc4+i*4,
// c+0x4d54+i*4) and the c+0x4cc0 shuffle moves it records at c+0x4fae.
//
// THE MODE GATE PICKS THE FACE SET, NOT THE POSITIONS, and the earlier version
// of this comment had that wrong. It said the slot-35 dispatch at 0x021061e8
// chose between data_ov006_0213dd4c and data_ov006_0213dd58. It does not:
// those two are read UNCONDITIONALLY, both indexed by [c+0x4cbc]-4 -- the board
// side length minus four -- and they are the x and y position tables, u16 per
// panel, shifted left twelve. The gate chooses between two OTHER tables that
// the earlier comment did not name at all:
//
//   relocs.txt  from:0x02106384 kind:load to:0x0213e070   gate == 0
//   relocs.txt  from:0x02106388 kind:load to:0x0213ded0   gate != 0
//
// both indexed by c+0x4cb4, the face-set number func_ov006_021063a0 rolls, and
// both feeding the two panel arrays. That is the mechanism behind this class
// carrying two player-facing titles on one actor id, and it is still recorded
// rather than resolved: nothing here says which mode is "Puzzle Panel" and
// which is "Puzzle Panic".
//
// The virtual call needs no help from this file. src/func_ov006_02106168.cpp
// dispatches slot 35 through the seated table the same way
// src/func_ov006_021063a0.cpp already does, and by the time either runs
// port_scene_fill_panel has replaced that slot's raw DS word with
// hal/scene_mg.cpp's mb_v35 thunk.

// ---- THE CLASS VTABLE'S ITANIUM NAME ---------------------------------------
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
