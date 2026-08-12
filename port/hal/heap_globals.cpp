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
// PORT_HOST_ABI: ARM asm primitive (stmia burst fill), MSVC cannot assemble.
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
#ifndef _WIN32
/* Linux: /alternatename has no GCC analog. The by-address identity the DS gets
   for free (one storage, C name data_020a4d38 == C++ name
   Memory::rootHeapIterator) is restored with a weak symbol alias onto the one
   storage above. Data only (no calling convention), so this is exactly safe. */
extern "C" char data_020a4d38[0x20]
    __attribute__((weak, alias("_ZN6Memory16rootHeapIteratorE")));
#endif
// FUNCTION alias only where the conventions MATCH: this reference and the C
// definition are both __cdecl free functions.
#pragma comment(linker, "/alternatename:?_ZN18NestedHeapIteratorC1Ej@@YAXPAXI@Z=__ZN18NestedHeapIteratorC1Ej")

// FUNCTION BRIDGES where aliasing would be a silent ABI bug: C TUs call the
// iterator entry points as __cdecl free functions under Itanium names, but
// the .cpp TUs define them as __thiscall METHODS (this in ecx). An
// /alternatename between those links fine and then reads garbage as `this`
// at runtime. Real forwarders convert the convention.
#include "NestedHeapIterator.h"
// LINUX CRITICAL: on GCC/Itanium these `extern "C"` forwarder NAMES mangle
// IDENTICALLY to the C++ methods they forward to (e.g.
// _ZN18NestedHeapIterator4NextEP13HeapAllocator IS the Itanium mangling of
// NestedHeapIterator::Next(HeapAllocator*)). So the forwarder body's `->Next(a)`
// call resolves to the forwarder itself -> infinite recursion, a SIGSEGV in the
// heap bring-up (seen in smoke_heap/smoke_roots/smoke_frames). On MSVC the two
// manglings differ, so the forwarder is a real __cdecl->__thiscall converter.
// On Linux the real src/ methods (src/_ZN18NestedHeapIterator4NextEP...cpp, in
// slice_gate2) already PROVIDE these exact symbols with the correct this-first
// ABI, so the forwarders are redundant AND harmful -- omit them and let the C
// callers bind straight to the real methods.
#ifdef _WIN32
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
#endif /* _WIN32 -- Linux binds C callers straight to the real src/ methods */
