// Gate 16: the MovingMeshCollider vtable.
//
// hal/clsn_vtable.cpp seeds this table with MeshCollider's, which is right for
// every binary whose only collider is the level's. Gate 16 puts real moving
// colliders on the list -- SIGN_POST, BLACK_BRICK_BLOCK and CASTLE_WATER each
// Enable one, and every Player probe walks data_020a0c80 -- so the six
// overrides go in, all matched src:
//
//   3/4/5  GetNormal, GetTriangleOrigin: func_02039db8 / func_02039e30 rotate
//          the KCL's own normal and origin out by the collider's matrix
//   6/7/8  the DetectClsn triple: transform the incoming ray or sphere INTO
//          the collider's space (func_02039e48), run MeshCollider's own walk,
//          scale the answer back out by the collider's scale
//  10/11/12 TransformPos, GetAngularVelY, GetVelocity: what a riding actor
//          reads to be carried
//
// Without them a moving collider is walked as if its matrix were the identity,
// which puts the sign post's collision at the world origin.
//
// SLOT NUMBERING is the seed table's existing mixture, not a new decision.
// 0..5 are MSVC's -- the ITCM walk is compiled as a real method and reaches
// GetNormal through slot 3 -- and 6..12 are the ROM's, because every
// DetectClsn caller in src is a transcribed shadow class that counts two
// destructor slots (SphereClsn::DetectClsn dispatches v8, RaycastGround's v6,
// MovingMeshCollider::Transform's v12). Index 5 takes GetTriangleOrigin as
// well as index 4, the same fill-the-table-twice the Model render seam uses.
#include "MovingMeshCollider.h"
#include <cstdio>

extern "C" {
extern void *_ZTV18MovingMeshCollider[16];   /* storage: hal/clsn_vtable.cpp */
int _ZN18MovingMeshCollider10DetectClsnER13RaycastGround(void *self, void *g);
int _ZN18MovingMeshCollider10DetectClsnER11RaycastLine(void *self, void *r);
int _ZN18MovingMeshCollider10DetectClsnER10SphereClsn(void *self, void *s);
extern unsigned char data_020a0d0c[], data_020a0d1c[], data_020a0d60[];
/* BATCH-3 LINKAGE SEAT: the MovingMeshCollider's OWN deleting dtor (D0, arm9
   0x0203a444, 2004/b56 byte-match). The seed table copies MeshCollider's D0
   into slot 0, which is the wrong class body for a moving collider; this seats
   MMC's own. Flat-C `MMC *D0(MMC *this)` (this on the stack), so slot 0 takes
   the ecx->arg adapter below rather than the body directly. */
void *_ZN18MovingMeshColliderD0Ev(void *self);
}

typedef MovingMeshCollider MMC;

static void __fastcall mmc_dtor(void *s, void *)
{ _ZN18MovingMeshColliderD0Ev(s); }
static void __fastcall mmc_v08(void *s, void *)
{ ((MMC *)s)->MMC::Virtual08(); }
static void __fastcall mmc_norm(void *s, void *, s16 tri, Vector3 *res)
{ ((MMC *)s)->MMC::GetNormal(tri, *res); }
static void __fastcall mmc_orig(void *s, void *, s16 tri, Vector3 *res)
{ ((MMC *)s)->MMC::GetTriangleOrigin(tri, *res); }
static int __fastcall mmc_ground(void *s, void *, void *g)
{ return _ZN18MovingMeshCollider10DetectClsnER13RaycastGround(s, g); }
static int __fastcall mmc_line(void *s, void *, void *r)
{ return _ZN18MovingMeshCollider10DetectClsnER11RaycastLine(s, r); }
static int __fastcall mmc_sphere(void *s, void *, void *sp)
{ return _ZN18MovingMeshCollider10DetectClsnER10SphereClsn(s, sp); }
static int __fastcall mmc_tpos(void *s, void *, const Vector3 *p, Vector3 *r)
{ return ((MMC *)s)->MMC::TransformPos(*p, *r); }
static s16 __fastcall mmc_angvel(void *s, void *)
{ return ((MMC *)s)->MMC::GetAngularVelY(); }
static void __fastcall mmc_vel(void *s, void *, Vector3 *r)
{ ((MMC *)s)->MMC::GetVelocity(*r); }
/* Slot 9 -- BeforeClsn, the platform-carry seat, inherited from
   MeshColliderBase (MovingMeshCollider.h: "Overrides every slot except
   BeforeClsn (slot 9) and GetSurfaceInfo"). A rider standing on a moving
   collider fires this every collision step to run the beforeClsnCallback that
   carries it along. The seed copy from _ZTV12MeshCollider now brings the base
   body's shim across (hal/clsn_vtable.cpp slot_beforeclsn), but this table names
   every slot it owns, so it is pinned here too rather than left implicit. Same
   inlined ROM body as the base shim -- `beforeClsnCallback(this, actor, &res,
   &pos, motionAng, ang)` off MeshColliderBase+0x18 with the arg-order swap --
   for the same reason (the matched src TU is not linked in every target). */
