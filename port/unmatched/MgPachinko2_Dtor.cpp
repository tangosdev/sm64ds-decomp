// PORT_HOST_ABI. dScMgPachinko2_c's DELETING DESTRUCTOR, vtable slot 17.
// Actor id 0x171, scene 369. Run mg6, lane LKT.
//
// ---- WHY THIS IS A HOST COPY AND NOT A SLICE LINE -------------------------
//
// This is the SIBLING of the case port/unmatched/MgPachinko_Dtor.cpp carries
// and the two files are the same defect with a different pool word. Read that
// header for the full argument; what follows is this class's own evidence,
// re-derived rather than inherited.
//
// src/_ZN16dScMgPachinko2_cD0Ev.cpp is a matched TU and reads, in full:
//
//     #include "decl_common.h"
//     int *_ZN16dScMgPachinko2_cD0Ev(int *t)
//     {
//         t[0] = (int)VT;
//         _ZN11dScMgBase_cD2Ev(t);
//         _ZN6Memory10DeallocateEPvP4Heap(t, HEAP);
//         return t;
//     }
//
// VT and HEAP are not macros. include/decl_common.h:415 declares
// `extern int VT[];` and :2958 declares `extern void*HEAP;` -- two SHARED
// placeholder names that many src TUs across nine overlays spell, each meaning
// its own class's vtable and its own heap word. There is exactly one binding
// for each in this build, in port/hal/cxx_aliases.cpp:
//
//     /alternatename:_VT=_data_ov002_021081e4      the ENEMY base vtable
//     /alternatename:_HEAP=_data_020a0eac          the game heap
//
// THE ROM SAYS WHICH TWO WORDS THIS BODY ACTUALLY WANTS, and only one of them
// is what the alias supplies. Disassembled out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0, 0x38 bytes:
//
//     020FF444  push  {r4, lr}
//     020FF448  ldr   r1, [pc, #0x24]     ; pool 020FF474 = 0x0213DBBC
//     020FF44C  mov   r4, r0
//     020FF450  str   r1, [r4]
//     020FF454  bl    #0x20b29c0          ; _ZN11dScMgBase_cD2Ev
//     020FF458  ldr   r1, [pc, #0x18]     ; pool 020FF478 = 0x020A0EAC
//     020FF45C  mov   r0, r4
//     020FF460  ldr   r1, [r1]
//     020FF464  bl    #0x203c1e8          ; Memory::Deallocate(void*, Heap*)
//     020FF468  mov   r0, r4
//     020FF46C  pop   {r4, lr}
//     020FF470  bx    lr
//
// HEAP is right -- 0x020a0eac is the word the alias already binds, and the
// ldr r1,[r1] is the dereference the src's `HEAP` (a void*, not an array)
// performs. VT IS WRONG BY A WHOLE CLASS: the pool holds 0x0213dbbc, this
// class's own vtable, and the alias would have written ov002's Enemy base
// table into a minigame scene object.
//
// So this body is excluded from port/slice_lkt.txt and hosted here with both
// addresses spelled. Nothing in src/ or include/ is touched, which is the
// PORT_HOST_ABI rule; the decl_common.h placeholder is a decomp-side question
// and is routed rather than taken.
//
// ---- THE RULING THIS BODY CARRIES ----------------------------------------
//
// _ZN16dScMgPachinko2_cD0Ev is one of this class's four marker-carrying override
// bodies. It was disassembled from the shipped overlay image and compared
// instruction for instruction with its src before being seated in any form;
// the verdict is REAL_DECOMP and the evidence is the listing above.
// port/tools/inferred_stub_adjudicated.txt carries the line.
//
// ITS RECOVERED NAME IS WRONG AND THAT IS NOT WHAT THE MARKER MEASURES. The
// src header says "dScMgPachinko2_c::OnYoshiTryEat - recovered from vtable
// slot identity"; the body is a D0 deleting destructor and slot 17 is where
// the family keeps one.
//
// AND THE D2 SIBLING IS THE CROSS-CHECK. src/_ZN16dScMgPachinko2_cD1Ev.cpp, vtable
// slot 16, stores the SAME word and spells it by its real config symbol name
// (`x[0] = (int)data_ov006_0213dbbc;`), so the resolution of VT here is
// confirmed by a second matched TU as well as by the pool. That is also why
// slot 16 is an ordinary slice line and slot 17 is this file.
//
// ---- WHY THIS COPY OUTLIVED ITS SIBLING (run linkfull wave 27, lane V3B) ---
//
// The first premise above is stale: src/_ZN16dScMgPachinko2_cD0Ev.cpp is a
// real C++ destructor now, spelling neither VT nor HEAP, and
// port/unmatched/MgPachinko_Dtor.cpp retired its copy the same night for an
// arm in its src file. This one was measured the same way and KEPT, because it
// is the last caller of the flat base destructor _ZN11dScMgBase_cD2Ev in the
// whole port: the matched D0 arm reaches the base through MSVC's
// ??1dScMgBase_c instead, so retiring both copies drops
// src/_ZN11dScMgBase_cD2Ev.cpp out of the link (measured on that night's
// build: +2 D0 rows, -1 D2 row). The base D2 needs a caller of its own before
// this copy can go.

// ---- RETIRED AS A HOST COPY (run linkfull wave 31, lane VARIANT4) ----------
//
// The base D2 keeps its caller: src/_ZN16dScMgPachinko2_cD0Ev.cpp now defines
// the ROM's flat D0 name itself, in its `#ifdef _MSC_VER` arm, and that arm makes
// the three calls this copy made, in the same order, with the same two words:
// the store of data_ov006_0213dbbc into the vptr, the flat base destructor
// _ZN11dScMgBase_cD2Ev, then Memory::Deallocate with the heap pointer read from
// data_020a0eac (dScMgBase_c's inline operator delete). The MSVC object of that
// arm and the object this file compiled to are the same instructions with the
// same relocations (runs/linkfull/out/VARIANT4/ holds both listings). So the
// name hal/scene_mg.cpp's slot-17 face calls only forwards to the ROM's body
// now, and src/_ZN11dScMgBase_cD2Ev.cpp stays linked through the ROM's own call.

extern "C" {

/* the ROM's own D0, src/_ZN16dScMgPachinko2_cD0Ev.cpp's _MSC_VER arm */
void *_ZN16dScMgPachinko2_cD0Ev(void *self);

void *port_mg_pachinko2_d0(void *self)
{
    return _ZN16dScMgPachinko2_cD0Ev(self);
}

}  /* extern "C" */
