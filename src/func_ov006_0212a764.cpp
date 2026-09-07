//cpp
// @symbol func_ov006_0212a764
/* recovered: dScMgFlower_c round setup, ov006 0x0212a764 (784 bytes). Called by
 * InitResources and again from OnYoshiTryEat when a round restarts. Clears the
 * cursor pair, frees the 0x20 OAM slot group, wipes all 0x16 petal records,
 * rolls the petal count (1-2 when the hold counter reads exactly 0x14, else a
 * 1-in-10 chance of 20-21, otherwise 8-15), then lays the petals out on a
 * circle: each gets angle = base + i * (full turn) / count, a (0, 3.0) offset
 * rotated by that angle through func_0203d388 and translated to (8.0, 6.0),
 * and is marked active. A shuffle pass then swaps angle and position between
 * each petal and a random partner, and the round state (held petal, toggle,
 * timers, face sprite, background phase, blend alpha) is reset.
 *
 * The petal record is addressed by raw offset off `this`, and that is
 * measured, not laziness. The cartridge forms every angle/position address as
 * `add rX, <this + i*0x20>, <pool 0x4f54/0x4f3c/0x4f40>` with a zero-offset
 * access, and keeps &angle (r5) and &pos.x (r4) across the two calls. Real
 * member access folds the constant into an `add #0x4f00` / `[#0x54]` pair,
 * keeps nothing across the calls, and the whole loop drifts (20 bytes short);
 * named pointers to the members get strength-reduced, or hoisted as
 * `this + 0x4f54` with a scaled index. Converting the element address to an
 * integer before adding the field offset -- `(unsigned int)(c + i * 0x20) +
 * 0x4f54` -- is the one spelling that keeps the constant out of the
 * addressing mode. It is a plain pointer-to-integer conversion, not the u64
 * mask or the long-long round trip. The tail members DO reproduce and are
 * named.
 *
 * Three more shapes are load-bearing: the two petal pointers `pa` and `px`
 * are block locals, `px` declared after the angle store (the cartridge
 * computes &angle before the divide and &pos.x after it, and colours them
 * r5/r4 ahead of the function-scope i2/ang/pang, which take r8/r7/r6 in
 * reverse declaration order); the divisor is the live petal count, so r1
 * stays occupied through the top of the loop and the 0x4f54 pool load lands
 * in r2; and the swap temp is a Vec2, whose empty destructor keeps its 8-byte
 * stack home (frame 0x1c) while the copy itself goes through registers --
 * see the header. */
#include "dScMgFlower_c.h"

extern "C" {
extern int RandomIntInternal(int *seed);
extern void func_0203d388(int *p, int angle);
extern void func_ov004_020b04d0(int n);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *reg, u16 a, u16 b, u16 c, u32 d);
extern int data_0209d4b8;
}

#pragma opt_strength_reduction off

extern "C" void func_ov006_0212a764(dScMgFlower_c *self)
{
    char *c = (char *)self;
    unsigned int r;
    int j;
    int i2;
    int ang;
    int *pang;
    int i;
    int angbase;

    self->mCursorX = 0;
    self->mCursorY = 0;
    self->mPrevCursorX = self->mCursorX;
    self->mPrevCursorY = self->mCursorY;
    func_ov004_020b04d0(0x20);
    for (i = 0; i < 0x16; i++) {
        *(u8 *)(c + i * 0x20 + 0x4f38) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f3a) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f39) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f3b) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f3c) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f40) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f4c) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f50) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f44) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f48) = 0;
    }
    if (self->mHoldTimer == 0x14) {
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        self->mPetalsLeft = ((r * 2) >> 15) + 1;
    } else {
        int raw = RandomIntInternal(&data_0209d4b8);
        r = ((unsigned int)raw >> 16) & 0x7fff;
        if (((r * 10) >> 15) == 0) {
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            self->mPetalsLeft = ((r * 2) >> 15) + 0x14;
        } else {
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            self->mPetalsLeft = ((r * 8) >> 15) + 8;
        }
    }
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    angbase = (r * 0x10000) >> 15;
    i2 = 0;
    if (self->mPetalsLeft > 0) {
        pang = (int *)(c + 0x4f3c);
        ang = 0;
        do {
            s16 *pa = (s16 *)((unsigned int)(c + i2 * 0x20) + 0x4f54);
            *pa = angbase + ang / self->mPetalsLeft;
            {
                s32 *px = (s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f3c);
                *px = 0;
                *(s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f40) = 0x30000;
                func_0203d388(pang, *pa);
                *px += 0x80000;
                *(s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f40) += 0x60000;
            }
            *(u8 *)((unsigned int)c + i2 * 0x20 + 0x4f38) = 1;
            i2++;
            ang += 0x10000;
            pang += 8;
        } while (i2 < self->mPetalsLeft);
    }
    j = 0;
    if (self->mPetalsLeft > 0) {
        do {
            int k;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            k = (self->mPetalsLeft * r) >> 15;
            if (k != j) {
                char *ej = c + j * 0x20;
                char *ek = c + k * 0x20;
                u16 t16;
                dScMgFlower_c::Vec2 tmp;
                t16 = *(u16 *)((unsigned int)ej + 0x4f54);
                *(u16 *)((unsigned int)ej + 0x4f54) = *(u16 *)((unsigned int)ek + 0x4f54);
                *(u16 *)((unsigned int)ek + 0x4f54) = t16;
                tmp.x = *(s32 *)((unsigned int)ej + 0x4f3c);
                tmp.y = *(s32 *)((unsigned int)ej + 0x4f40);
                *(s32 *)((unsigned int)ej + 0x4f3c) = *(s32 *)((unsigned int)ek + 0x4f3c);
                *(s32 *)((unsigned int)ej + 0x4f40) = *(s32 *)((unsigned int)ek + 0x4f40);
                *(s32 *)((unsigned int)ek + 0x4f3c) = tmp.x;
                *(s32 *)((unsigned int)ek + 0x4f40) = tmp.y;
            }
            j++;
        } while (j < self->mPetalsLeft);
    }
    self->mHeldPetal = -1;
    self->mPetalToggle = 0;
    self->mState = 0;
    self->mHintTimer = 0;
    self->mResultTimer = 0;
    self->mFaceSprite = 2;
    self->unk_5fcd = 0;
    self->mBgScrollPhase = 0;
    *(volatile u16 *)0x4000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4001050, 4, 8, 6, 0xa);
}
