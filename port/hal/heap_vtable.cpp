// Host globals and the synthetic ExpandingHeap vtable.
//
// THE VTABLE PROBLEM: the C-spelled ctor installs `&_ZTV13ExpandingHeap`
// as the object's vptr -- the Itanium vtable symbol, slot array starting at
// the symbol itself. C++ callers then dispatch through shadow-class casts
// (`((Base*)this)->m(...)`), which MSVC compiles as a __thiscall through a
// numbered slot. So the HAL provides _ZTV13ExpandingHeap as a real slot
// array whose entries are __fastcall shims (ecx carries `this` exactly as
// __thiscall passes it; the dummy edx parameter absorbs fastcall's second
// register) forwarding to the real V-methods from src/.
//
// Slot evidence, from the shadow classes in the callers themselves:
//   Heap::Allocate    casts to {v0,v1,v2, m(uint,int)}  -> slot 3 = VAllocate
//   Heap::Deallocate  casts to {v0..v3,   m(void*)}     -> slot 4 = VDeallocate
// Slots without caller evidence are TRAPS that abort loudly with the slot
// number -- a silent wrong-slot dispatch is the exact class of bug the
// hybrid's gates existed to catch, and the port has no gate to catch it.
#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "dsstate_seg.h"

typedef unsigned int u32;

// Shadow declarations that mangle identically to the src/ definitions.
struct ExpandingHeap {
    void *VAllocate(u32 size, int align);
    /* VDeallocate's shadow member is RETIRED at FACES4: see the block below. */
    void *VReallocate(void *p, u32 size);
    u32 VSizeof(void *p);
    /* LINKAGE SEAT: the three self-contained V-methods (no callees at all).
       Their matched TUs ride gate 3a, define these as real MSVC methods on
       the same struct, and the shadow resolves to them with no alias
       machinery -- naming them in the slots below is the reference edge. */
    bool VIntact();
    void VRescue();
    u32 VResizeToFit();
    /* LINKAGE SEAT 2: the NodeID pair. Real matched forwarders to the
       allocator's own (flat-C) NodeID accessors, bridged below. */
    u32 VGetNodeID();
    void VSetNodeID(u32 id);
};
struct ExpandingHeapAllocator {
    void *Allocate(u32 size, int align);
    void *Reallocate(void *p, u32 size);
    static u32 SizeofInternal(void *p);
    u32 MemoryLeft();
    u32 GetNodeID();
    void SetNodeID(u32 id);
};

// ---- globals the root-heap chain stores through --------------------------
//
// data_02099d90 is _ZTV4Heap, the BASE class's vtable, and it was hosted here
// as a four-byte int labelled "heap bring-up state flag". That label was
// wrong. Three matched TUs store its ADDRESS into an object's vptr word at
// +0x00 -- src/_ZN4HeapC2EPvjPS_.cpp (`heap->vtable = &data_02099d90`),
// src/_ZN4HeapD2Ev.cpp and src/_ZN4HeapD1Ev.cpp -- and nothing anywhere reads it
// as a value. config/arm9/relocs.txt settles the shape:
//
//   from:0x02099d90 to:0x0203ca44   _ZN4HeapD1Ev   (slot 0, complete dtor)
//   from:0x02099d94 to:0x0203ca20   _ZN4HeapD0Ev   (slot 1, deleting dtor)
//   0x02099d98..0x02099dcc          no relocations -- zero words
//
// so the ROM's table is the same SIXTEEN-slot Heap shape _ZTV13ExpandingHeap
// carries below, with two real bodies and fourteen null slots (Heap declares
// the rest pure virtual; see the class in src/_ZN4Heap11ResizeToFitEv.cpp).
// 0x02099d90 + 16*4 + 8 lands exactly on _ZTV13ExpandingHeap at 0x02099dd8,
// which is the sizing check.
//
// The storage is the right SHAPE here, for every target, so a stray dispatch
// cannot run off the end of it; the CONTENTS are seated on the walk_window
// family in hal/lk4_eh_dtor_seat.cpp, where the matched dtor pair rides.
// Targets without that seat keep the zeros, which is what they had when this
// was an int.
DSSTATE_BEGIN
extern "C" {
void *data_020a0e9c;   /* Heap::rootHeap */
void *data_020a0ea0;   /* Memory::defaultHeapPtr */
void *data_02099d90[16];   /* _ZTV4Heap */
}
DSSTATE_END

