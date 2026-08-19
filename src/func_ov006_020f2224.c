// NONMATCHING: register allocation + stack-spill scheduling. Compiles clean under
// 2004/b56 to 0x558 (342 words), five words short of the ROM's 0x56c (347 words). The
// control-flow graph is identical (four scripted-layout arms v==1/2/3/else plus the two
// trailing RND(2) seed loops and the done-flag bump), the RandomIntInternal call sequence
// is identical, and every immediate, shift and field offset matches; the divergence is
// pure codegen. The ROM keeps `self` in r8 for the whole function and spills the two
// loop-invariant inner-loop seeds (i0, x0) to the stack, reloading them at each outer-loop
// head; this candidate colours `self` into sl and schedules its spills differently, which
// nets the five-word difference. Logic verified instruction by instruction against the ROM
// across all four arms and the tail. This is the same documented regperm/coloring floor the
// sibling Wanted / minigame bodies (func_ov006_020f2790, func_ov006_0212a764) shipped
// NONMATCHING against. Counts as decompiled, not matched.
//
// dScMgLuigi_c ("Wanted!" minigame) SCRIPTED round layout. The round-state driver
// func_ov006_020f2790 delegates the whole layout to this in ONE call whenever
// data_ov006_0213ce98[stage] != 0, i.e. stages 0, 1, 2, 5 and 9 (Rounds 1, 2, 3, 6, 10);
// the other stages place one character per call through the random path in the driver.
// The layout switches on data_ov006_0213ce98[stage]:
//   v==1  stage 0 / Round 1: a 2x2 block of 4 characters, x in {0x70,0x90}, y in {0x50,0x70}
//         (Fix12), each face = (dir + k) & 3.  THIS is the first thing the player sees.
//   v==2  stage 1: a 4x4 grid of 16, x from 0x50 step 0x20, y from 0x30 step 0x20.
//   v==3  stages 2 and 5: 6 rows of 8 = 48, x and y from 0x10 step 0x20.
//   else  stage 9: 8 in a row, y = -0x5000 (start above the screen and drop in), x from 0x10.
// Per character k it writes position (self[0x47f8+k*4], self[0x49d8+k*4] as Fix12), the
// sprite-active byte self[0x52ed+k]=1, self[0x51fd+k]=0, and the face self[0x5365+k]; the
// wanted face (self[0x545a]) is forced onto the answer slot and self[0x5456] records which
// slot it is. It then seeds the two per-character wobble tables (self[0x51ed..0x51f4] and
// self[0x51f5..0x51fa]) with RND(2) and raises the done flag self[0x5455].
#pragma opt_strength_reduction off
#pragma opt_propagation off

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u8 data_ov006_0213ce98[];

