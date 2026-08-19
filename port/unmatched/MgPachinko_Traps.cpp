// PORT_HOST_ABI. dScMgPachinko_c's NAMED TRAPS: the ov006 addresses in this
// class's closure that have no delink block and no src TU, and that a SLICED
// body or the vtable fill reaches by name. Actor id 0x170, scene 368.
// Run mg5, lane PCH.
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

#include <cstdio>

static unsigned g_pch_trap_hits;

static void pch_trap(const char *name)
{
    static const char *said[8];
    static int nsaid;
    ++g_pch_trap_hits;
    for (int i = 0; i < nsaid; ++i)
        if (said[i] == name)
            return;
    if (nsaid < 8)
        said[nsaid++] = name;
    std::fprintf(stderr, "  [scene] UNMATCHED ov006 body entered: %s "
                 "(dScMgPachinko_c; returns without acting; "
                 "port/unmatched/MgPachinko_Traps.cpp)\n", name);
    std::fflush(stderr);
}

extern "C" unsigned port_mg_pachinko_trap_hits(void) { return g_pch_trap_hits; }

extern "C" {

/* Run mg5, lane INTEG: three of these six were recovered and are now seated, so
   their traps are gone:
     func_ov006_020fefc0  vtable slot 0 InitResources -- copy-across from main,
                          reached through the cdecl face + alias in
                          MgPachinko_Faces.cpp.
     func_ov006_020fc8c0  slot-9 render homing helper -- sliced, its sliced
                          caller now links to the real body by name.
     func_ov006_020fdd40  slot-6 spawn body -- sliced, same.
   The remaining three stay trapped: they are the floors this seat deliberately
   does not bring (020fcb4c is called by 020fd2d8 which has no body on main;
   020fdaf0 is the homing-seed worker recovered only on the pach-bodies tip, not
   in this lane's set; 020fb230 is a state floor the dispatch reports). */
void func_ov006_020fcb4c(char *, int)     { pch_trap("func_ov006_020fcb4c"); }
void func_ov006_020fdaf0(char *, int)     { pch_trap("func_ov006_020fdaf0"); }
void func_ov006_020fb230(void *, int)     { pch_trap("func_ov006_020fb230"); }

}  /* extern "C" */
