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
#pragma comment(linker, "/alternatename:?_ZN6Memory16rootHeapIteratorE@@3DA=__ZN6Memory16rootHeapIteratorE")
#pragma comment(linker, "/alternatename:?_ZN6Memory25isRootHeapIterInitializedE@@3HA=__ZN6Memory25isRootHeapIterInitializedE")
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
// And the reverse direction: AddLast/AddFirst reference Init as a __cdecl
// FREE function with char* args while Init.cpp defines the method. This face
// carried C++ linkage until include/decl_NestedHeapIterator.h was wrapped in
// extern "C" (#1049) -- the callers now emit the plain C symbol, so this must
// too, or both callers go unresolved.
extern "C" void _ZN18NestedHeapIterator4InitEP13HeapAllocator(char *self, char *a)
{ ((NestedHeapIterator *)self)->Init((HeapAllocator *)a); }
