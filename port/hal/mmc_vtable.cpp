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
#include "dBgW_KcMbg.h"
#include <cstdio>

extern "C" {
extern void *_ZTV10dBgW_KcMbg[16];   /* storage: hal/clsn_vtable.cpp */
int _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Gnd(void *self, void *g);
int _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Lin(void *self, void *r);
int _ZN10dBgW_KcMbg10DetectClsnER12dBgCh_SphCrr(void *self, void *s);
extern unsigned char data_020a0d0c[], data_020a0d1c[], data_020a0d60[];
/* BATCH-3 LINKAGE SEAT: the MovingMeshCollider's OWN deleting dtor (D0, arm9
   0x0203a444, 2004/b56 byte-match). The seed table copies MeshCollider's D0
   into slot 0, which is the wrong class body for a moving collider; this seats
   MMC's own. Flat-C `MMC *D0(MMC *this)` (this on the stack), so slot 0 takes
   the ecx->arg adapter below rather than the body directly. */
void *_ZN10dBgW_KcMbgD0Ev(void *self);
}

typedef dBgW_KcMbg MMC;

static void __fastcall mmc_dtor(void *s, void *)
{ _ZN10dBgW_KcMbgD0Ev(s); }
static void __fastcall mmc_v08(void *s, void *)
{ ((MMC *)s)->MMC::Virtual08(); }
static void __fastcall mmc_norm(void *s, void *, s16 tri, Vector3 *res)
{ ((MMC *)s)->MMC::GetNormal(tri, *res); }
static void __fastcall mmc_orig(void *s, void *, s16 tri, Vector3 *res)
{ ((MMC *)s)->MMC::GetTriangleOrigin(tri, *res); }
static int __fastcall mmc_ground(void *s, void *, void *g)
{ return _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Gnd(s, g); }
static int __fastcall mmc_line(void *s, void *, void *r)
{ return _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Lin(s, r); }
static int __fastcall mmc_sphere(void *s, void *, void *sp)
{ return _ZN10dBgW_KcMbg10DetectClsnER12dBgCh_SphCrr(s, sp); }
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
   carries it along. The seed copy from _ZTV7dBgW_Kc now brings the base
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
static void __fastcall mmc_beforeclsn(void *s, void *, dBgPi *res,
                                      dActor_c *actor, Vector3 *pos,
                                      Vector3_16 *motionAng, Vector3_16 *ang)
{
    dBgW *base = (dBgW *)s;
    /* Park TransformPos in the MSVC carry index (9) for the callback, restore
       BeforeClsn after. See the block comment above. */
    void *saved9 = _ZTV10dBgW_KcMbg[9];
    _ZTV10dBgW_KcMbg[9] = (void *)mmc_tpos;
    base->beforeClsnCallback(base, actor, res, pos, motionAng, ang);
    _ZTV10dBgW_KcMbg[9] = saved9;
}

extern "C" void hal_fill_moving_mesh_collider_vtable(void)
{
    /* Batch-3: slot 0 gets MovingMeshCollider's OWN deleting dtor, replacing
       the MeshCollider D0 the seed copy left there (the wrong class body). */
    _ZTV10dBgW_KcMbg[0] = (void *)mmc_dtor;
    _ZTV10dBgW_KcMbg[2] = (void *)mmc_v08;
    _ZTV10dBgW_KcMbg[3] = (void *)mmc_norm;
    _ZTV10dBgW_KcMbg[4] = (void *)mmc_orig;
    _ZTV10dBgW_KcMbg[5] = (void *)mmc_orig;
    _ZTV10dBgW_KcMbg[6] = (void *)mmc_ground;
    _ZTV10dBgW_KcMbg[7] = (void *)mmc_line;
    _ZTV10dBgW_KcMbg[8] = (void *)mmc_sphere;
    /* Carry slots seated at the MSVC indices their matched callers read
       (TransformPos 9, GetAngularVelY 10, GetVelocity 11), plus GetVelocity at
       the ROM index 12 for MovingMeshCollider::Transform's v12 shadow. Index 9
       starts as BeforeClsn for the ROM func_02038324 -> m9 entry; mmc_beforeclsn
       parks mmc_tpos there for the duration of the callback. See the block
       comment on mmc_beforeclsn for the full index-skew reasoning. */
    _ZTV10dBgW_KcMbg[9] = (void *)mmc_beforeclsn;
    _ZTV10dBgW_KcMbg[10] = (void *)mmc_angvel;
    _ZTV10dBgW_KcMbg[11] = (void *)mmc_vel;
    _ZTV10dBgW_KcMbg[12] = (void *)mmc_vel;
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
