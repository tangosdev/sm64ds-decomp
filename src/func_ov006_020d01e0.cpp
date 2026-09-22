//cpp
/* ov006 0x020d01e0: the minigame's touch-drawn line becomes a strip of grid
 * points, and the routine then picks the object that line cuts closest.
 *
 * recovered: 2026-09-13 (crack wave 10, lane CRK2-I), from the near-miss draft
 * lanes DBIG and CRK-I left at a size mismatch. Two things this body needs are
 * reachable only in C++, which is why it is a .cpp.
 *
 *   THE FRAME. The ROM opens with `sub sp, sp, #0xd4`: fourteen scratch words
 *   of which thirteen are ever written, then thirteen twelve-byte objects of
 *   which twelve are ever written. sp+0x34 and sp+0xc8 are reserved and never
 *   touched. In C that frame is unreachable, because an unused local emits
 *   nothing and is given no home (six shapes were measured, among them a dead
 *   struct copy, a guarded dead store and an aliasing pointer). A type with a
 *   DECLARED destructor behaves differently: the frontend still reserves its
 *   stack slot even when the optimiser empties it. Vector3 declares one already,
 *   for the arrays the ROM destroys through __cxa_vec_cleanup. `spareVec` and
 *   `spare4` are those two reservations written down. Their original spelling is
 *   not recoverable from the bytes; an elided copy of a by-value return is the
 *   likely source.
 *
 *   THE SWAP TEST. Its two reads go through a per-site const cast (see
 *   notes/mwccarm-codegen.md 6bk). The ROM re-reads both halfwords from memory
 *   after writing them, and the cast is its own CSE class, so the stores above
 *   do not forward into it.
 *
 * `colWeight += 0x555` sits at the tail of the inner loop with the other two
 * induction steps. Placed between the two `cell->x` statements instead, which is
 * where the earlier drafts had it, the whole 0x340..0x3e4 window schedules one
 * slot early and 35 words differ. Nothing else moves that residue: 246 pragma
 * names at on and off, every declaration order and type name, the loop form, the
 * pointer form and the multiply operand order are all inert on it.
 */
#include "types.h"

