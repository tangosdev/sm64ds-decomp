// Constructor-variant bridges: the C1/C2 pair collapses on a host compiler.
//
// mwccarm targets the Itanium ABI, which emits TWO constructor symbols per
// class -- C1 for a complete object and C2 for a base subobject. A split
// class enrolls each variant in its own file, objisolate stripping the
// sibling that file does not own. A class whose TU is whole emits both
// variants from one constructor instead; the host still gets only one.
//
// MSVC has ONE constructor symbol. Compile both files for the host and the
// pair lands on the same decorated name -- LNK2005, a duplicate definition
// that says nothing about the port and everything about the ABI difference.
// So the slice manifests carry exactly one file per pair, and the OTHER
// variant's raw Itanium name is defined here.
//
// The same seam catches the reverse case: a constructor that migrated from
// `extern "C" void _ZN17dExtSimpleModel_cC1Ev(void *)` to a real `dExtSimpleModel_c::dExtSimpleModel_c()` keeps
// callers -- still-flat C TUs, and local shadow classes -- that spell the raw
// _ZN...C1Ev name. Those callers are cdecl with `this` as an ordinary first
// argument, while the real method is __thiscall with `this` in ECX, so these
// have to be genuine forwarding functions. A /alternatename alias would link
// and then corrupt the stack.
//
// common.h comes FIRST deliberately. It and math/Matrix.h share one include
// guard for two spellings of Matrix4x3 (flat `s32 m[12]` versus structured
// `Matrix3x3 r; Vector3 t;`), and whichever a TU sees first stands. dExtSimpleModel_c
// embeds one, so a TU that saw the other spelling could disagree about
// sizeof(dExtSimpleModel_c) -- and placement new here would construct against the wrong
// layout. src/engine/model/d_ext_model.cpp orders its includes the same way, for
// the same reason.
#include "common.h"

#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "dExtSimpleModel_c.h"
#include "dExtModel_c.h"

// dExtModel_c and dBgW are abstract -- their headers declare pure virtuals
// because the ROM leaves those vtable slots null. Placement new cannot name
// an abstract type, so the base-subobject constructor is reached through a
// trivial concrete heir: no members, no new virtuals, so single inheritance
// gives it the base's exact layout and the base constructor runs over the
// same bytes. The vptr it leaves behind is the heir's, which is harmless
// precisely because these are C2 base-subobject constructors -- every caller
// is a derived constructor that stores its own vptr on the next line. The
// asserts below are what keeps that claim honest.
namespace {
struct dExtModelHeir : dExtModel_c {
    int DoSetFile(char *, int, int) { return 0; }
};
}

static_assert(sizeof(dExtModelHeir) == sizeof(dExtModel_c), "dExtModel_c heir changed the layout");
// ::new, not new: several of these classes declare a class-scope operator new
// that routes through the DS heap, and class-scope lookup would hide the
// placement form.
#define CTOR_BRIDGE(sym, cls) \
    void sym(void *self) { ::new (self) cls(); }

extern "C" {

// dExtSimpleModel_c is special: the port dispatches it through the SYNTHETIC _ZTV17dExtSimpleModel_c,
// which hal/cxxname_bridge.cpp deliberately fills at both slot 4 and slot 5 so
// that dExtSimpleModel_c.h-compiled callers (MSVC numbering, one destructor slot) and
// local-shadow callers (ROM/Itanium numbering, two destructor slots) can share
// one object. Placement new installs MSVC's own five-slot vtable instead, and
// smoke_actor's `vcall0(actor, 9)` -- Itanium numbering for Render -- then
// reads off the end of it and calls null. So the constructor runs for its
// member initialization and the vptr is put back to the table the port
// dispatches through, which is exactly what dExtSimpleModel_c::dExtSimpleModel_c() in
// src/engine/model/d_ext_model.cpp does on the line after its own base-constructor
// call.
extern "C" void *_ZTV17dExtSimpleModel_c[8];
void _ZN17dExtSimpleModel_cC1Ev(void *self)
{
    ::new (self) dExtSimpleModel_c();
    *(void **)self = _ZTV17dExtSimpleModel_c;
}
CTOR_BRIDGE(_ZN11dExtModel_cC2Ev, dExtModelHeir)


// The DS global operator delete routes through the Memory layer. This lived
// in cxxname_bridge.cpp, which only smoke_actor links; every target that
// builds a migrated model-family constructor needs it now, because the
// synthesized dExtModel_c::operator delete references it.
void _ZN6Memory10DeallocateEPv(void *p);
void _ZN6Memory16operator_delete2EPv(void *p) { _ZN6Memory10DeallocateEPv(p); }

}

// dExtSimpleModel_c::~dExtSimpleModel_c() is NOT bridged here: src/engine/model/d_ext_model.cpp defines a
// real C++ destructor and the gate-4b slice compiles it, so MSVC takes
// slot 0 from src/ directly. Defining it here as well would be a duplicate
// definition.

// dExtModel_c's destructor is not sliced for the host: the ROM body releases
// resources through the collision layer, and slicing it pulls that whole tree
// in. MSVC still needs the destructor defined to emit the vtable, so it takes
// hal/clsn_vtable.cpp's shape -- present for the vtable's sake, loud if
// anything ever dispatches through it. A gate that starts destroying a
// dExtModel_c fails with its name on stderr instead of leaking quietly.
dExtModel_c::~dExtModel_c()
{
    fprintf(stderr, "FATAL: dExtModel_c::~dExtModel_c dispatched, but the port "
                    "defines it only so MSVC can emit the vtable "
                    "(hal/ctor_bridge.cpp).\n");
    abort();
}

// _ZdlPv is the DS global operator delete. Its delete veneer, func_0203cbc0,
// which dExtSimpleModel_c's destructor calls, now comes from src/engine/heap/Heap.cpp with
// a host body over the default heap; this keeps the _ZdlPv spelling itself
// routed through the Memory layer.
extern "C" void _ZdlPv(void *p) { _ZN6Memory10DeallocateEPv(p); }
