// Host storage and linkage bridging for the heap subsystem.
//
// The NitroSystem heap registry -- NNS_FND_heapcommon.c's sRootList and
// sRootListInitialized -- is .bss the decomp TU only declares
// (src/engine/heap/NestedHeapIterator.cpp says why: no arm9 .bss range is
// enrolled for it). On the NDS the link places it at 0x020a4d34; on the host
// this file is the definition.
//
// The C++ methods below are a different seam: some TUs still reach them under
// their Itanium spellings, which MSVC cannot produce for a member.
#include "nnsys/fnd.h"

extern "C" {
int sRootListInitialized;
NNSFndList sRootList;

// SDK asm primitive (stmia burst fill) -> plain word fill on host. The
// frontier classifies its TU as HAL-owned; this is the HAL half.
void MultiStore_Int(int val, int *dst, int len)
{
    for (int i = 0; i < len / 4; ++i)
        dst[i] = val;
}
}

// C-name references left in the slice -> methods that have since migrated to
// real C++. These must be forwarders, not /alternatename aliases: instance
// methods use __thiscall on x86 MSVC while the historical C spellings are
// __cdecl and pass `this` on the stack.
#include "Heap.h"
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"
#include "MemoryNode.h"
extern "C" {
ExpandingHeapAllocator *_ZN5mHeap6Heap_t28CreateExpandingHeapAllocatorEPvjt(
    void *address, u32 size, u16 optFlag)
{ return Heap::CreateExpandingHeapAllocator(address, size, optFlag); }

void *_ZN22ExpandingHeapAllocator8AllocateEji(
    ExpandingHeapAllocator *self, u32 size, int align)
{ return self->Allocate(size, align); }

int _ZN22ExpandingHeapAllocator10DeallocateEPv(
    ExpandingHeapAllocator *self, void *ptr)
{ return self->Deallocate(ptr); }

void *_ZN22ExpandingHeapAllocator16AllocateForwardsEjj(
    ExpandingHeapAllocator *self, u32 size, u32 align)
{ return self->AllocateForwards(size, align); }

void *_ZN22ExpandingHeapAllocator17AllocateBackwardsEjj(
    ExpandingHeapAllocator *self, u32 size, u32 align)
{ return self->AllocateBackwards(size, align); }

void *_ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(
    MemoryNode::Target *extent, u16 tag)
{ return ExpandingHeapAllocator::CreateNode(extent, tag); }

void *_ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(
    MemoryNode *list, MemoryNode *node, MemoryNode *prev)
{ return ExpandingHeapAllocator::LinkNode(list, node, prev); }

void *_ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(
    MemoryNode *list, MemoryNode *node)
{ return ExpandingHeapAllocator::UnlinkNode(list, node); }
}

// ---- CONSTRUCTOR bridges -------------------------------------------------
//
// Same seam as the forwarders above, on the one kind of member whose host name
// cannot be spelled at all. A constructor has no name to declare: `C1' is an
// Itanium ABI VARIANT tag, and MSVC has no syntax that emits or references one.
// So a TU that says `extern "C" _ZN22ExpandingHeapAllocatorC1EPvt(...)' and calls
// it -- which is what src/engine/heap/ExpandingHeapAllocator.cpp's factory
// does, because on the NDS that string IS the symbol -- can only be satisfied by a hand-written function of that name. Placement new is
// the body: it runs the migrated constructor from src/ in place and, like the
// ROM's C1, hands back the object.
#include "ExpandingHeapAllocator.h"
#include <new>

// The signature is its caller's declaration: CreateExpandingHeapAllocator
// returns what the C1 handed back.
extern "C" {
ExpandingHeapAllocator *_ZN22ExpandingHeapAllocatorC1EPvt(
    ExpandingHeapAllocator *self, void *endAddress, u16 optFlag)
{ return ::new ((void *)self) ExpandingHeapAllocator(endAddress, optFlag); }
}

// MemoryNode::Target's constructor, same seam again. ExpandingHeapAllocator's
// constructor and Reallocate both build one on the stack under the C1 spelling.
// A `MemoryNode::Target *' return matches what the ROM's C1 hands back; both
// callers drop it.
#include "MemoryNode.h"

extern "C" MemoryNode::Target *_ZN10MemoryNode6TargetC1EPS_(
    MemoryNode::Target *self, MemoryNode *node)
{ return ::new ((void *)self) MemoryNode::Target(node); }

// cstd::abs under its C spelling, which ExpandingHeapAllocator::MaxAllocatableSize
// references. The forwarder was in hal/gx_upload_bridge.cpp and moved here with
// the source file's move from slice_gate4b.txt to slice_gate2.txt, for the same
// reason: the nine targets that link gx_upload_bridge.cpp all link this file too,
// so the pair can have one home instead of one per layer that reaches it.
//
// The definition is the host's own, like hal/cstd_div.c's fdiv. cstd::abs now
// lives in src/cstd4.cpp, the runtime-library TU, which also owns hand-asm
// primitives (swp, dcd, the soft-float compare stubs) that no host compiler
// accepts -- so the slice can no longer compile the src/ definition. The body
// is the one src/cstd4.cpp carries.
namespace cstd {
int abs(int x)
{
    return x < 0 ? -x : x;
}
}

extern "C" int _ZN4cstd3absEi(int value) { return cstd::abs(value); }
