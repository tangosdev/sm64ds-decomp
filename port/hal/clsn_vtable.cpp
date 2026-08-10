// The synthetic MeshCollider vtable (gate 8) -- the gate-3a mechanism at
// its second use, and the first with REAL slot fillers throughout the hot
// path: GetSurfaceInfo (matched, ITCM) calls GetNormal through the vtable
// (notes/itcm.md, "the one lever"), so slot 4 must dispatch for the octree
// walk to survive. Slots are __fastcall shims (ecx carries `this` exactly
// as __thiscall does; the dummy edx absorbs fastcall's second register),
// slot order per include/MeshCollider.h's ROM-read map: dtor 0/1,
// Virtual08 2, surface queries 3-5, the DetectClsn overloads 6-8.
// Unevidenced slots trap loudly.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MeshCollider.h"

extern "C" int g_sphere_dbg[16];   /* port/unmatched/MeshCollider_DetectClsn_Sphere.cpp */

static void __fastcall slot_v08(void *self, void *)
{ ((MeshCollider *)self)->MeshCollider::Virtual08(); }
static void __fastcall slot_surf(void *self, void *, s16 tri, SurfaceInfo *res)
{ ((MeshCollider *)self)->MeshCollider::GetSurfaceInfo(tri, *res); }
static void __fastcall slot_norm(void *self, void *, s16 tri, Vector3 *res)
{ ((MeshCollider *)self)->MeshCollider::GetNormal(tri, *res); }
static void __fastcall slot_orig(void *self, void *, s16 tri, Vector3 *res)
{ ((MeshCollider *)self)->MeshCollider::GetTriangleOrigin(tri, *res); }
static int __fastcall slot_ray(void *self, void *, RaycastLine *ray)
{ return ((MeshCollider *)self)->MeshCollider::DetectClsn(*ray); }

/* Slot 9 -- MeshColliderBase::BeforeClsn, the platform-carry seat. A moving
   collider (a lift, a rotating platform) that an actor is STANDING ON fires
   this every collision step; the body invokes the beforeClsnCallback the
   platform's InitResources stored via func_020393d4 (UpdatePosWithTransform and
   its siblings), which is what walks the rider along with the platform. Neither
   MeshColliderBase overrides it nor does MovingMeshCollider, so both tables
   inherit this one body (MeshColliderBase.h slot 9; MovingMeshCollider.h
   "Overrides every slot except BeforeClsn (slot 9) and GetSurfaceInfo").
   Left as slot_trap9 until this gate: the ride path was never proof-driven, so
   the first actor to stand on a moving platform aborted here.

   The one-line ROM body is inlined rather than dispatched to the matched
   src (_ZN16MeshColliderBase10BeforeClsn...cpp): that TU rides slice_gate16,
   which walk_window links but the gate-8/9 collision smoke targets do not, so a
   qualified `MeshColliderBase::BeforeClsn` call would leave them unresolved and
   only GATE8_EXTRA_SOURCES (CMakeLists) could repair that. The body is exactly
   `beforeClsnCallback(this, actor, &res, &pos, motionAng, ang)` with the
   documented arg-order swap (the virtual takes res-first, the callback
   actor-first); the pointer lives at MeshColliderBase+0x18, its type spelled by
   MeshColliderBase.h. PORT_HOST_ABI: __fastcall shim, ecx=this; the ROM body
   transcribed, the WaterBomb/host-copy reading (one less cross-TU dependency). */
static void __fastcall slot_beforeclsn(void *self, void *, ClsnResult *res,
                                       Actor *actor, Vector3 *pos,
                                       Vector3_16 *motionAng, Vector3_16 *ang)
{
    MeshColliderBase *base = (MeshColliderBase *)self;
    base->beforeClsnCallback(base, actor, res, pos, motionAng, ang);
}

/* Ground overload (ROM slot 6) -- MeshCollider::DetectClsn(RaycastGround &),
   ITCM 0x01ffd3f8, 0x498 bytes, unmatched. Adapter: a stack RaycastLine
   straight down from the ground ray's own position by the ground ray's own
   reach, walked by the hosted line walk, hit fields copied back. Both
   endpoints come from the caller -- a RaycastGround HAS no endpoints, it is
   pos + reach -- so the only invented number here is the fallback when the
   caller left reach at zero. Ground layout evidence: BgCh head 0x10,
   ClsnResult 0x10, pos Vec3 0x38 (func_020374d4 writes it), reach 0x4c
   (Player::InitResources writes td*2 there).

   NOT the tunneling path, contrary to the note this comment used to carry.
   WithMeshClsn's continuous update never builds a RaycastGround; it sweeps
   RaycastLine segments from the actor's PREV POS, and the fast falls that
   passed through thin decks did so because prev pos was stale, not because
   this adapter shortened anything. Gate 15 (the real BeforeBehavior) fixed
   that: a 2100-unit fall at 75 units/frame now stops on the bridge deck. */