/* THE CARRY-SLOT INDEX SKEW, and why this shim swaps a slot.
 *
 * The seat bodies below (mmc_tpos/mmc_angvel/mmc_vel) are the derived
 * MovingMeshCollider overrides a riding actor reads to be carried. The table
 * is filled in ROM slot order -- BeforeClsn 9, TransformPos 10, GetAngularVelY
 * 11, GetVelocity 12 -- because every ROM-transcribed shadow class that
 * dispatches into a collider counts the Itanium TWO destructor slots
 * (func_02038324's Obj::m9 hits BeforeClsn at 9, MovingMeshCollider::Transform's
 * VObj::v12 hits GetVelocity at 12).
 *
 * But the carry is not driven by a ROM shadow. It is driven by the four MATCHED
 * MeshColliderBase UpdatePos.../UpdateAngs... callbacks, and THOSE are compiled
 * by MSVC against MeshColliderBase.h, where the destructor is ONE slot (the D1/D0
 * fold this whole file already documents for slots 0-5). So MSVC emits their
 * virtual calls one index LOW: UpdatePosWithTransform calls TransformPos through
 * [ecx+0x24] = index 9, UpdateAngsWithAngularVelY calls GetAngularVelY through
 * index 10, UpdatePosWithVelocity calls GetVelocity through [eax+0x2C] = index
 * 11 (both confirmed by disassembly). Filled ROM-order, index 9 held
 * mmc_beforeclsn, so UpdatePosWithTransform re-entered BeforeClsn instead of
 * TransformPos and the carry recursed until pos went null -- the lift-ride
 * crash's deeper fault.
 *
 * The fix seats each carry override at the MSVC index its caller reads:
 * TransformPos at 9, GetAngularVelY at 10, GetVelocity at 11, and GetVelocity
 * ALSO at 12 for the ROM shadow (Transform's v12). Indices 10/11 are read by no
 * ROM shadow, so that is free. Index 9 is the ONE genuine collision: the ROM
 * shadow needs BeforeClsn there, the MSVC carry needs TransformPos there. They
 * never want it at the same instant -- BeforeClsn is the OUTER call
 * (func_02038324 -> m9), TransformPos is INNER (reached only from the callback
 * BeforeClsn invokes), and the callback never re-enters func_02038324. So the
 * shim parks TransformPos in index 9 for the duration of the callback and puts
 * BeforeClsn back on the way out. Single-threaded; the callback fully returns
 * before this frame does. */
static void __fastcall mmc_beforeclsn(void *s, void *, ClsnResult *res,
                                      Actor *actor, Vector3 *pos,
                                      Vector3_16 *motionAng, Vector3_16 *ang)
{
    MeshColliderBase *base = (MeshColliderBase *)s;
    /* Park TransformPos in the MSVC carry index (9) for the callback, restore
       BeforeClsn after. See the block comment above. */
    void *saved9 = _ZTV18MovingMeshCollider[9];
    _ZTV18MovingMeshCollider[9] = (void *)mmc_tpos;
    base->beforeClsnCallback(base, actor, res, pos, motionAng, ang);
    _ZTV18MovingMeshCollider[9] = saved9;
}

#ifndef _WIN32
/* Linux/Itanium: GCC dispatches the collision virtuals SysV-cdecl (`this` on
   the stack, not ecx) over a TWO-dtor-slot vtable. Straight ROM slot order,
   NO MSVC carry-slot skew: the matched MeshColliderBase carry callbacks are
   GCC-compiled here too, so they call TransformPos/GetAngularVelY/GetVelocity
   through the SAME Itanium indices (10/11/12) the ROM vtable uses. ROM layout
   (0x02099434): [2]Virtual08 [3]GetSurfaceInfo [4]GetNormal [5]GetTriangleOrigin
   [6]DetectClsn(RaycastGround) [7](RaycastLine) [8](SphereClsn) [9]BeforeClsn
   [10]TransformPos [11]GetAngularVelY [12]GetVelocity. */
/* MovingMeshCollider's OWN deleting dtor, cdecl for GCC dispatch. The MSVC path
   below seats the same body at its folded dtor slot 0; on Itanium there are two
   dtor slots and the deleting one is [1], so that is where this goes, leaving
   the seed copy's MeshCollider D1 at [0].
   Read off the ROM rather than assumed: _ZTV18MovingMeshCollider (0x02099434)
   relocates slot 0 -> 0x0203a470 (D1, complete) and slot 1 -> 0x0203a444 (D0,
   deleting), with the first real virtual at slot 2 -- which is also why the
   fill below starts at [2]. */
