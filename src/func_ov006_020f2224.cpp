//cpp
// @symbol func_ov006_020f2224
/* recovered: dScMgLuigi_c board layout, ov006 0x020f2224 (1388 bytes). Called
 * from func_ov006_020f2790 for the boards whose data_ov006_0213ce98 entry is
 * non-zero: instead of scattering pictures at random it lays the whole board
 * out at once. The table entry picks one of four fixed layouts -- 1 is a 2x2
 * block at (0x70,0x50) step 0x20 whose speed levels run consecutively from one
 * random start, 2 a 4x4 at (0x50,0x30), 3 a 6-row x 8-column sheet at
 * (0x10,0x10), and anything else a single row of eight at y = -5 -- each with
 * its own random count in unk_5456 and a per-slot random speed level that is
 * nudged off unk_545a when it collides with it (and forced back onto it for
 * the slot that unk_5456 - 1 names). Then eight plus six random 0/1 bytes and
 * unk_5455++.
 *
 * Two spellings are load-bearing (both measured):
 *   - every RandomIntInternal result goes through the `rnd` local first. Writing
 *     the call inline inside the store expression lets mwccarm hoist the
 *     `this + n + 0x5365` element address ABOVE the call into a callee-saved
 *     register; the ROM computes it after the call and folds it into
 *     `ldrb r0,[r1,r0]!` / `add sl,sl,r1`. Inline cost +8 bytes and 21 extra
 *     instructions (divergence 193/347 -> shape-exact).
 *   - i/j/n are declared INSIDE each layout arm, not once at the top. Sharing
 *     them across the four arms coalesces their spill slots into one order and
 *     rotates every register in the function (58 divergences); per-arm locals
 *     give each arm its own pair of stack slots in the ROM's order. Within an
 *     arm the order is i, n, j (reverse declaration order is the colouring
 *     order: j takes the low register, n the next).
 * The two tail loops keep the INLINE call: naming the result there stops
 * mwccarm hoisting the 0x7fff mask out of the loop (+33 divergences). */
#include "dScMgLuigi_c.h"

extern "C" {
int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u8 data_ov006_0213ce98[];
}

extern "C" void func_ov006_020f2224(dScMgLuigi_c *self)
{
    int mode;
    int rnd;
    int i;

    mode = data_ov006_0213ce98[self->unk_5174];
    if (mode == 1) {
        int i;
        int n;
        int j;
        int base;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        base = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
        rnd = RandomIntInternal(&data_0209d4b8);
        self->unk_5456 = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                self->mPosX[n] = (0x70 + j * 0x20) << 12;
                self->mPosY[n] = (0x50 + i * 0x20) << 12;
                self->unk_52ed[n] = 1;
                self->unk_51fd[n] = 0;
                self->mSpeedLevel[n] = (base + n) & 3;
                if (self->unk_545a == self->mSpeedLevel[n])
                    self->unk_5456 = n + 1;
                n++;
            }
        }
    } else if (mode == 2) {
        int i;
        int n;
        int j;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        self->unk_5456 = ((((u32)rnd >> 16) & 0x7fff) * 16 >> 15) + 1;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                u8 step;
                self->mPosX[n] = (0x50 + j * 0x20) << 12;
                self->mPosY[n] = (0x30 + i * 0x20) << 12;
                self->unk_52ed[n] = 1;
                self->unk_51fd[n] = 0;
                rnd = RandomIntInternal(&data_0209d4b8);
                self->mSpeedLevel[n] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
                if (self->unk_545a == self->mSpeedLevel[n]) {
                    rnd = RandomIntInternal(&data_0209d4b8);
                    step = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
                    self->mSpeedLevel[n] += step + 1;
                    self->mSpeedLevel[n] &= 3;
                }
                if (n == self->unk_5456 - 1)
                    self->mSpeedLevel[n] = self->unk_545a;
                n++;
            }
        }
    } else if (mode == 3) {
        int i;
        int n;
        int j;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        self->unk_5456 = ((((u32)rnd >> 16) & 0x7fff) * 0x30 >> 15) + 1;
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 8; j++) {
                u8 step;
                self->mPosX[n] = (0x10 + j * 0x20) << 12;
                self->mPosY[n] = (0x10 + i * 0x20) << 12;
                self->unk_52ed[n] = 1;
                self->unk_51fd[n] = 0;
                rnd = RandomIntInternal(&data_0209d4b8);
                self->mSpeedLevel[n] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
                if (self->unk_545a == self->mSpeedLevel[n]) {
                    rnd = RandomIntInternal(&data_0209d4b8);
                    step = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
                    self->mSpeedLevel[n] += step + 1;
                    self->mSpeedLevel[n] &= 3;
                }
                if (n == self->unk_5456 - 1)
                    self->mSpeedLevel[n] = self->unk_545a;
                n++;
            }
        }
    } else {
        int n;
        int i;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        self->unk_5456 = ((((u32)rnd >> 16) & 0x7fff) * 8 >> 15) + 1;
        for (i = 0; i < 8; i++) {
            u8 step;
            self->mPosX[n] = (0x10 + i * 0x20) << 12;
            self->mPosY[n] = -(5 << 12);
            self->unk_52ed[n] = 1;
            self->unk_51fd[n] = 0;
            rnd = RandomIntInternal(&data_0209d4b8);
            self->mSpeedLevel[n] = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
            if (self->unk_545a == self->mSpeedLevel[n]) {
                rnd = RandomIntInternal(&data_0209d4b8);
                step = ((((u32)rnd >> 16) & 0x7fff) * 2 >> 15) + 1;
                self->mSpeedLevel[n] += step;
                if (self->mSpeedLevel[n] >= 3)
                    self->mSpeedLevel[n] = 0;
            }
            if (n == self->unk_5456 - 1)
                self->mSpeedLevel[n] = self->unk_545a;
            n++;
        }
    }

    for (i = 0; i < 8; i++)
        self->unk_51ed[i] = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15;
    for (i = 0; i < 6; i++)
        self->unk_51f5[i] = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15;
    self->unk_5455++;
}
