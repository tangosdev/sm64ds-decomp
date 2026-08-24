// @symbol func_ov006_02126b4c
// NONMATCHING: register allocation (div=64 of 230 words). Logic verified against
// the ROM instruction by instruction, checked mechanically rather than by eye:
// 0 shape divergences over all 230 words -- every divergent word decodes to the
// same mnemonic with the same operand shape, and the register substitution is a
// consistent bijection within each of the two arms, which mwccarm colours
// independently:
//     arm which==1  [+0x000,+0x1c4)  14 words, a 2-cycle  r8<->sl
//     arm else      [+0x1c4,+0x398)  50 words, a six-way permutation
// Declaration order, scope depth, use-count boosters and coalescing copies are
// all inert here (2187 scope/order configurations measured, bitwise identical),
// which is the signature of an allocator decision the source cannot address.
// Counts as decompiled, not matched.
#include "types.h"

extern void MultiStore16(u16 val, char *dst, int nbytes);
extern u16 *_ZN2G212GetBG2ScrPtrEv(void);
extern u16 *_ZN3G2S12GetBG2ScrPtrEv(void);

typedef struct Quad { u16 a, b, c, d; } Quad;
extern Quad data_ov006_0212f3bc[];

#pragma opt_strength_reduction off
void func_ov006_02126b4c(char *o, int row, int which)
{
    if (which == 1) {
        char *dst;
        char *p = o + row * 2;
        int off = (row & 0xf) << 7;
        volatile u16 t0, t1, t2, t3, t4, t5, t6, t7;
        u16 v;
        int i = 0;
        int col = i;
        for (; i < 0x10; i++) {
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].a;
            dst = (char *)_ZN2G212GetBG2ScrPtrEv() + off + col * 2;
            t0 = v;
            MultiStore16(t0, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].b;
            dst = (char *)_ZN2G212GetBG2ScrPtrEv() + off + col * 2 + 0x2;
            t1 = v;
            MultiStore16(t1, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].c;
            dst = (char *)_ZN2G212GetBG2ScrPtrEv() + off + col * 2 + 0x40;
            t2 = v;
            MultiStore16(t2, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].d;
            dst = (char *)_ZN2G212GetBG2ScrPtrEv() + off + col * 2 + 0x42;
            t3 = v;
            MultiStore16(t3, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].a;
            dst = (char *)_ZN3G2S12GetBG2ScrPtrEv() + off + col * 2;
            t4 = v;
            MultiStore16(t4, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].b;
            dst = (char *)_ZN3G2S12GetBG2ScrPtrEv() + off + col * 2 + 0x2;
            t5 = v;
            MultiStore16(t5, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].c;
            dst = (char *)_ZN3G2S12GetBG2ScrPtrEv() + off + col * 2 + 0x40;
            t6 = v;
            MultiStore16(t6, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].d;
            dst = (char *)_ZN3G2S12GetBG2ScrPtrEv() + off + col * 2 + 0x42;
            t7 = v;
            MultiStore16(t7, dst, 2);
            p += 0x5c0;
            col += 2;
        }
        return;
    }
    {
        char *dst;
        char *p = o + row * 2;
        int off = (row & 0xf) << 7;
        volatile u16 t0, t1, t2, t3, t4, t5, t6, t7;
        u16 v;
        int i = 0;
        int col = i;
        for (; i < 0x10; i++) {
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].a;
            dst = (char *)_ZN2G212GetBG2ScrPtrEv() + 0x800 + off + col * 2;
            t0 = v;
            MultiStore16(t0, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].b;
            dst = (char *)_ZN2G212GetBG2ScrPtrEv() + 0x802 + off + col * 2;
            t1 = v;
            MultiStore16(t1, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].c;
            dst = (char *)_ZN2G212GetBG2ScrPtrEv() + 0x840 + off + col * 2;
            t2 = v;
            MultiStore16(t2, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].d;
            dst = (char *)_ZN2G212GetBG2ScrPtrEv() + 0x842 + off + col * 2;
            t3 = v;
            MultiStore16(t3, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].a;
            dst = (char *)_ZN3G2S12GetBG2ScrPtrEv() + 0x800 + off + col * 2;
            t4 = v;
            MultiStore16(t4, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].b;
            dst = (char *)_ZN3G2S12GetBG2ScrPtrEv() + 0x802 + off + col * 2;
            t5 = v;
            MultiStore16(t5, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].c;
            dst = (char *)_ZN3G2S12GetBG2ScrPtrEv() + 0x840 + off + col * 2;
            t6 = v;
            MultiStore16(t6, dst, 2);
            v = data_ov006_0212f3bc[*(u16 *)(p + 0x4f38)].d;
            dst = (char *)_ZN3G2S12GetBG2ScrPtrEv() + 0x842 + off + col * 2;
            t7 = v;
            MultiStore16(t7, dst, 2);
            p += 0x5c0;
            col += 2;
        }
    }
}
