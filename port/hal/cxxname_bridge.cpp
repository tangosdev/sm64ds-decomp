// Bridges for TUs that declare C-NAMED symbols at C++ linkage (an extern
// declaration outside the file's extern "C" block): the reference mangles
// as ?_ZN...@@YA..., so a same-shape C++ definition here forwards to the
// real implementation. The forward hop goes through a differently-named
// extern "C" helper because one TU cannot name both linkages of the same
// identifier.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "MeshColliderBase.h"

extern "C" {
void hal_fileptr_release(void *self);
}

void _ZN13SharedFilePtr7ReleaseEv(void *self)
{
    hal_fileptr_release(self);
}
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN16MeshColliderBase9IsEnabledEv(void *self)
{
    return ((MeshColliderBase *)self)->MeshColliderBase::IsEnabled();
}
#else
int _ZN16MeshColliderBase9IsEnabledEv(void *self);  /* Linux: real symbol from src/_ZN16MeshColliderBase9IsEnabledEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN16MeshColliderBase7DisableEv(void *self)
{
    ((MeshColliderBase *)self)->MeshColliderBase::Disable();
}
#else
void _ZN16MeshColliderBase7DisableEv(void *self);  /* Linux: real symbol from src/_ZN16MeshColliderBase7DisableEv */
#endif /* _WIN32 */

// The ov098 file table's second column is addressed through its own symbol
// (base+4 on the DS -- an offset alias). The harness used to fake it with a
// separate 0xc-stride array here; gate 19 mounts ov098 per symbol with
// --pack, which puts data_ov098_0213c384 exactly four bytes after
// data_ov098_0213c380 the way the ROM does, so the alias is real. The
// gate-9 smoke keeps the old two-view fiction in port/tests/smoke_actor.cpp.

// ---- gate-9 method bridges (C name -> MSVC method), the gx_upload pattern -
#include "Platform.h"
#include "ShadowModel.h"
#include "Model.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self)
{ ((Platform *)self)->Platform::UpdateClsnPosAndRot(); }
#else
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);  /* Linux: real symbol from src/_ZN8Platform19UpdateClsnPosAndRotEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self)
{ ((Platform *)self)->Platform::UpdateModelPosAndRotY(); }
#else
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);  /* Linux: real symbol from src/_ZN8Platform21UpdateModelPosAndRotYEv */
#endif /* _WIN32 */
/* SHADOW SYSTEM DEFERRED (cosmetic). The cause recorded here was WRONG and is
   corrected: the cuboid template BMD at data_020ad524 is NOT built at runtime.
   It is static .data in OVERLAY 1 (ov001 .data spans 0x020ab800..0x020ad620),
   a complete BMD_File with numBones 1, bones 0x020ad59c, numMaterials 1,
   materials 0x020ad4f4, and its pointers already absolute in the ROM image.
   Nothing writes it: the only two references to the address in all of config/
   are the literal-pool loads inside InitCuboid and InitCylinder themselves.
   The ov000 path strings this comment saw are the OTHER overlay sharing the
   0x020aa420 window, which is why the address reads as garbage today.

   The real blockers are three, all mechanical: (1) the port mounts ov002,
   ov009, ov085, ov098, ov100, ov089 and ov102 but never ov001, so the bytes
   are absent and these stubs stand in for them; (2) _ZTV11ShadowModel
   (actor_vtables.cpp) is eight nulls and hal_fill_shadow_vtable below is
   empty, so ModelBase::SetFile's vtable+0x8 dispatch would fault even once the
   data lands; (3) no per-frame ShadowModel::RenderAll caller is matched in
   src/ outside UnknownVsEntry::Render, so the host render path would have to
   call it. Until those, InitCuboid and the drop-shadow install stay no-ops. */