// ---- allocator methods -> the C-linkage definitions from gate 2 ----------
extern "C" {
void *_ZN22ExpandingHeapAllocator8AllocateEji(void *self, u32 size, int align);
int _ZN22ExpandingHeapAllocator10DeallocateEPv(void *self, void *p);
u32 _ZN22ExpandingHeapAllocator14SizeofInternalEPv(void *p);
u32 _ZN22ExpandingHeapAllocator10MemoryLeftEv(void *self);
}
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): main's tree defines these
   three as real ExpandingHeapAllocator members now, so src/ emits
   ?Allocate@ExpandingHeapAllocator@@QAEPAXIH@Z,
   ?SizeofInternal@ExpandingHeapAllocator@@SAIPAX@Z and
   ?MemoryLeft@ExpandingHeapAllocator@@QAEIXZ itself and these faces were the
   second definition (LNK2005). The declarations above stay: other rows in this
   file still reach the flat names.
void *ExpandingHeapAllocator::Allocate(u32 size, int align)
{ return _ZN22ExpandingHeapAllocator8AllocateEji(this, size, align); }
u32 ExpandingHeapAllocator::SizeofInternal(void *p)
{ return _ZN22ExpandingHeapAllocator14SizeofInternalEPv(p); }
u32 ExpandingHeapAllocator::MemoryLeft()
{ return _ZN22ExpandingHeapAllocator10MemoryLeftEv(this); }                  */
/* LINKAGE SEAT 2: the NodeID accessors. The V-method TUs call these as C++
   methods while the definitions are flat C (self on the stack), the same
   two-name-space bridge Allocate and MemoryLeft take above. The flat
   SetNodeID returns the OLD id; the ROM's method face is void, so it drops. */
extern "C" u32 _ZN22ExpandingHeapAllocator9GetNodeIDEv(void *self);
extern "C" int _ZN22ExpandingHeapAllocator9SetNodeIDEj(void *self, u32 id);
/* RETIRED at ALIAS2, same reason: src/ defines
   ?GetNodeID@ExpandingHeapAllocator@@QAEIXZ itself now.  SetNodeID below is
   NOT retired: its src TU still defines only the flat C name.
u32 ExpandingHeapAllocator::GetNodeID()
{ return _ZN22ExpandingHeapAllocator9GetNodeIDEv(this); }                    */
/* RETIRED at FACES4 (wave 9c) under the SHADOW RULE, same shape as Reallocate
   below: this spells the return void and the owning TU
   (src/_ZN22ExpandingHeapAllocator9SetNodeIDEj.cpp) spells it int, so the two
   mangle apart. ?SetNodeID@ExpandingHeapAllocator@@QAEHI@Z is the definition
   and src/_ZN13ExpandingHeap10VSetNodeIDEj.cpp, the only caller, asks for it.
   Zero objects in walk_window's link reference this spelling.
void ExpandingHeapAllocator::SetNodeID(u32 id)
{ _ZN22ExpandingHeapAllocator9SetNodeIDEj(this, id); }                       */
/* gate 16: ExpandingHeap::VReallocate calls the allocator as a method while
   its definition is a C name, the Allocate case one line up. */
extern "C" u32 _ZN22ExpandingHeapAllocator10ReallocateEPvj(void *self,
                                                           char *p, u32 size);
