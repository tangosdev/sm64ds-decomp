// PORT_HOST_ABI. dScMgPachinko_c's NAMED TRAPS: the ov006 addresses in this
// class's closure that have no delink block and no src TU, and that a SLICED
// body or the vtable fill reaches by name. Actor id 0x170, scene 368.
// Run mg5, lane PCH.
//
// READ THE "THIS FILE NOW DEFINES NO TRAP BODY" SECTION AT THE FOOT FIRST. Runs
// mg5 INTEG and PCOLL seated all six of the bodies this header derives, so the
// derivation below is history and the file is now the counter alone. It is kept
// as written rather than rewritten because the arity and address work in it is
// what the seated bodies were checked AGAINST, and it is the record of that.
//
// ---- WHY A TRAP AND NOT A BODY -------------------------------------------
//
// port/hal/scene_mg_faces.cpp section 3 is the precedent and its argument is
// not repeated here beyond the one sentence that decides it: a plausible body
// is worse than an absent one, because a plausible body is indistinguishable
// from a correct one in every check this tree runs. Each trap below is named,
// counted and reports itself once, so a run that enters one says so instead of
// producing a subtly wrong frame.
//
// THIS FILE DEFINES ONLY WHAT SOMETHING REFERENCES. Eight ov006 addresses in
// this class's closure have no source at all; six are here and two are not,
// and the split is measured rather than chosen:
//
//   0x020fefc0  VTABLE SLOT 0, size 0x42c. Referenced by no sliced TU -- it is
//               reached only through the table -- so its reference is the face
//               thunk in hal/scene_mg.cpp. THIS IS THE CLASS'S nosrc BODY, the
//               one port/mg_fanout_costs.txt section 3's nosrc column counts
//               for id 0x170, and standing this trap is what that column costs.
//   0x020fc8c0  size 0xf0,  called by src/func_ov006_020fedc4.c (slot 9)
//   0x020fcb4c  size 0x240, called by src/func_ov006_020fd17c.c and _020fd894.c
//   0x020fdaf0  size 0x250, called by src/func_ov006_020fd088.c, _020fd17c.c
//               and _020fd894.c
//   0x020fdd40  size 0x468, called by src/func_ov006_020fee24.c (slot 6)
//   0x020fb230  size 0x22c, called by src/func_ov006_020fb45c.c, AND state 2 of
//               data_ov006_0214266c
//
//   NOT HERE, because nothing names them: 0x020fd2d8 (state 2 of
//   data_ov006_02142694) and 0x020fe394 (state 1 of data_ov006_02142644). Both
//   are reached only through a mounted pair word, so there is no symbol to
//   resolve and a definition here would be an unreferenced invention. They are
//   handled at the dispatch site in unmatched/MgPachinko_StateDispatch.cpp,
//   which reports the address and calls nothing -- the treatment
//   unmatched/MgCurling_StateDispatch.cpp gave func_ov006_020e1854 before run
//   link60 lane CT1 transcribed it.
//
// ---- THE SIGNATURES ARE THE ROM'S, AND THAT IS NOT COSMETIC ---------------
//
// Run link60 lane CUR2 recorded the cost of getting this wrong: curling's two
// collision traps took (void *) where the ROM takes (self, idx), so when the
// bodies were finally transcribed the veneers dropped the index and collided
// shell 0 every time. Each arity below is checked TWICE -- against every
// sliced caller's own declaration, and against the ROM's register use in
// extracted/overlays/overlay_0006.bin at base 0x020bfec0:
//
//   020fc8c0  callers declare (void *c)          ROM reads r0 only
//   020fcb4c  callers declare (char *, int)      ROM: mov sl,r0 then r1 below
//   020fdaf0  callers declare (char *, int)      ROM: mul r4, r1, #0x38
//   020fdd40  callers declare (void *c)          ROM: mov r5,r0 only
//   020fb230  caller declares (Obj *, int)       ROM: mul r5, r1, #0x1c
//   020fefc0  vtable slot 0, the family's Init   ROM: str r0,[sp] only
//
// The two that multiply r1 by a structure stride are the ones an arity mistake
// would have cost most: they are per-entry workers over the 0x1c and 0x38
// arrays this class's state tables walk.
//
// NOTHING HERE CLAIMS A DECOMPILATION. port/tools/inferred_stub_guard has no
// row for any of these six and cannot have one: it ratchets guessed bodies in
// src/, and these are host traps in port/unmatched/ carrying this banner.
// Decompiling any of the eight is a byte-gated-tree job. Routed, not taken.

// ---- THIS FILE NOW DEFINES NO TRAP BODY, AND THAT IS THE FINISHED STATE ----
//
// Run mg5, lane PCOLL. All six of the trap bodies this file was written to
// carry have been seated, in two steps, and the counter below is kept so the
// run report's "unmatched-body traps entered" line keeps printing a measured
// zero instead of disappearing. A line that vanishes is not evidence; a line
// that prints 0 is.
//
//   lane INTEG   020fefc0  vtable slot 0 InitResources -- copy-across from
//                          main, reached through the cdecl face + alias in
//                          MgPachinko_Faces.cpp
//                020fc8c0  slot-9 render homing helper
//                020fdd40  slot-6 spawn body
//
//   lane PCOLL   020fcb4c  THE HIT REACTION, byte-matched on decomp/pach-
//                          bodies. Its three tail-call sites (0x020fd2b0 in
//                          func_ov006_020fd17c, 0x020fd864 in _020fd2d8,
//                          0x020fd9b0 in _020fd894) all landed here before.
//                020fdaf0  the proximity scan, NONMATCHING but logic-verified
//                020fb230  state 2 of data_ov006_0214266c, byte-matched on
//                          origin/main; it was BOTH a trap here and a reported
//                          floor in the dispatch switch, because it is reached
//                          two ways (by name from func_ov006_020fb45c, and
//                          through the mounted pair word)
//
// A DEFINITION HERE BESIDE A REAL ONE IS A DUPLICATE SYMBOL, which is why the
// three bodies are deleted rather than left in place next to the slice lines:
// port/slice_pch.txt now carries all three src TUs and MSVC would fail the
// link on LNK2005 for each. The counter and its accessor are NOT dead code --
// hal/scene_mg.cpp calls port_mg_pachinko_trap_hits by name at line 1421 and a
// target that dropped it would fail the link there.
//
// IF A LATER LANE ADDS A TRAP BACK, the helper this file used to carry is in
// git history at this path; it deduplicated by name pointer and printed one
// line per distinct body. Nothing about the reporting contract has changed.

static unsigned g_pch_trap_hits;

extern "C" unsigned port_mg_pachinko_trap_hits(void) { return g_pch_trap_hits; }
