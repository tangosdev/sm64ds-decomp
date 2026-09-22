// HOST TRANSCRIPTION of dBgW_Kc::DetectClsn(dBgCh_SphCrr &)
// ITCM 0x01ffb830, 0x1bc8 bytes (7112, 1778 ARM instructions) -- vtable slot 8
// and the largest unmatched function in the game (notes/itcm.md).
//
// PROVENANCE. This is not a banked near-miss: there is no draft for this symbol
// in nearmiss/db.jsonl. It was read straight off the ROM, block by block, from
// a capstone listing of extracted/dsd/arm9/itcm.bin (base 0x01ff8000, file
// offset 0x3830) with the relocations resolved out of config/arm9/itcm/
// relocs.txt. Every block below carries the ROM address it came from, and the
// block boundaries are the ones in that annotated disassembly analysis. The
// fourteen callees are all already matched in src/, so nothing had to be
// invented to host it -- see CALLEES below.
//
// NOT byte-verified. When the byte match lands in src/, this file retires per
// the port rule, exactly like MeshCollider_DetectClsn_RaycastLine.cpp next to
// it.
//
// -------------------------------------------------------------------------
// BASIS CONVENTION -- read this before changing any shift.
//
// The ROM converts world Fix12i to KCL file units with a PLAIN >>6 (0x01ffb870
// `asr r1, r5, #6`), no scale multiply; DetectClsn(dBgCh_Lin&) at 0x01ffb0fc
// does the same at 0x01ffb110. SIX BITS, THE SAME SIX FOR EVERY COLLIDER IN
// THE GAME, and that is what both passes run now:
//
//   centre -> file    c >> 6              R, dot basis   radius << 4
//   box pad           (radius >> 6) + 0x40
//   push component    (depth*n) >> 16     vertex -> world   v0 << 6
//
// Both passes read the SAME KCL data through the SAME collider in the same
// frame -- WithMeshClsn sweeps a dBgCh_Lin and then places a dBgCh_SphCrr at
// the result -- so they have to agree, and now they agree by construction.
//
// WHAT THIS REPLACED, and why it had to go. The conversion used to route
// through the collider's own dBgW_Kc+0x2c / +0x38 words, with the level's
// pair written by hand as 0x40000 / 0x40 (port/hal/level_boot.cpp) so the
// multiply reduced to the ROM's shift. It reduced for the level and for
// nothing else: MovingMeshCollider::SetFile leaves those words at SetFile's
// 1.0, so every actor-owned collider ran its walk in WORLD units against a
// FILE-unit mesh and answered no to everything. The moat is what caught it --
// CASTLE_WATER registers a collider whose own CLPS carries the water flag
// 0x20, and no ray in the level could find it, so the Player's water probe
// never fired and walk never became swim.
//
// The aliasing hazard went with it. dBgW_Kc+0x28..0x30 is a Vector3 "up"
// axis (SetFile seeds 0, 0x1000, 0; MovingMeshCollider::SetFile and Transform
// rewrite it through func_02039e18 as the collider's own transformed axis),
// and +0x2c -- that vector's Y -- was the very word the port had repurposed.
// collider_up() had to rebuild it from the seed; it reads the real field now,
// which is what a rotated platform needs.
//
// -------------------------------------------------------------------------
// CALLEES (all matched; slice_gate8.txt carries them)
//   func_020396dc  triIdx = (prism - tris)/16      func_02039794  floor/wall/ceil
//   func_020397dc  |x| <= 8 near-zero guard        func_02037e58  clps type
//   func_02037fd4  dBgPi <- (tri, info)       func_020379f4  -> sphere+0x74
//   func_020379c0  -> sphere+0x9c                  func_0203798c  -> sphere+0xc4
//   func_0203794c  -> sphere+0xfc best normal      func_02037a6c  push AABB
//   DotVec3        ((s64)sum + 0x800) >> 12        cstd::fdiv     (a<<12)/b
//   SurfaceInfo::CopyNormalTo    BgCh::ShouldPassThroughImpl
//   dBgW_Kc::GetSurfaceInfo (ITCM 0x01ffd920, vtable slot 3)
//
//cpp
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dBgW_Kc.h"
#include "ntr/mmio.h"
#include "hal/dsstate_seg.h"

/* Completed here the way the line walk next door completes it: the 8-byte CLPS
   entry then the face normal, pinned by SurfaceInfo::CopyNormalTo (0x02037dcc).
   include/ only forward-declares it. */
struct SurfaceInfo { u8 clps[8]; Vector3 normal; };

