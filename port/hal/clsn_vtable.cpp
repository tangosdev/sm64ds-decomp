// The synthetic dBgW_Kc vtable (gate 8) -- the gate-3a mechanism at
// its second use, and the first with REAL slot fillers throughout the hot
// path: GetSurfaceInfo (matched, ITCM) calls GetNormal through the vtable
// (notes/itcm.md, "the one lever"), so slot 4 must dispatch for the octree
// walk to survive. Slots are __fastcall shims (ecx carries `this` exactly
// as __thiscall does; the dummy edx absorbs fastcall's second register),
// slot order per include/dBgW_Kc.h's ROM-read map: dtor 0/1,
// Virtual08 2, surface queries 3-5, the DetectClsn overloads 6-8.
// Unevidenced slots trap loudly.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dBgW_Kc.h"
#include "hal/dsstate_seg.h"

// BATCH-3 LINKAGE SEAT bridge. The real dBgW_Kc::DetectClsn(RaycastGround&)
// body (now seated in slot 6) reads/writes the shared prism SurfaceInfo global
// data_020a0cec, which it references with C++ type (mangled
// ?data_020a0cec@@3USurfaceInfo@@A), and so does the sphere overload in slot 8.
// The global is DEFINED once, flat-C, just below (_data_020a0cec). Bridge the
// typed reference to the flat definition, the same /alternatename mechanism the
// dBgW seat uses for its method bodies. One storage, both names.
#pragma comment(linker, "/alternatename:?data_020a0cec@@3USurfaceInfo@@A=_data_020a0cec")

/* THE GLOBAL SurfaceInfo the ROM reuses for every prism -- data_020a0cec,
   constructed once by __sinit_02074fe4 (_ZN5dBgPcC1Ev) with _ZN5dBgPcD1Ev
   registered as its atexit destructor. Five words carrying _ZN5dBgPcC1Ev's
   exact seed: c[0] = 0xfc0, c[1] = 0xff, c[2..4] = 0. GetSurfaceInfo rewrites
   all five before every read, so the seed only ever describes the state before
   the first prism of the first frame. It lived in the sphere pass's host
   transcription (port/unmatched/MeshCollider_DetectClsn_Sphere.cpp) until run
   linkfull lane RS5B retired that file for the matched body, and moved here,
   beside its alias, unchanged. */
extern "C" {
DSSTATE_BEGIN
u32 data_020a0cec[5] = { 0xfc0u, 0xffu, 0u, 0u, 0u };
DSSTATE_END
}

static void __fastcall slot_v08(void *self, void *)
{ ((dBgW_Kc *)self)->dBgW_Kc::Virtual08(); }
static void __fastcall slot_surf(void *self, void *, s16 tri, SurfaceInfo *res)
{ ((dBgW_Kc *)self)->dBgW_Kc::GetSurfaceInfo(tri, *res); }
static void __fastcall slot_norm(void *self, void *, s16 tri, Vector3 *res)
{ ((dBgW_Kc *)self)->dBgW_Kc::GetNormal(tri, *res); }
static void __fastcall slot_orig(void *self, void *, s16 tri, Vector3 *res)
{ ((dBgW_Kc *)self)->dBgW_Kc::GetTriangleOrigin(tri, *res); }
static int __fastcall slot_ray(void *self, void *, dBgCh_Lin *ray)
{ return ((dBgW_Kc *)self)->dBgW_Kc::DetectClsn(*ray); }

/* Slot 9 -- dBgW::BeforeClsn, the platform-carry seat. A moving
   collider (a lift, a rotating platform) that an actor is STANDING ON fires
   this every collision step; the body invokes the beforeClsnCallback the
   platform's InitResources stored via func_020393d4 (UpdatePosWithTransform and
   its siblings), which is what walks the rider along with the platform. Neither
   dBgW overrides it nor does MovingMeshCollider, so both tables
   inherit this one body (dBgW.h slot 9; MovingMeshCollider.h
   "Overrides every slot except BeforeClsn (slot 9) and GetSurfaceInfo").
   Left as slot_trap9 until this gate: the ride path was never proof-driven, so
   the first actor to stand on a moving platform aborted here.

   The one-line ROM body is inlined rather than dispatched to the matched
   src (_ZN16MeshColliderBase10BeforeClsn...cpp): that TU rides slice_gate16,
   which walk_window links but the gate-8/9 collision smoke targets do not, so a
   qualified `dBgW::BeforeClsn` call would leave them unresolved and
   only GATE8_EXTRA_SOURCES (CMakeLists) could repair that. The body is exactly
   `beforeClsnCallback(this, actor, &res, &pos, motionAng, ang)` with the
   documented arg-order swap (the virtual takes res-first, the callback
   actor-first); the pointer lives at dBgW+0x18, its type spelled by
   dBgW.h. PORT_HOST_ABI: __fastcall shim, ecx=this; the ROM body
   transcribed, the WaterBomb/host-copy reading (one less cross-TU dependency). */
