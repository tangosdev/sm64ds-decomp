//cpp
// @symbol _ZN7dBgW_Kc10DetectClsnER9dBgCh_Lin
/* recovered: dBgW_Kc::DetectClsn(dBgCh_Lin&), the collision engine's line check (ITCM). */
// NONMATCHING: div 200 of 461 words. mwccarm 2004/b56, --module itcm,
// @ 0x01ffb0fc size 0x734. Residue class: callee-saved register budget (six long-lived values against five registers).
// Draft from nearmiss/db.jsonl (stored divergence 200), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
#include "dBgW_Kc.h"

struct dBgPi { u8 raw[0x1c]; };
struct SurfaceInfo { u8 clps[8]; Vector3 normal; };

struct dBgCh_Lin {
    u8      head[0x10];
    dBgPi result;      /* 0x10 */
    u8      pad_02c[0xc];
    Vector3 lineStart;      /* 0x38 */
    u8      pad_044[0xc];
    u8      hasClsn;        /* 0x50 */
    u8      pad_051[0x3];
    Vector3 lineEnd;        /* 0x54 */
    Fix12i  clsnDist;       /* 0x60 */
};

struct dBgPc { SurfaceInfo surface; dBgPc(); ~dBgPc(); };

extern "C" {
int  func_020397dc(s32 x);
int  func_020397b8(s32 x);
void _ZN5dBgPcC1Ev(dBgPc *info);
void _ZN5dBgPcD1Ev(dBgPc *info);
void func_02037fd4(int *res, short triIdx, int *info);
void func_020375ec(int *ray, int *pos);
short func_020396dc(int **self, unsigned int prism);
int  _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(void *self, SurfaceInfo *surf,
                                                    void *ray, int isSteep);
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
s32  _ZN4cstd4fdivEii(s32 a, s32 b);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo *self, Vector3 &out);
}