static int __fastcall slot_ground(void *self, void *, unsigned char *g)
{
    unsigned char line[0x64];
    memset(line, 0, sizeof line);
    memcpy(line, g, 0x10);                       /* BgCh flags ride along */
    memcpy(line + 0x38, g + 0x38, 12);           /* lineStart = pos */
    memcpy(line + 0x54, g + 0x38, 12);           /* lineEnd = pos ... */
    int reach = *(int *)(g + 0x4c);
    if (reach <= 0) reach = 0xA0000;             /* 160 units default */
    *(int *)(line + 0x58) -= reach;              /* ... straight down */
    *(int *)(line + 0x60) = 0x7FFFFFF;           /* best-dist seed */
    int hit = ((MeshCollider *)self)->MeshCollider::DetectClsn(
        *(RaycastLine *)line);
    if (getenv("PORT_TRACE_CLSN"))
        fprintf(stderr, "  [ground] start(%d,%d,%d) reach=%d head4=%d "
                        "-> hit=%d y=%d\n",
                ((int *)(line + 0x38))[0], ((int *)(line + 0x38))[1],
                ((int *)(line + 0x38))[2], reach, line[4], hit,
                ((int *)(line + 0x54))[1]);
    if (hit) {
        /* hit fields only: SurfaceInfo (+0x14, 20 bytes) and tri (+0x28).
           NEVER the whole result head -- the old 0x1c-byte copy from the
           un-ctor'd stack line clobbered the ground result's vtable,
           clsn slot (0x18 = empty sentinel) and objID (-1 = no actor),
           and the zeros sent UpdateExtraContinous walking slot 0xffff
           and FindWithID hunting actor 0 */
        memcpy(g + 0x14, line + 0x14, 20);
        *(unsigned short *)(g + 0x28) = *(unsigned short *)(line + 0x28);
        /* ground result fields (func_02037464 resets them: y at +0x44
           seeded 0x80000000, flag at +0x48) */
        if (((int *)(line + 0x54))[1] > *(int *)(g + 0x44))
            *(int *)(g + 0x44) = ((int *)(line + 0x54))[1];
        g[0x48] = 1;
    }
    return hit;
}

/* direct probe for harnesses: vertical line walk, returns 1 and writes
   *out_y on hit */
extern "C" int hal_ground_ray(void *mc, int x, int y, int z, int reach,
                              int *out_y)
{
    unsigned char line[0x64];
    memset(line, 0, sizeof line);
    line[4] = 1;                                 /* collide ordinary */
    ((int *)(line + 0x38))[0] = x;
    ((int *)(line + 0x38))[1] = y;
    ((int *)(line + 0x38))[2] = z;
    ((int *)(line + 0x54))[0] = x;
    ((int *)(line + 0x54))[1] = y - reach;
    ((int *)(line + 0x54))[2] = z;
    *(int *)(line + 0x60) = 0x7FFFFFF;           /* best-dist seed */
    int hit = ((MeshCollider *)mc)->MeshCollider::DetectClsn(
        *(RaycastLine *)line);
    if (hit && out_y)
        *out_y = ((int *)(line + 0x54))[1];      /* clsnPos.y (walk writes
                                                    the end as the hit) */
    return hit;
}

/* arbitrary line for camera occlusion: from a toward b, returns hit and
   the clip point */
extern "C" int hal_line_ray(void *mc, const int *a, const int *b, int *out)
{
    unsigned char line[0x64];
    memset(line, 0, sizeof line);
    line[4] = 1;
    memcpy(line + 0x38, a, 12);
    memcpy(line + 0x54, b, 12);
    *(int *)(line + 0x60) = 0x7FFFFFF;
    int hit = ((MeshCollider *)mc)->MeshCollider::DetectClsn(
        *(RaycastLine *)line);
    if (hit && out)
        memcpy(out, line + 0x54, 12);            /* clsnPos */
    return hit;
}