extern "C" {
extern void Vec3_Add(Vector3* out, Vector3* a, Vector3* b);
extern void Vec3_MulScalar(Vector3* out, Vector3* in, int s);
extern void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
extern void func_0203ce80(Vector3* dst, Vector3* src);
extern void func_0203cf00(Vector3* out, Vector3* a, Vector3* b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
extern Fix12i DotVec3(const Vector3 *a, const Vector3 *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_ov006_020cf040(void *a, void *b, void *c);
extern void func_ov006_020cf124(void *a);
extern void func_ov006_020e6db4(int a0, int a1, int a2);
extern void func_ov006_020cfa28(char *p);
extern void func_ov006_020cfc58(char *p);

extern void *data_ov006_0214097c[];
extern s32 data_ov006_0213b2fc;
extern s32 data_ov006_0213b300;
extern s32 data_ov006_0213b2f8;
extern s32 data_ov006_0213b304;
}

struct Spare4 { s32 v; ~Spare4() {} };
typedef s32 (*IsActiveFn)(void *);
typedef Vector3 *(*GetVecFn)(void *);

extern "C" void func_ov006_020d01e0(char *c, short *p1, short *p2)
{
    Spare4 spare4;
    Vector3 va, vb;
    Vector3 bestRel, bestDir;
    Vector3 relPos, rawDir;
    Vector3 sumVec, scaledVec, diffVec, tmpVec, zAxis, crossVec;
    Vector3 spareVec;
    short swapX, swapZ;
    Fix12i dist, rowStep;
    s32 j;
    s32 flag;
    Vector3 *cell, *mirror;
    void *bestObj;
    void *obj;
    Vector3 *pos, *dir;
    s32 bestVal, bestD1;
    s32 k;
    s32 alongDot, crossDot, sideDot;
    s32 halfLen;
    s32 absOff, blendVal, ratio;

    *(s16 *)(c + 0x50) = p1[0];
    *(s16 *)(c + 0x52) = p1[1];
    *(s16 *)(c + 0x54) = p2[0];
    *(s16 *)(c + 0x56) = p2[1];

    p1[0] = (s16)(p1[0] - 0x80);
    p1[1] = (s16)(0 - p1[1]);
    p2[0] = (s16)(p2[0] - 0x80);
    p2[1] = (s16)(0 - p2[1]);

    if (*(const s16 *)p1 > *(const s16 *)p2) {
        swapX = p1[0];
        swapZ = p1[1];
        p1[0] = p2[0];
        p1[1] = p2[1];
        p2[0] = swapX;
        p2[1] = swapZ;
    }

    {
        s32 ax = ((s32)p1[0]) << 12;
        s32 ay = ((s32)p1[1]) << 12;
        va.x = ax; va.y = ay; va.z = 0;
    }
    {
        s32 bx = ((s32)p2[0]) << 12;
        s32 by = ((s32)p2[1]) << 12;
        vb.x = bx; vb.y = by; vb.z = 0;
    }

    Vec3_Add(&sumVec, &va, &vb);
    Vec3_MulScalar(&scaledVec, &sumVec, 0x800);
    *(s32 *)(c + 0x8) = scaledVec.x;
    *(s32 *)(c + 0xC) = scaledVec.y;
    *(s32 *)(c + 0x10) = scaledVec.z;

    Vec3_Sub(&diffVec, &vb, &va);
    *(s32 *)(c + 0x20) = diffVec.x;
    *(s32 *)(c + 0x24) = diffVec.y;
    *(s32 *)(c + 0x28) = diffVec.z;

    func_0203ce80(&tmpVec, (Vector3 *)(c + 0x20));

    zAxis.x = 0;
    zAxis.y = 0;
    zAxis.z = 0x1000;
    func_0203cf00(&crossVec, (Vector3 *)(c + 0x20), &zAxis);
    *(s32 *)(c + 0x14) = crossVec.x;
    *(s32 *)(c + 0x18) = crossVec.y;
    *(s32 *)(c + 0x1C) = crossVec.z;

    if (*(s32 *)(c + 0x18) < 0) {
        *(s32 *)(c + 0x14) = 0 - *(s32 *)(c + 0x14);
        *(s32 *)(c + 0x18) = 0 - *(s32 *)(c + 0x18);
    }

    *(s32 *)(c + 0x38) = 0;
    *(s32 *)(c + 0x3C) = 0;
    *(s32 *)(c + 0x40) = 0;
    *(s32 *)(c + 0x44) = 0 - *(s32 *)(c + 0x14);
    *(s32 *)(c + 0x48) = 0 - *(s32 *)(c + 0x18);
    *(s32 *)(c + 0x4C) = 0 - *(s32 *)(c + 0x1C);
    Vec3_MulScalarInPlace((s32 *)(c + 0x44), 0x800);

    SubVec3(&va, (Vector3 *)(c + 8), &va);
    SubVec3(&vb, (Vector3 *)(c + 8), &vb);
    dist = Vec3_Dist(&va, &vb);
    *(s32 *)(c + 0x58) = dist >> 1;
    rowStep = dist / 3;

    {
        s32 i;
        s32 rowFrac, colWeight, colFrac;
        s32 accum;
        s32 negRowFrac;
        s32 rowAbs, colAbs;
        s32 colW, invColWeight, rowW, cornerW, weightA, weightB, weightSq;

        j = 0;
        accum = 0;
        rowFrac = -0x180;
        cell = (Vector3 *)(c + 0x5C);
        mirror = (Vector3 *)(c + 0x11C);
        do {
            i = 0;
            negRowFrac = 0 - rowFrac;
            colFrac = -0x180;
            colWeight = 0;
            do {
                colAbs = (colFrac < 0) ? (0 - colFrac) : colFrac;
                colW = 0x1000 - (0x180 - colAbs);
                invColWeight = 0x1000 - colWeight;
                rowAbs = (rowFrac < 0) ? negRowFrac : rowFrac;
                rowW = 0x1000 - (0x180 - rowAbs);
                cornerW = (s32)(((s64)colW * rowW + 0x800) >> 12);
                flag = 0;
                weightA = (s32)(((s64)invColWeight * cornerW + 0x800) >> 12);
                weightSq = (s32)(((s64)cornerW * cornerW + 0x800) >> 12);
                weightB = (s32)(((s64)colWeight * cornerW + 0x800) >> 12);

                cell->x = (s32)(((s64)vb.x * weightB + 0x800) >> 12);
                cell->x += (s32)(((s64)va.x * weightA + 0x800) >> 12);
                cell->y = (s32)(((s64)vb.y * weightB + 0x800) >> 12);
                cell->y += (s32)(((s64)va.y * weightA + 0x800) >> 12);

                cell->z = accum - *(s32 *)(c + 0x58);
                cell->z = (s32)(((s64)cell->z * weightSq + 0x800) >> 12);

                mirror->x = cell->x;
                mirror->y = cell->y;
                mirror->z = cell->z;

                i++;
                colFrac += 0x100;
                colWeight += 0x555;
                cell++;
                mirror++;
            } while (i < 4);
            j++;
            accum += rowStep;
            rowFrac += 0x100;
        } while (j < 4);
    }

    *(s16 *)(c + 0x320) = (s16)flag;
    *(s16 *)(c + 0x322) = 0x1400;
    *(u8 *)(c + 0x329) = 0x1F;
    func_ov006_020cf040(c, c + 0x38, c + 0x44);
    func_ov006_020cf124(c);

    *(s32 *)(c + 0x2C) = 0x1000;
    *(s32 *)(c + 0x30) = 0x1000;
    *(s32 *)(c + 0x34) = 0x1000;

    bestObj = 0;
    bestVal = 0;
    bestD1 = 0;
    bestRel.x = 0;
    bestRel.y = 0;
    bestRel.z = 0;
    bestDir.x = 0;
    bestDir.y = 0;
    bestDir.z = 0;

    k = 0;
    do {
        obj = data_ov006_0214097c[k];
        if (obj != 0) {
            if (((IsActiveFn)((*(void ***)obj))[3])(obj) != 0) {
                obj = data_ov006_0214097c[k];
                pos = ((GetVecFn)((*(void ***)obj))[0])(obj);
                relPos.x = pos->x;
                relPos.y = pos->y;
                relPos.z = pos->z;
                obj = data_ov006_0214097c[k];
                dir = ((GetVecFn)((*(void ***)obj))[2])(obj);
                rawDir.x = dir->x;
                rawDir.y = dir->y;
                rawDir.z = dir->z;

                SubVec3(&relPos, (Vector3 *)(c + 8), &relPos);
                alongDot = DotVec3((Vector3 *)(c + 0x20), &relPos);
                crossDot = DotVec3((Vector3 *)(c + 0x14), &relPos);
                sideDot = DotVec3((Vector3 *)(c + 0x14), &rawDir);
                DotVec3((Vector3 *)(c + 0x20), &rawDir);

                if (sideDot < 0x100 && crossDot <= bestVal && crossDot > -0x24000) {
                    s32 h = *(s32 *)(c + 0x58);
                    if (alongDot > -h && alongDot < h) {
                        bestRel = relPos;
                        bestDir = rawDir;
                        bestObj = data_ov006_0214097c[k];
                        bestVal = crossDot;
                        bestD1 = alongDot;
                    }
                }
            }
        }
        k++;
    } while (k < 5);

    if (bestObj != 0) {
        *(s32 *)(c + 0x38) = bestRel.x;
        *(s32 *)(c + 0x3C) = bestRel.y;
        *(s32 *)(c + 0x40) = bestRel.z;
        *(s32 *)(c + 0x44) = bestDir.x;
        *(s32 *)(c + 0x48) = bestDir.y;
        *(s32 *)(c + 0x4C) = bestDir.z;

        absOff = (bestD1 < 0) ? (0 - bestD1) : bestD1;
        ratio = _ZN4cstd4fdivEii(absOff, *(s32 *)(c + 0x58));
        halfLen = *(s32 *)(c + 0x58);
        absOff = (bestD1 < 0) ? (0 - bestD1) : bestD1;
        blendVal = (s32)(((s64)absOff * halfLen + 0x800) >> 12);
        if (blendVal < 0x400000) {
            *(s16 *)((char *)bestObj + 0x18) = 2;
            func_ov006_020e6db4(0x1B1, *(s32 *)(c + 8),
                (s32)(data_ov006_0213b2fc * ratio + data_ov006_0213b300 * (0x1000 - ratio)) >> 0xC);
        } else {
            *(s16 *)((char *)bestObj + 0x18) = 1;
            func_ov006_020e6db4(0x1AE, *(s32 *)(c + 8),
                (s32)(data_ov006_0213b2f8 * ratio + data_ov006_0213b304 * (0x1000 - ratio)) >> 0xC);
        }
        *(s32 *)((char *)bestObj + 0x4) = *(s32 *)(c + 0x14);
        *(s32 *)((char *)bestObj + 0x8) = *(s32 *)(c + 0x18);
        *(s32 *)((char *)bestObj + 0xC) = *(s32 *)(c + 0x1C);
        *(s32 *)((char *)bestObj + 0x10) = bestD1;
        *(s32 *)((char *)bestObj + 0x14) = *(s32 *)(c + 0x58);
        *(u8 *)(c + 0x328) = 3;
        func_ov006_020cf040(c, c + 0x38, c + 0x44);
        func_ov006_020cfa28(c);
        return;
    }

    *(s16 *)(c + 0x31C) = 0x258;
    *(u8 *)(c + 0x328) = 1;
    func_ov006_020cfc58(c);
}