/* ---------------------------------------------------------------------- */
/* dBgCh_SphCrr as this function touches it. Named SphereClsnLayout and not
   dBgCh_SphCrr on purpose: include/dBgCh_SphCrr.h is auto-generated from matched-
   function evidence and stops well short of what the sphere walk needs (it has
   neither the centre nor the radius), and quietly redefining the game's own
   type in one translation unit is exactly how records get skewed -- see the
   dBgPi packing lesson in 1e6025d2a. Every offset below is pinned by
   MATCHED code, named beside it, and asserted underneath. */
struct SphereClsnLayout {
    u8      head_000[0x10];     /* 0x00 BgCh head                            */
    u8      result[0x28];       /* 0x10 dBgPi, func_02037fd4's target   */
    u8      pad_038[0x4];       /* 0x38 the Sphere sub-object's vtable       */
    Vector3 center;             /* 0x3c func_0203abd4 via SetObjAndSphere    */
    Fix12i  radius;             /* 0x48 same call; MovingMeshCollider scales it */
    Vector3 resolvedPush;       /* 0x4c func_02037a38 = pushMin + pushMax    */
    Vector3 pushMin;            /* 0x58 func_02037a6c's AABB min             */
    Vector3 pushMax;            /* 0x64 func_02037a6c's AABB max             */
    u8      flags;              /* 0x70 bit0 any / 1 gate / 2 floor / 3 wall /
                                        4 ceiling / 5 no-slope / 6 no-corner;
                                        func_02037b5c clears exactly these   */
    u8      pad_071[3];
    u8      floorResult[0x28];  /* 0x74 func_020379f4                        */
    u8      wallResult[0x28];   /* 0x9c func_020379c0                        */
    u8      ceilResult[0x28];   /* 0xc4 func_0203798c                        */
    Fix12i  wallHeight;         /* 0xec 0 from the ctor; MMC scales it       */
    u8      pad_0f0[0xc];
    Vector3 bestFloorNormal;    /* 0xfc func_0203794c writes d[63..65];
                                        func_02037b5c seeds (0, -0x1000, 0)  */
    Fix12i  maxNormalY;         /* 0x108 SetObjAndSphere seeds 0x1000        */
};

/* Not decoration. A host build that slides one of these fails here rather than
   pushing actors around with a record read four bytes late. */
typedef char SphereClsnLayout_center_at_3c[
    (offsetof(struct SphereClsnLayout, center) == 0x3c) ? 1 : -1];
typedef char SphereClsnLayout_radius_at_48[
    (offsetof(struct SphereClsnLayout, radius) == 0x48) ? 1 : -1];
typedef char SphereClsnLayout_resolvedPush_at_4c[
    (offsetof(struct SphereClsnLayout, resolvedPush) == 0x4c) ? 1 : -1];
typedef char SphereClsnLayout_pushMin_at_58[
    (offsetof(struct SphereClsnLayout, pushMin) == 0x58) ? 1 : -1];
typedef char SphereClsnLayout_flags_at_70[
    (offsetof(struct SphereClsnLayout, flags) == 0x70) ? 1 : -1];
typedef char SphereClsnLayout_wallHeight_at_ec[
    (offsetof(struct SphereClsnLayout, wallHeight) == 0xec) ? 1 : -1];
typedef char SphereClsnLayout_bestFloorNormal_at_fc[
    (offsetof(struct SphereClsnLayout, bestFloorNormal) == 0xfc) ? 1 : -1];
typedef char SphereClsnLayout_maxNormalY_at_108[
    (offsetof(struct SphereClsnLayout, maxNormalY) == 0x108) ? 1 : -1];