/* RETIRED at FACES4 (wave 9c) under the SHADOW RULE. This body is a SECOND
   DECLARATION of one function, not a second function: it spells the return
   void * where src/_ZN22ExpandingHeapAllocator10ReallocateEPvj.cpp, the TU
   config says owns the ROM address, spells it unsigned -- so MSVC mangles the
   two differently and facegen read the pair as an overload and refused to bind
   the flat name at all. The owning TU's spelling
   ?Reallocate@ExpandingHeapAllocator@@QAEIPAXI@Z is the definition, this one
   was the shadow, and the one caller that used to need it
   (src/_ZN13ExpandingHeap11VReallocateEPvj.cpp) asks for the owning TU's
   spelling since today's main merge. Measured before removal: dumpbin over all
   8662 of walk_window's link inputs reports ZERO objects referencing
   ?Reallocate@ExpandingHeapAllocator@@QAEPAXPAXI@Z. The flat name above is now
   defined by the face in port/faces_sync.txt.
void *ExpandingHeapAllocator::Reallocate(void *p, u32 size)
{ return (void *)(size_t)_ZN22ExpandingHeapAllocator10ReallocateEPvj(
      this, (char *)p, size); }                                              */

// ---- cross-linkage bridges surfaced by the link, both directions ---------
// C++ method VDeallocate -> its C-linkage definition.
//
// RETIRED at FACES4 (wave 9c) under the SHADOW RULE. The body below spelled
// the return int and the virtualness non-virtual; the TU config says owns ROM
// 0x0203c50c, src/_ZN13ExpandingHeap11VDeallocateEPv.cpp, emits
// ?VDeallocate@ExpandingHeap@@UAEXPAX@Z -- public VIRTUAL, returning void.
// Two declarations of one function, so facegen read them as an overload and
// refused the flat name. The face in port/faces_sync.txt now defines the flat
// name onto the owning TU's spelling, and slot_dealloc calls the flat name
// straight rather than going through a host method that forwards to it.
// THE SLOT'S OWN SHAPE IS UNCHANGED: it still returns int, which is the ARM r0
// ride-through this table has always taken, and it took it one hop later
// before.
extern "C" int _ZN13ExpandingHeap11VDeallocateEPv(void *self, void *p);
/*
int ExpandingHeap::VDeallocate(void *p)
{ return _ZN13ExpandingHeap11VDeallocateEPv(this, p); }                      */

// C references to Heap::Allocate/Deallocate -> the MSVC method definitions.
// The src/ TUs declare `class Heap` (mangles PAV); a struct shadow here would
// mangle PAU and miss, so the method shadow must be a class too.
class Heap {
public:
    int Allocate(u32 size, int align);
    void Deallocate(void *p);
};
extern "C" {
int _ZN4Heap8AllocateEji(void *self, u32 size, int align)
{ return ((Heap *)self)->Allocate(size, align); }
void _ZN4Heap10DeallocateEPv(void *self, void *p)
{ ((Heap *)self)->Deallocate(p); }
}

// C++ references to Memory::Allocate(u32,int,Heap*) -> the C definition
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): main's tree defines this
   overload itself now, so src/ emits ?Allocate@Memory@@YAPAXIHPAVHeap@@@Z and
   the face here was the second definition (LNK2005).
extern "C" void *_ZN6Memory8AllocateEjiP4Heap(u32 size, int align, void *heap);
namespace Memory {
void *Allocate(u32 size, int align, Heap *heap)
{ return _ZN6Memory8AllocateEjiP4Heap(size, align, heap); }
}                                                                            */

// Memory::defaultHeapPtr is data_020a0ea0 by its address-name (data alias).
/* V is "class" and U is "struct". include/Heap.h declares `struct Heap` (lines
 * 133 and 143), so MSVC mangles the type as U and the six objects that ask for
 * this datum -- Heap::SetDefault, SetupRootHeap, CreateSolidHeap,
 * CreateExpandingHeap, SetupSolidHeapAsDefault and _ZdlPv -- all spell it
 * ?defaultHeapPtr@Memory@@3PAUHeap@@A. The row had gone stale on a
 * class-to-struct change and named a symbol nothing references. Measured by
 * lane DTORS2, corrected at the wave-9c fold. */