void func_ov006_020f2224(char *self)
{
    int v = data_ov006_0213ce98[*(u16 *)(self + 0x5174)];

    if (v == 1) {
        int k = 0;
        u32 dir;
        int outer;
        int y;
        int zero;
        int one;
        int i0;
        int x0;

        dir = ((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 4) >> 15;
        outer = k;
        x0 = 0x70;
        *(u8 *)(self + 0x5456) = ((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 4) >> 15;
        y = 0x50;
        zero = outer;
        i0 = outer;
        one = 1;
        do {
            int ii = i0;
            int xx = x0;
            do {
                char *p;
                *(int *)(self + 0x47f8 + k * 4) = xx << 12;
                *(int *)(self + 0x49d8 + k * 4) = y << 12;
                p = self + k;
                *(u8 *)(p + 0x52ed) = one;
                *(u8 *)(p + 0x51fd) = zero;
                p = (char *)(((long long)(int)(p + 0x5365)) & 0xFFFFFFFFFFFFFFFFLL);
                *(u8 *)p = (dir + k) & 3;
                ii++;
                xx += 0x20;
                if (*(u8 *)(self + 0x545a) == *(u8 *)p)
                    *(u8 *)(self + 0x5456) = k + 1;
                k++;
            } while (ii < 2);
            outer++;
            y += 0x20;
        } while (outer < 2);
    } else if (v == 2) {
        int i = 0;
        int outer;
        int y;
        int zero;
        int one;
        int i0;
        int x0;
        char *p;

        *(u8 *)(self + 0x5456) = (((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 16) >> 15) + 1;
        outer = i;
        zero = outer;
        y = 0x30;
        i0 = zero;
        one = 1;
        x0 = 0x50;
        do {
            int ii = i0;
            int xx = x0;
            do {
                *(int *)(self + 0x47f8 + i * 4) = xx << 12;
                *(int *)(self + 0x49d8 + i * 4) = y << 12;
                p = self + i;
                *(u8 *)(p + 0x52ed) = one;
                *(u8 *)(p + 0x51fd) = zero;
                {
                    int r = ((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 4) >> 15;
                    p += 0x5365;
                    *(u8 *)p = r;
                }
                if (*(u8 *)(self + 0x545a) == *(u8 *)p) {
                    u8 d = (3 * (0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16))) >> 15;
                    *(u8 *)(((unsigned int)(self + i + 0x5365)) & 0xFFFFFFFFFFFFFFFF) += d + 1;
                    *(u8 *)(((unsigned int)(self + i + 0x5365)) & 0xFFFFFFFFFFFFFFFF) &= 3;
                }
                ii++;
                xx += 0x20;
                if (i == *(u8 *)(self + 0x5456) - 1)
                    *(u8 *)p = *(u8 *)(self + 0x545a);
                i++;
            } while (ii < 4);
            outer++;
            y += 0x20;
        } while (outer < 4);
    } else if (v == 3) {
        int i = 0;
        int outer;
        int y;
        int zero;
        int one;
        int i0;
        int x0;
        char *p;

        *(u8 *)(self + 0x5456) = ((0x30 * (0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16))) >> 15) + 1;
        outer = i;
        zero = outer;
        y = 0x10;
        i0 = zero;
        one = 1;
        x0 = y;
        do {
            int ii = i0;
            int xx = x0;
            do {
                *(int *)(self + 0x47f8 + i * 4) = xx << 12;
                *(int *)(self + 0x49d8 + i * 4) = y << 12;
                p = self + i;
                *(u8 *)(p + 0x52ed) = one;
                *(u8 *)(p + 0x51fd) = zero;
                {
                    int r = ((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 4) >> 15;
                    p += 0x5365;
                    *(u8 *)p = r;
                }
                if (*(u8 *)(self + 0x545a) == *(u8 *)p) {
                    u8 d = (3 * (0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16))) >> 15;
                    *(u8 *)(((unsigned int)(self + i + 0x5365)) & 0xFFFFFFFFFFFFFFFF) += d + 1;
                    *(u8 *)(((unsigned int)(self + i + 0x5365)) & 0xFFFFFFFFFFFFFFFF) &= 3;
                }
                ii++;
                xx += 0x20;
                if (i == *(u8 *)(self + 0x5456) - 1)
                    *(u8 *)p = *(u8 *)(self + 0x545a);
                i++;
            } while (ii < 8);
            outer++;
            y += 0x20;
        } while (outer < 6);
    } else {
        int i = 0;
        int cnt;
        int x;
        int zero;
        int fp0;
        int one;
        char *p;

        *(u8 *)(self + 0x5456) = (((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 8) >> 15) + 1;
        cnt = i;
        x = 0x10;
        zero = cnt;
        fp0 = cnt;
        one = 1;
        do {
            *(int *)(self + 0x47f8 + i * 4) = x << 12;
            *(int *)(self + 0x49d8 + i * 4) = -0x5000;
            p = self + i;
            *(u8 *)(p + 0x52ed) = one;
            *(u8 *)(p + 0x51fd) = fp0;
            {
                int r = ((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 3) >> 15;
                p += 0x5365;
                *(u8 *)p = r;
            }
            if (*(u8 *)(self + 0x545a) == *(u8 *)p) {
                u8 d = (((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 2) >> 15) + 1;
                *(u8 *)(((unsigned int)(self + i + 0x5365)) & 0xFFFFFFFFFFFFFFFF) += d;
                if (*(u8 *)(((unsigned int)(self + i + 0x5365)) & 0xFFFFFFFFFFFFFFFF) >= 3)
                    *(u8 *)p = zero;
            }
            cnt++;
            x += 0x20;
            if (i == *(u8 *)(self + 0x5456) - 1)
                *(u8 *)p = *(u8 *)(self + 0x545a);
            i++;
        } while (cnt < 8);
    }

    {
        int i = 0;
        do {
            *(u8 *)(self + 0x51ed + i) = ((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 2) >> 15;
            i++;
        } while (i < 8);
    }
    {
        int i = 0;
        do {
            *(u8 *)(self + 0x51f5 + i) = ((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 2) >> 15;
            i++;
        } while (i < 6);
    }
    {
        u8 *fin = (u8 *)(((long long)(int)(self + 0x5455)) & 0xFFFFFFFFFFFFFFFFLL);
        *fin += 1;
    }
}