extern "C" {
s32  func_020396dc(dBgW_Kc *self, KCL_Tri *prism);
s32  func_02039794(s32 normalY);
s32  func_020397dc(s32 x);
s32  func_02037e58(const void *info);
void func_02037fd4(void *clsnResult, s16 tri, const void *info);
void func_020379f4(void *sph, s16 tri, const void *info);
void func_020379c0(void *sph, s16 tri, const void *info);
void func_0203798c(void *sph, s16 tri, const void *info);
void func_0203794c(void *sph, const Vector3 *normal);
void func_02037a6c(void *sph, s32 x1, s32 y1, s32 z1, s32 x2, s32 y2, s32 z2);
s32  DotVec3(const Vector3 *a, const Vector3 *b);
s32  _ZN4cstd4fdivEii(s32 a, s32 b);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(const void *self, Vector3 *out);
s32  _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(void *self, const void *info,
                                                    const void *bg, s32 isWall);

/* THE GLOBAL SurfaceInfo the ROM reuses for every prism -- data_020a0cec,
   constructed once by __sinit_02074fe4 (_ZN5dBgPcC1Ev) with _ZN5dBgPcD1Ev
   registered as its atexit destructor. The port has no such sinit and no other
   consumer, so it lives here, as five words carrying _ZN5dBgPcC1Ev's exact
   seed: c[0] = 0xfc0, c[1] = 0xff, c[2..4] = 0. GetSurfaceInfo rewrites all
   five before every read, so the seed only ever describes the state before the
   first prism of the first frame. */
DSSTATE_BEGIN
u32 data_020a0cec[5] = { 0xfc0u, 0xffu, 0u, 0u, 0u };
DSSTATE_END

/* Stage telemetry, read by the probes -- the g_walk_dbg convention from the
   line walk next door, cheap enough to keep permanently.
   [0] calls [1] leaves [2] prisms [3] contacts [4] face [5] edge [6] corner
   [7] floor [8] wall [9] ceiling [10] slope gate [11] corner solve
   [12] pass-through [13] last mask [14] last triIdx [15] last kind */
int g_sphere_dbg[16] = {0};
}

/* the 64-bit-product idiom the ROM uses everywhere:
   smull a,b,x,y ; lsr a,#0xa ; orr a,b,lsl #22 */
#define MUL10(a, b)  ((s32)(((s64)(a) * (s32)(b)) >> 10))
/* DotVec3's INNER step: a per-component rounded multiply, which is not DotVec3
   itself (that rounds the sum once). 0x01ffce58 and its five clones. */
#define FMUL(a, b)   ((s32)((((s64)(a) * (s32)(b)) + 0x800) >> 12))

/* dot basis * 0x1000 (a depth*normal product) -> world Fix12i.
   ROM: ((s64)depth*n >> 14) truncated to s32, then >>2.  The truncation point
   is kept where the ROM has it. */
static s32 push_world(s64 p, s32 f2w) { return (s32)((p * f2w) >> 32) >> 2; }
/* file units << 20 (the wall cull's edge step) -> world Fix12i.  ROM: >>14. */
static s32 step_world(s64 p, s32 f2w) { return (s32)((p * f2w) >> 32); }

/* The DS square-root unit in 64-bit mode, driven through the port's own MMIO
   window (port/ntr/io.cpp implements SQRTCNT / SQRT_PARAM / SQRT_RESULT and
   runs the unit on the parameter write). This is the same hardware the ROM
   inlines at 0x01ffca30 and at each of the three edge slope gates. The ROM's
   IME save/zero/restore around the parameter write is interrupt masking with
   no host meaning and is dropped; the parameter goes down as ONE 64-bit store
   so the proxy sees the whole operand, which is what the hostgen'd cstd::sqrt
   does too. Every input here is provably non-negative -- see the guard above
   each call. */
static s32 hw_sqrt64(u64 v)
{
    NTR_MMIO(u16, 0x040002b0) = 1;              /* SQRTCNT, 64-bit mode */
    NTR_MMIO(u64, 0x040002b8) = v;              /* SQRT_PARAM, one store */
    while (((u16)NTR_MMIO(u16, 0x040002b0)) & 0x8000) { }
    return (s32)(u32)NTR_MMIO(u32, 0x040002b4); /* SQRT_RESULT */
}

/* The collider's "up" axis, dBgW_Kc+0x28..0x30, read as the ROM reads it.
   dBgW_Kc::SetFile seeds (0, 0x1000, 0); MovingMeshCollider::SetFile and
   Transform overwrite it with the collider's own transformed axis, which is
   what makes the slope gate mean anything on a rotated platform. Nothing
   aliases these three words any more -- see BASIS CONVENTION. */
static void collider_up(const dBgW_Kc *self, Vector3 *out)
{
    out->x = self->unk_28;
    out->y = self->unk_2c;
    out->z = self->unk_30;
}

/* min/max accumulate, 0x01ffd07c and its five clones */
static void accum(s32 v, s32 *lo, s32 *hi)
{
    if (v > *hi) *hi = v;
    else if (v < *lo) *lo = v;
}

/* CORNER solve, 0x01ffc63c / 0x01ffc750 / 0x01ffc89c. The three ROM copies are
   instruction-for-instruction identical apart from which (normal, distance)
   pair plays P and Q, so they fold into one helper. Closest approach to the
   prism edge where planes P and Q meet:
       denom = cos^2 - 1              (0x400 basis: cos*cos>>10 - 0x400)
       a     = (cos*dQ - dP) / denom
       b     = dQ - a*cos
       V     = a*P + b*Q              (dot basis)
   Returns 0 when the near-parallel guard fires, which REJECTS the prism. */
