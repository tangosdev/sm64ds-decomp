// Bridges for TUs that declare C-NAMED symbols at C++ linkage (an extern
// declaration outside the file's extern "C" block): the reference mangles
// as ?_ZN...@@YA..., so a same-shape C++ definition here forwards to the
// real implementation. The forward hop goes through a differently-named
// extern "C" helper because one TU cannot name both linkages of the same
// identifier.
#include "MeshColliderBase.h"

extern "C" {
void hal_fileptr_release(void *self);
}

void _ZN13SharedFilePtr7ReleaseEv(void *self)
{
    hal_fileptr_release(self);
}
int _ZN16MeshColliderBase9IsEnabledEv(void *self)
{
    return ((MeshColliderBase *)self)->MeshColliderBase::IsEnabled();
}
void _ZN16MeshColliderBase7DisableEv(void *self)
{
    ((MeshColliderBase *)self)->MeshColliderBase::Disable();
}

// The ov098 file table's second column is addressed through its own symbol
// (base+4 on the DS -- an offset alias hosts cannot express). Column b gets
// its own 0xc-stride view; the smoke seeds both views with the same
// SharedFilePtr pointers.
extern "C" {
char data_ov098_0213c384[0x18];
}

// ---- gate-9 method bridges (C name -> MSVC method), the gx_upload pattern -
#include "Platform.h"
#include "ShadowModel.h"
#include "Model.h"
extern "C" {
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self)
{ ((Platform *)self)->Platform::UpdateClsnPosAndRot(); }
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self)
{ ((Platform *)self)->Platform::UpdateModelPosAndRotY(); }
/* SHADOW SYSTEM DEFERRED (cosmetic): the cuboid template BMD at
   data_020ad524 is BUILT AT RUNTIME by boot code not yet hosted (the ov000
   static image holds path strings there). InitCuboid and the per-frame
   drop-shadow install are no-ops until that boot path lands. */
void _ZN11ShadowModel10InitCuboidEv(void *) {}
void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *, void *, void *, int, int, int, unsigned) {}
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp)
{ return Model::LoadFile(*(SharedFilePtr *)fp); }

// BSS the shadow/collider systems use
char data_020ad524[0x40];       /* ShadowModel's template BMD stub */
void *data_020a0c80[24];        /* the collision actor registry (gate 8) */
}

#include "MeshCollider.h"
#include "ModelBase.h"
extern "C" {
void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp)
{ return MeshCollider::LoadFile(*(SharedFilePtr *)fp); }
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b)
{ ((ModelBase *)self)->ModelBase::SetFile((BMD_File *)bmd, a, b); }
}
#pragma comment(linker, "/alternatename:?data_ov098_0213c380@@3PADA=_data_ov098_0213c380")
#pragma comment(linker, "/alternatename:?data_ov098_0213c384@@3PADA=_data_ov098_0213c384")

extern "C" void _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(
    void *self, void *kcl, void *clps)
{
    ((MeshCollider *)self)->MeshCollider::SetFile((KCL_File *)kcl,
                                                  *(CLPS_Block *)clps);
}

// operator new support: the game heap pointer for actors (the smoke seeds
// it with the root heap), and the zero-fill veneer -- its DS chain rides
// arguments through registers, so the host supplies the semantics direct.
extern "C" {
void *data_020a0eac_c;
}
#pragma comment(linker, "/alternatename:?data_020a0eac@@3PAUHeap@@A=_data_020a0eac_c")
#pragma comment(linker, "/alternatename:_data_020a0eac=_data_020a0eac_c")
void func_0206e2f8(void *p, int v, unsigned n)
{
    unsigned char *b = (unsigned char *)p;
    for (unsigned i = 0; i < n; ++i) b[i] = (unsigned char)v;
}
extern "C" void hal_m43_roty(void *m, int a);
void Matrix4x3_FromRotationY(void *m, int a) { hal_m43_roty(m, a); }

#include "MeshColliderBase.h"
extern "C" int _ZN16MeshColliderBase6EnableEP5Actor(void *self, void *actor)
{
    return ((MeshColliderBase *)self)->MeshColliderBase::Enable((Actor *)actor);
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

// gate 9: the Model vtable gains real slots at RUNTIME (targets without the
// Model methods keep the zero storage; nothing dispatches there). MSVC
// order per Model.h: dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
// Render 4. ModelBase::SetFile dispatches DoSetFile -- the first virtual
// the actor lifecycle exercises.
static int __fastcall mv_dosetfile(void *self, void *, char *f, int a, int b)
{ return ((Model *)self)->Model::DoSetFile(f, a, b); }
static void __fastcall mv_updateverts(void *self, void *)
{ ((Model *)self)->Model::UpdateVerts(); }
static void __fastcall mv_virtual10(void *self, void *, void *m)
{ ((Model *)self)->Model::Virtual10(*(Matrix4x3 *)m); }
static void __fastcall mv_render(void *self, void *, const void *s)
{ ((Model *)self)->Model::Render((const Vector3 *)s); }
extern "C" {
extern void *_ZTV5Model[8];
void hal_fill_model_vtable(void)
{
    _ZTV5Model[1] = (void *)mv_dosetfile;
    _ZTV5Model[2] = (void *)mv_updateverts;
    _ZTV5Model[3] = (void *)mv_virtual10;
    _ZTV5Model[4] = (void *)mv_render;
    /* Slot 5 too: TUs that dispatch through LOCAL shadow classes count in
       ROM/Itanium numbering (two dtor slots), which lands Render at 5.
       Model.h-compiled TUs land it at 4. The object serves both. */
    _ZTV5Model[5] = (void *)mv_render;
}
}

#include "ShadowModel.h"
extern "C" {
void hal_fill_shadow_vtable(void) {}   /* shadow system deferred */
}

extern "C" {
void *_ZN5Model23AddToCommonModelDataArrER8BMD_File(void *file)
{ return Model::AddToCommonModelDataArr(*(BMD_File *)file); }
void *func_0203cc0c(unsigned size);
void _ZN6Memory10DeallocateEPv(void *p);
/* the DS global operator new/delete route through the Memory layer */
void *_Znwj(unsigned size) { return func_0203cc0c(size); }
void _ZN6Memory16operator_delete2EPv(void *p) { _ZN6Memory10DeallocateEPv(p); }
}