void _ZN11ShadowModel10InitCuboidEv(void *) {}
void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *, void *, void *, int, int, int, unsigned) {}
void _ZN13SharedFilePtr8LoadFileEv(void *fp);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp)
{
    int trace = getenv("PORT_TRACE_SETFILE") != 0;
    if (trace)
        fprintf(stderr, "  model_loadfile fp=%p id=%u refs=%u\n", fp,
                *(unsigned short *)fp, ((unsigned char *)fp)[2]);
    /* expanded body of the matched Model::LoadFile so the load stages are
       individually traceable on host (Reallocate is a DS heap shrink, no-op
       here -- see gx_upload_bridge.cpp) */
    _ZN13SharedFilePtr8LoadFileEv(fp);
    void *filePtr = *(void **)((char *)fp + 4);
    if (((unsigned char *)fp)[2] == 1 && filePtr != 0) {
        if (trace) fprintf(stderr, "    fixups buf=%p\n", filePtr);
        Model::UpdateFileOffsets(*(BMD_File *)filePtr);
        if (trace) fprintf(stderr, "    offsets ok\n");
        Model::AddToCommonModelDataArr(*(BMD_File *)filePtr);
        if (trace) fprintf(stderr, "    common-arr ok\n");
    }
    return filePtr;
}

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
{
    if (getenv("PORT_TRACE_SETFILE")) {
        extern void *_ZTV5Model[8];
        extern void *_ZTV10ModelAnim2[12];
        fprintf(stderr, "  setfile self=%p vt=%p slot1=%p bmd=%p b=%d"
                " (model_vt=%p ma2_vt=%p)\n",
                self, *(void **)self,
                self ? ((void ***)self)[0][1] : 0, bmd, b,
                (void *)_ZTV5Model, (void *)_ZTV10ModelAnim2);
    }
    ((ModelBase *)self)->ModelBase::SetFile((BMD_File *)bmd, a, b);
}
}
#pragma comment(linker, "/alternatename:?data_ov098_0213c380@@3PADA=_data_ov098_0213c380")
#pragma comment(linker, "/alternatename:?data_ov098_0213c384@@3PADA=_data_ov098_0213c384")

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(
    void *self, void *kcl, void *clps)
{
    ((MeshCollider *)self)->MeshCollider::SetFile((KCL_File *)kcl,
                                                  *(CLPS_Block *)clps);
}
#else
extern "C" void _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(
    void *self, void *kcl, void *clps);  /* Linux: real symbol from src/_ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block */
#endif /* _WIN32 */

// operator new support: the game heap pointer for actors (the smoke seeds
// it with the root heap), and the zero-fill veneer -- its DS chain rides
// arguments through registers, so the host supplies the semantics direct.
extern "C" {
void *data_020a0eac_c;
}
#pragma comment(linker, "/alternatename:?data_020a0eac@@3PAUHeap@@A=_data_020a0eac_c")
#pragma comment(linker, "/alternatename:_data_020a0eac=_data_020a0eac_c")
/* C linkage since main's mangled-declaration sweep: ActorBase::operator new
   now spells this plain, so the definition has to be the plain name. The
   alias below still catches any TU that kept the old C++ mangling.
   PORT_HOST_ABI: ARM r0/r1/r2 ride-through -- src veneer (int x) forwards p/v/n
   through registers to the actual fill; host spells the args out. */
extern "C" void func_0206e2f8(void *p, int v, unsigned n)
{
    unsigned char *b = (unsigned char *)p;
    for (unsigned i = 0; i < n; ++i) b[i] = (unsigned char)v;
}
#pragma comment(linker, "/alternatename:?func_0206e2f8@@YAXPAXHI@Z=_func_0206e2f8")
extern "C" void hal_m43_roty(void *m, int a);
void Matrix4x3_FromRotationY(void *m, int a) { hal_m43_roty(m, a); }

