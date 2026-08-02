// HOST ADOPTION of the banked near-miss draft -- not byte-verified.
//
// Source: nearmiss/db.jsonl, _ZN12MeshCollider10DetectClsnER11RaycastLine,
// banked at div=476. Per notes/itcm.md the residual is REGISTER ALLOCATION
// (the ROM's 0xfc frame vs our 0xc4 -- fourteen spilled scalars), not
// logic: every step of the algorithm is confirmed against the disassembly
// in that note (setup/march/node-lookup/leaf-skips/prism-math/acceptance).
// When the byte match lands in src/, this file retires per the port rule.
//
//cpp
#include "MeshCollider.h"

struct ClsnResult { u8 raw[0x1c]; };
struct SurfaceInfo { u8 clps[8]; Vector3 normal; };

struct RaycastLine {
    u8      head[0x10];
    ClsnResult result;      /* 0x10 */
    u8      pad_02c[0xc];
    Vector3 lineStart;      /* 0x38 */
    u8      pad_044[0xc];
    u8      hasClsn;        /* 0x50 */
    u8      pad_051[0x3];
    Vector3 lineEnd;        /* 0x54 */
    Fix12i  clsnDist;       /* 0x60 */
};

extern "C" {
int  func_020397dc(s32 x);
int  func_020397b8(s32 x);
void func_02037eec(SurfaceInfo *info);
void func_02037ee8(SurfaceInfo *info);
void func_02037fd4(ClsnResult *res, s16 triIdx, SurfaceInfo *info);
void func_020375ec(RaycastLine *ray, Vector3 *pos);
u32  func_020396dc(MeshCollider *self, KCL_Tri *prism);
int  _ZN4BgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(void *self, SurfaceInfo *surf,
                                                    RaycastLine *ray, int isSteep);
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
s32  _ZN4cstd4fdivEii(s32 a, s32 b);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo *self, Vector3 *out);
}

extern "C" int g_walk_dbg[16];
/* Stage telemetry, read by the smoke: [1] leaves [2] prisms [3] planeS
   [4] planeE [5..7] edge rejections [8] e3-high [9] denom [10] parallel
   [11] dist [12] pass-through. Cheap enough to keep permanently. */