/* Sphere overload (ROM slot 8) -- MeshCollider::DetectClsn(SphereClsn &),
   ITCM 0x01ffb830, 0x1bc8 bytes = 7112, the largest unmatched function in
   the game (notes/itcm.md). It is not just the wall pass: it returns floor,
   wall and ceiling as one three-bit mask (see SphereClsn::DetectClsn, which
   fans bit 0/1/2 out to func_020379d0/9c/68), and it is what holds a STANDING
   actor up. WithMeshClsn's swept RaycastLine finds a floor only once the
   sweep crosses the floor plane, so while this was stubbed the Player sank
   about his own height before the sweep caught him and shoved him back --
   a 46-unit bob at 3 Hz, and the whole reason the harness ground snap and
   wall stop existed.

   HOSTED since the slot-8 transcription: port/unmatched/
   MeshCollider_DetectClsn_Sphere.cpp, read straight off the ROM. Its header
   carries the provenance and the basis convention.

   SM64DS_NO_SPHERE=1 puts the stub back, which is how the two configurations
   get A/B'd (and what the pre-transcription screenshots were taken under).

   SM64DS_SPHERE_RADIUS_PCT=N scales the incoming sphere's radius, which is
   the only place that lever fits. WithMeshClsn+0x18 is the radius and +0x1c
   the vertical offset added to pos before the sphere is placed, and
   Player::Behavior recomputes BOTH every frame from the mega/balloon factor
   and stores the SAME value in each -- so an actor-level radius change is
   overwritten before it is read, and even the game's own mega lever cancels
   out of the resting height, which is floor - vo + radius. Scaled here the
   two separate, and the resting height has to move by exactly the radius
   delta: that is the cheapest end-to-end check that R = radius << 4 carries
   the right shift. */
static int __fastcall slot_sphere(void *self, void *, void *sph)
{
    static int off = -1;
    static int radius_pct = 0;
    if (off < 0) {
        off = getenv("SM64DS_NO_SPHERE") ? 1 : 0;
        if (const char *p = getenv("SM64DS_SPHERE_RADIUS_PCT"))
            radius_pct = atoi(p);
    }
    if (radius_pct) {
        int *r = (int *)((char *)sph + 0x48);
        *r = (int)((long long)*r * radius_pct / 100);
    }
    if (off) {
        static int warned;
        if (!warned) {
            warned = 1;
            fprintf(stderr, "  [clsn] sphere DetectClsn forced OFF "
                            "(SM64DS_NO_SPHERE) -- no ground or wall contact "
                            "from the game's own tracking\n");
        }
        return 0;
    }
    int r = ((MeshCollider *)self)->MeshCollider::DetectClsn(
        *(SphereClsn *)sph);
    if (r && getenv("PORT_TRACE_CLSN")) {
        const unsigned char *s = (const unsigned char *)sph;
        fprintf(stderr, "  [sphere] mask=%d flags=%02x tri=%d kind=%d "
                        "r=%d cy=%d class(f/e/c)=%d/%d/%d push=(%d,%d,%d)\n",
                r, s[0x70], g_sphere_dbg[14], g_sphere_dbg[15],
                ((const int *)(s + 0x48))[0], ((const int *)(s + 0x3c))[1],
                g_sphere_dbg[4], g_sphere_dbg[5], g_sphere_dbg[6],
                ((const int *)(s + 0x58))[0] + ((const int *)(s + 0x64))[0],
                ((const int *)(s + 0x58))[1] + ((const int *)(s + 0x64))[1],
                ((const int *)(s + 0x58))[2] + ((const int *)(s + 0x64))[2]);
    }
    return r;
}

#define TRAP(n) \
    static void __fastcall slot_trap##n(void *, void *) { \
        fprintf(stderr, "FATAL: MeshCollider vtable slot %d dispatched " \
                        "with no filler (clsn_vtable.cpp)\n", n); \
        abort(); }
TRAP(0) TRAP(1) TRAP(6) TRAP(8) TRAP(10) TRAP(11) TRAP(12)

#ifndef _WIN32
/* Linux/Itanium: the collision vtable is dispatched by GCC virtual calls
   (RaycastGround::DetectClsn does o->v6(this)), which use the SysV cdecl
   convention -- `this` is the FIRST STACK argument, not ecx -- and the ROM
   vtable keeps TWO dtor slots (D1/D0). The __fastcall slots above expect this
   in ecx, so a GCC dispatch handed slot_ground a garbage this from ecx and it
   walked a garbage octree. These plain-cdecl thunks take `this` first, and the
   Linux table below is laid out in ROM/Itanium slot order (see the map next to
   the array). */
