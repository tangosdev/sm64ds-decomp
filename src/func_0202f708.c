#include "types.h"
// @symbol func_0202f708
// recovered name: dWipe_c_SetForwardTime
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dWipe_c::SetForwardTime - recovered from vtable slot identity */
struct MyFader {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u8  unk0c;
    u8  unk0d;
    u8  unk0e;
    u8  flag0f;     /* 0xf */
    s32 state;      /* 0x10 */
    s32 type;       /* 0x14 */
    u32 unk18;      /* 0x18 */
    s32 interpVal;  /* 0x1c */
    s32 speed1;     /* 0x20 */
    s32 speed2;     /* 0x24 */
};

extern int _ZN15FaderBrightness14SetForwardTimeEj(struct MyFader *self, unsigned int frames);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int addr, unsigned int len);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *data, unsigned int offset, unsigned int size);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *data, unsigned int offset, unsigned int size);
extern void func_0202f58c(struct MyFader *self);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned int irq, void (*handler)(void));
extern void _ZN3IRQ10EnableIRQsEj(unsigned int irq);


int func_0202f708(struct MyFader *self, u32 frames)
{
    if (self->state == 0 || self->state == 2) {
        int t;
        u16 saved;

        if (self->type == 1) {
            return _ZN15FaderBrightness14SetForwardTimeEj(self, frames);
        }

        if (frames == 0) {
            self->speed1 = -0x200000;
            self->speed2 = 0;
        } else {
            int *p;
            t = (self->type == 0) ? 0x2d : 0x3c;
            self->speed1 = -0x200000 / t;
            self->speed2 = (-self->speed1 << 1) / t;
            p = (int *)(((int)self + 0x20) & 0xFFFFFFFFFFFFFFFFULL);
            *p = *p << 1;
        }

        if (self->type == 2) {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209f600, 2);
            _ZN2GX10LoadBGPlttEPKvjj(data_0209f600, 0, 2);
            _ZN3GXS10LoadBGPlttEPKvjj(data_0209f600, 0, 2);
        } else {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_020926c8, 2);
            _ZN2GX10LoadBGPlttEPKvjj(data_020926c8, 0, 2);
            _ZN3GXS10LoadBGPlttEPKvjj(data_020926c8, 0, 2);
        }

        self->interpVal = 0x200000;
        *(volatile u16 *)0x4000040 = 0x7f;
        *(volatile u16 *)0x4000044 = 0xc0;
        *(volatile u16 *)0x4001040 = 0x7f;
        *(volatile u16 *)0x4001044 = 0xc0;
        *(volatile u16 *)0x4000042 = 0x80ff;
        *(volatile u16 *)0x4000046 = 0xc0;
        *(volatile u16 *)0x4001042 = 0x80ff;
        *(volatile u16 *)0x4001046 = 0xc0;

        self->state = 3;
        func_0202f58c(self);

        saved = *(volatile u16 *)0x4000208;
        *(volatile u16 *)0x4000208 = 0;
        _ZN3IRQ13SetIRQHandlerEjPFvvE(2, func_0202f2c4);
        _ZN3IRQ10EnableIRQsEj(2);
        func_02053c10(1);
        if (saved != 0) {
            u16 tmp = *(volatile u16 *)0x4000208;
            *(volatile u16 *)0x4000208 = 1;
        }

        self->flag0f = 1;
        return 0;
    } else {
        if (self->state == 4) {
            return 1;
        }
        self->state = 3;
        return 0;
    }
}
