// Memory::operator_delete2 under its Itanium spelling, for every target that
// compiles a class in one of the seventeen hierarchies whose header declares it.
//
// THE SEAM. include/Heap.h, include/Fader.h:69 and fifteen more headers spell
//
//     extern "C" void _ZN6Memory16operator_delete2EPv(void *);
//
// and call it from an inline `operator delete', so EVERY class in those
// hierarchies emits a reference to that exact string. On the NDS the mangled
// string is the symbol and the ROM's own veneer at 0x0203cbcc answers it. MSVC
// decorates it as a cdecl name that needs a real definition, and
// src/_ZN6Memory16operator_delete2EPv.cpp cannot be it: that file is a migrated
// C++ `namespace Memory' function, so MSVC gives it a different decoration, and
// its body calls `extern "C" _ZdlPv', which no host runtime provides either.
//
// THE BODY IS WHAT THE ROM DOES. The ROM veneer is three words -- ldr ip / bx ip
// / .word -- tail-calling _ZdlPv at 0x0203cbf0, the global operator delete. That
// is `::operator delete' here.
//
// WHY ITS OWN FILE. It first appeared in hal/shims.cpp, which only `smoke' links,
// and hal/ctor_bridge.cpp separately defines it for the nine model-family targets
// by forwarding to Memory::Deallocate instead. Adding smoke_roots and smoke_fs
// would have made a third copy of a definition that is not target-specific, so
// the shims.cpp copy moved here and the two new targets link this file. The
// ctor_bridge definition is deliberately left where it is: it is a DIFFERENT
// route, chosen for targets that run a live Memory layer, and consolidating the
// two is a decision with a measurement behind it rather than a file move.
//
// No target links both this file and ctor_bridge.cpp. If one ever does the
// duplicate is a link error, which is loud -- not a silent divergence.
#include <new>

extern "C" void _ZN6Memory16operator_delete2EPv(void *ptr)
{
    ::operator delete(ptr);
}
