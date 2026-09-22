// port/hal/smoke_heap_seam_actor.cpp -- the heap and fBase_c seam rows of
// out/SMOKELINK4B/remaining.md section D, for smoke_actor, smoke_savestate
// and smoke_persist: the two faces and seven alias rows of the eleven-row
// group that are not plain SRC additions.
//
// Lane SMOKELINK5, run link100 wave 10 round 5.
//
// FOUR OF THE ELEVEN ARE PLAIN SRC ROWS, added to PORT_SMOKE_ACTOR_SRC
// directly and not copied here, because each is a real C++ method whose
// decorated name IS what the wall's decorated row names once the file
// compiles:
//   src/_ZN4Heap15CreateSolidHeapEjPS_i.cpp     -> ?CreateSolidHeap@Heap@@SAPAU1@IPAU1@H@Z
//   src/_ZN4Heap11_DeallocateEPv.cpp            -> ?_Deallocate@Heap@@QAEXPAX@Z
//   src/_ZN4Heap11ResizeToFitEv.cpp             -> ?ResizeToFit@Heap@@QAEIXZ (backs the
//                                                   flat face below; not itself on the wall)
// (the fourth, src/_ZN4Heap21MaxAllocationUnitSizeEv.cpp, was already on this
// batch's PORT_SMOKE_ACTOR_SRC before this commit -- it is what the
// MaxAllocationUnitSize alias pair below points at).
//
// TWO ARE FACES, because the flat ABI and the decorated member ABI disagree
// (__cdecl with the receiver on the stack against __thiscall with the
// receiver in ecx), copied character for character from where the window
// build already carries the same face:
//   _ZN4Heap11ResizeToFitEv          hal/lk4_solidheap_seat.cpp:540
//   _ZN7fBase_c18MarkForDestructionEv  hal/method_faces.cpp:189-190 (src/
//       _ZN7fBase_c18MarkForDestructionEv.cpp, already on this batch's
//       PORT_SMOKE_ACTOR_SRC, defines the decorated fBase_c::MarkForDestruction
//       this face calls; that src file is the DECORATED member, not the flat
//       name the wall wants, which is exactly why a face and not an alias)
//
// FIVE ARE ALIAS PAIRS, three of them chained through an intermediate
// decorated spelling the way hal/lk4_solidheap_seat.cpp already chains them
// for the window targets (that file is not taken whole here: it also seats
// eleven ExpandingHeap/SolidHeap virtuals and four Destroy-family rows this
// batch does not ask for). Copied character for character:
//   ?Allocate@Memory@@SAPAXIHPAUHeap@@@Z
//       -> ?Allocate@Memory@@YAPAXIHPAVHeap@@@Z   hal/lk4_solidheap_seat.cpp:188
//       (itself the real definition src/_ZN6Memory8AllocateEjiP4Heap.cpp
//       emits -- that file's own private `class Heap` shadow is what makes it
//       PAVHeap rather than PAUHeap; the src file is already on this batch's
//       PORT_SMOKE_ACTOR_SRC)
//   ?InitializeSolidHeapAsDefault@Heap@@SAPAU1@IPAU1@H@Z
//       -> ?InitializeSolidHeapAsDefault@Heap@@SAPAXIPAU1@H@Z   hal/lk4_solidheap_seat.cpp:183
//   __ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i
//       -> ?InitializeSolidHeapAsDefault@Heap@@SAPAXIPAU1@H@Z   hal/cxx_aliases.cpp:3300
//       (both point at the same decorated symbol, which
//       src/_ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i.cpp -- already on
//       this batch's PORT_SMOKE_ACTOR_SRC -- defines)
//   ?MaxAllocationUnitSize@Heap@@QAEHXZ
//       -> ?MaxAllocationUnitSize@Heap@@QAEIXZ   hal/cxx_aliases.cpp:3017
//   __ZN4Heap21MaxAllocationUnitSizeEv
//       -> ?MaxAllocationUnitSize@Heap@@QAEIXZ   NOT an existing row anywhere
//       in the tree (measured: grep for "MaxAllocationUnitSizeEv=" across
//       port/ before writing this line found nothing). It is the same
//       flat-plus-decorated pattern as the RestoreFromTemporary and
//       InitializeSolidHeapAsDefault rows beside it, pointing at the same
//       already-real decorated symbol src/_ZN4Heap21MaxAllocationUnitSizeEv.cpp
//       (already on this batch's PORT_SMOKE_ACTOR_SRC) defines, so it extends
//       an established mechanism rather than inventing one.
//   __ZN4Heap20RestoreFromTemporaryEv
//       -> ?RestoreFromTemporary@Heap@@SAXXZ   hal/cxx_aliases.cpp:3301
//       (src/_ZN4Heap20RestoreFromTemporaryEv.cpp, already on this batch's
//       PORT_SMOKE_ACTOR_SRC, defines the decorated side)
//   __ZN18NestedHeapIterator10FindNestedEPv
//       -> ?FindNested@NestedHeapIterator@@SAPAU1@PAX@Z   hal/cxx_aliases.cpp:3299
//       (src/_ZN18NestedHeapIterator10FindNestedEPv.cpp, already on this
//       batch's PORT_SMOKE_ACTOR_SRC, defines the decorated side)
//
// No target carries both this file and hal/lk4_solidheap_seat.cpp,
// hal/cxx_aliases.cpp or hal/method_faces.cpp, so none of these definitions or
// pragmas can collide.