#pragma comment(linker, "/alternatename:?defaultHeapPtr@Memory@@3PAUHeap@@A=_data_020a0ea0")
/* BOTH SPELLINGS, not one. Replacing the V row with the U row closed the six
 * objects that ask for U and immediately opened a row for V, because
 * src/_ZN6Memory8AllocateEjiP4Heap.cpp and src/_ZN6Memory10DeallocateEPvP4Heap.cpp
 * still declare `class Heap`. Measured at the wave-9c fold in
 * tmp/build_int4_r3.log: "class Heap * Memory::defaultHeapPtr"
 * (?defaultHeapPtr@Memory@@3PAVHeap@@A) referenced in Memory::Allocate. Two
 * callers, two spellings, one address, so the file carries a row for each
 * until the sources agree on the keyword. DTORS2 named this alternative in
 * out/DTORS2/handoff.txt: "change the V to a U, or carry both rows." */
#pragma comment(linker, "/alternatename:?defaultHeapPtr@Memory@@3PAVHeap@@A=_data_020a0ea0")

// Crash(): the game's fatal stop. Loud on host. C linkage for the .c TUs;
// the C++-linkage references alias onto the same definition.
// PORT_HOST_ABI: src halts forever (IRQ-disable + CP15 WaitForInterrupt loop)
//                and drives the DS crash screen; the host reports and aborts.
extern "C" void Crash(void)
{
    fprintf(stderr, "FATAL: game Crash() reached\n");
    void *frames[12];
    unsigned n = CaptureStackBackTrace(0, 12, frames, 0);
    char *base = (char *)GetModuleHandleA(0);
    for (unsigned i = 0; i < n; ++i)
        fprintf(stderr, "  frame %u: +0x%08x\n", i,
                (unsigned)((char *)frames[i] - base));
    abort();
}
#pragma comment(linker, "/alternatename:?Crash@@YAXXZ=_Crash")

// Heap::Allocate(u32): the one-argument overload the allocator veneer path
// uses (func_0203cc0c). Align 4, same as Memory::Allocate(u32)'s default.
//
// FALLBACK NOW, NOT THE FACE. src/_ZN4Heap8AllocateEj.cpp is the ROM's own
// body (arm9 0x0203c28c) and it is seated on the walk_window family, where
// hal/lk4_eh_dtor_seat.cpp defines the flat __cdecl name func_0203cc0c calls
// and forwards it into that matched method. This file, though, links into
// sixteen targets and ten of them carry gate 4b (func_0203cc0c) without
// slice_w1l2, so they still need a definition of the flat name. Hence the
// /alternatename: on the three seated targets the real face wins and this
// body is never reached; everywhere else the alias supplies it, unchanged.
extern "C" void *hal_heap_allocate_align4(void *self, u32 size)
{
    return (void *)(size_t)((Heap *)self)->Allocate(size, 4);
}
#pragma comment(linker, "/alternatename:__ZN4Heap8AllocateEj=_hal_heap_allocate_align4")

