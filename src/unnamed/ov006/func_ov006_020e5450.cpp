//cpp
// @symbol func_ov006_020e5450
/* recovered: Shell Smash (dScMgCurling2_c): a per-index update with sqrt and atan2 over the shell records. */
// NONMATCHING: div 30 of 344 words. mwccarm 2004/b56, --module ov006,
// @ 0x020e5450 size 0x560 (exact size). Residue class: one contiguous schedule
// residue in the collision block, +0x1b4..+0x224, plus one reload of the slot it
// moves at +0x3d0. This draft hoists the contact angle's table word (sP) and its
// asr #31 sign-extension above the hit stone's table chain, spilling the sign at
// sp+0x34 ahead of the hit stone's cosine at 0x38; the ROM loads the two contact
// table words after the hit stone's cosine and sine, sign-extends both after vex,
// and keeps the cosine at 0x34 with the signs at 0x38 and 0x3c.
// Draft first banked from nearmiss/db.jsonl at 191, landed 2026-09-14 under Tango's ruling
// that functionally-equivalent drafts live on main with an honest banner so the port and
// readers have source. Improved to 70 on 2026-10-02 (dx/dy reuse, one rel/k/c/s set for
// both stones, table value first in each product, overshoot kept in xi), then to 30 the
// same day with the twin func_ov006_020e20bc's levers: the hit stone's angle is bound as
// a reference right after atan2, before the velocities are read (70 -> 43); the x
// quotient is taken before the y quotient, which colours the idx.y reload r1 (43 -> 41);
// and the three fields written after a call (the hit stone's x and y, the moving
// stone's x for the sound) are reached through pointers assigned after the dx/dy
// reads, so their addresses sit in the frame chain while the dx/dy addressing stays
// (41 -> 30; assigning them before the reads grows the frame and the function to
// 0x540). Inert at 30: every placement of the sP/cP reads, E unscaled or inlined,
// s16 or long long for sP/cP, explicit wide copies, a cached hit speed, the hit
// stone's sine before its cosine, declaration-order swaps. Worse: reusing k or rel for
// the E index (0x578), a table pointer for T[E] (0x51c), references for the two stones
// (size change), rereading T[E] for the 0x1b000 products (0x56c).
// Counts as decompiled, not matched. tools/enroll.py leaves it out of the ROM build, which
// keeps the original bytes for this range. A byte-exact match replaces this file and
// drops the banner.
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
        dy = self->mStone[i].y;
        dx = self->mStone[i].x - self->mStone[idx].x;
        dy -= self->mStone[idx].y;
        if ((_ZN4cstd4sqrtEy((u64)((long long)dx * dx + (long long)dy * dy)) >> 12) >= 0x18) continue;
        {
            int nex;
            int ney;
            u16 ang;
            u16 rel;
            int k;
            int E;
            int c;
            int s;
            int sP;
            int cP;
            int vmx;
            int vmy;
            int vex;
            int vey;
            int yi;
            int xi;
            int *pHitX;
            int *pX;
            int *pHitY;

            dx = self->mStone[i].x - self->mStone[idx].x;
            dy = self->mStone[i].y - self->mStone[idx].y;
            pHitX = &self->mStone[i].x;
            pX = &self->mStone[idx].x;
            pHitY = &self->mStone[i].y;
            ang = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
            u16 &hitAngle = self->mStone[i].angle;
            E = (ang >> 4) * 2;
            rel = self->mStone[idx].angle - ang;
            k = (rel >> 4) * 2;
            c = data_02082214[k + 1];
            s = data_02082214[k];
            vmx = FMUL(c, self->mStone[idx].speed);
            vmy = FMUL(s, self->mStone[idx].speed);
            rel = self->mStone[i].angle - ang;
            k = (rel >> 4) * 2;
            c = data_02082214[k + 1];
            s = data_02082214[k];
            sP = data_02082214[E];
            cP = data_02082214[E + 1];
            vex = FMUL(c, self->mStone[i].speed);
            vey = FMUL(s, self->mStone[i].speed);
            dy = FMUL(cP, vex) - FMUL(sP, vmy);
            dx = FMUL(sP, vex) + FMUL(cP, vmy);
            nex = FMUL(cP, vmx) - FMUL(sP, vey);
            ney = FMUL(sP, vmx) + FMUL(cP, vey);
            self->mStone[idx].angle = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
            self->mStone[idx].speed = _ZN4cstd4sqrtEy((u64)((long long)dy * dy + (long long)dx * dx));
            self->mStone[idx].x = self->mStone[i].x - FMUL(cP, 0x1b000);
            self->mStone[idx].y = self->mStone[i].y - FMUL(sP, 0x1b000);
            xi = self->mStone[idx].x >> 12;
            yi = self->mStone[idx].y >> 12;
            if (xi - 0xc < 0) {
                xi = self->mStone[idx].x - 0xc000;
                *pHitX += xi;
                self->mStone[idx].x = 0xc000;
            }
            if (xi + 0xc > 0x100) {
                *pHitX += self->mStone[idx].x - 0xf4000;
                self->mStone[idx].x = 0xf4000;
            }
            if (yi - 0xc < -0xe0) {
                self->mStone[idx].y = -0xd4000;
                *pHitY = self->mStone[idx].y + 0x18000;
            }
            hitAngle = _ZN4cstd5atan2E5Fix12IiES1_(ney, nex);
            self->mStone[i].speed = _ZN4cstd4sqrtEy((u64)((long long)nex * nex + (long long)ney * ney));
            self->mStone[idx].state = 1;
            self->mStone[i].state = 1;
            if (self->mStone[i].speed >= 0x3800) {
                self->mStone[i].fast = 1;
            } else {
                self->mStone[i].fast = 0;
            }
            func_02012718((void *) 0xe8, *pX);
            self->SpawnValue(idx, i);
            return;
        }
    }
}
