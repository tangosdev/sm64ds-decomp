// NONMATCHING: register colouring + instruction scheduling (14 of 330 words differ under
// 2004/b56). Size is exact (0x528), the literal pool, every relocation slot, both stack
// spill slots and all branch offsets are exact, and the register-erased instruction
// multiset and the immediate-constant multiset are both IDENTICAL to the ROM -- the
// residue is entirely which register holds a value and where two independent instructions
// sit relative to each other, in two spots (the scripted-layout branch at +0x228..+0x24c
// and the random-placement store at +0x348..+0x368). Logic verified against the ROM
// instruction by instruction. Counts as decompiled, not matched.
//
// dScMgLuigi_c ("Wanted!" minigame) round setup, state slot 1. func_ov006_020f31dc calls
// this eight times per frame until the layout is finished; each call places one character
// on the 13x9 spawn grid and then bumps the cursor at self[0x5456]. Once the cursor reaches
// the stage's character count the done flag self[0x5455] is raised and four random
// wobble/scroll seeds are written to self[0x515c..0x5162].
//
// Stage index = *(u16 *)(self + 0x5174). Stages whose data_ov006_0213ce98[] entry is
// non-zero use the scripted-layout routine func_ov006_020f2224 instead (stages 0, 1, 2, 5
// and 9); everything else lands here.
//
// The `(t = RND(4))` form is a codegen scaffold: t is otherwise unused, but assigning
// through it is what makes mwccarm compute the store address after the RandomIntInternal
// call instead of holding it across the call.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u8 data_ov006_0213ce98[];
extern u16 data_ov006_0213cee0[];
extern u8 data_ov006_0213ce84[];
extern int data_0209d4b8;

extern void func_ov006_020f2224(char *self);
extern int RandomIntInternal(int *seed);

#define RND(n) (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * (n)) >> 15)

#pragma opt_dead_code off

void func_ov006_020f2790(char *self)
{
    int cur;
    int lim;
    int idx;
    int gx;
    int gy;
    int jx;
    int jy;
    int n;
    u8 *cell;
    u8 *pf;
    char *d;
    char *k;
    int t;
    int last;

    if (*(u8 *)(self + 0x5455) != 0)
        return;

    idx = *(u16 *)(self + 0x5174);
    if (data_ov006_0213ce98[idx] != 0) {
        func_ov006_020f2224(self);
        return;
    }

    lim = data_ov006_0213cee0[idx];
    cur = *(u8 *)(self + 0x5456);

    /* pick this slot's face, then nudge it off the wanted face so decoys never match */
    *(u8 *)(self + 0x5365 + cur) = (t = RND(4));
    if (*(u8 *)(self + 0x545a) == *(u8 *)(self + 0x5365 + cur)) {
        *(u8 *)(self + 0x5365 + cur) += RND(3) + 1;
        *(u8 *)(self + 0x5365 + cur) &= 3;
    }

    if (cur == 0) {
        /* the wanted character: anywhere on the grid except the centre 5x5 block */
        do {
            gx = RND(13);
            gy = RND(9);
        } while (gx >= 6 && gx <= 10 && gy >= 4 && gy <= 8);

        jx = jy = -2;
        jx += RND(5);
        jy += RND(5);

        if (data_ov006_0213ce84[idx] != 0) {
            *(int *)(self + 0x47f8) = (gx * 20 + (jx + 8)) << 12;
            *(int *)(self + 0x49d8) = (gy * 20 + (jy + 16)) << 12;
            *(u8 *)(self + 0x52ed) = 1;
            *(u8 *)(self + 0x51fd) = 0;
            *(u8 *)(self + 0x5365) = *(u8 *)(self + 0x545a);
            *(u8 *)(self + 0x5178 + gx * 9 + gy) = 1;
            *(u8 *)(self + 0x5456) += 1;
            return;
        } else {
            /* other layout: the wanted character takes the last slot, un-jittered, and
               slot 0 is then placed by the general path below */
            last = lim - 1;
            d = self + last * 4 + 0x4000;
            *(int *)(d + 0x7f8) = (gx * 20 + 8) << 12;
            *(int *)(d + 0x9d8) = (gy * 20 + 16) << 12;
            k = self + last;
            *(u8 *)(k + 0x5000 + 0x2ed) = 1;
            *(u8 *)(k + 0x5000 + 0x1fd) = 0;
            *(u8 *)(k + 0x5000 + 0x365) = *(u8 *)(self + 0x545a);
            *(u8 *)(self + 0x5178 + gx * 9 + gy) = 1;
        }
    }

    /* find a free cell: 100 random tries, then a sequential scan */
    n = 0;
    while (1) {
        if (n >= 100) {
            gx += 1;
            if (gx >= 12) {
                gy += 1;
                gx = 0;
                if (gy >= 11)
                    gy = 0;
            }
        } else {
            gx = RND(13);
            gy = RND(9);
        }
        cell = (u8 *)(self + gx * 9 + gy + 0x5178);
        if (*cell == 0) {
            {
                int jx2, jy2;
                jx2 = jy2 = -2;
                jx2 += RND(5);
                jy2 += RND(5);
                *(int *)(self + cur * 4 + 0x4000 + 0x7f8) = (gx * 20 + (jx2 + 8)) << 12;
                *(int *)(self + cur * 4 + 0x4000 + 0x9d8) = (gy * 20 + (jy2 + 16)) << 12;
            }
            *cell = 1;
            break;
        }
        n++;
    }

    *(u8 *)(self + cur + 0x52ed) = 1;
    *(u8 *)(self + cur + 0x51fd) = 0;
    *(u8 *)(self + 0x5456) += 1;

    pf = &data_ov006_0213ce84[idx];
    if (*(u8 *)(self + 0x5456) >= (*pf != 0 ? lim : lim - 1)) {
        /* on these stages the last-placed character is stacked 20px above the wanted one */
        if (idx == 0xe || idx == 0x10 || idx == 0x11 || idx == 0x13) {
            *(int *)(self + cur * 4 + 0x4000 + 0x7f8) = *(int *)(self + (lim - 1) * 4 + 0x4000 + 0x7f8);
            *(int *)(self + cur * 4 + 0x4000 + 0x9d8) = *(int *)(self + (lim - 1) * 4 + 0x4000 + 0x9d8) - 0x14000;
        }
        *(u8 *)(self + 0x5455) = 1;
        if (*pf != 0)
            *(u8 *)(self + 0x5456) = 1;
        else
            *(u8 *)(self + 0x5456) = lim;
    }

    if (*(u8 *)(self + 0x5455) == 0)
        return;

    *(u16 *)(self + 0x515c) = RND(16) << 12;
    *(u16 *)(self + 0x515e) = RND(16) << 12;
    *(u16 *)(self + 0x5160) = RND(16) << 12;
    *(u16 *)(self + 0x5162) = RND(16) << 12;
}
