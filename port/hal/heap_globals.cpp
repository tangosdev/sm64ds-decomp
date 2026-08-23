// Host storage and linkage bridging for the heap subsystem.
//
// THE PROBLEM THIS FILE SOLVES: on the NDS, symbols resolve by ADDRESS, so
// one global can be referenced as C++ `Memory::rootHeapIterator` in one TU
// and as C `data_020a4d38` in another and both land on 0x020a4d38. A host
// linker resolves by NAME; without bridging, those become two separate
// storages and the heap registry silently forks -- the ctor registers into
// one list while FindNested searches the other. /alternatename restores the
// by-address identity: one storage, every historical name an alias.
//
// The same split affects functions: some C++ TUs declare the iterator entry
// points without extern "C" (MSVC-mangled references) while the defining
// TUs are C (plain-name definitions). Each alias below names the x86-32
// decorated forms.

extern "C" {
// The single real storage. Memory::rootHeapIterator is a NestedHeapIterator
// (a list head); the decomp TUs address it as a blob. 0x20 covers the
// evidenced fields with room; the ctor initializes it.
char _ZN6Memory16rootHeapIteratorE[0x20];
int _ZN6Memory25isRootHeapIterInitializedE;

// SDK asm primitive (stmia burst fill) -> plain word fill on host. The
// frontier classifies its TU as HAL-owned; this is the HAL half.
void MultiStore_Int(int val, int *dst, int len)
{
    for (int i = 0; i < len / 4; ++i)
        dst[i] = val;
}
}

// DATA aliases: point every historical name at the one storage above. Safe
// for data (no calling convention); the decorated spellings come verbatim
// from link errors -- the linker is the authority on decoration.
#pragma comment(linker, "/alternatename:_data_020a4d38=__ZN6Memory16rootHeapIteratorE")
#pragma comment(linker, "/alternatename:_data_020a4d34=__ZN6Memory25isRootHeapIterInitializedE")
#pragma comment(linker, "/alternatename:?_ZN6Memory16rootHeapIteratorE@@3DA=__ZN6Memory16rootHeapIteratorE")
#pragma comment(linker, "/alternatename:?_ZN6Memory25isRootHeapIterInitializedE@@3HA=__ZN6Memory25isRootHeapIterInitializedE")
#pragma comment(linker, "/alternatename:?data_020a4d34@@3HA=__ZN6Memory25isRootHeapIterInitializedE")
#pragma comment(linker, "/alternatename:?data_020a4d38@@3DA=__ZN6Memory16rootHeapIteratorE")
// FUNCTION alias only where the conventions MATCH: this reference and the C
// definition are both __cdecl free functions.
#pragma comment(linker, "/alternatename:?_ZN18NestedHeapIteratorC1Ej@@YAXPAXI@Z=__ZN18NestedHeapIteratorC1Ej")

// FUNCTION BRIDGES where aliasing would be a silent ABI bug: C TUs call the
// iterator entry points as __cdecl free functions under Itanium names, but
// the .cpp TUs define them as __thiscall METHODS (this in ecx). An
// /alternatename between those links fine and then reads garbage as `this`
// at runtime. Real forwarders convert the convention.
#include "NestedHeapIterator.h"
extern "C" {
void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(void *self, HeapAllocator *a)
{ ((NestedHeapIterator *)self)->AddLast(a); }
void _ZN18NestedHeapIterator8AddFirstEP13HeapAllocator(void *self, HeapAllocator *a)
{ ((NestedHeapIterator *)self)->AddFirst(a); }
int _ZN18NestedHeapIterator4NextEP13HeapAllocator(void *self, HeapAllocator *a)
{ return ((NestedHeapIterator *)self)->Next(a); }
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
ExpandingHeapAllocator *_ZN4Heap28CreateExpandingHeapAllocatorEPvjj(
    void *address, u32 size, u32 flags)
{ return Heap::CreateExpandingHeapAllocator(address, size, flags); }

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

NestedHeapIterator *_ZN18NestedHeapIterator10FindNestedEPv(void *address)
{ return NestedHeapIterator::FindNested(address); }

void _ZN18NestedHeapIterator4InitEP13HeapAllocator(
    NestedHeapIterator *self, HeapAllocator *allocator)
{ self->Init(allocator); }

}