static int corner_offset(const s16 *P, s32 dP, const s16 *Q, s32 dQ, s32 cosPQ,
                         s32 *vx, s32 *vy, s32 *vz)
{
    s32 denom, a, b;
    denom = MUL10(cosPQ, cosPQ) - 0x400;
    if (func_020397dc(denom)) return 0;
    a = _ZN4cstd4fdivEii(MUL10(cosPQ, dQ) - dP, denom) >> 2;
    b = dQ - MUL10(a, cosPQ);
    *vx = MUL10(a, P[0]) + MUL10(b, Q[0]);
    *vy = MUL10(a, P[1]) + MUL10(b, Q[1]);
    *vz = MUL10(a, P[2]) + MUL10(b, Q[2]);
    return 1;
}

/* EDGE gate, 0x01ffc1ec / 0x01ffc35c / 0x01ffc4cc -- three identical ROM copies
   folded the same way. Returns 1 to accept the contact. */
static int edge_gate(dBgW_Kc *self, SphereClsnLayout *sph, s32 d,
                     s32 faceDot, s32 kind, const Vector3 *normal)
{
    if (sph->flags & 2) {                                   /* 0x01ffc310 */
        if (kind == 1) return !(d > faceDot);
        return !(d > (faceDot >> self->unk_48));
    }
    if (kind == 1) {                                        /* wall */
        if (func_02037e58(data_020a0cec) == 1)
            return !(d > (faceDot >> self->unk_48));
        if (self->unk_4d != 0)
            return !(d > (faceDot >> self->unk_48));
        return !(d > faceDot);
    }
    if (d <= (faceDot >> self->unk_48)) return 1;
    /* SLOPE GATE, 0x01ffc258 */
    if (kind != 0) return 0;
    if (sph->flags & 0x20) return 0;
    {
        Vector3 up;
        s32 dd = d >> 4;
        s32 ff = faceDot >> 4;
        s32 h, upDot;
        ++g_sphere_dbg[10];
        collider_up(self, &up);
        upDot = DotVec3(normal, &up);       /* the ROM's arg order, 0x01ffc274 */
        h = hw_sqrt64((u64)((s64)dd * dd + (s64)ff * ff));
        if (func_020397dc(h)) return 0;
        return !(upDot > _ZN4cstd4fdivEii(ff, h));
    }
}

/* ---------------------------------------------------------------------- */

