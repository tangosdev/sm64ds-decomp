//cpp
// @symbol _ZN7dBgW_Kc10DetectClsnER9dBgCh_Gnd
/* dBgW_Kc::DetectClsn(dBgCh_Gnd &) at 0x01ffd3f8 (ITCM)
 *
 * vtable slot 6. The downward ground probe: given a world position, find the
 * highest upward-facing KCL triangle that is below it and above the incoming
 * `clsnY`, and record it. Same octree walk as DetectClsn(dBgCh_Lin &) next
 * door, but a ground query is a vertical probe through a single (x, z) column,
 * so only Y marches -- which is why the frame is 0x4c and not 0xfc.
 *
 * SCALES. Positions and the octree origin live at 1/64 of a Fix12i unit, so
 * every Fix12i coming in is `>> 6` and the answer is `<< 6` on the way out
 * (same conversion as GetTriangleOrigin / GetOctreeOriginY). One octree cell is
 * a further `>> 6`, i.e. 0x40 raw units = one whole world unit.
 *
 * X AND Z EARLY-OUT, Y CLAMPS. A column outside the octree in X or Z can never
 * hit anything, so those return 0. A column that starts *above* the octree
 * still can, so Y is clamped down to the top cell (~yMask) and the march
 * proceeds. Y below the octree is still an early-out.
 *
 * THE MARCH steps by whole leaf nodes: after each cell is tested, Y is snapped
 * to the bottom of the node it landed in and stepped one below
 * (`y = (y & ~((1 << shift) - 1)) - 1`), so a tall empty node costs one
 * iteration. `shift` is left holding the leaf's level by the descent loop.
 *
 * PER PRISM. Reject downward faces (`normal[1] <= 0`) and faces too close to
 * vertical to divide by (func_020397dc guards the denominator). Solve the plane
 * for the height under the probe --
 * `dy = -fdiv((dx*nx >> 10) + (dz*nz >> 10), ny) >> 2` -- where the `>> 10`
 * keeps the numerator in 32 bits and the `>> 2` undoes fdiv's 12-bit gain.
 * Then the three edge normals: the first two must dot to <= 0x20000 and the
 * third must land in [-0x20000, tri->length + 0x20000]. `length` is the prism's
 * extent along that third edge normal and this is the only place it is read.
 * The 64-bit dot that follows is the "probe is above the plane" test, using the
 * probe's own Y rather than the solved one.
 *
 * GetSurfaceInfo IS A REAL VIRTUAL CALL -- see the note on
 * dBgW_Kc::GetSurfaceInfo: mwccarm's own dispatch reads `this` from r0,
 * a hand-rolled `(*(fn**)this)[3](...)` reads it from the callee-saved copy,
 * and that one word is not otherwise reachable.
 *
 * MATCHING NOTES, all four load-bearing and all found by bisection:
 *   - Declaration order IS the stack layout. The ROM's frame is
 *     x, z, y, found, bestY, leaf, normal, rawX, rawZ, rawY at sp+4..sp+0x28;
 *     mwccarm hands out spill slots in declaration order, so the C89 block
 *     below has to be in exactly that order, including `leaf` and `normal`
 *     sitting between `bestY` and `rawX`.
 *   - The root octree index is built in two statements. Folding
 *     `| ((u32)x >> shift)` into the same expression as the z and y terms makes
 *     mwccarm hoist the `file->unk_0c` load and burn r4 on it; splitting the x
 *     term into its own `|=` keeps the load late and the block inside r0-r3.
 *   - Both index expressions run z, y, x. The child index has to be
 *     `zbit << 2 | ybit << 1 | xbit` -- writing it x-first folds both shifts
 *     into two ORRs, where the ROM materialises `zbit << 2` on its own.
 *   - `rawY - vtx[1]` is a NAMED LOCAL. Written inline inside the 64-bit dot it
 *     compiles to the same nine instructions but swaps two temp slots
 *     (sp+0x38 / sp+0x3c).
 */
#include "dBgW_Kc.h"
#include "dBgCh_Gnd.h"

/* extern "C" because these exist unmangled in config/arm9/symbols.txt.
   ShouldPassThroughImpl is a real mangled C++ static, spelled out rather than
   declared in dBgCh.h so this file changes no shared header it does not have to. */
extern "C" {

/* cstd::fdiv(int, int) at 0x02053258 -- (a << 12) / b. */
Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

/* dBgCh::ShouldPassThroughImpl(void *, const CLPS &, const dBgCh &, bool) at
   0x02039488. Static: no `this`. The collider goes in as the void *, the
   SurfaceInfo as the CLPS, and the query object as the dBgCh. */
int _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(void *self, SurfaceInfo *info,
                                                   dBgCh_Gnd *ray, int flag);

/* 0x020397dc: true when the argument is too near zero to divide by. */
int func_020397dc(int denom);

/* 0x020396dc: (tri - file->tris), i.e. the triangle's index. */
s16 func_020396dc(dBgW_Kc *self, KCL_Tri *tri);

/* 0x02037fd4: record a hit -- triangle index and surface into the dBgPi. */
void func_02037fd4(void *res, s16 triID, SurfaceInfo *info);

}

