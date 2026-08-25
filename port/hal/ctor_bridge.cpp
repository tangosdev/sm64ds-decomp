// Constructor-variant bridges: the C1/C2 pair collapses on a host compiler.
//
// mwccarm targets the Itanium ABI, which emits TWO constructor symbols per
// class -- C1 for a complete object and C2 for a base subobject -- and this
// decomp enrolls each variant in its own file, objisolate stripping the
// sibling the file does not own. That is correct for the cartridge and it is
// why src/_ZN9AnimationC1Ev.cpp and src/_ZN9AnimationC2Ev.cpp both exist with
// the same body.
//
// MSVC has ONE constructor symbol. Compile both files for the host and the
// pair lands on the same decorated name -- LNK2005, a duplicate definition
// that says nothing about the port and everything about the ABI difference.
// So the slice manifests carry exactly one file per pair, and the OTHER
// variant's raw Itanium name is defined here.
//
// The same seam catches the reverse case: a constructor that migrated from
// `extern "C" void _ZN5ModelC1Ev(void *)` to a real `Model::Model()` keeps
// callers -- still-flat C TUs, and local shadow classes -- that spell the raw
// _ZN...C1Ev name. Those callers are cdecl with `this` as an ordinary first
// argument, while the real method is __thiscall with `this` in ECX, so these
// have to be genuine forwarding functions. A /alternatename alias would link
// and then corrupt the stack.
//
// common.h comes FIRST deliberately. It and math/Matrix.h share one include
// guard for two spellings of Matrix4x3 (flat `s32 m[12]` versus structured
// `Matrix3x3 r; Vector3 t;`), and whichever a TU sees first stands. Model
// embeds one, so a TU that saw the other spelling could disagree about
// sizeof(Model) -- and placement new here would construct against the wrong
// layout. src/_ZN5ModelC1Ev.cpp orders its includes the same way, for the
// same reason.
#include "common.h"

#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "Model.h"
#include "ModelBase.h"

// ModelBase and dBgW are abstract -- their headers declare pure virtuals
// because the ROM leaves those vtable slots null. Placement new cannot name
// an abstract type, so the base-subobject constructor is reached through a
// trivial concrete heir: no members, no new virtuals, so single inheritance
// gives it the base's exact layout and the base constructor runs over the
// same bytes. The vptr it leaves behind is the heir's, which is harmless
// precisely because these are C2 base-subobject constructors -- every caller
// is a derived constructor that stores its own vptr on the next line. The
// asserts below are what keeps that claim honest.
namespace {
struct ModelBaseHeir : ModelBase {
    int DoSetFile(char *, int, int) { return 0; }
};
}

static_assert(sizeof(ModelBaseHeir) == sizeof(ModelBase), "ModelBase heir changed the layout");
// ::new, not new: several of these classes declare a class-scope operator new
// that routes through the DS heap, and class-scope lookup would hide the
// placement form.
#define CTOR_BRIDGE(sym, cls) \
    void sym(void *self) { ::new (self) cls(); }

extern "C" {

// Model is special: the port dispatches it through the SYNTHETIC _ZTV5Model,
// which hal/cxxname_bridge.cpp deliberately fills at both slot 4 and slot 5 so
// that Model.h-compiled callers (MSVC numbering, one destructor slot) and
// local-shadow callers (ROM/Itanium numbering, two destructor slots) can share
// one object. Placement new installs MSVC's own five-slot vtable instead, and
// smoke_actor's `vcall0(actor, 9)` -- Itanium numbering for Render -- then
// reads off the end of it and calls null. So the constructor runs for its
// member initialization and the vptr is put back to the table the port
// dispatches through, which is exactly what src/_ZN5ModelC2Ev.c does on the
// line after its own base-constructor call.
extern "C" void *_ZTV5Model[8];
void _ZN5ModelC1Ev(void *self)
{
    ::new (self) Model();
    *(void **)self = _ZTV5Model;
}
CTOR_BRIDGE(_ZN9ModelBaseC2Ev, ModelBaseHeir)


// The DS global operator delete routes through the Memory layer. This lived
// in cxxname_bridge.cpp, which only smoke_actor links; every target that
// builds a migrated model-family constructor needs it now, because the
// synthesized ModelBase::operator delete references it.
void _ZN6Memory10DeallocateEPv(void *p);
void _ZN6Memory16operator_delete2EPv(void *p) { _ZN6Memory10DeallocateEPv(p); }

}

// ModelBase's destructor is not sliced for the host: the ROM body releases
// resources through the collision layer, and slicing it pulls that whole tree
// in. MSVC still needs the destructor defined to emit the vtable, so it takes
// hal/clsn_vtable.cpp's shape -- present for the vtable's sake, loud if
// anything ever dispatches through it. A gate that starts destroying a
// ModelBase fails with its name on stderr instead of leaking quietly.
ModelBase::~ModelBase()
{
    fprintf(stderr, "FATAL: ModelBase::~ModelBase dispatched, but the port "
                    "defines it only so MSVC can emit the vtable "
                    "(hal/ctor_bridge.cpp).\n");
    abort();
}

// src/func_0203cbc0.c is a tail-call veneer to _ZdlPv, the DS global operator
// delete, and Model's destructor calls it. The veneer is sliced now, so its
// target routes through the Memory layer here -- matching how _Znwj is
// handled in hal/cxxname_bridge.cpp.
extern "C" void _ZdlPv(void *p) { _ZN6Memory10DeallocateEPv(p); }