static void __fastcall slot_beforeclsn(void *self, void *, dBgPi *res,
                                       dActor_c *actor, Vector3 *pos,
                                       Vector3_16 *motionAng, Vector3_16 *ang)
{
    dBgW *base = (dBgW *)self;
    base->beforeClsnCallback(base, actor, res, pos, motionAng, ang);
}

/* Ground overload (ROM slot 6) -- dBgW_Kc::DetectClsn(RaycastGround &),
   ITCM 0x01ffd3f8, 0x498 bytes. BATCH-3 LINKAGE SEAT: this now dispatches the
   REAL matched body (src/_ZN7dBgW_Kc10DetectClsnER9dBgCh_Gnd.cpp,
   2004/b56 byte-match), replacing the earlier stack-dBgCh_Lin adapter. The
   TU rides slice_gate8 (and GATE8_EXTRA_SOURCES for the gate-8/9 smoke
   targets), so the qualified call resolves in every target that links this
   table, and naming it here is the reference edge that pulls it in.

   The real body IS the downward ground probe (its own octree march through the
   (x, z) column), so the adapter that synthesised a vertical dBgCh_Lin is no
   longer needed: it built the same query the ROM body runs natively, only via
   the hosted line walk. The shim is now the same one-line qualified dispatch as
   slot_ray; the RaycastGround the caller passed carries its own pos and reach
   (BgCh head 0x10, dBgPi 0x10, pos Vec3 0x38, reach 0x4c) and the body
   fills the result fields in place. */
static int __fastcall slot_ground(void *self, void *, dBgCh_Gnd *g)
{
    int hit = ((dBgW_Kc *)self)->dBgW_Kc::DetectClsn(*g);
    if (getenv("PORT_TRACE_CLSN"))
        fprintf(stderr, "  [ground] real DetectClsn(RaycastGround) -> hit=%d\n",
                hit);
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
    int hit = ((dBgW_Kc *)mc)->dBgW_Kc::DetectClsn(
        *(dBgCh_Lin *)line);
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
    int hit = ((dBgW_Kc *)mc)->dBgW_Kc::DetectClsn(
        *(dBgCh_Lin *)line);
    if (hit && out)
        memcpy(out, line + 0x54, 12);            /* clsnPos */
    return hit;
}

