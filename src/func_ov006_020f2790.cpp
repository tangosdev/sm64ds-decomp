//cpp
// @symbol func_ov006_020f2790
/* recovered: dScMgLuigi_c picture placer, ov006 0x020f2790 (1320 bytes). Runs
 * once per frame while the board is still filling. Boards whose
 * data_ov006_0213ce98 entry is set are laid out in one go by
 * func_ov006_020f2224 instead; the rest place one picture per call. The slot
 * gets a random speed level, nudged off unk_545a when it lands on it, and the
 * very first slot of a board is dropped at a random cell outside the middle
 * block with a small random jitter. Every later slot retries a random cell up
 * to 100 times, then walks the grid linearly until it finds a free one. When
 * unk_5456 reaches the board's capacity the board is marked full (unk_5455)
 * and four random 4-bit values go into unk_515c.
 *
 * Two pointer locals are load-bearing (both measured against the ROM):
 *   - `p` in the speed-level fixup. The ROM stores and re-reads
 *     mSpeedLevel[cur] through the scaled-index form `[r3,r0]` but keeps
 *     `&mSpeedLevel[cur]` in a register for the three accesses inside the
 *     fixup. Declaring p before the `if` (whose test still uses the subscript)
 *     puts the `add` in the pre-branch block exactly where the ROM has it;
 *     spelling the fixup with subscripts costs the address register and
 *     re-derives the base from the literal pool.
 *   - `f` over data_ov006_0213ce84[idx]. mwccarm shares one loaded value
 *     across both tests when both are written as subscripts, which drops the
 *     re-load the ROM performs after `unk_5455 = 1`. Naming a const pointer
 *     and using it for the SECOND test only breaks that: the first test stays
 *     folded into `[base,idx]`, the `add` survives for the pointer, and the
 *     tail re-loads through it.
 * `#pragma opt_propagation off` is worth 219 -> 330 aligned instructions here:
 * without it `this` colours into r6 and the whole callee-saved file permutes. */
#include "dScMgLuigi_c.h"

extern "C" {
int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u8 data_ov006_0213ce84[];
extern u8 data_ov006_0213ce98[];
extern u16 data_ov006_0213cee0[];
void func_ov006_020f2224(dScMgLuigi_c *self);
}

#pragma opt_propagation off
extern "C" void func_ov006_020f2790(dScMgLuigi_c *self)
{
    int idx;
    int cur;
    int lim;
    int col;
    int row;
    int tries;
    int rnd;

    if (self->unk_5455 != 0)
        return;

    idx = self->unk_5174;
    if (data_ov006_0213ce98[idx] != 0) {
        func_ov006_020f2224(self);
        return;
    }

    lim = data_ov006_0213cee0[idx];
    cur = self->unk_5456;

    rnd = RandomIntInternal(&data_0209d4b8);
    self->mSpeedLevel[cur] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
    {
        u8 *p = &self->mSpeedLevel[cur];
        if (self->unk_545a == self->mSpeedLevel[cur]) {
            rnd = RandomIntInternal(&data_0209d4b8);
            *p += ((((u32)rnd >> 16) & 0x7fff) * 3 >> 15) + 1;
            *p &= 3;
        }
    }

    if (cur == 0) {
        int jx;
        int jy;
        do {
            col = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 13 >> 15;
            row = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15;
        } while (col >= 6 && col <= 10 && row >= 4 && row <= 8);

        jy = -2;
        jx = jy;
        jx += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
        jy += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;

        if (data_ov006_0213ce84[idx] != 0) {
            self->mPosX[0] = (col * 20 + (jx + 8)) << 12;
            self->mPosY[0] = (row * 20 + (jy + 16)) << 12;
            self->unk_52ed[0] = 1;
            self->unk_51fd[0] = 0;
            self->mSpeedLevel[0] = self->unk_545a;
            self->mGrid[col][row] = 1;
            self->unk_5456++;
            return;
        } else {
            int n = lim - 1;
            self->mPosX[n] = (col * 20 + 8) << 12;
            self->mPosY[n] = (row * 20 + 16) << 12;
            self->unk_52ed[n] = 1;
            self->unk_51fd[n] = 0;
            self->mSpeedLevel[n] = self->unk_545a;
            self->mGrid[col][row] = 1;
        }
    }

    tries = 0;
    while (1) {
        u8 *cell;
        if (tries >= 100) {
            col++;
            if (col >= 12) {
                row++;
                col = 0;
                if (row >= 11)
                    row = 0;
            }
        } else {
            col = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 13 >> 15;
            row = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15;
        }
        cell = (u8 *)((unsigned int)((char *)self + col * 9 + row) + 0x5178);
        if (*cell == 0) {
            int jx;
            int jy;
            jy = -2;
            jx = jy;
            jx += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
            jy += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
            self->mPosX[cur] = (col * 20 + (jx + 8)) << 12;
            self->mPosY[cur] = (row * 20 + (jy + 16)) << 12;
            *cell = 1;
            break;
        }
        tries++;
    }

    self->unk_52ed[cur] = 1;
    self->unk_51fd[cur] = 0;
    self->unk_5456++;

    const u8 *f = &data_ov006_0213ce84[idx];
    if (self->unk_5456 >= (data_ov006_0213ce84[idx] != 0 ? lim : lim - 1)) {
        if (idx == 0xe || idx == 0x10 || idx == 0x11 || idx == 0x13) {
            self->mPosX[cur] = self->mPosX[lim - 1];
            self->mPosY[cur] = self->mPosY[lim - 1] - 0x14000;
        }
        self->unk_5455 = 1;
        if (*f != 0)
            self->unk_5456 = 1;
        else
            self->unk_5456 = lim;
    }

    if (self->unk_5455 == 0)
        return;

    self->unk_515c[0] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    self->unk_515c[1] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    self->unk_515c[2] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    self->unk_515c[3] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
}
