// @symbol func_ov006_02125f68
// NONMATCHING: register allocation (div=9 of 632 words). Logic verified against
// the ROM instruction by instruction; the residual is a PURE REGISTER
// TRANSPOSITION r6<->r8 over the whole body, checked mechanically rather than by
// eye -- every divergent word decodes to the same mnemonic with the same operand
// shape, and the substitution is a consistent bijection (0 shape divergences).
// The ROM ranks the direction counter above the address c+d; mwccarm ranks them
// the other way. Same class as the confirmed floor in notes/mwccarm-codegen.md
// section 6y, "allocator priority between two register-computed values".
// Counts as decompiled, not matched.
/* dScMgSnowball_c - scene 377 "Snowball Slalom", obstacle and collision pass.
 *
 * Called from slot 6 (Behavior, func_ov006_021283a4) in two of its phase arms,
 * with the ball's proposed velocity already in +0xab60/+0xab64 and before the
 * position integrate.  It probes 0x20 directions around the ball, pushes the
 * ball out of geometry it is inside, sucks it toward a hole it is over, and
 * finally reflects the velocity off the accumulated contact normal.
 *
 * Per-direction byte maps, all indexed 0..0x1f (0x800 of angle per step):
 *   +0xabf8[d]  blocked THIS resolve pass (cleared at the top of every pass)
 *   +0xac18[d]  blocked at any point during the whole call (never re-cleared,
 *               so it is the union) - this is what builds the bounce normal
 *   +0xac38[d]  the probe point is over a hole/special surface
 */
#include "types.h"

typedef struct Vec2 { int x, y; } Vec2;
typedef struct V3 { int x, y, z; } V3;

extern int func_ov006_021259d8(char *c, Vec2 *p);
extern int func_ov006_02125bbc(char *c, Vec2 *p);
extern int func_ov006_02125cdc(char *c, Vec2 *p);
extern int Vec2_Len(int *v);
extern void func_0203d388(int *v, s16 ang);
extern int func_0203d434(int *v);
extern void func_0203d630(int *v, int len);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int x, int y, int z, const void *f, void *g);
extern int *_ZN8Particle6System12FromUniqueIDEj(u32 id);
extern void func_02012718(int id, int x);
extern s16 data_02082214[];

#define atan2 _ZN4cstd5atan2E5Fix12IiES1_
#define pnew _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE
#define pfromid _ZN8Particle6System12FromUniqueIDEj

#define I(o) (*(int *)(c + (o)))
#define B(o) (*(u8 *)(c + (o)))
#define BJ(o) (*(u8 *)(c + j + (o)))
#define AT(p, o) (*(int *)((char *)(p) + (o)))
#define FX(a, b) ((int)((((s64)(a) * (s64)(b)) + 0x800) >> 12))

