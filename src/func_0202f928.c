#include "types.h"
// @symbol func_0202f928
// @emits dWipe_c_SetBackwardTime
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dWipe_c::SetBackwardTime - recovered from vtable slot identity */
/* The third parameter (param_2) is load-bearing: it arrives in r2 and is
 * forwarded to the guard call with zero instructions, which keeps r2 live
 * from entry to the call and forces the cached `type` into r3 as in the ROM.
 * Without it every spelling colors `type` to r2 (3-word regperm miss). */
struct FaderBrightness;
extern int _ZN15FaderBrightness15SetBackwardTimeEj(struct FaderBrightness *self, u32 time, u32 c);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(u32 irqBits, void (*handler)(void));
extern void _ZN3IRQ10EnableIRQsEj(u32 irqBits);
extern void func_0202f58c(void *self);


struct MyFader {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0c;
    s32 unk10;      /* 0x10 */
    s32 type;       /* 0x14 */
    u32 unk18;      /* 0x18 */
    s32 interpVal;  /* 0x1c */
    u32 unk20;      /* 0x20 */
    u32 unk24;      /* 0x24 */
};

int dWipe_c_SetBackwardTime(struct MyFader *self, u32 param_1, u32 param_2)
{
    s32 type, unk10;

    type = self->type;
    if (type == 1) {
        return _ZN15FaderBrightness15SetBackwardTimeEj((struct FaderBrightness *)self, param_1, param_2);
    }

    unk10 = self->unk10;
    if (unk10 == 0 || unk10 == 4) {
        if (param_1 == 0) {
            self->unk20 = 0x200000;
            self->unk24 = 0;
        } else {
            type = (type == 0) ? 0x2d : 0x3c;
            self->unk20 = 0x200000 / type;
            self->unk24 = ((s32)self->unk20 << 1) / type;
            self->unk20 = 0;
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

        self->interpVal = 0;

        *(volatile u16 *)0x4000040 = 0x7f7f;
        *(volatile u16 *)0x4000044 = 0xc0;
        *(volatile u16 *)0x4001040 = 0x7f7f;
        *(volatile u16 *)0x4001044 = 0xc0;
        *(volatile u16 *)0x4000042 = 0x8080;
        *(volatile u16 *)0x4000046 = 0xc0;
        *(volatile u16 *)0x4001042 = 0x8080;
        *(volatile u16 *)0x4001046 = 0xc0;

        self->unk10 = 1;
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
        if (unk10 == 2) {
            return 1;
        }
        self->unk10 = 1;
        return 0;
    }
}
