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
// src/func_ov006_020ff444.c is a matched TU and reads, in full:
//
//     #include "decl_common.h"
//     int *func_ov006_020ff444(int *t)
//     {
//         t[0] = (int)VT;
//         func_ov004_020b29c0(t);
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
//     020FF454  bl    #0x20b29c0          ; func_ov004_020b29c0
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
// func_ov006_020ff444 is one of this class's four marker-carrying override
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
// AND THE D2 SIBLING IS THE CROSS-CHECK. src/func_ov006_020ff420.c, vtable
// slot 16, stores the SAME word and spells it by its real config symbol name
// (`x[0] = (int)data_ov006_0213dbbc;`), so the resolution of VT here is
// confirmed by a second matched TU as well as by the pool. That is also why
// slot 16 is an ordinary slice line and slot 17 is this file.

extern "C" {

/* the base's D1, called with the object still holding this class's table */
void func_ov004_020b29c0(void *t);

/* Memory::Deallocate(void*, Heap*). Spelled in its Itanium form because that
   is what the matched arm9 TU defines and what every other host copy in this
   tree calls. */
void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *heap);

/* the two words the ROM's literal pool names. Both are mounted: the vtable is
   ov006 .data that port_scene_fill_pachinko2 has already filled with host
   thunks by the time any object of this class exists, and data_020a0eac is the
   arm9 game-heap pointer every D0 in the family dereferences. */
extern int data_ov006_0213dbbc[];
extern void *data_020a0eac;

void *port_mg_pachinko2_d0(void *self)
{
    int *t = (int *)self;
    /* the vptr store the ROM makes with r1 = 0x0213dbbc */
    t[0] = (int)(size_t)data_ov006_0213dbbc;
    func_ov004_020b29c0(t);
    /* ldr r1,[r1] -- the heap POINTER, not the word's address */
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}

}  /* extern "C" */