s32 dBgW_Kc::DetectClsn(dBgCh_Lin &ray)
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
    u32 y, x, z;
    Vector3 s;
    Vector3 e, min, max, best;
    Vector3 d0, d1, delta, scaled, rel;
    Vector3 hit;

    u32 lp;
    const Vector3 *lineStart = &ray.lineStart;
    const Vector3 *lineEnd = &ray.lineEnd;
    KCL_File *f;
    const Vector3 *origin;

    s.x = lineStart->x >> 6;
    s.y = lineStart->y >> 6;
    s.z = lineStart->z >> 6;
    e.x = lineEnd->x >> 6;
    e.y = lineEnd->y >> 6;
    e.z = lineEnd->z >> 6;

    min.x = s.x;
    min.y = s.y;
    min.z = s.z;
    max.x = s.x;
    max.y = s.y;
    max.z = s.z;
    if (s.x > e.x) min.x = e.x; else max.x = e.x;
    if (min.y > e.y) min.y = e.y; else max.y = e.y;
    if (min.z > e.z) min.z = e.z; else max.z = e.z;

    f = this->kclFile;
    origin = &f->origin;

    min.x -= 0x40;
    loX = (min.x - origin->x) >> 6;
    if (loX < 0) loX = 0;
    max.x += 0x40;
    hiX = (max.x - origin->x) >> 6;
    if (hiX > (s32)~f->xMask) hiX = ~f->xMask;
    if (loX >= hiX) return 0;

    min.y -= 0x40;
    loY = (min.y - origin->y) >> 6;
    if (loY < 0) loY = 0;
    max.y += 0x40;
    hiY = (max.y - origin->y) >> 6;
    if (hiY > (s32)~f->yMask) hiY = ~f->yMask;
    if (loY >= hiY) return 0;

    min.z -= 0x40;
    loZ = (min.z - origin->z) >> 6;
    if (loZ < 0) loZ = 0;
    max.z += 0x40;
    hiZ = (max.z - origin->z) >> 6;
    if (hiZ > (s32)~f->zMask) hiZ = ~f->zMask;
    if (loZ >= hiZ) return 0;

    found = 0;
    prevLeaf = 0;
    bestDist = ray.clsnDist >> 6;
    rowLeaf = 0;

    z = loZ;
    do {
        stepZ = 1000000;
        y = loY;
        do {
            stepY = 1000000;
            rowStep = 0;
            x = loX;
            do {
                u32 shift = f->coordShift;
                u32 *node;
                u32 idx;
                s32 v;
                idx = (z >> shift) << f->zShift
                    | (y >> shift) << f->yShift;
                idx |= x >> shift;
                node = (u32 *)f->unk_0c;
                v = node[idx];
                s32 size, mask, cy, cz;
                u32 lv;

                while (v >= 0) {
                    node = (u32 *)((u8 *)node + v);
                    shift--;
                    v = node[((z >> shift) & 1) << 2
                           | ((y >> shift) & 1) << 1
                           | ((x >> shift) & 1)];
                }
                lp = (u32)((u8 *)node + (v & ~0x80000000));

                size = 1 << shift;
                mask = size - 1;
                stepX = size - (x & mask);
                cy = size - (y & mask);
                cz = size - (z & mask);
                if (cz < stepZ) stepZ = cz;
                if (cy < stepY) stepY = cy;

                if ((u16 *)lp != prevLeaf) {
                if (cy > rowStep && *(u16 *)(lp + 2) != 0) {
                    rowStep = cy;
                    rowLeaf = (u16 *)lp;
                }

                while ((lv = *(u16 *)(lp += 2)) != 0) {
                    KCL_Tri *prism = &f->tris[lv];
                    lp = prism ? lp : lp;
                    s16 *fnrm = f->normals[prism->normalIdx];
                    s32 *v0 = f->positions[prism->posIdx];
                    s16 *enrm;
                    s32 dotS, dotE, denom, t, dot, dist;
                    s32 dx, dy, dz;
                    s16 triIdx;

                    dz = s.z - v0[2];
                    dy = s.y - v0[1];
                    dx = s.x - v0[0];
                    d0.x = dx;
                    d0.y = dy;
                    d0.z = dz;
                    dotS = fnrm[0] * dx + fnrm[1] * dy + fnrm[2] * dz;
                    if (dotS <= 0) continue;

                    dz = e.z - v0[2];
                    dy = e.y - v0[1];
                    dx = e.x - v0[0];
                    d1.x = dx;
                    d1.y = dy;
                    d1.z = dz;
                    dotE = fnrm[0] * dx + fnrm[1] * dy + fnrm[2] * dz;
                    if (dotE >= 0) continue;

                    denom = (dotS - dotE) >> 4;
                    if (denom <= 0) continue;
                    if (func_020397dc(denom)) continue;
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

                    enrm = f->normals[prism->edgeNormal1Idx];
                    if (enrm[0] * rel.x + enrm[1] * rel.y + enrm[2] * rel.z > 0x20000)
                        continue;
                    enrm = f->normals[prism->edgeNormal2Idx];
                    if (enrm[0] * rel.x + enrm[1] * rel.y + enrm[2] * rel.z > 0x20000)
                        continue;
                    enrm = f->normals[prism->edgeNormal3Idx];
                    dot = enrm[0] * rel.x + enrm[1] * rel.y + enrm[2] * rel.z;
                    if (dot < -0x20000) continue;
                    if (dot > *(s32 *)prism + 0x20000) continue;

                    hit.x = rel.x + v0[0];
                    hit.y = rel.y + v0[1];
                    hit.z = rel.z + v0[2];
                    dist = Vec3_Dist(&hit, &s) >> 6;
                    if (bestDist <= dist) continue;

                    {
                    dBgPc pc;
                    Vector3 normal;
                    triIdx = func_020396dc((int **) this, (unsigned int) prism);
                    GetSurfaceInfo(triIdx, pc.surface);
                    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(&pc.surface, normal);
                    if (!_ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(
                            this, &pc.surface, &ray, func_020397b8(normal.y))) {
                        best.x = hit.x;
                        best.y = hit.y;
                        best.z = hit.z;
                        bestDist = dist;
                        func_02037fd4((int *) &ray.result, triIdx, (int *) &pc.surface);
                        found = 1;
                    }
                    }
                }
                }
                x += stepX;
            } while (x <= hiX);
            prevLeaf = rowLeaf;
            y += stepY;
        } while (y <= hiY);
        z += stepZ;
    } while (z <= hiZ);

    if (!found) return 0;

    ray.clsnDist = bestDist << 6;
    {
    Vector3 pos;
    pos.x = best.x << 6;
    pos.y = best.y << 6;
    pos.z = best.z << 6;
    func_020375ec((int *) &ray, (int *) &pos);
    }
    ray.hasClsn = 1;
    return 1;
}
