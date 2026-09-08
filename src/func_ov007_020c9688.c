// @symbol func_ov007_020c9688
// recovered: ov007 scene table builder, ov007 0x020c9688 (768 bytes).
/* func_ov007_020c9688 -- ov007 0x020c9688, 0x300 bytes.
 *
 * Builds the packed runtime tables for one scene description. The context
 * filled in by func_ov007_020c9a2c carries two counted lists; the allocation
 * made by func_ov007_020c12ac carries the two output arrays.
 *
 * The first pass walks list A. Each list-A header word gives the number of
 * 0x14-byte records that follow it; a 6-byte output slot is allocated per
 * record and each record is folded into three halfwords -- a 32-bit key
 * (fields 0x6/0x7/0x8/0xc/0xd, plus 0x0, 0x2, 0x5, 0x9, 0xa and, unless
 * field 0x9 is 3, 0xb) and a 16-bit tail (fields 0x10, 0xe, 0xf).
 *
 * The second pass walks list B. Each 0xc-byte group header gives a member
 * count at +8; func_ov007_020c13a4 sizes the group, then every 0x18-byte
 * member contributes an entry pointer (index at +0) and a value (+2).
 *
 * ONE SPELLING IS LOAD BEARING, measured against the ROM on 2004/b56:
 * THE SECOND PASS WALKS WITH `p`, THE SAME CURSOR THE FIRST PASS USED.
 * Given its own local pointer the second pass is byte-identical except for a
 * three-way rotation of the callee-saved registers -- the cursor colours to
 * r7 and the two hoisted zeroes to sb and r8, where the ROM has the cursor in
 * sb and the zeroes in r8 and r7. Ten instructions, no other difference.
 * Reusing `p` puts all three where the ROM has them. That rotation survived
 * every declaration order of the block (all 120), the function-top ordering
 * climb in the near-miss note, both loop forms, the named-zero and
 * named-init spellings, and every integer width for the counters; only the
 * shared cursor moves it. */
#include "types.h"

typedef struct RecA {
    s16 f0;
    s16 f2;
    u8 f4;
    u8 f5;
    u8 f6;
    u8 f7;
    u8 f8;
    u8 f9;
    u8 fA;
    u8 fB;
    u8 fC;
    u8 fD;
    u8 fE;
    u8 fF;
    u16 f10;
    u16 f12;
} RecA;

typedef struct Entry {
    u16 *data;
    u32 count;
} Entry;

typedef struct Group {
    Entry **arr;
    u16 *vals;
    u32 f8;
    u32 fC;
} Group;

typedef struct Res {
    u32 f0;
    u8 *entries;
    u32 f8;
    u8 *groups;
} Res;

typedef struct Ctx {
    u8 pad[0x44];
    u32 *listA;
    u8 pad2[8];
    u32 *listB;
    u8 pad3[0x30];
} Ctx;

typedef struct Packed {
    u32 val;
    u16 ext;
} Packed;

extern void func_ov007_020c9a2c(Ctx *ctx, int a);
extern Res *func_ov007_020c12ac(u32 a, u32 b);
extern void *func_ov007_020c3df4(int a, u32 b);
extern void func_ov007_020c13a4(void *c, int r1, short r2, short r3);

Res *func_ov007_020c9688(int arg)
{
    Res *s;
    u32 i;
    u32 nA;
    u32 nB;
    u32 cnt;
    // Every other caller of func_ov007_020c3df4 passes a literal 0; this named local is deliberate (one of the near-miss row's nine zero-plumbing shapes).
    u32 heap = 0;
    Packed tmp;
    Ctx ctx;
    u32 j;
    u8 *p;

    func_ov007_020c9a2c(&ctx, arg);
    nA = *ctx.listA;
    nB = *ctx.listB;
    s = func_ov007_020c12ac(nA, nB);

    p = (u8 *)ctx.listA + 4;
    for (i = 0; i < nA; i++) {
        Entry *e;
        u8 *base;
        cnt = *p;
        p = p + 4;
        base = s->entries;
        e = (Entry *)(base + i * 8);
        e->data = (u16 *)func_ov007_020c3df4(heap, cnt * 6);
        e->count = cnt;
        for (j = 0; j < cnt; j++) {
            s16 h0;
            u32 key;
            u32 hi;
            u8 bE;
            u32 h10;
            u8 b9;
            u8 bA;
            u8 bB;
            u32 bF;
            RecA *r;
            s16 h2;
            u8 b5;
            r = (RecA *)p;
            key = ((u32)r->f8 << 8) | ((u32)r->f6 << 28) | ((u32)r->f7 << 29);
            hi = ((u32)r->fC << 14) | ((u32)r->fD << 30);
            p = p + 0x14;
            bF = r->fF;
            h10 = r->f10;
            b5 = r->f5;
            bB = r->fB;
            bA = r->fA;
            b9 = r->f9;
            bE = r->fE;
            h2 = r->f2;
            h0 = r->f0;
            if (key == 0x100 || key == 0x300) {
                if (b9 == 3) {
                    tmp.val = key | ((hi | ((h2 & 0xff) | (b5 << 25) | (b9 << 10) | (bA << 12))) | ((h0 & 0x1ff) << 16));
                } else {
                    tmp.val = key | ((hi | ((h2 & 0xff) | ((b5 << 25) | (bB << 13)) | (b9 << 10) | (bA << 12))) | ((h0 & 0x1ff) << 16));
                }
            } else {
                if (b9 == 3) {
                    tmp.val = key | ((hi | ((h2 & 0xff) | (b9 << 10) | (bA << 12))) | ((h0 & 0x1ff) << 16));
                } else {
                    tmp.val = key | ((hi | ((h2 & 0xff) | (bB << 13) | (b9 << 10) | (bA << 12))) | ((h0 & 0x1ff) << 16));
                }
            }
            tmp.ext = h10 | (bE << 10) | (bF << 12);
            *(u16 *)((u8 *)e->data + j * 6) = ((u16 *)&tmp)[0];
            *(u16 *)((u8 *)e->data + j * 6 + 2) = ((u16 *)&tmp)[1];
            *(u16 *)((u8 *)e->data + j * 6 + 4) = ((u16 *)&tmp)[2];
        }
    }

    {
        u32 k;
        Group *g;
        u32 m;
        u32 c2;
        p = (u8 *)ctx.listB + 4;
        for (k = 0; k < nB; k++) {
            c2 = *(p + 8);
            g = (Group *)(s->groups + k * 0x10);
            p = p + 0xc;
            func_ov007_020c13a4(g, c2, 0, 0);
            for (m = 0; m < c2; m++) {
                u8 *q0 = p;
                p = p + 0x18;
                g->arr[m] = (Entry *)((Entry *)s->entries + *(u16 *)q0);
                g->vals[m] = *(u16 *)(q0 + 2);
            }
        }
    }
    return s;
}