static void slotL_dtor(void *) {}
static void slotL_v08(void *self)
{ ((MeshCollider *)self)->MeshCollider::Virtual08(); }
static void slotL_surf(void *self, s16 tri, SurfaceInfo *res)
{ ((MeshCollider *)self)->MeshCollider::GetSurfaceInfo(tri, *res); }
static void slotL_norm(void *self, s16 tri, Vector3 *res)
{ ((MeshCollider *)self)->MeshCollider::GetNormal(tri, *res); }
static void slotL_orig(void *self, s16 tri, Vector3 *res)
{ ((MeshCollider *)self)->MeshCollider::GetTriangleOrigin(tri, *res); }
static int  slotL_ray(void *self, RaycastLine *ray)
{ return ((MeshCollider *)self)->MeshCollider::DetectClsn(*ray); }
static int  slotL_ground(void *self, unsigned char *g)
{ return slot_ground(self, 0, g); }        /* reuse the ground->line adapter */
static int  slotL_sphere(void *self, void *sph)
{ return slot_sphere(self, 0, sph); }
static void slotL_beforeclsn(void *self, ClsnResult *res, Actor *actor,
                             Vector3 *pos, Vector3_16 *motionAng, Vector3_16 *ang)
{
    MeshColliderBase *base = (MeshColliderBase *)self;
    base->beforeClsnCallback(base, actor, res, pos, motionAng, ang);
}
static void slotL_trap(void *)
{ fprintf(stderr, "FATAL: MeshCollider vtable trap slot dispatched "
                  "(clsn_vtable.cpp, Linux)\n"); abort(); }
#endif

// SLOT ORDER IS MSVC'S, NOT THE ROM'S. The dispatching code here is
// MSVC-compiled against include/MeshCollider.h, and MSVC lays the table
// with a ONE-slot destructor (the ROM's Itanium layout spends two). Filling
// the array in ROM order put GetSurfaceInfo where MSVC reads GetNormal, and
// its internal virtual call recursed into itself until the stack died --
// the exact D1/D0-vs-scalar-dtor skew the earlier gates dodged by never
// dispatching. One more MSVC quirk pinned here: adjacent overloads
// (the DetectClsn trio) are emitted in REVERSE declaration order.
#ifdef _WIN32
extern "C" void *_ZTV12MeshCollider[13] = {
    (void *)slot_trap0,         /* 0: scalar deleting dtor */
    (void *)slot_v08,           /* 1: Virtual08 */
    (void *)slot_surf,          /* 2: GetSurfaceInfo */
    (void *)slot_norm,          /* 3: GetNormal -- the walk's hot slot */
    (void *)slot_orig,          /* 4: GetTriangleOrigin */
    (void *)slot_trap1,         /* 5: GetTriangleOrigin (ROM numbering) */
    (void *)slot_ground,        /* 6: DetectClsn(RaycastGround) - adapter */
    (void *)slot_ray,           /* 7: DetectClsn(RaycastLine) */
    (void *)slot_sphere,        /* 8: DetectClsn(SphereClsn) - stub */
    (void *)slot_beforeclsn,    /* 9: BeforeClsn - the platform-carry seat */
    (void *)slot_trap10,
    (void *)slot_trap11, (void *)slot_trap12,
};
#else
/* Linux/Itanium slot order (ROM vtable 0x020993dc, two dtor slots), cdecl
   thunks. Confirmed from config relocs: [0]=D1 [1]=D0 [2]=Virtual08
   [3]=GetSurfaceInfo [4]=GetNormal [5]=GetTriangleOrigin
   [6]=DetectClsn(RaycastGround) [7]=DetectClsn(RaycastLine)
   [8]=DetectClsn(SphereClsn) [9]=BeforeClsn. */
