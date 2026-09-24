// PORT_HOST_ABI. dScMgPachinko_c's DELETING DESTRUCTOR, vtable slot 17.
// Actor id 0x170, scene 368. Run mg5, lane PCH.
//
// ---- WHY THIS IS A HOST COPY AND NOT A SLICE LINE -------------------------
//
// src/_ZN15dScMgPachinko_cD0Ev.cpp is a matched TU and reads, in full:
//
//     #include "decl_common.h"
//     int *_ZN15dScMgPachinko_cD0Ev(int *t)
//     {
//         t[0] = (int)VT;
//         _ZN11dScMgBase_cD2Ev(t);
//         _ZN6Memory10DeallocateEPvP4Heap(t, HEAP);
//         return t;
//     }
//
// VT and HEAP are not macros. include/decl_common.h:415 declares
// `extern int VT[];` and :2958 declares `extern void*HEAP;` -- two SHARED
// placeholder names that twenty-three src TUs across nine overlays spell, each
// meaning its own class's vtable and its own heap word. There is exactly one
// binding for each in this build, in port/hal/cxx_aliases.cpp:1231:
//
//     /alternatename:_VT=_data_ov002_021081e4      the ENEMY base vtable
//     /alternatename:_HEAP=_data_020a0eac          the game heap
//
// and the comment above those two rows says in as many words that "these names
// are SINGLE in this build ... a second VT-spelling TU cannot join a target
// that links this reader, its store would land on the Enemy's objects".
//
// THE ROM SAYS WHICH TWO WORDS THIS BODY ACTUALLY WANTS, and only one of them
// is what the alias supplies. Disassembled out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0, 0x38 bytes:
//
//     020fa780  push  {r4, lr}
//     020fa784  ldr   r1, [pc, #0x24]     ; pool 020fa7b0 = 0x0213d9cc
//     020fa788  mov   r4, r0
//     020fa78c  str   r1, [r4]
//     020fa790  bl    0x20b29c0           ; _ZN11dScMgBase_cD2Ev
//     020fa794  ldr   r1, [pc, #0x18]     ; pool 020fa7b4 = 0x020a0eac
//     020fa798  mov   r0, r4
//     020fa79c  ldr   r1, [r1]
//     020fa7a0  bl    0x203c1e8           ; Memory::Deallocate(void*, Heap*)
//     020fa7a4  mov   r0, r4
//     020fa7a8  pop   {r4, lr}
//     020fa7ac  bx    lr
//
// HEAP is right -- 0x020a0eac is the word the alias already binds, and the ldr
// r1,[r1] is the dereference the src's `HEAP` (a void*, not an array) performs.
// VT IS WRONG BY A WHOLE CLASS: the pool holds 0x0213d9cc, dScMgPachinko_c's
// own vtable, and the alias would have written ov002's Enemy base table into a
// minigame scene object.
//
// So this body is excluded from port/slice_pch.txt and hosted here with both
// addresses spelled. Nothing in src/ or include/ is touched, which is the
// PORT_HOST_ABI rule; the decl_common.h placeholder is a decomp-side question
// and is routed rather than taken.
//
// THE SAME DEFECT IS LIVE ON CURLING'S SEAT TODAY and this lane did not fix it,
// because it is another lane's file. src/_ZN14dScMgCurling_cD0Ev.cpp is
// dScMgCurling_c's slot 17, is byte-for-byte the same four statements, IS in
// port/slice_mg1.txt line 151, and has no per-source -D in port/CMakeLists.txt.
// Its own ROM pool holds 0x0213c304 (curling's table). So on a scene 374 boot
// the Enemy base vtable is written into a dScMgCurling_c whenever slot 17 runs.
// It is a store into a dying object between two direct calls -- the same
// reading hal/actor_classes_ov063.cpp gave the BookShotSpawner case, which
// cxx_aliases.cpp then overruled with "the store costs nothing to make
// correct" -- and it is reported rather than repaired from here.
//
// ---- THE RULING THIS BODY CARRIES ----------------------------------------
//
// _ZN15dScMgPachinko_cD0Ev is one of this class's three marker-carrying override
// bodies. It was disassembled from the shipped overlay image and compared
// instruction for instruction with its src before being seated in any form;
// the verdict is REAL_DECOMP and the evidence is the listing above.
// port/tools/inferred_stub_adjudicated.txt carries the line.
//
// ITS RECOVERED NAME IS WRONG AND THAT IS NOT WHAT THE MARKER MEASURES. The
// src header says "dScMgPachinko_c::OnYoshiTryEat - recovered from vtable slot
// identity"; the body is a D0 deleting destructor and slot 17 is where the
// family keeps one. That is the w16 finding restated: the marker records how
// the NAME was recovered, not where the BODY came from.

// ---- RETIRED AS A HOST COPY (run linkfull wave 27, lane V3B) --------------
//
// Everything above is the history of the copy that stood here, and its first
// premise no longer holds: src/_ZN15dScMgPachinko_cD0Ev.cpp is not the
// decl_common.h text quoted above any more. It is a real C++ destructor,
// `dScMgPachinko_c::~dScMgPachinko_c() {}`, which spells neither VT nor HEAP,
// and under _MSC_VER it now defines the ROM's flat D0 name itself: the D1 body
// through the one host destructor src/_ZN15dScMgPachinko_cD1Ev.cpp defines
// (??1dScMgPachinko_c), then dScMgBase_c's inline operator delete,
// Memory::Deallocate with the game heap word at 0x020a0eac -- the ROM body's
// own two steps. So the name hal/scene_mg.cpp's slot-17 face calls only
// forwards to the ROM's body now. The retired copy stored the ROM-shaped port
// table data_ov006_0213d9cc before the base destructor; the matched body's
// ??1dScMgPachinko_c stores MSVC's own vftable there instead, which the ROM
// body (0x020fa780: the store, then the base D2 with nothing between) leaves
// inert, and tools/dtor_store_guard.py's SRC_ARMS re-proves that from the
// cartridge on every build. The class's slot 16 already ran that same MSVC
// destructor. runs/linkfull/out/V3B/ holds the object check.

extern "C" {

/* the ROM's own D0, src/_ZN15dScMgPachinko_cD0Ev.cpp's _MSC_VER arm */
void *_ZN15dScMgPachinko_cD0Ev(void *self);

void *port_mg_pachinko_d0(void *self)
{
    return _ZN15dScMgPachinko_cD0Ev(self);
}

}  /* extern "C" */