// ---- the synthetic vtable ------------------------------------------------
//
// SIXTEEN slots, and they are the ROM's own: _ZTV13ExpandingHeap at arm9
// 0x02099dd8 resolves every one of them to a named ExpandingHeap V-method.
// The eight-slot version this replaces was written before that table had been
// read and sized itself off the two callers gate 3a could evidence -- which
// meant slot 8 and slot 9, the two Model::LoadAndSetFile reaches through
// Heap::Reallocate and Heap::Sizeof, were past the end of the array.
//
//   [ 0] ~ExpandingHeap D1      [ 8] VReallocate
//   [ 1] ~ExpandingHeap D0      [ 9] VSizeof
//   [ 2] VDestroy               [10] VMaxAllocationUnitSize
//   [ 3] VAllocate              [11] VMaxAllocatableSize
//   [ 4] VDeallocate            [12] VMemoryLeft
//   [ 5] VDeallocateAll         [13] VSetNodeID
//   [ 6] VIntact                [14] VGetNodeID
//   [ 7] VRescue                [15] VResizeToFit
//
// Slots the port has not yet had a caller for still trap by name; the table
// being the right SHAPE is what stops a dispatch running off the end of it.
static void *__fastcall slot_alloc(void *self, void *, u32 size, int align)
{ return ((ExpandingHeap *)self)->VAllocate(size, align); }
static int __fastcall slot_dealloc(void *self, void *, void *p)
{ return _ZN13ExpandingHeap11VDeallocateEPv(self, p); }
static void *__fastcall slot_realloc(void *self, void *, void *p, u32 size)
{ return ((ExpandingHeap *)self)->VReallocate(p, size); }
static u32 __fastcall slot_sizeof(void *self, void *, void *p)
{ return ((ExpandingHeap *)self)->VSizeof(p); }
/* LINKAGE SEAT: slots 6/7/15 get the class's own matched bodies (arm9
   0x0203c65c VIntact, 0x0203c630 VRescue, 0x0203c388 VResizeToFit, all
   2004/b56 byte-matches). The int-returning thunk widens VIntact's bool the
   way ARM r0 carries it, so a caller reading the slot as int sees 0/1. */
static int __fastcall slot_intact(void *self, void *)
{ return ((ExpandingHeap *)self)->VIntact(); }
static void __fastcall slot_rescue(void *self, void *)
{ ((ExpandingHeap *)self)->VRescue(); }
static u32 __fastcall slot_resizetofit(void *self, void *)
{ return ((ExpandingHeap *)self)->VResizeToFit(); }
/* LINKAGE SEAT 2: slots 13/14, the class's own matched NodeID forwarders
   (arm9 0x0203c3f8 VSetNodeID, 0x0203c3e0 VGetNodeID, 2004/b56
   byte-matches), which reach the allocator's flat-C accessors through the
   method bridges above. */
static void __fastcall slot_setnodeid(void *self, void *, u32 id)
{ ((ExpandingHeap *)self)->VSetNodeID(id); }
static u32 __fastcall slot_getnodeid(void *self, void *)
{ return ((ExpandingHeap *)self)->VGetNodeID(); }

#define TRAP(n) \
    static void __fastcall slot_trap##n(void *, void *) { \
        fprintf(stderr, "FATAL: ExpandingHeap vtable slot %d dispatched " \
                        "with no caller evidence (see heap_vtable.cpp)\n", n); \
        abort(); }
TRAP(0) TRAP(1) TRAP(2) TRAP(5)
TRAP(10) TRAP(11) TRAP(12)

extern "C" void *_ZTV13ExpandingHeap[16] = {
    (void *)slot_trap0, (void *)slot_trap1, (void *)slot_trap2,
    (void *)slot_alloc,        /* 3: VAllocate */
    (void *)slot_dealloc,      /* 4: VDeallocate */
    (void *)slot_trap5,
    (void *)slot_intact,       /* 6: VIntact - real matched body */
    (void *)slot_rescue,       /* 7: VRescue - real matched body */
    (void *)slot_realloc,      /* 8: VReallocate */
    (void *)slot_sizeof,       /* 9: VSizeof */
    (void *)slot_trap10, (void *)slot_trap11, (void *)slot_trap12,
    (void *)slot_setnodeid,    /* 13: VSetNodeID - real matched body */
    (void *)slot_getnodeid,    /* 14: VGetNodeID - real matched body */
    (void *)slot_resizetofit,  /* 15: VResizeToFit - real matched body */
};