void func_ov006_02125f68(char *c)
{
    int fastHit;
    int centreOut;
    int allOut;
    int j;
    int j2;
    int j3;
    int j4;
    int hit;
    int pass;
    s16 ang;
    s16 ang2;
    Vec2 unitA;
    Vec2 unitB;
    Vec2 probe;
    Vec2 hp;
    Vec2 push;
    Vec2 stepA;
    Vec2 stepB;
    Vec2 nrm;
    Vec2 stepC;
    Vec2 argA;
    Vec2 argB;
    Vec2 argC;
    Vec2 argD;
    V3 ppos;
    u32 id1, id2;
    int *pt;
    int py, px, pz;
    int dot;
    int sc;
    s16 tv;

    for (j = 0; j < 0x20; j++) {
        BJ(0xabf8) = 0;
        BJ(0xac18) = 0;
        BJ(0xac38) = 0;
    }
    B(0xb9e4) = 0;
    fastHit = 0;

    argA.x = I(0xab38);
    argA.y = I(0xab3c);
    centreOut = func_ov006_021259d8(c, &argA);

    pass = 0;
    unitA.x = 0;
    unitA.y = 0x800;
    unitB.x = 0;
    unitB.y = 0x1000;
    do {
        hit = 0;
        ang = 0;
        for (j = 0; j < 0x20; j++) {
            char *e = c + j;
            u8 *f = (u8 *)(e + 0xabf8);
            *f = 0;
            probe.x = 0;
            probe.y = 0;
            probe.y = probe.y + I(0xaba0);
            func_0203d388((int *)&probe, ang);
            probe.x = probe.x + I(0xab38);
            probe.y = probe.y + I(0xab3c);
            argB.x = probe.x;
            argB.y = probe.y;
            if (func_ov006_02125cdc(c, &argB) != 0) {
                *f = 1;
                *(u8 *)(e + 0xac18) = 1;
                hit = 1;
                if (Vec2_Len((int *)(c + 0xab60)) >= 0x800)
                    fastHit = 1;
                AT(&hp, 0) = probe.x;
                AT(&hp, 4) = probe.y;
            } else {
                argC.x = probe.x;
                argC.y = probe.y;
                if (func_ov006_02125bbc(c, &argC) != 0) {
                    *f = 1;
                    hit = 1;
                    *(u8 *)(e + 0xac18) = 1;
                    B(0xb9e4) = 1;
                    AT(&hp, 0) = probe.x;
                    AT(&hp, 4) = probe.y;
                }
            }
            argD.x = probe.x;
            argD.y = probe.y;
            if (func_ov006_021259d8(c, &argD) != 0)
                *(u8 *)(e + 0xac38) = 1;
            else
                *(u8 *)(e + 0xac38) = 0;
            ang = (s16)(ang + 0x800);
        }

        push.x = 0;
        push.y = 0;
        ang2 = 0;
        if (hit != 0) {
            for (j2 = 0; j2 < 0x20; j2++) {
                if (*(u8 *)(c + j2 + 0xabf8) == 1) {
                    stepA.x = unitA.x;
                    stepA.y = unitA.y;
                    func_0203d388((int *)&stepA, ang2);
                    push.x = push.x + stepA.x;
                    push.y = push.y + stepA.y;
                }
                ang2 = (s16)(ang2 + 0x800);
            }
            func_0203d434((int *)&push);
            if (Vec2_Len((int *)(c + 0xab60)) < 0x800)
                func_0203d630((int *)&push, Vec2_Len((int *)(c + 0xab60)));
            else
                func_0203d630((int *)&push, unitA.y);
            I(0xab38) = I(0xab38) - push.x;
            I(0xab3c) = I(0xab3c) - push.y;
        } else if (centreOut == 1) {
            allOut = 1;
            for (j3 = 0; j3 < 0x20; j3++) {
                if (*(u8 *)(c + j3 + 0xac38) == 1) {
                    stepB.x = unitB.x;
                    stepB.y = unitB.y;
                    func_0203d388((int *)&stepB, ang2);
                    push.x = push.x + stepB.x;
                    push.y = push.y + stepB.y;
                } else {
                    allOut = 0;
                }
                ang2 = (s16)(ang2 + 0x800);
            }
            func_0203d434((int *)&push);
            I(0xab60) = I(0xab60) + push.x;
            I(0xab64) = I(0xab64) + push.y;
            if (allOut == 1)
                B(0xb9e5) = 1;
        }
        pass++;
    } while (hit == 1 && pass < 0x21);

    if (B(0xb9e4) == 1) {
        int len = Vec2_Len((int *)(c + 0xab60));
        I(0xaba0) = I(0xaba0) - len;
        if (I(0xaba0) < 0x4000)
            I(0xaba0) = 0x4000;
        if (len >= 0x800) {
            pz = I(0xaba0);
            py = I(0xab6c) - AT(&hp, 4) + 0x110000;
            px = AT(&hp, 0) - 0x80000;
            AT(&ppos, 0) = px;
            AT(&ppos, 4) = py;
            AT(&ppos, 8) = pz;
            id1 = pnew(0, 0xf1, AT(&ppos, 0) << 3, AT(&ppos, 4) << 3, AT(&ppos, 8) << 3, 0, 0);
            id2 = pnew(0, 0xf2, AT(&ppos, 0) << 3, AT(&ppos, 4) << 3, AT(&ppos, 8) << 3, 0, 0);
            pt = pfromid(id1);
            if (pt != 0) {
                pt[0x11] = (s16)((I(0xaba0) - 0x4000) * 0xb / 60 + 0x3000);
                pt[0x12] = (s16)((I(0xaba0) - 0x4000) * 0x24 / 10 / 60 + 0x1333);
                pt[0x13] = (s16)((I(0xaba0) - 0x4000) * 0x1a / 10 / 60 + 0x1666);
                pt[0x14] = (s16)((I(0xaba0) - 0x4000) * 8 / 10 / 60 + 0x666);
            }
            pt = pfromid(id2);
            if (pt != 0) {
                pt[0x12] = (s16)((I(0xaba0) - 0x4000) * 0x3e / 10 / 60 + 0x1ccc);
                pt[0x14] = (s16)((I(0xaba0) - 0x4000) * 0x2d / 10 / 60 + 0x1800);
            }
            if (I(0xb9f4) == 1) {
                func_02012718(0x169, AT(&hp, 0));
                if (B(0xb9e6) == 1) {
                    func_02012718(0x16a, I(0xb9e8));
                    B(0xb9e6) = 0;
                }
            }
        }
    } else if (fastHit == 1) {
        if (I(0xb9f4) == 1)
            func_02012718(0x168, AT(&hp, 0));
    }

    B(0xb9e6) = 0;
    nrm.x = 0;
    nrm.y = 0;
    for (j4 = 0; j4 < 0x20; j4++) {
        if (*(u8 *)(c + j4 + 0xac18) == 1) {
            stepC.x = 0;
            stepC.y = 0x1000;
            func_0203d388((int *)&stepC, (s16)(j4 * 0x10000 / 32));
            nrm.x = nrm.x - stepC.x;
            nrm.y = nrm.y - stepC.y;
        }
    }
    if (nrm.x == 0 && nrm.y == 0)
        return;
    func_0203d434((int *)&nrm);
    dot = FX(I(0xab60), nrm.x) + FX(I(0xab64), nrm.y);
    if (B(0xb9e4) == 1) {
        sc = FX(dot, 0x1800);
        I(0xab60) = I(0xab60) - FX(sc, nrm.x);
        I(0xab64) = I(0xab64) - FX(sc, nrm.y);
        tv = data_02082214[((int)(u16)atan2(I(0xab64), I(0xab60)) >> 4) * 2];
        I(0xab60) = I(0xab60) / 8 + FX(I(0xab60), tv) / 4;
        I(0xab64) = I(0xab64) / 8 + FX(I(0xab64), tv) / 4;
    } else {
        sc = FX(dot, 0x1200);
        I(0xab60) = I(0xab60) - FX(sc, nrm.x);
        I(0xab64) = I(0xab64) - FX(sc, nrm.y);
    }
}
