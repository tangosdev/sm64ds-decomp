// Seat ExpandingHeap's dtor chain and its two remaining allocator forwarders,
// closing the class: slots 0/1/2/5/12 of _ZTV13ExpandingHeap were the last
// traps while src/ carried the matched bodies. The audited closure
// (Heap::D2, HeapAllocator::Destroy/Remove, ExpandingHeapAllocator::
// DeallocateAll/InvokeDeallocate) is enrolled alongside; every deeper callee
// (Heap::Deallocate, the NestedHeapIterator pair, Memory::operator_delete2,
// data_02099d90) was already linked.
//
//   [ 0] ~ExpandingHeap D1   0x0203c9f0  flat .c, ecx->arg adapter
//   [ 1] ~ExpandingHeap D0   0x0203c9c0  flat .c, ecx->arg adapter
//   [ 2] VDestroy            0x0203c72c  flat .c, ecx->arg adapter
//   [ 5] VDeallocateAll      0x0203c4b0  MSVC method, qualified dispatch
//   [12] VMemoryLeft         0x0203c5ac  MSVC method, qualified dispatch
//
// TWO NAME SPACES, the batch-2 mechanism: VDeallocateAll and VMemoryLeft are
// real MSVC methods whose callee references are MSVC-mangled, while the
// allocator bodies they reach are flat-C definitions
// (_ZN22ExpandingHeapAllocator13DeallocateAllEPPFvPvPS_jEj,
// _ZN22ExpandingHeapAllocator16InvokeDeallocateEPvPS_j, and the
// already-linked _ZN22ExpandingHeapAllocator10MemoryLeftEv). The
// /alternatename bridges below carry the decorated spellings verbatim from
// the link errors, the heap_globals law (the linker is the authority on
// decoration).
//
// The slots are written at boot (level_boot's port_stage_a2_seat) for the
// walk_window family only, the lk1_eh_vmax_seat treatment: the D0/D1 closure
// needs Memory::operator_delete2, which rides slice_gate16, and the minimal
// root-heap targets do not link it. Those targets keep the traps, which they
// never dispatch (nothing there destroys the root heap).

#include "types.h"

// Flat C name -> the MSVC-mangled reference the matched method TUs emit.
#pragma comment(linker, "/alternatename:?DeallocateAll@ExpandingHeapAllocator@@QAEXPAP6AXPAXPAV1@I@ZI@Z=__ZN22ExpandingHeapAllocator13DeallocateAllEPPFvPvPS_jEj")
#pragma comment(linker, "/alternatename:?InvokeDeallocate@ExpandingHeapAllocator@@SAXPAXPAV1@I@Z=__ZN22ExpandingHeapAllocator16InvokeDeallocateEPvPS_j")
#pragma comment(linker, "/alternatename:?MemoryLeft@ExpandingHeapAllocator@@QAEIXZ=__ZN22ExpandingHeapAllocator10MemoryLeftEv")
// ...and the reverse direction once: HeapAllocator::Destroy is flat C and
// calls Remove by its flat name, while the matched Remove TU is a real MSVC
// method against include/HeapAllocator.h.
#pragma comment(linker, "/alternatename:__ZN13HeapAllocator6RemoveEv=?Remove@HeapAllocator@@QAEXXZ")

/* the matched methods, dispatched qualified through a local shadow (the
   heap_vtable slot_intact mechanism; both sides are MSVC C++ on the same
   class name, so the references resolve with no alias machinery) */
class ExpandingHeap
{
public:
    void VDeallocateAll();
    u32 VMemoryLeft();
};

extern "C" {

extern void *_ZTV13ExpandingHeap[16];   /* storage in hal/heap_vtable.cpp */

void *_ZN13ExpandingHeapD1Ev(void *self);   /* slot 0 */
void *_ZN13ExpandingHeapD0Ev(void *self);   /* slot 1 */
void _ZN13ExpandingHeap8VDestroyEv(void *self);   /* slot 2 */

}

static void __fastcall eh_d1(void *s, void *)      { _ZN13ExpandingHeapD1Ev(s); }
static void __fastcall eh_d0(void *s, void *)      { _ZN13ExpandingHeapD0Ev(s); }
static void __fastcall eh_vdestroy(void *s, void *){ _ZN13ExpandingHeap8VDestroyEv(s); }
static void __fastcall eh_vdeallall(void *s, void *)
{ ((ExpandingHeap *)s)->VDeallocateAll(); }
static u32 __fastcall eh_vmemleft(void *s, void *)
{ return ((ExpandingHeap *)s)->VMemoryLeft(); }

extern "C" void hal_seat_expandingheap_dtors(void)
{
    _ZTV13ExpandingHeap[0]  = (void *)eh_d1;
    _ZTV13ExpandingHeap[1]  = (void *)eh_d0;
    _ZTV13ExpandingHeap[2]  = (void *)eh_vdestroy;
    _ZTV13ExpandingHeap[5]  = (void *)eh_vdeallall;
    _ZTV13ExpandingHeap[12] = (void *)eh_vmemleft;
}