s32 dBgW_Kc::DetectClsn(dBgCh_SphCrr &sphere)
{
    SphereClsnLayout *sph = (SphereClsnLayout *)&sphere;
    KCL_File *file = this->kclFile;
    const Vector3 *origin;

    /* the ROM's own conversion, spelled as the multiply the arithmetic below
       is written around -- see BASIS CONVENTION */
    const s32 w2f = 0x40;           /* world -> file: the ROM's >>6 */
    const s32 f2w = 0x40000;        /* file  -> world: the ROM's <<6 */

    s32 cx, cy, cz;                 /* centre, file units */
    s32 pad;                        /* box pad, file units */
    s32 loX, hiX, loY, hiY, loZ, hiZ;
    s32 R;                          /* radius in the dot basis */
    s64 radiusSq;
    u32 x, y, z;
    s32 stepX = 0, stepY = 0, stepZ = 0;
    s32 rowExt[3];
    u16 *rowLeaf[3];
    u16 *prevLeaf[3];
    s32 minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
    s32 mask = 0;                   /* [sp+0x40] -- the return value */
    s32 anyFloor = 0;               /* [sp+0x44] */

    ++g_sphere_dbg[0];
    if (!file) return 0;
    origin = &file->origin;

    /* === SETUP, 0x01ffb840..0x01ffb9b8 === */
    cx = (s32)(((s64)sph->center.x * w2f) >> 12);
    cy = (s32)(((s64)sph->center.y * w2f) >> 12);
    cz = (s32)(((s64)sph->center.z * w2f) >> 12);
    R  = (s32)(((s64)sph->radius   * w2f) >> 2);    /* file units * 0x400 */
    pad = (R >> 10) + 0x40;

    loX = ((cx - origin->x) - pad) >> 6;  if (loX < 0) loX = 0;
    hiX = ((cx - origin->x) + pad) >> 6;
    if (hiX > (s32)~file->xMask) hiX = (s32)~file->xMask;
    if (loX >= hiX) return 0;

    loY = ((cy - origin->y) - pad) >> 6;  if (loY < 0) loY = 0;
    hiY = ((cy - origin->y) + pad) >> 6;
    if (hiY > (s32)~file->yMask) hiY = (s32)~file->yMask;
    if (loY >= hiY) return 0;

    loZ = ((cz - origin->z) - pad) >> 6;  if (loZ < 0) loZ = 0;
    hiZ = ((cz - origin->z) + pad) >> 6;
    if (hiZ > (s32)~file->zMask) hiZ = (s32)~file->zMask;
    if (loZ >= hiZ) return 0;

    radiusSq = (s64)R * R;                          /* 0x01ffba04 */

    rowLeaf[0] = rowLeaf[1] = rowLeaf[2] = 0;
    prevLeaf[0] = prevLeaf[1] = prevLeaf[2] = 0;

    /* === OCTREE DESCENT, 0x01ffba54..0x01ffbb2c === */
    for (z = (u32)loZ; z <= (u32)hiZ; z += (u32)stepZ) {     /* 0x01ffba54 */
        stepZ = 1000000;
        for (y = (u32)loY; y <= (u32)hiY; y += (u32)stepY) { /* 0x01ffba64 */
            stepY = 1000000;
            rowExt[0] = rowExt[1] = rowExt[2] = 0;
            /* rowLeaf[] is deliberately NOT reset here: the ROM lets a sparse
               row carry the previous row's pointers forward into prevLeaf. */
            for (x = (u32)loX; x <= (u32)hiX; x += (u32)stepX) { /* 0x01ffba84 */
                u32 shift = file->coordShift;
                u32 *node = (u32 *)file->unk_0c;
                s32 v = (s32)node[(z >> shift) << file->zShift
                                | (y >> shift) << file->yShift
                                | (x >> shift)];
                u16 *leaf;
                s32 size, cellMask, ey, ez;

                while (v >= 0) {                            /* 0x01ffbac8 */
                    node = (u32 *)((u8 *)node + v);
                    shift--;
                    v = (s32)node[((z >> shift) & 1) << 2
                                | ((y >> shift) & 1) << 1
                                | ((x >> shift) & 1)];
                }
                leaf = (u16 *)((u8 *)node + (v & ~0x80000000));

                size = 1 << shift;                          /* 0x01ffbb18 */
                cellMask = size - 1;
                stepX = size - (s32)(x & (u32)cellMask);
                ez = size - (s32)(z & (u32)cellMask);
                ey = size - (s32)(y & (u32)cellMask);
                if (ez < stepZ) stepZ = ez;
                if (ey < stepY) stepY = ey;

                /* === LEAF SKIPS, 0x01ffbb6c..0x01ffbc18 ===
                   THREE remembered leaves, where the line walk keeps one. */
                if (leaf == prevLeaf[0] || leaf == prevLeaf[1]
                                        || leaf == prevLeaf[2]) continue;

                if (leaf[1] != 0) {                         /* 0x01ffbb8c */
                    if (ey > rowExt[2]) {
                        if (ey > rowExt[1]) {
                            if (ey > rowExt[0]) {           /* 0x01ffbbc0 */
                                rowExt[2] = rowExt[1]; rowExt[1] = rowExt[0];
                                rowExt[0] = ey;
                                rowLeaf[2] = rowLeaf[1]; rowLeaf[1] = rowLeaf[0];
                                rowLeaf[0] = leaf;
                            } else {                        /* 0x01ffbbf0 */
                                rowExt[2] = rowExt[1]; rowExt[1] = ey;
                                rowLeaf[2] = rowLeaf[1]; rowLeaf[1] = leaf;
                            }
                        } else {                            /* 0x01ffbc10 */
                            rowExt[2] = ey;
                            rowLeaf[2] = leaf;
                        }
                    }
                }

                ++g_sphere_dbg[1];
                while (*++leaf != 0) {                      /* 0x01ffbc1c */
                    KCL_Tri *prism = &file->tris[*leaf];
                    const s32 *v0 = (const s32 *)file->positions[prism->posIdx];
                    const s16 *e1 = (const s16 *)file->normals[prism->edgeNormal1Idx];
                    const s16 *e2 = (const s16 *)file->normals[prism->edgeNormal2Idx];
                    const s16 *e3 = (const s16 *)file->normals[prism->edgeNormal3Idx];
                    const s16 *fn = (const s16 *)file->normals[prism->normalIdx];
                    s32 relx, rely, relz;
                    s32 d1, d2, d3, faceDot, length;
                    s32 depth, kind, contactClass;
                    s32 triIdx;
                    Vector3 normal;
                    s64 sq64 = 0;
                    s32 vx = 0, vy = 0, vz = 0;
                    int isCorner = 0;

                    ++g_sphere_dbg[2];

                    /* === PER-PRISM REJECTS, 0x01ffbc30..0x01ffbd80 ===
                       R is the only lateral slack, and it replaces the line
                       walk's fixed +/-0x20000 entirely. */
                    relx = cx - v0[0];                      /* 0x01ffbc30 */
                    rely = cy - v0[1];
                    relz = cz - v0[2];

                    d1 = e1[0] * relx + e1[1] * rely + e1[2] * relz;
                    if (d1 >= R) continue;                  /* 0x01ffbcb4 */
                    d2 = e2[0] * relx + e2[1] * rely + e2[2] * relz;
                    if (d2 >= R) continue;
                    length = *(const s32 *)prism;           /* prism word 0 */
                    d3 = e3[0] * relx + e3[1] * rely + e3[2] * relz - length;
                    if (d3 >= R) continue;
                    faceDot = fn[0] * relx + fn[1] * rely + fn[2] * relz;
                    if (faceDot > R) continue;              /* strictly > */

                    /* === BACK-FACE / MAX-DEPTH GATE, 0x01ffbd84 ===
                       how far BEHIND the plane a contact may still sit. SetFile
                       seeds unk_34 = unk_35 = 0, so only the third arm runs in
                       practice; the first two are transcribed for fidelity and
                       are untested. */
                    if (this->unk_34 != 0) {
                        if (faceDot < -0x50000) continue;
                    } else if (this->unk_35 != 0) {
                        /* the Vector3 at this+0x38, SetFile-seeded (0x1000,0,0).
                           Its X is the port's world->file scale, so the seed
                           stands in -- the collider_up situation again. */
                        Vector3 fnf, axis;
                        fnf.x = fn[0] << 2;                 /* 0x400 -> Fix12i */
                        fnf.y = fn[1] << 2;
                        fnf.z = fn[2] << 2;
                        axis.x = 0x1000;
                        axis.y = this->unk_3c;
                        axis.z = this->unk_40;
                        if (DotVec3(&fnf, &axis) > this->unk_44) {
                            if (faceDot < -file->unk_10) continue;
                        } else {
                            if (faceDot < -0xa0000) continue;
                        }
                    } else {
                        if (faceDot < -file->unk_10) continue;
                    }

                    /* === SURFACE FETCH, 0x01ffbe30 === */
                    depth = R - faceDot;               /* FACE penetration */
                    triIdx = func_020396dc(this, prism);

                    /* The ROM dispatches slot 3 virtually (ldr r3,[r0];
                       ldr r3,[r3,#0xc]; blx r3). The port calls it direct --
                       same target; the precedent and the reasoning are in
                       MeshCollider_DetectClsn_RaycastLine.cpp. */
                    dBgW_Kc::GetSurfaceInfo(
                        (s16)triIdx, *(SurfaceInfo *)data_020a0cec);
                    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(data_020a0cec,
                                                              &normal);
                    kind = func_02039794(normal.y);  /* 0 floor 1 wall 2 ceil */
                    contactClass = 0;

                    if (_ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(
                            this, data_020a0cec, sph, kind == 1)) {
                        ++g_sphere_dbg[12];
                        continue;
                    }

                    /* === FEATURE CLASSIFICATION, 0x01ffbea8 ===
                       the largest of d1/d2/d3 wins, ties fall to d3. Then the
                       Voronoi split against the larger of the other two: the
                       centre is in the VERTEX region iff cos*dFeature <=
                       dPartner, and in the plain EDGE region otherwise. */
                    {
                        s32 feat, dFeat, c;
                        const s16 *nFeat;

                        if (d1 > d2) feat = (d1 > d3) ? 1 : 3;
                        else         feat = (d2 > d3) ? 2 : 3;

                        if (feat == 1)      { dFeat = d1; nFeat = e1; }
                        else if (feat == 2) { dFeat = d2; nFeat = e2; }
                        else                { dFeat = d3; nFeat = e3; }

                        if (dFeat > 0) {
                            const s16 *nPart;
                            s32 dPart;
                            const s16 *P, *Q;   /* corner_offset's two roles */
                            s32 dP, dQ;

                            if (this->unk_4c == 0) continue;

                            /* which partner, and which of the three ROM corner
                               blocks it lands in: CORNER_12 solves with
                               (P,Q) = (e1,e2), CORNER_23 with (e2,e3),
                               CORNER_31 with (e3,e1). */
                            if (feat == 1) {
                                if (d2 > d3) { nPart = e2; dPart = d2;
                                               P = e1; dP = d1; Q = e2; dQ = d2; }
                                else         { nPart = e3; dPart = d3;
                                               P = e3; dP = d3; Q = e1; dQ = d1; }
                            } else if (feat == 2) {
                                if (d3 > d1) { nPart = e3; dPart = d3;
                                               P = e2; dP = d2; Q = e3; dQ = d3; }
                                else         { nPart = e1; dPart = d1;
                                               P = e1; dP = d1; Q = e2; dQ = d2; }
                            } else {
                                if (d1 > d2) { nPart = e1; dPart = d1;
                                               P = e3; dP = d3; Q = e1; dQ = d1; }
                                else         { nPart = e2; dPart = d2;
                                               P = e2; dP = d2; Q = e3; dQ = d3; }
                            }

                            c = MUL10(nFeat[0], nPart[0]) + MUL10(nFeat[1], nPart[1])
                              + MUL10(nFeat[2], nPart[2]);

                            if (MUL10(c, dFeat) <= dPart) {     /* VERTEX */
                                ++g_sphere_dbg[11];
                                if (!corner_offset(P, dP, Q, dQ, c,
                                                   &vx, &vy, &vz)) continue;
                                isCorner = 1;
                            } else {                            /* EDGE */
                                if (!edge_gate(this, sph, dFeat, faceDot,
                                               kind, &normal)) continue;
                                sq64 = radiusSq - (s64)dFeat * dFeat;
                                goto do_sqrt;
                            }
                        }
                        /* dFeat <= 0: the centre is laterally inside the prism,
                           which is a FACE contact -- straight to the post-accept
                           filters with depth still R - faceDot. */
                    }

                    if (isCorner) {     /* CORNER ACCEPTANCE, 0x01ffc9cc */
                        s64 sqLen;
                        if (sph->flags & 0x40) continue;
                        sqLen = (s64)vy * vy + (s64)vx * vx + (s64)vz * vz;
                        if (faceDot < 0) continue;
                        if ((s64)faceDot * faceDot < sqLen) continue;
                        if (radiusSq - sqLen <= 0) continue;
                        contactClass = 3;
                        sq64 = radiusSq - sqLen;
                        goto do_sqrt;
                    }

                    goto post_accept;                       /* FACE contact */

do_sqrt:                                                    /* 0x01ffca30 */
                    depth = hw_sqrt64((u64)sq64) - faceDot;
                    if (depth < 0) continue;
                    if (contactClass == 0) contactClass = 2;   /* edge */

post_accept:                                                /* 0x01ffcaa4 */
                    if (sph->maxNormalY < normal.y) continue;

                    /* WALL-HEIGHT CULL -- drop walls too short to block.
                       Dormant in practice (dBgCh_SphCrr's ctor leaves wallHeight
                       0 and no matched caller raises it), and the only place
                       the ROM reconstructs the prism's other two vertices,
                       which is what pins `length` as the extent along edge
                       normal 3. */
                    if (sph->wallHeight > 0 && kind == 1
                            && (length & 0xf0000000) == 0) {
                        s16 dir[3];
                        s32 dot, t;
                        Vector3 A, B, C, up;
                        s32 hA, hB, hC, lo, hi;

                        A.x = (s32)(((s64)v0[0] * f2w) >> 12);   /* -> world */
                        A.y = (s32)(((s64)v0[1] * f2w) >> 12);
                        A.z = (s32)(((s64)v0[2] * f2w) >> 12);

                        /* B: A + (fn x e2) * length / ((fn x e2).e3). The cross
                           product is TRUNCATED TO s16 per component (the strh
                           at 0x01ffcb40), which the s16 dir[] preserves. */
                        dir[0] = (s16)(MUL10(fn[1], e2[2]) - MUL10(fn[2], e2[1]));
                        dir[1] = (s16)(MUL10(fn[2], e2[0]) - MUL10(fn[0], e2[2]));
                        dir[2] = (s16)(MUL10(fn[0], e2[1]) - MUL10(fn[1], e2[0]));
                        dot = MUL10(dir[0], e3[0]) + MUL10(dir[1], e3[1])
                            + MUL10(dir[2], e3[2]);
                        if (func_020397dc(dot)) continue;  /* rejects the prism */
                        t = _ZN4cstd4fdivEii(length, dot) >> 2;
                        B.x = A.x + step_world((s64)dir[0] * t, f2w);
                        B.y = A.y + step_world((s64)dir[1] * t, f2w);
                        B.z = A.z + step_world((s64)dir[2] * t, f2w);

                        /* C: the same with e1 */
                        dir[0] = (s16)(MUL10(fn[1], e1[2]) - MUL10(fn[2], e1[1]));
                        dir[1] = (s16)(MUL10(fn[2], e1[0]) - MUL10(fn[0], e1[2]));
                        dir[2] = (s16)(MUL10(fn[0], e1[1]) - MUL10(fn[1], e1[0]));
                        dot = MUL10(dir[0], e3[0]) + MUL10(dir[1], e3[1])
                            + MUL10(dir[2], e3[2]);
                        if (func_020397dc(dot)) continue;
                        t = _ZN4cstd4fdivEii(length, dot) >> 2;
                        C.x = A.x + step_world((s64)dir[0] * t, f2w);
                        C.y = A.y + step_world((s64)dir[1] * t, f2w);
                        C.z = A.z + step_world((s64)dir[2] * t, f2w);

                        A.x -= sph->center.x; A.y -= sph->center.y;
                        A.z -= sph->center.z;
                        B.x -= sph->center.x; B.y -= sph->center.y;
                        B.z -= sph->center.z;
                        C.x -= sph->center.x; C.y -= sph->center.y;
                        C.z -= sph->center.z;

                        collider_up(this, &up);
                        hA = FMUL(up.x, A.x) + FMUL(up.y, A.y) + FMUL(up.z, A.z);
                        hB = FMUL(up.x, B.x) + FMUL(up.y, B.y) + FMUL(up.z, B.z);
                        hC = FMUL(up.x, C.x) + FMUL(up.y, C.y) + FMUL(up.z, C.z);

                        hi = sph->wallHeight - sph->radius;
                        lo = -(sph->wallHeight + sph->radius);
                        if (hA >= lo && hA <= hi && hB >= lo && hB <= hi
                                     && hC >= lo && hC <= hi) continue;
                    }

                    /* === RECORD, 0x01ffcfe4 === */
                    if (contactClass == 0) contactClass = 1;    /* face */
                    func_02037fd4((u8 *)sph + 0x10, (s16)triIdx, data_020a0cec);
                    sph->flags |= 1;

                    ++g_sphere_dbg[3];
                    ++g_sphere_dbg[3 + contactClass];  /* 4 face 5 edge 6 corner */
                    g_sphere_dbg[14] = triIdx;
                    g_sphere_dbg[15] = kind;

                    if (kind == 0) {                            /* FLOOR */
                        ++g_sphere_dbg[7];
                        if (!(sph->flags & 4)) {
                            func_020379f4(sph, (s16)triIdx, data_020a0cec);
                            mask |= 1;
                        }
                        /* anyFloor forces the push-out even when the floor slot
                           was already taken, and the mask bit stays CLEAR in
                           that case -- the same `r &= ~1` dance
                           MovingMeshCollider::DetectClsn does. */
                        anyFloor = 1;
                        sph->flags |= 4;
                        accum(push_world((s64)depth * normal.y, f2w),
                              &minY, &maxY);          /* Y ONLY */
                        if (normal.y > sph->bestFloorNormal.y)
                            func_0203794c(sph, &normal);
                    } else if (kind == 1) {                     /* WALL */
                        ++g_sphere_dbg[8];
                        sph->flags |= 8;
                        func_020379c0(sph, (s16)triIdx, data_020a0cec);
                        mask |= 2;
                        accum(push_world((s64)depth * normal.x, f2w),
                              &minX, &maxX);
                        /* face contacts push along XZ; edge and corner contacts
                           push along all three (0x01ffd0f0 vs 0x01ffd170) */
                        if (contactClass != 1)
                            accum(push_world((s64)depth * normal.y, f2w),
                                  &minY, &maxY);
                        accum(push_world((s64)depth * normal.z, f2w),
                              &minZ, &maxZ);
                    } else {                                    /* CEILING */
                        ++g_sphere_dbg[9];
                        sph->flags |= 0x10;
                        func_0203798c(sph, (s16)triIdx, data_020a0cec);
                        mask |= 4;
                        accum(push_world((s64)depth * normal.x, f2w),
                              &minX, &maxX);
                        accum(push_world((s64)depth * normal.y, f2w),
                              &minY, &maxY);
                        accum(push_world((s64)depth * normal.z, f2w),
                              &minZ, &maxZ);
                    }
                }
            }
            prevLeaf[0] = rowLeaf[0];                       /* 0x01ffd35c */
            prevLeaf[1] = rowLeaf[1];
            prevLeaf[2] = rowLeaf[2];
        }
    }

    g_sphere_dbg[13] = mask;
    if (mask == 0 && anyFloor == 0) return 0;               /* 0x01ffd394 */
    func_02037a6c(sph, minX, minY, minZ, maxX, maxY, maxZ);
    return mask;
}
