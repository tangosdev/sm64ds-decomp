#include "types.h"
// @symbol _ZN7dWipe_c15SetBackwardTimeEj
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* dWipe_c::SetBackwardTime(u32 frames) -- vtable slot 3. Starts the opening
 * half of a wipe; the mirror of SetForwardTime.
 *
 * DO NOT REMOVE `param_2', AND DO NOT MAKE THIS A REAL dWipe_c:: METHOD.
 * MEASURED: the extra parameter arrives in r2 and is forwarded to the guard
 * call with zero instructions, which is what keeps the cached `type' in r3 as
 * in the ROM. A real method fixes the arity at one (`Ej'), the parameter cannot
 * exist, and three words miss. Eleven spellings tried; full record in
 * notes/scene-provenance.md. */
struct FaderBrightness;
extern int _ZN15FaderBrightness15SetBackwardTimeEj(struct FaderBrightness *self, u32 time, u32 c);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(u32 irqBits, void (*handler)(void));
extern void _ZN3IRQ10EnableIRQsEj(u32 irqBits);
extern void func_0202f58c(void *self);


struct dWipe_c {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0c;
    s32 state;      /* 0x10 */
    s32 type;       /* 0x14 */
    u32 unk18;      /* 0x18 */
    s32 wipeInterp; /* 0x1c */
    u32 wipeSpeed;  /* 0x20 */
    u32 wipeAccel;  /* 0x24 */
};

int _ZN7dWipe_c15SetBackwardTimeEj(struct dWipe_c *self, u32 param_1, u32 param_2)
{
    s32 type, state;

    type = self->type;
    if (type == 1) {
        return _ZN15FaderBrightness15SetBackwardTimeEj((struct FaderBrightness *)self, param_1, param_2);
    }

    state = self->state;
    if (state == 0 || state == 4) {
        if (param_1 == 0) {
            self->wipeSpeed = 0x200000;
            self->wipeAccel = 0;
        } else {
            type = (type == 0) ? 0x2d : 0x3c;
            self->wipeSpeed = 0x200000 / type;
            self->wipeAccel = ((s32)self->wipeSpeed << 1) / type;
            self->wipeSpeed = 0;
        }

        if (self->type == 2) {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209f604, 2);
            _ZN2GX10LoadBGPlttEPKvjj(data_0209f604, 0, 2);
            _ZN3GXS10LoadBGPlttEPKvjj(data_0209f604, 0, 2);
        } else {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_020926cc, 2);
            _ZN2GX10LoadBGPlttEPKvjj(data_020926cc, 0, 2);
            _ZN3GXS10LoadBGPlttEPKvjj(data_020926cc, 0, 2);
        }

        self->wipeInterp = 0;

        *(volatile u16 *)0x4000040 = 0x7f7f;
        *(volatile u16 *)0x4000044 = 0xc0;
        *(volatile u16 *)0x4001040 = 0x7f7f;
        *(volatile u16 *)0x4001044 = 0xc0;
        *(volatile u16 *)0x4000042 = 0x8080;
        *(volatile u16 *)0x4000046 = 0xc0;
        *(volatile u16 *)0x4001042 = 0x8080;
        *(volatile u16 *)0x4001046 = 0xc0;

        self->state = 1;
        func_0202f58c(self);

        {
            u16 ime = *(volatile u16 *)0x4000208;
            *(volatile u16 *)0x4000208 = 0;
            _ZN3IRQ13SetIRQHandlerEjPFvvE(2, func_0202f2c4);
            _ZN3IRQ10EnableIRQsEj(2);
            func_02053c10(1);
            if (ime != 0) {
                u16 dead = *(volatile u16 *)0x4000208;
                *(volatile u16 *)0x4000208 = 1;
                (void)dead;
            }
        }

        *((u8 *)self + 0xf) = 1;
        return 0;
    } else {
        if (state == 2) {
            return 1;
        }
        self->state = 1;
        return 0;
    }
}