#include "MeshColliderBase.h"
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN16MeshColliderBase6EnableEP5Actor(void *self, void *actor)
{
    return ((MeshColliderBase *)self)->MeshColliderBase::Enable((Actor *)actor);
}
#else
extern "C" int _ZN16MeshColliderBase6EnableEP5Actor(void *self, void *actor);  /* Linux: real symbol from src/_ZN16MeshColliderBase6EnableEP5Actor */
#endif /* _WIN32 */
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
{
    if (getenv("PORT_TRACE_SETFILE"))
        fprintf(stderr, "  dosetfile self=%p f=%p a=%d b=%d\n", self, f, a, b);
    return ((Model *)self)->Model::DoSetFile(f, a, b);
}
/* ---- the actor bucket draws in the frame's own units --------------------
   THE SEAM IS GONE. The whole frame is SCENE units now (world >> 3): an
   actor's Render fills its model matrix with rotation rows at 1.0 and a
   scene-unit translation, Camera::Render's LookAt_ leaves the view matrix's
   translation row in scene units to match, and the port's own two draws --
   the stage model and the Player -- were moved onto the same convention.
   So processing list 5 needs no conversion at all: Model::Render gets the
   matrix the actor wrote and the scale the actor asked for.

   What used to be here: the model matrix's translation row and the view
   matrix's translation row were both scaled by 8 for the duration of one
   draw, and the scene->world factor was spent through the Vector3 scale
   argument. That bought a world-unit frame at the price of two hacks --
   this one, and putting the view matrix back before Actor::BeforeBehavior
   read it for the Clipper. Both retired together.

   SM64DS_TRACE_ACTOR_MAT=1 stays: it is how the collapsed-geometry
   questions get answered on one model rather than by argument. */
extern "C" int data_0209b3ec[12];       /* the view matrix Model::Render composes with */

/* SM64DS_ACTOR_BOX=1: the SCREEN box each Model::Render through this seam
   draws into, one line the first time a given BMD appears. This is where
   actor size is measured rather than argued about: a class's box against
   Mario's ("[w] mario screen box") at comparable depth is the proportion
   check, and a class that renders nothing at all prints no line. */
namespace ntr { struct GxTriangle; const GxTriangle *gx_polygons(size_t &n); }
#include "ntr/gx.h"
static void mv_box(const Model *m, size_t before)
{
    size_t after = 0;
    const ntr::GxTriangle *t = ntr::gx_polygons(after);
    static const void *said[32];
    static int n;
    const void *key = m->data.modelFile;
    for (int i = 0; i < n; ++i)
        if (said[i] == key) return;
    if (n < 32) said[n++] = key;
    if (after <= before) {
        printf("[abox] file %p shift %u: NO TRIANGLES\n", key,
               m->data.modelFile ? m->data.modelFile->scaleShift : 0u);
        return;
    }
    float mnx = 1e30f, mxx = -1e30f, mny = 1e30f, mxy = -1e30f;
    for (size_t i = before; i < after; ++i)
        for (int v = 0; v < 3; ++v) {
            if (t[i].v[v].x < mnx) mnx = t[i].v[v].x;
            if (t[i].v[v].x > mxx) mxx = t[i].v[v].x;
            if (t[i].v[v].y < mny) mny = t[i].v[v].y;
            if (t[i].v[v].y > mxy) mxy = t[i].v[v].y;
        }
    const int *q = (const int *)&m->mat4x3;
    printf("[abox] file %p shift %u scene(%d,%d,%d) %zu tris screen "
           "x[%.0f..%.0f] y[%.0f..%.0f] h=%.0f\n", key,
           m->data.modelFile ? m->data.modelFile->scaleShift : 0u,
           q[9] >> 12, q[10] >> 12, q[11] >> 12, after - before, mnx, mxx,
           mny, mxy, mxy - mny);
}

static void __fastcall mv_updateverts(void *self, void *)
{ ((Model *)self)->Model::UpdateVerts(); }
static void __fastcall mv_virtual10(void *self, void *, void *m)
{ ((Model *)self)->Model::Virtual10(*(Matrix4x3 *)m); }
/* Both of these sit in Model::Render's dispatch, which runs once per actor
   per frame -- eighty-odd times a frame at the castle. MSVC's getenv walks
   the whole environment block on every call, so they are latched once. */
