// Bridges for TUs that declare C-NAMED symbols at C++ linkage (an extern
// declaration outside the file's extern "C" block): the reference mangles
// as ?_ZN...@@YA..., so a same-shape C++ definition here forwards to the
// real implementation. The forward hop goes through a differently-named
// extern "C" helper because one TU cannot name both linkages of the same
// identifier.
#include "dBgW.h"

int _ZN4dBgW9IsEnabledEv(void *self)
{
    return ((dBgW *)self)->dBgW::IsEnabled();
}

void _ZN4dBgW7DisableEv(void *self)
{
    ((dBgW *)self)->dBgW::Disable();
}

// The ov098 file table's second column is addressed through its own symbol
// (base+4 on the DS -- an offset alias hosts cannot express). Column b gets
// its own 0xc-stride view; the smoke seeds both views with the same
// SharedFilePtr pointers.
extern "C" {
char data_ov098_0213c384[0x18];
char data_ov098_0213c388[0x18];
}

// ---- gate-9 method bridges (C name -> MSVC method), the gx_upload pattern -
#include "dBgActor_c.h"
#include "dExtShadowModel_c.h"
#include "dExtSimpleModel_c.h"
extern "C" {
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self)
{ ((dBgActor_c *)self)->dBgActor_c::UpdateClsnPosAndRot(); }
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self)
{ ((dBgActor_c *)self)->dBgActor_c::UpdateModelPosAndRotY(); }
/* SHADOW SYSTEM DEFERRED (cosmetic): the cuboid template BMD at
   data_020ad524 is BUILT AT RUNTIME by boot code not yet hosted (the ov000
   static image holds path strings there). InitCuboid and the per-frame
   drop-shadow install are no-ops until that boot path lands. */
void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x3iiih(
    void *, void *, void *, int, int, int, unsigned char) {}
void *_ZN17dExtSimpleModel_c8LoadFileER13SharedFilePtr(void *fp)
{ return dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)fp); }

// BSS the shadow/collider systems use
char data_020ad524[0x40];       /* dExtShadowModel_c's template BMD stub */
void *data_020a0c80[24];        /* the collision actor registry (gate 8) */
}

#include "dBgW_Kc.h"
#include "dExtModel_c.h"
extern "C" {
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp)
{ return dBgW_Kc::LoadFile(*(SharedFilePtr *)fp); }
void _ZN11dExtModel_c7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b)
{ ((dExtModel_c *)self)->dExtModel_c::SetFile((BMD_File *)bmd, a, b); }
}
#pragma comment(linker, "/alternatename:?data_ov098_0213c380@@3PADA=_data_ov098_0213c380")
#pragma comment(linker, "/alternatename:?data_ov098_0213c384@@3PADA=_data_ov098_0213c384")

extern "C" void _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block(
    void *self, void *kcl, void *clps)
{
    ((dBgW_Kc *)self)->dBgW_Kc::SetFile((KCL_File *)kcl,
                                                  *(CLPS_Block *)clps);
}

// operator new support: the game heap pointer for actors (the smoke seeds
// it with the root heap), and the zero-fill veneer -- its DS chain rides
// arguments through registers, so the host supplies the semantics direct.
extern "C" {
void *data_020a0eac_c;
}
#pragma comment(linker, "/alternatename:?data_020a0eac@@3PAUHeap_t@mHeap@@A=_data_020a0eac_c")
#pragma comment(linker, "/alternatename:_data_020a0eac=_data_020a0eac_c")
extern "C" void func_0206e2f8(void *p, int v, unsigned n)
{
    unsigned char *b = (unsigned char *)p;
    for (unsigned i = 0; i < n; ++i) b[i] = (unsigned char)v;
}
extern "C" void hal_m43_roty(void *m, int a);
void Matrix4x3_FromRotationY(void *m, int a) { hal_m43_roty(m, a); }

#include "dBgW.h"
extern "C" int _ZN4dBgW6EnableEP8dActor_c(void *self, void *actor)
{
    return ((dBgW *)self)->dBgW::Enable((dActor_c *)actor);
}
extern "C" {
/* player-list globals ClosestPlayer scans: empty world -> null result */
int data_0208e37c[2];
int data_0208e380[2];
int data_0209b450[2];
int data_0209b458[2];
int data_0209f21c[8];
int data_0209f394[8];
}

