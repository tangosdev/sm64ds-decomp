// PORT_HOST_ABI. dScMgBomroom_c's one name-spelling alias. Run mg6, lane SOS.
//
// Named by a LINK of the seat, not predicted. Wave 1 produced EXACTLY ONE
// unresolved external across walk_window, walk_window_hires and smoke_player,
// identically on all three:
//
//     _data_ov090_021343b0   3 objects   referenced in func_ov006_020d5e5c
//
// ---- IT IS A WRONG-MODULE SPELLING, AND THE ROM SETTLES IT -----------------
//
// ov006 and ov090 share an address window, so 0x021343b0 is a live symbol in
// BOTH config trees:
//
//     config/arm9/overlays/ov090/symbols.txt:108  data_ov090_021343b0
//     config/arm9/overlays/ov006/symbols.txt:2381 data_ov006_021343b0
//
// An address alone therefore does not name a symbol -- which is the same
// finding lane CCN's closure walker recorded when resolving by address across
// every symbol file reported the overlay window four times over. THE
// RELOCATION'S OWN MODULE FIELD DECIDES, and it is unambiguous:
//
//     config/arm9/overlays/ov006/relocs.txt
//     from:0x020d5eb4 kind:load to:0x021343b0 module:overlay(6)
//
// 0x020d5eb4 is the literal pool of func_ov006_020d5e5c itself (0x020d5e5c,
// size 0x5c), so the TU's own load names ov006. And ov090 is not co-resident
// with a minigame scene in any case: the resident set for scene 370 is main +
// itcm + ov001 + ov004 + ov006, which is what makes the mount define the ov006
// name and nothing define the ov090 one.
//
// So this is a defect in src/func_ov006_020d5e5c.c's declaration -- one of the
// wrong-overlay crossings the decomp tracks -- and NOT something for this seat
// to work around by inventing storage. The alias points the misspelling at the
// symbol the ROM's own relocation names. It is the same shape and the same
// ruling as unmatched/MgCoin_Faces.cpp's _func_020beb68 row and
// hal/scene_mg_faces.cpp's bare `func_020adc74`.
//
// THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN. src/func_ov006_020d5e5c.c should
// declare data_ov006_021343b0, and whether it still builds byte-identically
// under mwccarm is a byte-gated-tree question, not a port one. src/ is not
// edited here.
//
// AN ALIAS AND NOT A HOST COPY, deliberately: the left-hand side is a symbol
// nothing in this build DEFINES, which is the condition
// port/tools/alternatename_guard.py checks post-link. A defined LHS defeats an
// /alternatename silently, and that is exactly the class the guard exists to
// catch; this one has no definition anywhere, on any target.

/* data_ov090_021343b0 -> data_ov006_021343b0. Same address, and the ov006
   spelling is the one the relocation names and the mount defines. */
#pragma comment(linker, "/alternatename:_data_ov090_021343b0=_data_ov006_021343b0")