static void mmcL_dtor(void *s) { _ZN18MovingMeshColliderD0Ev(s); }
static void mmcL_v08(void *s) { ((MMC *)s)->MMC::Virtual08(); }
static void mmcL_norm(void *s, s16 tri, Vector3 *res) { ((MMC *)s)->MMC::GetNormal(tri, *res); }
static void mmcL_orig(void *s, s16 tri, Vector3 *res) { ((MMC *)s)->MMC::GetTriangleOrigin(tri, *res); }
static int  mmcL_ground(void *s, void *g) { return _ZN18MovingMeshCollider10DetectClsnER13RaycastGround(s, g); }
static int  mmcL_line(void *s, void *r) { return _ZN18MovingMeshCollider10DetectClsnER11RaycastLine(s, r); }
static int  mmcL_sphere(void *s, void *sp) { return _ZN18MovingMeshCollider10DetectClsnER10SphereClsn(s, sp); }
static int  mmcL_tpos(void *s, const Vector3 *p, Vector3 *r) { return ((MMC *)s)->MMC::TransformPos(*p, *r); }
static s16  mmcL_angvel(void *s) { return ((MMC *)s)->MMC::GetAngularVelY(); }
static void mmcL_vel(void *s, Vector3 *r) { ((MMC *)s)->MMC::GetVelocity(*r); }
static void mmcL_beforeclsn(void *s, ClsnResult *res, Actor *actor, Vector3 *pos,
                            Vector3_16 *motionAng, Vector3_16 *ang)
{
    MeshColliderBase *base = (MeshColliderBase *)s;
    base->beforeClsnCallback(base, actor, res, pos, motionAng, ang);
}
#endif

extern "C" void hal_fill_moving_mesh_collider_vtable(void)
{
#ifndef _WIN32
    _ZTV18MovingMeshCollider[1]  = (void *)mmcL_dtor;
    _ZTV18MovingMeshCollider[2]  = (void *)mmcL_v08;
    /* [3] GetSurfaceInfo is NOT overridden by MovingMeshCollider -- the seed
       copy from _ZTV12MeshCollider already carries MeshCollider's, leave it. */
    _ZTV18MovingMeshCollider[4]  = (void *)mmcL_norm;
    _ZTV18MovingMeshCollider[5]  = (void *)mmcL_orig;
    _ZTV18MovingMeshCollider[6]  = (void *)mmcL_ground;
    _ZTV18MovingMeshCollider[7]  = (void *)mmcL_line;
    _ZTV18MovingMeshCollider[8]  = (void *)mmcL_sphere;
    _ZTV18MovingMeshCollider[9]  = (void *)mmcL_beforeclsn;
    _ZTV18MovingMeshCollider[10] = (void *)mmcL_tpos;
    _ZTV18MovingMeshCollider[11] = (void *)mmcL_angvel;
    _ZTV18MovingMeshCollider[12] = (void *)mmcL_vel;
    return;
#endif
    /* Batch-3: slot 0 gets MovingMeshCollider's OWN deleting dtor, replacing
       the MeshCollider D0 the seed copy left there (the wrong class body). */
    _ZTV18MovingMeshCollider[0] = (void *)mmc_dtor;
    _ZTV18MovingMeshCollider[2] = (void *)mmc_v08;
    _ZTV18MovingMeshCollider[3] = (void *)mmc_norm;
    _ZTV18MovingMeshCollider[4] = (void *)mmc_orig;
    _ZTV18MovingMeshCollider[5] = (void *)mmc_orig;
    _ZTV18MovingMeshCollider[6] = (void *)mmc_ground;
    _ZTV18MovingMeshCollider[7] = (void *)mmc_line;
    _ZTV18MovingMeshCollider[8] = (void *)mmc_sphere;
    /* Carry slots seated at the MSVC indices their matched callers read
       (TransformPos 9, GetAngularVelY 10, GetVelocity 11), plus GetVelocity at
       the ROM index 12 for MovingMeshCollider::Transform's v12 shadow. Index 9
       starts as BeforeClsn for the ROM func_02038324 -> m9 entry; mmc_beforeclsn
       parks mmc_tpos there for the duration of the callback. See the block
       comment on mmc_beforeclsn for the full index-skew reasoning. */
    _ZTV18MovingMeshCollider[9] = (void *)mmc_beforeclsn;
    _ZTV18MovingMeshCollider[10] = (void *)mmc_angvel;
    _ZTV18MovingMeshCollider[11] = (void *)mmc_vel;
    _ZTV18MovingMeshCollider[12] = (void *)mmc_vel;
    /* The three moving-collider DetectClsn bodies fill ONE static
       RaycastLine at arm9 0x020a0d0c and read the answer back out of two
       symbols dsd split off its interior. Say so once if the linker ever
       stops packing them. */
    if (data_020a0d1c - data_020a0d0c != 0x10 ||
        data_020a0d60 - data_020a0d0c != 0x54)
        std::fprintf(stderr, "  [clsn] MOVING-COLLIDER SCRATCH NOT "
                     "CONTIGUOUS: +%d +%d\n",
                     (int)(data_020a0d1c - data_020a0d0c),
                     (int)(data_020a0d60 - data_020a0d0c));
}