s32 MeshCollider::DetectClsn(RaycastLine &ray)
{
    s32 loX, hiX;
    s32 loY, hiY;
    s32 loZ, hiZ;
    s32 stepX, stepY, stepZ;
    s32 rowStep;
    s32 found;
    u16 *rowLeaf;
    u16 *prevLeaf;
    s32 bestDist;
    u32 x, y, z;
    Vector3 s, e, min, max, best;
    Vector3 d0, d1, delta, scaled, rel, hit;
    SurfaceInfo info;
    Vector3 normal;
    Vector3 pos;

    const Vector3 *lineStart = &ray.lineStart;
    const Vector3 *lineEnd = &ray.lineEnd;
    KCL_File *f;
    const Vector3 *origin;

    /* Everything downstream -- origin, vertices, the plane math and its
       0x20000 thresholds -- lives in Fix12i (the castle KCL proves it: its
       floor vertices only sit inside the octree box at fx scale). The
       endpoints therefore stay fx; cells are still (fx - origin) >> 6. */
    s.x = lineStart->x;
    s.y = lineStart->y;
    s.z = lineStart->z;
    e.x = lineEnd->x;
    e.y = lineEnd->y;
    e.z = lineEnd->z;

    min.x = s.x; max.x = s.x;
    min.y = s.y; max.y = s.y;
    min.z = s.z; max.z = s.z;
    if (s.x > e.x) min.x = e.x; else max.x = e.x;
    if (min.y > e.y) min.y = e.y; else max.y = e.y;
    if (min.z > e.z) min.z = e.z; else max.z = e.z;

    f = 0;
    origin = &this->kclFile->origin;

    min.x -= 0x40;
    loX = (min.x - origin->x) >> 6;
    if (loX < 0) loX = 0;
    max.x += 0x40;
    hiX = (max.x - origin->x) >> 6;
    if (hiX > (s32)~this->kclFile->xMask) hiX = ~this->kclFile->xMask;
    if (loX >= hiX) return 0;

    min.y -= 0x40;
    loY = (min.y - origin->y) >> 6;
    if (loY < 0) loY = 0;
    max.y += 0x40;
    hiY = (max.y - origin->y) >> 6;
    if (hiY > (s32)~this->kclFile->yMask) hiY = ~this->kclFile->yMask;
    if (loY >= hiY) return 0;

    min.z -= 0x40;
    loZ = (min.z - origin->z) >> 6;
    if (loZ < 0) loZ = 0;
    max.z += 0x40;
    hiZ = (max.z - origin->z) >> 6;
    if (hiZ > (s32)~this->kclFile->zMask) hiZ = ~this->kclFile->zMask;
    if (loZ >= hiZ) return 0;

    found = 0;
    rowLeaf = 0;
    prevLeaf = 0;
    bestDist = ray.clsnDist >> 6;

    for (z = loZ; z <= hiZ; z += stepZ) {
        stepZ = 1000000;
        for (y = loY; y <= hiY; y += stepY) {
            stepY = 1000000;
            rowStep = 0;
            for (x = loX; x <= hiX; x += stepX) {
                u32 shift = this->kclFile->coordShift;
                u32 *node = (u32 *)this->kclFile->unk_0c;
                s32 v = node[(z >> shift) << this->kclFile->zShift
                           | (y >> shift) << this->kclFile->yShift
                           | (x >> shift)];
                u16 *leaf;
                s32 size, mask, cy, cz;

                while (v >= 0) {
                    node = (u32 *)((u8 *)node + v);
                    shift--;
                    v = node[((z >> shift) & 1) << 2
                           | ((y >> shift) & 1) << 1
                           | ((x >> shift) & 1)];
                }
                leaf = (u16 *)((u8 *)node + (v & ~0x80000000));

                size = 1 << shift;
                mask = size - 1;
                stepX = size - (x & mask);
                cy = size - (y & mask);
                cz = size - (z & mask);
                if (cz < stepZ) stepZ = cz;
                if (cy < stepY) stepY = cy;

                if (leaf == prevLeaf) continue;
                if (cy > rowStep && leaf[1] != 0) {
                    rowStep = cy;
                    rowLeaf = leaf;
                }

                ++g_walk_dbg[1];
                while (*++leaf != 0) {
                    ++g_walk_dbg[2];
                    KCL_Tri *prism = &this->kclFile->tris[*leaf];
                    Vector3_16 *fnrm = (Vector3_16 *)this->kclFile->normals[prism->normalIdx];
                    Vector3 *v0 = (Vector3 *)this->kclFile->positions[prism->posIdx];
                    Vector3_16 *enrm;
                    s32 dotS, dotE, denom, t, dot, dist;
                    u32 triIdx;

                    d0.x = s.x - v0->x;
                    d0.y = s.y - v0->y;
                    d0.z = s.z - v0->z;
                    dotS = fnrm->x * d0.x + fnrm->y * d0.y + fnrm->z * d0.z;
                    if (dotS <= 0) { ++g_walk_dbg[3]; continue; }

                    d1.x = e.x - v0->x;
                    d1.y = e.y - v0->y;
                    d1.z = e.z - v0->z;
                    dotE = fnrm->x * d1.x + fnrm->y * d1.y + fnrm->z * d1.z;
                    if (dotE >= 0) { ++g_walk_dbg[4]; continue; }

                    denom = (dotS - dotE) >> 4;
                    if (denom <= 0) { ++g_walk_dbg[9]; continue; }
                    if (func_020397dc(denom)) { ++g_walk_dbg[10]; continue; }
                    t = _ZN4cstd4fdivEii(dotS >> 4, denom) << 4;

                    delta.x = d1.x - d0.x;
                    scaled.x = (s32)(((s64)delta.x * t) >> 16);
                    rel.x = d0.x + scaled.x;
                    delta.y = d1.y - d0.y;
                    scaled.y = (s32)(((s64)delta.y * t) >> 16);
                    rel.y = d0.y + scaled.y;
                    delta.z = d1.z - d0.z;
                    scaled.z = (s32)(((s64)delta.z * t) >> 16);
                    rel.z = d0.z + scaled.z;

                    enrm = (Vector3_16 *)this->kclFile->normals[prism->edgeNormal1Idx];
                    if (enrm->x * rel.x + enrm->y * rel.y + enrm->z * rel.z > 0x20000)
                        { ++g_walk_dbg[5]; continue; }
                    enrm = (Vector3_16 *)this->kclFile->normals[prism->edgeNormal2Idx];
                    if (enrm->x * rel.x + enrm->y * rel.y + enrm->z * rel.z > 0x20000)
                        { ++g_walk_dbg[6]; continue; }
                    enrm = (Vector3_16 *)this->kclFile->normals[prism->edgeNormal3Idx];
                    dot = enrm->x * rel.x + enrm->y * rel.y + enrm->z * rel.z;
                    if (dot < -0x20000) { ++g_walk_dbg[7]; continue; }
                    if (dot > (*(s32 *)prism) + 0x20000) { ++g_walk_dbg[8]; continue; }

                    hit.x = rel.x + v0->x;
                    hit.y = rel.y + v0->y;
                    hit.z = rel.z + v0->z;
                    dist = Vec3_Dist(&hit, &s) >> 6;
                    if (bestDist <= dist) { ++g_walk_dbg[11]; continue; }

                    func_02037eec(&info);
                    triIdx = func_020396dc(this, prism);
                    /* the ROM dispatches this virtually (notes/itcm.md, the
                       one lever); the port calls it direct -- same target,
                       no dependence on the synthetic vtable being filled */
                    MeshCollider::GetSurfaceInfo(triIdx, info);
                    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(&info, &normal);
                    if (!_ZN4BgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(
                            this, &info, &ray, func_020397b8(normal.y))) {
                        best.x = hit.x;
                        best.y = hit.y;
                        best.z = hit.z;
                        bestDist = dist;
                        func_02037fd4(&ray.result, triIdx, &info);
                        found = 1;
                    } else ++g_walk_dbg[12];
                    func_02037ee8(&info);
                }
            }
            prevLeaf = rowLeaf;
        }
    }

    if (!found) return 0;

    ray.clsnDist = bestDist << 6;   /* undo the >>6 of the dist metric */
    /* best is already Fix12i under the fx-consistent scale (the raw-scale
       draft shifted here; that shift moved into the metric only) */
    pos.x = best.x;
    pos.y = best.y;
    pos.z = best.z;
    func_020375ec(&ray, &pos);
    ray.hasClsn = 1;
    return 1;
}