#include "Heap.h"
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"
#include "fBase_c.h"
#include <new>

// CreateSolidHeap's and CreateSolidHeapAllocator's own bodies (src/
// _ZN4Heap15CreateSolidHeapEjPS_i.cpp and src/
// _ZN4Heap24CreateSolidHeapAllocatorEPvjj.cpp, both added to this batch's
// PORT_SMOKE_ACTOR_SRC) call these two flat names AS FREE FUNCTIONS with the
// "this" pointer as an explicit first argument -- the shape a placement
// constructor takes when its caller is C rather than a `new` expression.
// src/_ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator.cpp and src/
// _ZN18SolidHeapAllocatorC1EPvj.cpp (also on this batch) define the REAL,
// decorated C++ constructors; a plain SRC addition of those two files does
// not, by itself, produce a symbol under the flat spelling, because a
// constructor is never called through an ordinary function pointer -- it has
// to be placement-new'd. These two faces are that placement, not a second
// definition: no code runs that the two src files do not already contain.
extern "C" SolidHeap *_ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator(
    SolidHeap *self, void *start, u32 size, Heap *root,
    SolidHeapAllocator *allocator)
{ return new (self) SolidHeap(start, size, root, allocator); }

extern "C" SolidHeapAllocator *_ZN18SolidHeapAllocatorC1EPvj(
    SolidHeapAllocator *self, void *heapEnd, u32 flags)
{ return new (self) SolidHeapAllocator(heapEnd, flags); }

extern "C" unsigned int _ZN4Heap11ResizeToFitEv(void *thiz)
{ return ((Heap *)thiz)->Heap::ResizeToFit(); }

extern "C" void _ZN7fBase_c18MarkForDestructionEv(void *self)
{ ((fBase_c *)self)->fBase_c::MarkForDestruction(); }

#pragma comment(linker, "/alternatename:?Allocate@Memory@@SAPAXIHPAUHeap@@@Z=?Allocate@Memory@@YAPAXIHPAVHeap@@@Z")
#pragma comment(linker, "/alternatename:?InitializeSolidHeapAsDefault@Heap@@SAPAU1@IPAU1@H@Z=?InitializeSolidHeapAsDefault@Heap@@SAPAXIPAU1@H@Z")
#pragma comment(linker, "/alternatename:__ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i=?InitializeSolidHeapAsDefault@Heap@@SAPAXIPAU1@H@Z")
#pragma comment(linker, "/alternatename:?MaxAllocationUnitSize@Heap@@QAEHXZ=?MaxAllocationUnitSize@Heap@@QAEIXZ")
#pragma comment(linker, "/alternatename:__ZN4Heap21MaxAllocationUnitSizeEv=?MaxAllocationUnitSize@Heap@@QAEIXZ")
#pragma comment(linker, "/alternatename:__ZN4Heap20RestoreFromTemporaryEv=?RestoreFromTemporary@Heap@@SAXXZ")
#pragma comment(linker, "/alternatename:__ZN18NestedHeapIterator10FindNestedEPv=?FindNested@NestedHeapIterator@@SAPAU1@PAX@Z")

// THIRD ORDER, surfaced by CreateSolidHeap's own dependency chain
// (func_0204ebb8.c, SolidHeap::VDestroy's forward, wants the flat spelling;
// src/_ZN13HeapAllocator6RemoveEv.cpp -- already on this batch -- compiles
// the decorated member): the same flat/decorated pair shape as the seven
// above.
#pragma comment(linker, "/alternatename:__ZN13HeapAllocator6RemoveEv=?Remove@HeapAllocator@@QAEXXZ")