/* The shared scratch SurfaceInfo at 0x020a0cec, filled by GetSurfaceInfo and
   read straight back by ShouldPassThroughImpl and the hit record. */
extern SurfaceInfo data_020a0cec;

int dBgW_Kc::DetectClsn(dBgCh_Gnd &ray)
{
    KCL_File *file = kclFile;
    Vector3 *pos = &ray.pos;
    /* Declaration order below IS the ROM's stack frame -- do not reorder. */
    s32 x, z, y;                /* octree cell coordinates */
    KCL_Tri *found;
    s32 bestY;
    u16 *leaf;
    s16 *normal;
    s32 rawX, rawZ, rawY;       /* the probe in the file's 1/64 units */
    u32 shift;
    u32 *node;
    s32 word;
    u32 idx;
    KCL_Tri *tri;
    s32 *vtx;
    s16 *en;
    s32 dy, dx, dz, dot, dyv;

    rawX = pos->x >> 6;
    x = (rawX - file->origin.x) >> 6;
    if (x < 0) return 0;
    if (x > (s32)~file->xMask) return 0;

    rawZ = pos->z >> 6;
    z = (rawZ - file->origin.z) >> 6;
    if (z < 0) return 0;
    if (z > (s32)~file->zMask) return 0;

    rawY = pos->y >> 6;
    y = (rawY - file->origin.y) >> 6;
    if (y < 0) return 0;
    /* Above the octree is not a miss: start at its top cell and fall. */
    if (y > (s32)~file->yMask) y = ~file->yMask;

    found = 0;
    bestY = ray.clsnY >> 6;

    do {
        shift = file->coordShift;
        idx = ((u32)z >> shift) << file->zShift
            | ((u32)y >> shift) << file->yShift;
        idx |= (u32)x >> shift;
        node = (u32 *)file->unk_0c;
        word = node[idx];

        /* Non-negative: a byte offset to the child block. One bit per axis. */
        while (word >= 0) {
            node = (u32 *)((char *)node + word);
            shift--;
            word = node[((((u32)z >> shift) & 1) << 2)
                      | ((((u32)y >> shift) & 1) << 1)
                      | (((u32)x >> shift) & 1)];
        }

        /* Negative: bit 31 clear gives the byte offset of a zero-terminated
           u16 triangle list, biased by -2. */
        leaf = (u16 *)((char *)node + (word & 0x7fffffff));
        while (*++leaf) {
            tri = &file->tris[*leaf];
            normal = file->normals[tri->normalIdx];
            if (normal[1] <= 0) continue;       /* not a floor */

            vtx = file->positions[tri->posIdx];
            dx = rawX - vtx[0];
            dz = rawZ - vtx[2];
            if (func_020397dc(normal[1])) continue;   /* too steep to solve */

            /* Height of the plane under the probe, relative to vertex 0. */
            dy = -(_ZN4cstd4fdivEii((s32)(((s64)dx * normal[0]) >> 10)
                                  + (s32)(((s64)dz * normal[2]) >> 10),
                                    normal[1]) >> 2);

            en = file->normals[tri->edgeNormal1Idx];
            if (dx * en[0] + dy * en[1] + dz * en[2] > 0x20000) continue;
            en = file->normals[tri->edgeNormal2Idx];
            if (dx * en[0] + dy * en[1] + dz * en[2] > 0x20000) continue;
            en = file->normals[tri->edgeNormal3Idx];
            dot = dx * en[0] + dy * en[1] + dz * en[2];
            if (dot < -0x20000) continue;
            if (dot > tri->length + 0x20000) continue;

            /* The probe itself has to be on the front side of the plane. */
            dyv = rawY - vtx[1];
            if ((s64)dx * normal[0] + (s64)dyv * normal[1]
              + (s64)dz * normal[2] < 0) continue;

            GetSurfaceInfo(func_020396dc(this, tri), data_020a0cec);
            if (_ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(this, &data_020a0cec, &ray, 0))
                continue;

            if (dy + vtx[1] < file->origin.y) continue;   /* below the octree */
            if (bestY >= dy + vtx[1]) continue;           /* not an improvement */
            if (rawY <= dy + vtx[1]) continue;            /* not below the probe */

            bestY = dy + vtx[1];
            found = tri;
            /* through the REFERENCE: a pointer-level upcast makes mwcc emit
               the null-checked MI adjustment (movs/addne), the ROM's is
               unconditional */
            func_02037fd4(&(dBgPi &)ray, func_020396dc(this, tri), &data_020a0cec);
        }

        /* Snap to the bottom of the node just tested and drop one cell. */
        y = (y & ~((1 << shift) - 1)) - 1;
    } while (y >= 0);

    if (!found) return 0;

    ray.clsnY = bestY << 6;
    ray.hasClsn = 1;
    return 1;
}
