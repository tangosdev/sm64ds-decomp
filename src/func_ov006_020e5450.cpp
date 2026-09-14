//cpp
// @symbol func_ov006_020e5450
/* recovered: Shell Smash (dScMgCurling2_c): a per-index update with sqrt and atan2 over the shell records. */
// NONMATCHING: div 191 of 344 words. mwccarm 2004/b56, --module ov006,
// @ 0x020e5450 size 0x560. Residue class: colouring and ordering (W12-14 seed 187 in the DB).
// Draft from nearmiss/db.jsonl (stored divergence 191), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
// @symbol func_ov006_020e5450
#include "dScMgCurling2_c.h"

extern "C" {
extern int _ZN4cstd4sqrtEy(u64 v);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_02012718(void *id, int v);
extern s16 data_02082214[];
}

#define FMUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

extern "C" void func_ov006_020e5450(dScMgCurling2_c *self, int idx)
{
    int i;
    int dx;
    int dy;

    for (i = 0; i < 11; i++) {
        if (self->mStone[i].active == 0) continue;
        if (idx == i) continue;
        if (self->mStone[i].state == 0) continue;
        if (self->mStone[i].state == 3) continue;
        dx = self->mStone[i].x - self->mStone[idx].x;
        dy = self->mStone[i].y - self->mStone[idx].y;
        if ((_ZN4cstd4sqrtEy((u64)((long long)dy * dy + (long long)dx * dx)) >> 12) >= 0x18) continue;
        {
            int nex;
            int ney;
            u16 ang;
            u16 relA;
            u16 relB;
            int A;
            int B;
            int E;
            int cM;
            int sM;
            int cE;
            int sE;
            int sP;
            int cP;
            int vmx;
            int vmy;
            int vex;
            int vey;
            int nmx;
            int nmy;
            int yi;
            int xi;


            ang = _ZN4cstd5atan2E5Fix12IiES1_(self->mStone[i].y - self->mStone[idx].y, self->mStone[i].x - self->mStone[idx].x);
            E = (ang >> 4) * 2;
            relA = self->mStone[idx].angle - ang;
            A = (relA >> 4) * 2;
            cM = data_02082214[A + 1];
            sM = data_02082214[A];
            relB = self->mStone[i].angle - ang;
            B = (relB >> 4) * 2;
            cE = data_02082214[B + 1];
            sE = data_02082214[B];
            sP = data_02082214[E];
            cP = data_02082214[E + 1];
vmx = FMUL(cM, self->mStone[idx].speed); vmy = FMUL(sM, self->mStone[idx].speed);
            vex = FMUL(cE, self->mStone[i].speed);
            vey = FMUL(sE, self->mStone[i].speed);
nmx = FMUL(cP, vex); nmx -= FMUL(vmy, sP); nmy = FMUL(vex, sP) + FMUL(vmy, cP); nex = FMUL(cP, vmx); nex -= FMUL(sP, vey); ney = FMUL(sP, vmx) + FMUL(cP, vey);
            self->mStone[idx].angle = _ZN4cstd5atan2E5Fix12IiES1_(nmy, nmx);
            self->mStone[idx].speed = _ZN4cstd4sqrtEy((u64)((long long)nmx * nmx + (long long)nmy * nmy));
            self->mStone[idx].x = self->mStone[i].x - FMUL(cP, 0x1b000);
            self->mStone[idx].y = self->mStone[i].y - FMUL(sP, 0x1b000);
            yi = self->mStone[idx].y >> 12;
            xi = self->mStone[idx].x >> 12;
            if (xi - 0xc < 0) {
                self->mStone[i].x += self->mStone[idx].x - 0xc000;
                self->mStone[idx].x = 0xc000;
            }
            if (xi + 0xc > 0x100) {
                self->mStone[i].x += self->mStone[idx].x - 0xf4000;
                self->mStone[idx].x = 0xf4000;
            }
            if (yi - 0xc < -0xe0) {
                self->mStone[idx].y = -0xd4000;
                self->mStone[i].y = self->mStone[idx].y + 0x18000;
            }
            self->mStone[i].angle = _ZN4cstd5atan2E5Fix12IiES1_(ney, nex);
            self->mStone[i].speed = _ZN4cstd4sqrtEy((u64)((long long)nex * nex + (long long)ney * ney));
            self->mStone[idx].state = 1;
            self->mStone[i].state = 1;
            if (self->mStone[i].speed >= 0x3800) {
                self->mStone[i].fast = 1;
            } else {
                self->mStone[i].fast = 0;
            }
            func_02012718((void *) 0xe8, self->mStone[idx].x);
            self->SpawnValue(idx, i);
            return;
        }
    }
}