static int trace_actor_mat()
{
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_TRACE_ACTOR_MAT") ? 1 : 0;
    return on;
}
static int actor_box()
{
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_ACTOR_BOX") ? 1 : 0;
    return on;
}

static void __fastcall mv_render(void *self, void *, const void *s)
{
    Model *m = (Model *)self;
    if (trace_actor_mat()) {
        const unsigned char *bf = (const unsigned char *)m->data.modelFile;
        const int *q = (const int *)&m->mat4x3;
        const int *bt = (const int *)m->data.transforms;
        fprintf(stderr, "  [amat] file %p shift %u bones %u scale %d | "
                "%d %d %d | %d %d %d | %d %d %d | %d %d %d\n",
                (const void *)bf, bf ? bf[0] : 0u,
                bf ? *(const unsigned *)(bf + 4) : 0u,
                s ? *(const int *)s : 0, q[0], q[1], q[2], q[3], q[4], q[5],
                q[6], q[7], q[8], q[9], q[10], q[11]);
        if (bt)
            fprintf(stderr, "  [abone] %d %d %d | %d %d %d | %d %d %d | "
                    "%d %d %d\n", bt[0], bt[1], bt[2], bt[3], bt[4], bt[5],
                    bt[6], bt[7], bt[8], bt[9], bt[10], bt[11]);
    }
    if (actor_box()) {
        size_t before = 0;
        ntr::gx_polygons(before);
        m->Model::Render((const Vector3 *)s);
        mv_box(m, before);
        return;
    }
    m->Model::Render((const Vector3 *)s);
}
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

// gate 10: ModelAnim2's primary table (the Player's two body ModelAnims
// dispatch DoSetFile through it via ModelBase::SetFile). MSVC order: dtor 0,
// DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4, Virtual18 5. ROM slots
// carry the ModelAnim overrides for everything past DoSetFile. No dual-fill
// here: Render's ROM slot (5) is Virtual18's MSVC slot, so shadow-TU Render
// dispatch cannot be served by the same array -- trap-by-Virtual18 will name
// it if such a TU ever appears.
#include "ModelAnim.h"
static void __fastcall ma2_dtor(void *, void *) {}
static void __fastcall ma2_updateverts(void *self, void *)
{ ((ModelAnim *)self)->ModelAnim::UpdateVerts(); }
static void __fastcall ma2_virtual10(void *self, void *, void *m)
{ ((ModelAnim *)self)->ModelAnim::Virtual10(*(Matrix4x3 *)m); }
static void __fastcall ma2_render(void *self, void *, const void *s)
{ ((ModelAnim *)self)->ModelAnim::Render((const Vector3 *)s); }
static void __fastcall ma2_virtual18(void *self, void *, unsigned m, const void *s)
{ ((ModelAnim *)self)->ModelAnim::Virtual18(m, (const Vector3 *)s); }
extern "C" {
extern void *_ZTV10ModelAnim2[12];
extern void *VTable_Animation_ModelAnim2Thunk[12];
extern void *_ZTV9ModelAnim[10];
extern void *VTable_Animation_ModelAnimThunk[8];
void hal_fill_modelanim2_vtable(void)
{
    _ZTV10ModelAnim2[0] = (void *)ma2_dtor;
    _ZTV10ModelAnim2[1] = (void *)mv_dosetfile;
    _ZTV10ModelAnim2[2] = (void *)ma2_updateverts;
    _ZTV10ModelAnim2[3] = (void *)ma2_virtual10;
    _ZTV10ModelAnim2[4] = (void *)ma2_render;
    _ZTV10ModelAnim2[5] = (void *)ma2_virtual18;
    /* the Animation-base secondary table only ever destructs */
    VTable_Animation_ModelAnim2Thunk[0] = (void *)ma2_dtor;
    VTable_Animation_ModelAnim2Thunk[1] = (void *)ma2_dtor;
    /* plain ModelAnim (the Player's head models) shares every slot */
    _ZTV9ModelAnim[0] = (void *)ma2_dtor;
    _ZTV9ModelAnim[1] = (void *)mv_dosetfile;
    _ZTV9ModelAnim[2] = (void *)ma2_updateverts;
    _ZTV9ModelAnim[3] = (void *)ma2_virtual10;
    _ZTV9ModelAnim[4] = (void *)ma2_render;
    _ZTV9ModelAnim[5] = (void *)ma2_virtual18;
    VTable_Animation_ModelAnimThunk[0] = (void *)ma2_dtor;
    VTable_Animation_ModelAnimThunk[1] = (void *)ma2_dtor;
}
}

