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
// The sources now compile against the real Heap hierarchy. On x86 MSVC one
// deleting-destructor entry precedes the fourteen V* methods, so VAllocate and
// VDeallocate are host slots 2 and 3 (their ROM/Itanium slots are 3 and 4).
// Slots without caller evidence are TRAPS that abort loudly with the slot
// number -- a silent wrong-slot dispatch is the exact class of bug the
// hybrid's gates existed to catch, and the port has no gate to catch it.
#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Heap.h"
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

namespace Memory {
void *Allocate(u32 size, int align, Heap *heap);
}
#pragma comment(linker, "/alternatename:?Allocate@Memory@@YAPAXIHPAUHeap@@@Z=?Allocate@Memory@@YAPAXIHPAVHeap@@@Z")

extern "C" {
u32 _ZN22ExpandingHeapAllocator14SizeofInternalEPv(void *ptr)
{ return ExpandingHeapAllocator::SizeofInternal(ptr); }
u32 _ZN22ExpandingHeapAllocator10MemoryLeftEv(
    ExpandingHeapAllocator *self)
{ return self->MemoryLeft(); }
Heap *_ZN4Heap13SetupRootHeapEv()
{ return Heap::SetupRootHeap(); }
void *_ZN6Memory8AllocateEjiP4Heap(u32 size, int align, Heap *heap)
{ return Memory::Allocate(size, align, heap); }
}

// ---- globals the root-heap chain stores through --------------------------
extern "C" {
void *data_020a0e9c;   /* Heap::rootHeap */
void *data_020a0ea0;   /* Memory::defaultHeapPtr */
int data_02099d90;     /* heap bring-up state flag */
}

// C references to Heap::Allocate/Deallocate -> the MSVC method definitions.
extern "C" {
void *_ZN4Heap8AllocateEji(void *self, u32 size, int align)
{ return ((Heap *)self)->Allocate(size, align); }
void _ZN4Heap10DeallocateEPv(void *self, void *p)
{ ((Heap *)self)->Deallocate(p); }
}

// Memory::defaultHeapPtr is data_020a0ea0 by its address-name (data alias).
#pragma comment(linker, "/alternatename:?defaultHeapPtr@Memory@@3PAVHeap@@A=_data_020a0ea0")
#pragma comment(linker, "/alternatename:?defaultHeapPtr@Memory@@3PAUHeap@@A=_data_020a0ea0")

// Crash(): the game's fatal stop. Loud on host. C linkage for the .c TUs;
// the C++-linkage references alias onto the same definition.
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
extern "C" void *_ZN4Heap8AllocateEj(void *self, u32 size)
{
    return (void *)(size_t)((Heap *)self)->Allocate(size, 4);
}

// ---- the synthetic vtable ------------------------------------------------
static void *__fastcall slot_alloc(void *self, void *, u32 size, int align)
{ return ((ExpandingHeap *)self)->ExpandingHeap::VAllocate(size, align); }
static void __fastcall slot_dealloc(void *self, void *, void *p)
{ ((ExpandingHeap *)self)->ExpandingHeap::VDeallocate(p); }
static u32 __fastcall slot_sizeof(void *self, void *, void *p)
{ return ((ExpandingHeap *)self)->ExpandingHeap::VSizeof(p); }
static u32 __fastcall slot_memory_left(void *self, void *)
{ return ((ExpandingHeap *)self)->ExpandingHeap::VMemoryLeft(); }

#define TRAP(n) \
    static void __fastcall slot_trap##n(void *, void *) { \
        fprintf(stderr, "FATAL: ExpandingHeap vtable slot %d dispatched " \
                        "with no caller evidence (see heap_vtable.cpp)\n", n); \
        abort(); }
TRAP(0) TRAP(1) TRAP(4) TRAP(5) TRAP(6) TRAP(7)
TRAP(9) TRAP(10) TRAP(12) TRAP(13) TRAP(14)

extern "C" void *_ZTV13ExpandingHeap[15] = {
    (void *)slot_trap0,        /*  0: deleting destructor */
    (void *)slot_trap1,        /*  1: VDestroy */
    (void *)slot_alloc,        /*  2: VAllocate */
    (void *)slot_dealloc,      /*  3: VDeallocate */
    (void *)slot_trap4,        /*  4: VDeallocateAll */
    (void *)slot_trap5,        /*  5: VIntact */
    (void *)slot_trap6,        /*  6: VRescue */
    (void *)slot_trap7,        /*  7: VReallocate */
    (void *)slot_sizeof,       /*  8: VSizeof */
    (void *)slot_trap9,        /*  9: VMaxAllocationUnitSize */
    (void *)slot_trap10,       /* 10: VMaxAllocatableSize */
    (void *)slot_memory_left,  /* 11: VMemoryLeft */
    (void *)slot_trap12,       /* 12: VSetNodeID */
    (void *)slot_trap13,       /* 13: VGetNodeID */
    (void *)slot_trap14,       /* 14: VResizeToFit */
};