extern "C" void *_ZTV12MeshCollider[13] = {
    (void *)slotL_dtor,         /* 0: ~MeshCollider D1 */
    (void *)slotL_dtor,         /* 1: ~MeshCollider D0 */
    (void *)slotL_v08,          /* 2: Virtual08 */
    (void *)slotL_surf,         /* 3: GetSurfaceInfo */
    (void *)slotL_norm,         /* 4: GetNormal */
    (void *)slotL_orig,         /* 5: GetTriangleOrigin */
    (void *)slotL_ground,       /* 6: DetectClsn(RaycastGround) */
    (void *)slotL_ray,          /* 7: DetectClsn(RaycastLine) */
    (void *)slotL_sphere,       /* 8: DetectClsn(SphereClsn) */
    (void *)slotL_beforeclsn,   /* 9: BeforeClsn */
    (void *)slotL_trap,
    (void *)slotL_trap, (void *)slotL_trap,
};
#endif

// MovingMeshCollider inherits the surface queries; its table starts as a copy
// of MeshCollider's, which is all the binaries that stop at gate 8/9 need --
// the only collider on the level then is the level's, a plain MeshCollider.
//
// GATE 16 GOES FURTHER. SIGN_POST, BLACK_BRICK_BLOCK and CASTLE_WATER each
// Enable a MovingMeshCollider of their own onto data_020a0c80, and everything
// a moving collider answers is expressed in ITS OWN space and transformed
// back -- so its six overrides have to dispatch or the sign post's collision
// sits at the world origin instead of under the sign. Those slots are filled
// by hal_fill_moving_mesh_collider_vtable in hal/actor_class_faces.cpp, which
// only the binaries carrying the actor classes link; this stays the base.
extern "C" {
void *_ZTV18MovingMeshCollider[16];
void hal_fill_mmc_vtable(void)
{
    for (int i = 0; i < 13; ++i)
        _ZTV18MovingMeshCollider[i] = _ZTV12MeshCollider[i];
}
}

// ---- the moving collider's scratch RaycastLine ------------------------------
//
// MovingMeshCollider::DetectClsn(RaycastLine &) and its RaycastGround sibling
// do not build a ray on the stack: they fill ONE static one at arm9
// 0x020a0d0c, walk it, and read the answer back out. dsd split that object
// three ways at the offsets code happened to name --
//
//     data_020a0d0c  the ray itself
//     data_020a0d1c  +0x10, its ClsnResult (copied back to the caller's)
//     data_020a0d60  +0x54, its line END, which is where the walk writes the
//                    hit position
//
// -- so three separate host arrays would have the walk fill one object and the
// caller read two strangers. Grouped sections put them back in ROM order, the
// mechanism hal/level_boot.cpp uses for the save block and romdata.py for the
// camera-mode table: MSVC sorts by the part after the `$`, and every delta
// here equals the symbol's own size.
#include "port_msvc_compat.h"
#define CLSNSCRATCH(sec, name, size) \
    extern "C" PORT_GROUPED_DECL(sec, ".mmcray") unsigned char name[size] = {0}

CLSNSCRATCH(".mmcray$0000", data_020a0d0c, 0x10);
CLSNSCRATCH(".mmcray$0001", data_020a0d1c, 0x44);
CLSNSCRATCH(".mmcray$0002", data_020a0d60, 0x24);

#undef CLSNSCRATCH


#ifndef _WIN32
/* ARM tail-call VENEER passthrough (Linux/x86 fix, no src edit).
 *
 * Several matched TUs are one-instruction ARM veneers: `ldr ip,[pc]; bx ip;
 * .word <target>` -- a transparent tail-branch that leaves r0 (this) untouched,
 * so the caller's `this` flows straight into the target. Their byte-matched C
 * form is `void veneer(void){ target(); }`, which is correct on ARM (bx keeps
 * r0) but BREAKS on x86/GCC: the C function establishes its own frame and does
 * NOT forward the caller's stack `this`, so the target reads garbage (SIGSEGV
 * in the first frame's Player continuous-collision, func_020383e4 -> garbage
 * this -> WithMeshClsn::UpdateExtraContinous).
 *
 * These strong definitions take the `this` the caller actually passes and
 * tail-forward it. They win over the src TU by link order (hal/ links before
 * src/, and the port links with --allow-multiple-definition). Windows is
 * unaffected: guarded !_WIN32, and on MSVC the src veneers work as-is.
 */
extern "C" {
void _ZN12WithMeshClsn20UpdateExtraContinousEv(void *self);
void _ZN12WithMeshClsn16UpdateContinuousEv(void *self);
void func_020383e4(void *self)               { _ZN12WithMeshClsn20UpdateExtraContinousEv(self); }
void WithMeshClsn_UpdateContinuous_Veneer(void *self) { _ZN12WithMeshClsn16UpdateContinuousEv(self); }
}
#endif