#include "ShadowModel.h"
extern "C" void _ZN11ShadowModelD1Ev(void *self);
/* Fallback so the narrow gate-8/9 harness (smoke_actor) still links: it slices
   ShadowModel's constructor but not its destructor (slice_gate9.txt carries
   _ZN11ShadowModelC1Ev only), yet it compiles this TU. The full targets
   (walk_window, walk_window_hires, smoke_player) link the real matched D1 from
   gate 10/16, which wins over this alternatename. The stub is never CALLED in
   smoke_actor -- that harness spawns gate-8/9 collision actors, none of which
   dispatch the shadow slot -- so it traps loudly if a target ever does reach it
   without the real dtor. The InitCuboid/DropShadowScaleXYZ stubs above are the
   same reasoning for the same reason. */
extern "C" void hal_shadow_d1_fallback(void *)
{ std::fprintf(stderr, "hal_shadow_d1_fallback: real _ZN11ShadowModelD1Ev not "
                       "linked in this target but the shadow slot was reached\n");
  std::abort(); }
#pragma comment(linker, "/alternatename:__ZN11ShadowModelD1Ev=_hal_shadow_d1_fallback")
/* combined dtor (Itanium D1) thunk, __fastcall to match the MSVC virtual ABI
   the rest of this file's fills use (self in ecx, ignored second arg) */
static void __fastcall shadow_dtor(void *self, void *)
{ _ZN11ShadowModelD1Ev(self); }
extern "C" {
extern void *_ZTV11ShadowModel[8];
void hal_fill_shadow_vtable(void)
{
    /* PowerStar state-5 lift-off destroys its drop shadow through slot 0
       (ROM 0x020EA284-0x020EA2A4: r0=actor+0x3d4, ldr vptr, ldr vtable[0], blx).
       Seat that single reachable slot; the double-unlink (this virtual call
       plus the later non-virtual _ZN11ShadowModelD1Ev in PowerStarD1Ev) is
       self-guarding in the matched source (ShadowModelD1Ev.c:33-51 checks
       prev/head and zeroes both). */
    _ZTV11ShadowModel[0] = (void *)shadow_dtor;
    /* vt[1] (D0 deleting dtor) stays null DELIBERATELY: nothing reachable in
       matched src dispatches it (every other ~90 embedders call the dtor
       non-virtually by name). A loud crash here beats silently running a
       wrong slot if an unmatched path ever reaches it. */
}
}

extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void *_ZN5Model23AddToCommonModelDataArrER8BMD_File(void *file)
{ return Model::AddToCommonModelDataArr(*(BMD_File *)file); }
#else
void *_ZN5Model23AddToCommonModelDataArrER8BMD_File(void *file);
#endif
void *func_0203cc0c(unsigned size);
void _ZN6Memory10DeallocateEPv(void *p);
/* the DS global operator new/delete route through the Memory layer */
void *_Znwj(unsigned size) { return func_0203cc0c(size); }
void _ZN6Memory16operator_delete2EPv(void *p) { _ZN6Memory10DeallocateEPv(p); }
}
