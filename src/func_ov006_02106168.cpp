//cpp
// @symbol func_ov006_02106168
/* dScMgPanel_c layout pass: reads the panel layout (x/y from the layout tables,
   face from the face table picked by Virtual8C), draws unk_4cc0 random picks
   through func_ov006_02106080, and retries the whole thing until
   func_ov006_02106664 accepts it, at least one panel face differs from its
   copy, and no two picks are the same panel.

   Register shape: the loop-1 counter and the loop-2 counter are DIFFERENT
   locals (i, j); a single `i` for both loops colours out of the callee-saved
   band (r5) no matter where it is declared. With them split, the declaration
   order i, v, cnt2, j, cnt1, pd, pe is what puts i and j on r7, v on sb, cnt2
   on r8, cnt1 on r6 and the ded0 table pointer on r5; every other slot for j
   rotates loop 2 or loop 1. The pd/pe pointer locals keep the byte tables
   hoisted above the do-loop and the [idx4] reload inside it. */
#include "dScMgPanel_c.h"

extern "C" {
int RandomIntInternal(int *seed);
extern int data_0209d4b8;
void func_ov006_02106080(dScMgPanel_c *self, int x);
int func_ov006_02106664(dScMgPanel_c *self);
extern u16 *data_ov006_0213dd4c[];
extern u16 *data_ov006_0213dd58[];
extern u8 *data_ov006_0213ded0[];
extern u8 *data_ov006_0213e070[];
}

extern "C" void func_ov006_02106168(dScMgPanel_c *self)
{
    int i;
    int v;
    int cnt2;
    int j;
    int cnt1;
    u8 **pd;
    u8 **pe;
    int mismatch, dup, a, b, k;
    do {
        self->unk_4fde = 0;
        for (i = 0; i < (cnt1 = self->unk_4cb8); i++) {
            int m = self->unk_4cbc - 4;
            int idx4 = self->unk_4cb4;
            self->unk_4cc4[i] = data_ov006_0213dd4c[m][i] << 12;
            self->unk_4d54[i] = data_ov006_0213dd58[m][i] << 12;
            pe = data_ov006_0213e070;
            pd = data_ov006_0213ded0;
            if (self->Virtual8C()) {
                self->unk_4f1e[i] = pd[idx4][i];
                self->unk_4f42[i] = pd[idx4][i];
            } else {
                self->unk_4f1e[i] = pe[idx4][i];
                self->unk_4f42[i] = pe[idx4][i];
            }
        }
        cnt2 = self->unk_4cc0;
        for (j = 0; j < cnt2; j++) {
            u32 rnd = (u32)RandomIntInternal(&data_0209d4b8);
            u32 s = (rnd >> 16) & 0x7fff;
            v = (cnt1 * s) >> 15;
            func_ov006_02106080(self, v);
            self->unk_4fae[self->unk_4fde] = v;
            self->unk_4fde++;
        }
        mismatch = 0;
        for (k = 0; k < self->unk_4cb8; k++) {
            if (self->unk_4f1e[k] != self->unk_4f42[k]) {
                mismatch++;
                break;
            }
        }
        dup = 0;
        for (a = 0; a < cnt2; a++) {
            for (b = a + 1; b < cnt2; b++) {
                if (self->unk_4fae[a] == self->unk_4fae[b]) {
                    dup++;
                    break;
                }
            }
        }
        if (dup != 0) mismatch = 0;
    } while (func_ov006_02106664(self) == 0 || mismatch == 0);
}