/* Sphere overload (ROM slot 8) -- dBgW_Kc::DetectClsn(SphereClsn &),
   ITCM 0x01ffb830, 0x1bc8 bytes = 7112, the largest unmatched function in
   the game (notes/itcm.md). It is not just the wall pass: it returns floor,
   wall and ceiling as one three-bit mask (see SphereClsn::DetectClsn, which
   fans bit 0/1/2 out to func_020379d0/9c/68), and it is what holds a STANDING
   actor up. WithMeshClsn's swept dBgCh_Lin finds a floor only once the
   sweep crosses the floor plane, so while this was stubbed the Player sank
   about his own height before the sweep caught him and shoved him back --
   a 46-unit bob at 3 Hz, and the whole reason the harness ground snap and
   wall stop existed.

   THE ROM'S OWN BODY since run linkfull lane RS5B:
   src/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp, the 2004/b56 byte-match,
   built from its tools/hostgen.py copy so the DS square-root unit it drives
   (SQRTCNT / SQRT_PARAM stores) is the port's modelled hardware
   (port/CMakeLists.txt, the RS5B block). It replaced the slot-8 host
   transcription, port/unmatched/MeshCollider_DetectClsn_Sphere.cpp, whose
   counters (g_sphere_dbg) the trace line below used to print.

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
    int r = ((dBgW_Kc *)self)->dBgW_Kc::DetectClsn(
        *(dBgCh_SphCrr *)sph);
    if (r && getenv("PORT_TRACE_CLSN")) {
        const unsigned char *s = (const unsigned char *)sph;
        fprintf(stderr, "  [sphere] mask=%d flags=%02x "
                        "r=%d cy=%d push=(%d,%d,%d)\n",
                r, s[0x70],
                ((const int *)(s + 0x48))[0], ((const int *)(s + 0x3c))[1],
                ((const int *)(s + 0x58))[0] + ((const int *)(s + 0x64))[0],
                ((const int *)(s + 0x58))[1] + ((const int *)(s + 0x64))[1],
                ((const int *)(s + 0x58))[2] + ((const int *)(s + 0x64))[2]);
    }
    return r;
}

#define TRAP(n) \
    static void __fastcall slot_trap##n(void *, void *) { \
        fprintf(stderr, "FATAL: dBgW_Kc vtable slot %d dispatched " \
                        "with no filler (clsn_vtable.cpp)\n", n); \
        abort(); }
TRAP(0) TRAP(1) TRAP(8) TRAP(10) TRAP(11) TRAP(12)

// SLOT ORDER IS MSVC'S, NOT THE ROM'S. The dispatching code here is
// MSVC-compiled against include/dBgW_Kc.h, and MSVC lays the table
// with a ONE-slot destructor (the ROM's Itanium layout spends two). Filling
// the array in ROM order put GetSurfaceInfo where MSVC reads GetNormal, and
// its internal virtual call recursed into itself until the stack died --
// the exact D1/D0-vs-scalar-dtor skew the earlier gates dodged by never
// dispatching. One more MSVC quirk pinned here: adjacent overloads
// (the DetectClsn trio) are emitted in REVERSE declaration order.
extern "C" void *_ZTV7dBgW_Kc[13] = {
    (void *)slot_trap0,         /* 0: scalar deleting dtor (real D0 seated at
                                       boot by hal_seat_meshcollider_dtor in the
                                       walk_window family; smoke targets never
                                       delete the level collider, keep the trap) */
    (void *)slot_v08,           /* 1: Virtual08 */
    (void *)slot_surf,          /* 2: GetSurfaceInfo */
    (void *)slot_norm,          /* 3: GetNormal -- the walk's hot slot */
    (void *)slot_orig,          /* 4: GetTriangleOrigin */
    (void *)slot_trap1,         /* 5: GetTriangleOrigin (ROM numbering) */
    (void *)slot_ground,        /* 6: DetectClsn(RaycastGround) - real body */
    (void *)slot_ray,           /* 7: DetectClsn(dBgCh_Lin) */
    (void *)slot_sphere,        /* 8: DetectClsn(SphereClsn) - stub */
    (void *)slot_beforeclsn,    /* 9: BeforeClsn - the platform-carry seat */
    (void *)slot_trap10,
    (void *)slot_trap11, (void *)slot_trap12,
};

// MovingMeshCollider inherits the surface queries; its table starts as a copy
// of dBgW_Kc's, which is all the binaries that stop at gate 8/9 need --
// the only collider on the level then is the level's, a plain dBgW_Kc.
//
// GATE 16 GOES FURTHER. SIGN_POST, BLACK_BRICK_BLOCK and CASTLE_WATER each
// Enable a MovingMeshCollider of their own onto data_020a0c80, and everything
// a moving collider answers is expressed in ITS OWN space and transformed
// back -- so its six overrides have to dispatch or the sign post's collision
// sits at the world origin instead of under the sign. Those slots are filled
// by hal_fill_moving_mesh_collider_vtable in hal/actor_class_faces.cpp, which
// only the binaries carrying the actor classes link; this stays the base.
extern "C" {
void *_ZTV10dBgW_KcMbg[16];
void hal_fill_mmc_vtable(void)
{
    for (int i = 0; i < 13; ++i)
        _ZTV10dBgW_KcMbg[i] = _ZTV7dBgW_Kc[i];
}
}

// ---- the moving collider's scratch dBgCh_Lin ------------------------------
//
// MovingMeshCollider::DetectClsn(dBgCh_Lin &) and its RaycastGround sibling
// do not build a ray on the stack: they fill ONE static one at arm9
// 0x020a0d0c, walk it, and read the answer back out. dsd split that object
// three ways at the offsets code happened to name --
//
//     data_020a0d0c  the ray itself
//     data_020a0d1c  +0x10, its dBgPi (copied back to the caller's)
//     data_020a0d60  +0x54, its line END, which is where the walk writes the
//                    hit position
//
// -- so three separate host arrays would have the walk fill one object and the
// caller read two strangers. Grouped sections put them back in ROM order, the
// mechanism hal/level_boot.cpp uses for the save block and romdata.py for the
// camera-mode table: MSVC sorts by the part after the `$`, and every delta
// here equals the symbol's own size.
#define CLSNSCRATCH(sec, name, size) \
    __pragma(section(sec, read, write))                          \
    extern "C" __declspec(allocate(sec)) __declspec(align(4))    \
    unsigned char name[size] = {0}

CLSNSCRATCH(".dsstate$mmcray0000", data_020a0d0c, 0x10);
CLSNSCRATCH(".dsstate$mmcray0001", data_020a0d1c, 0x44);
CLSNSCRATCH(".dsstate$mmcray0002", data_020a0d60, 0x24);

#undef CLSNSCRATCH

