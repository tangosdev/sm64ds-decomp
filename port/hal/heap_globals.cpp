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
// ...and the same storage under the TYPED spelling. src/func_0204df54.cpp now
// declares `extern NestedHeapIterator data_020a4d38', so MSVC decorates the
// reference with the struct name (@@3U...@@A) rather than as the char array
// (@@3DA) the line above covers. Both are references to one address on the NDS.
#pragma comment(linker, "/alternatename:?data_020a4d38@@3UNestedHeapIterator@@A=__ZN6Memory16rootHeapIteratorE")

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
void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(void *self, HeapAllocator *a)
{ ((NestedHeapIterator *)self)->Remove(a); }
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

// ---- CONSTRUCTOR bridges -------------------------------------------------
//
// Same seam as the forwarders above, on the one kind of member whose host name
// cannot be spelled at all. A constructor has no name to declare: `C1' is an
// Itanium ABI VARIANT tag, and MSVC has no syntax that emits or references one.
// So a TU that says `extern "C" _ZN22ExpandingHeapAllocatorC1EPvj(...)' and calls
// it -- which is what src/_ZN4Heap28CreateExpandingHeapAllocatorEPvjj.cpp and
// src/func_0204df54.cpp do, because on the NDS that string IS the symbol -- can
// only be satisfied by a hand-written function of that name. Placement new is
// the body: it runs the migrated constructor from src/ in place and, like the
// ROM's C1, hands back the object.
//
// _ZN18NestedHeapIteratorC1Ej USED TO BE AN /alternatename HERE and the alias is
// removed with this change. It pointed the MSVC-mangled reference at the C
// spelling __ZN18NestedHeapIteratorC1Ej, which was a definition back when
// src/_ZN18NestedHeapIteratorC1Ej.cpp was C. That file is C++ now and defines a
// real __thiscall constructor, so the alias aimed at nothing: it named neither a
// symbol anyone referenced nor one anything defined, and the target linked only
// because no gate built it. This is the shape the file's own comment above warns
// about -- an alias across a convention boundary -- landing on the migration
// rather than on the original C.
#include "ExpandingHeapAllocator.h"
#include <new>

// Each signature is its CALLERS' declaration, not a tidier one: the iterator is
// declared `void' in include/decl_NestedHeapIterator.h and its result is dropped,
// while CreateExpandingHeapAllocator returns what the C1 handed back.
extern "C" {
void _ZN18NestedHeapIteratorC1Ej(void *self, u32 linkOffset)
{ ::new (self) NestedHeapIterator(linkOffset); }

ExpandingHeapAllocator *_ZN22ExpandingHeapAllocatorC1EPvj(
    ExpandingHeapAllocator *self, void *heapEnd, u32 flags)
{ return ::new ((void *)self) ExpandingHeapAllocator(heapEnd, flags); }
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
// Declared locally: there is no cstd header in include/, and this is the
// declaration hal/gx_upload_bridge.cpp carried for the same call.
namespace cstd { int abs(int); }

extern "C" int _ZN4cstd3absEi(int value) { return cstd::abs(value); }
