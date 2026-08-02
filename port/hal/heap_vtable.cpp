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

typedef unsigned int u32;

// Shadow declarations that mangle identically to the src/ definitions.
struct ExpandingHeap {
    void *VAllocate(u32 size, int align);
    int VDeallocate(void *p);
};
struct ExpandingHeapAllocator {
    void *Allocate(u32 size, int align);
    static u32 SizeofInternal(void *p);
    u32 MemoryLeft();
};

// ---- globals the root-heap chain stores through --------------------------
extern "C" {
void *data_020a0e9c;   /* Heap::rootHeap */
void *data_020a0ea0;   /* Memory::defaultHeapPtr */
int data_02099d90;     /* heap bring-up state flag */
}

// ---- allocator methods -> the C-linkage definitions from gate 2 ----------
extern "C" {
void *_ZN22ExpandingHeapAllocator8AllocateEji(void *self, u32 size, int align);
int _ZN22ExpandingHeapAllocator10DeallocateEPv(void *self, void *p);
u32 _ZN22ExpandingHeapAllocator14SizeofInternalEPv(void *p);
u32 _ZN22ExpandingHeapAllocator10MemoryLeftEv(void *self);
}
void *ExpandingHeapAllocator::Allocate(u32 size, int align)
{ return _ZN22ExpandingHeapAllocator8AllocateEji(this, size, align); }
u32 ExpandingHeapAllocator::SizeofInternal(void *p)
{ return _ZN22ExpandingHeapAllocator14SizeofInternalEPv(p); }
u32 ExpandingHeapAllocator::MemoryLeft()
{ return _ZN22ExpandingHeapAllocator10MemoryLeftEv(this); }

// ---- cross-linkage bridges surfaced by the link, both directions ---------
// C++ method VDeallocate -> its C-linkage definition
extern "C" int _ZN13ExpandingHeap11VDeallocateEPv(void *self, void *p);
int ExpandingHeap::VDeallocate(void *p)
{ return _ZN13ExpandingHeap11VDeallocateEPv(this, p); }

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
extern "C" void *_ZN6Memory8AllocateEjiP4Heap(u32 size, int align, void *heap);
namespace Memory {
void *Allocate(u32 size, int align, Heap *heap)
{ return _ZN6Memory8AllocateEjiP4Heap(size, align, heap); }
}

// Memory::defaultHeapPtr is data_020a0ea0 by its address-name (data alias).
#pragma comment(linker, "/alternatename:?defaultHeapPtr@Memory@@3PAVHeap@@A=_data_020a0ea0")

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
{ return ((ExpandingHeap *)self)->VAllocate(size, align); }
static int __fastcall slot_dealloc(void *self, void *, void *p)
{ return ((ExpandingHeap *)self)->VDeallocate(p); }

#define TRAP(n) \
    static void __fastcall slot_trap##n(void *, void *) { \
        fprintf(stderr, "FATAL: ExpandingHeap vtable slot %d dispatched " \
                        "with no caller evidence (see heap_vtable.cpp)\n", n); \
        abort(); }
TRAP(0) TRAP(1) TRAP(2) TRAP(5) TRAP(6) TRAP(7)

extern "C" void *_ZTV13ExpandingHeap[8] = {
    (void *)slot_trap0, (void *)slot_trap1, (void *)slot_trap2,
    (void *)slot_alloc,        /* 3: VAllocate, pinned by Heap::Allocate */
    (void *)slot_dealloc,      /* 4: VDeallocate, pinned by Heap::Deallocate */
    (void *)slot_trap5, (void *)slot_trap6, (void *)slot_trap7,
};