extern "C" void _ZN6Memory10DeallocateEPv(void *p);
extern "C" void Memory_Deallocate(void *p) { _ZN6Memory10DeallocateEPv(p); }
extern "C" int hal_f0204424c(char *c);
int func_0204424c(int c) { return hal_f0204424c((char *)c); }

// gate 9: the dExtSimpleModel_c vtable gains real slots at RUNTIME (targets without the
// dExtSimpleModel_c methods keep the zero storage; nothing dispatches there). MSVC
// order per dExtSimpleModel_c.h: dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
// Render 4. dExtModel_c::SetFile dispatches DoSetFile -- the first virtual
// the actor lifecycle exercises.
static int __fastcall mv_dosetfile(void *self, void *, char *f, int a, int b)
{ return ((dExtSimpleModel_c *)self)->dExtSimpleModel_c::DoSetFile(f, a, b); }
static void __fastcall mv_updateverts(void *self, void *)
{ ((dExtSimpleModel_c *)self)->dExtSimpleModel_c::UpdateVerts(); }
static void __fastcall mv_virtual10(void *self, void *, void *m)
{ ((dExtSimpleModel_c *)self)->dExtSimpleModel_c::Virtual10(*(Matrix4x3 *)m); }
static void __fastcall mv_render(void *self, void *, const void *s)
{ ((dExtSimpleModel_c *)self)->dExtSimpleModel_c::Render((const Vector3 *)s); }
extern "C" {
extern void *_ZTV17dExtSimpleModel_c[8];
void hal_fill_model_vtable(void)
{
    _ZTV17dExtSimpleModel_c[1] = (void *)mv_dosetfile;
    _ZTV17dExtSimpleModel_c[2] = (void *)mv_updateverts;
    _ZTV17dExtSimpleModel_c[3] = (void *)mv_virtual10;
    _ZTV17dExtSimpleModel_c[4] = (void *)mv_render;
    /* Slot 5 too: TUs that dispatch through LOCAL shadow classes count in
       ROM/Itanium numbering (two dtor slots), which lands Render at 5.
       dExtSimpleModel_c.h-compiled TUs land it at 4. The object serves both. */
    _ZTV17dExtSimpleModel_c[5] = (void *)mv_render;
}
}

#include "dExtShadowModel_c.h"
int dExtShadowModel_c::InitCuboid() { return 1; }
extern "C" int _ZN17dExtShadowModel_c10InitCuboidEv(dExtShadowModel_c *self)
{ return self->dExtShadowModel_c::InitCuboid(); }
extern "C" {
void hal_fill_shadow_vtable(void) {}   /* shadow system deferred */
}

// Raw Itanium spellings retained by C and local-shadow callers after the real
// definitions migrated to C++. Each wrapper uses an explicit qualified call so
// a synthetic host vtable cannot recurse back into the shim.
#include "dActor_c.h"
#include "Heap.h"
extern "C" {
Player *_ZN8dActor_c13ClosestPlayerEv(dActor_c *self)
{ return self->dActor_c::ClosestPlayer(); }
void Heap_Destroy(void *heap);
}
void Heap::_Destroy() { Heap_Destroy(this); }

// fBase_c's local static-class shadow and the migrated namespace function are
// both cdecl with the same two pointer arguments; only their decorations differ.
#pragma comment(linker, "/alternatename:?Deallocate@Memory@@SAXPAXPAUHeap_t@mHeap@@@Z=?Deallocate@Memory@@YAXPAXPAUHeap_t@mHeap@@@Z")

extern "C" {
void *_ZN17dExtSimpleModel_c23AddToCommonModelDataArrER8BMD_File(void *file)
{ return dExtSimpleModel_c::AddToCommonModelDataArr(*(BMD_File *)file); }
/* The DS global operator new is spelled `operator new(size_t)` (_Znwm) in
   src/engine/heap/Heap.cpp and stays the CRT's on the host; the one
   host-compiled allocation that must come from the DS heap
   (src/engine/model/d_ext_model.cpp) calls func_0203cc0c directly.
   _ZN6Memory16operator_delete2EPv now lives in hal/ctor_bridge.cpp:
   every target that builds a migrated model-family constructor needs
   it, not just the ones that link this file. */
}
