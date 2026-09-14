// @symbol func_ov006_020e20bc
/* recovered: Shuffle Shell (dScMgCurling_c): a per-index update with sqrt and atan2 over the shell records. */
// NONMATCHING: div 299 of 376 words. mwccarm 2004/b56, --module ov006,
// @ 0x020e20bc size 0x5e0. Residue class: colouring and ordering (Andrew's #2692 carries a 203-word improvement in the DB; refresh from it when landed).
// Draft from nearmiss/db.jsonl (stored divergence 299), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
#pragma opt_propagation off
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;
typedef unsigned long long u64;

extern s16 data_02082214[];
extern s32 _ZN4cstd4sqrtEy(u64 v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern void func_02012718(void *a, s32 b);

#define FXMUL(a, b) ((s32)((((s64)(a)) * (b) + 0x800) >> 12))

void func_ov006_020e20bc(char *self, s32 idx)
{
    s32 selfOff;
    s32 *selfXp;
    s32 *selfZp;
    char *p;
    s32 i;

    selfOff = idx * 0x2c;
    selfXp = (s32 *)(self + selfOff + 0x4660);
    selfZp = (s32 *)(self + selfOff + 0x4664);
    p = self;
    i = 0;
    do {
        char *q = p + 0x4000;
        if (*(u8 *)(q + 0x689) != 0 && idx != i &&
            *(u8 *)(q + 0x688) != 0 && *(u8 *)(q + 0x688) != 3) {
            s32 dz = (*(s32 *)(q + 0x664) - *selfZp) >> 12;
            s32 dx = (*(s32 *)(q + 0x660) - *selfXp) >> 12;
            if (_ZN4cstd4sqrtEy((u64)(s64)(dz * dz + dx * dx)) <= 0x18) {
                s32 candOff = i * 0x2c;
                s32 *candX = (s32 *)(self + 0x4660 + candOff);
                s32 *candZ = (s32 *)(self + 0x4664 + candOff);
                s32 *sX = (s32 *)(self + 0x4660 + selfOff);
                s16 newAngle;
                s16 *selfAngleP;
                s16 *candAngleP;
                s32 *selfDistP;
                s32 A, B, C, E;
                s32 sinSelf, cosSelf, sinCand, cosCand;
                s32 selfDist, candDist;
                s32 V1, V2, V3, V4;
                u16 negA, posA;
                s32 cosNegA, sinNegA, cosPosA, sinPosA;
                s32 RA, RB, RC, RD, RE, RF, RG, RH;

                newAngle = _ZN4cstd5atan2E5Fix12IiES1_(
                    *(s32 *)(self + selfOff + 0x4000 + 0x664) - *candZ,
                    *sX - *candX);

                selfAngleP = (s16 *)(self + 0x4686 + selfOff);
                candAngleP = (s16 *)(self + 0x4686 + candOff);
                A = ((u16)*selfAngleP >> 4) * 2;
                B = ((u16)*candAngleP >> 4) * 2;
                selfDistP = (s32 *)(self + 0x4668 + selfOff);
                selfDist = *(s32 *)(self + 0x4668 + selfOff);
                candDist = *(s32 *)(self + 0x4000 + candOff + 0x668);

                sinSelf = data_02082214[A + 1];
                cosSelf = data_02082214[A];
                sinCand = data_02082214[B + 1];
                cosCand = data_02082214[B];

                V1 = FXMUL(sinSelf, selfDist);
                V2 = FXMUL(cosSelf, selfDist);
                V3 = FXMUL(sinCand, candDist);
                V4 = FXMUL(cosCand, candDist);

                negA = (u16)(0 - (u16)newAngle);
                posA = (u16)(0 - negA);
                C = ((u16)negA >> 4) * 2;
                E = ((u16)posA >> 4) * 2;
                cosNegA = data_02082214[C];
                sinNegA = data_02082214[C + 1];
                cosPosA = data_02082214[E];
                sinPosA = data_02082214[E + 1];

                if (V4 == 0) V4 = V2 >> 1;

                RA = FXMUL(sinNegA, V1) - FXMUL(cosNegA, V2);
                RB = FXMUL(cosNegA, V1) + FXMUL(sinNegA, V2);
                RC = FXMUL(sinNegA, V3) - FXMUL(cosNegA, V4);
                RF = FXMUL(cosNegA, V3) + FXMUL(sinNegA, V4);
                RD = FXMUL(sinPosA, RC) - FXMUL(cosPosA, RB);
                RE = FXMUL(cosPosA, RC) + FXMUL(sinPosA, RB);
                RG = FXMUL(sinPosA, RA) - FXMUL(cosPosA, RF);
                RH = FXMUL(cosPosA, RA) + FXMUL(sinPosA, RF);

                *selfAngleP = _ZN4cstd5atan2E5Fix12IiES1_(RE, RD);
                *selfDistP = _ZN4cstd4sqrtEy((u64)((s64)RE * RE + (s64)RD * RD));

                *(s32 *)(self + 0x4660 + selfOff) = *candX + FXMUL(sinPosA, (s64)0x1b000);
                *(s32 *)(self + 0x4664 + selfOff) = *candZ + FXMUL(cosPosA, (s64)0x1b000);

                {
                    s32 x12 = *(s32 *)(self + 0x4660 + selfOff) >> 12;
                    s32 z12 = *(s32 *)(self + 0x4664 + selfOff) >> 12;
                    s32 v1 = x12 - 0xc;
                    if (v1 < 0) {
                        s32 delta = (*(s32 *)(self + 0x4660 + selfOff)) - 0xc000;
                        *candX = *candX + delta;
                        *(s32 *)(self + selfOff + 0x4000 + 0x660) = 0xc000;
                        v1 = delta;
                    }
                    if (v1 + 0xc > 0x100) {
                        *candX = *candX + (*(s32 *)(self + 0x4660 + selfOff) - 0xf4000);
                        *(s32 *)(self + 0x4660 + selfOff) = 0xf4000;
                    }
                    if (z12 - 0xc < -0xe0) {
                        *candZ = *candZ + (*(s32 *)(self + 0x4664 + selfOff) + 0xd4000);
                        *(s32 *)(self + 0x4664 + selfOff) = -0xd4000;
                    }
                }

                *candAngleP = _ZN4cstd5atan2E5Fix12IiES1_(RH, RG);
                *(s32 *)(self + 0x4668 + candOff) =
                    _ZN4cstd4sqrtEy((u64)((s64)RH * RH + (s64)RG * RG));

                *(u8 *)(self + selfOff + 0x4000 + 0x688) = 1;
                *(u8 *)(self + 0x4000 + candOff + 0x688) = 1;
                if (*(s32 *)(self + 0x4668 + candOff) >= 0x3800)
                    *(u8 *)(self + 0x4000 + candOff + 0x68b) = 1;
                else
                    *(u8 *)(self + 0x4000 + candOff + 0x68b) = 0;

                func_02012718((void *)0xe8, *(s32 *)(self + 0x4660 + candOff));
                return;
            }
        }
        i++;
        p += 0x2c;
    } while (i < 5);
}
