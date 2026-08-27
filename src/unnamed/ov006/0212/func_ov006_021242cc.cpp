//cpp
#include "types.h"
// @symbol func_ov006_021242cc
// recovered name: dScMgTrampoline2_c_OnYoshiTryEat_021242cc
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
typedef struct {
    u8 pad0[0xbc];
    u32 unkBC;
    u8 pad1[0x7ac8 - 0xc0];
    u32 unk7AC8;
    u8 pad2[0x7b88 - 0x7acc];
    u32 unk7B88;
    u8 pad3[0x7b94 - 0x7b8c];
    u32 unk7B94;
    u32 unk7B98;
    s16 unk7B9C;
    s16 unk7B9E;
    s16 unk7BA0;
    s16 unk7BA2;
    s16 unk7BA4;
    u8 pad4[2];
    s16 unk7BA8;
} T;

extern "C" void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *p, u16 a, u16 b, u16 c, u16 d);
extern "C" void func_ov006_020d0b04(u32 a);
extern "C" s32 GetGameLanguage(void);
extern "C" void MultiStore16(u16 val, char *dst, int nbytes);
extern "C" void func_ov006_02124228(T *self);

extern volatile s16 data_020a0dbc[];

extern "C" void func_ov006_021242cc(T *self)
{
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4000050, 1, 0x2e, 0x10, 0x10);

    self->unk7BA0 = data_020a0dbc[0];
    self->unk7BA2 = data_020a0dbc[1];
    self->unk7B9C = data_020a0dbc[0];
    self->unk7B9E = data_020a0dbc[1];

    self->unkBC = 0;
    if (self->unkBC > 0x270e)
        self->unkBC = 0x270e;

    func_ov006_020cad3c(0x1000);
    func_ov006_020cac9c(self->unkBC);
    data_ov006_021405bc = 1;
    func_ov006_020d0b04(self->unkBC);
    func_ov006_020cee5c(self->unkBC);
    func_ov006_020eeff0();
    func_ov006_02122b24();
    func_ov006_02120ca0();
    func_ov006_020c8a9c(0, data_ov006_0213fc20[GetGameLanguage()]);

    self->unk7BA4 = 0;
    self->unk7BA8 = 0;
    self->unk7B88 = 0;
    self->unk7B94 = 0;
    self->unk7B98 = 0;
    self->unk7AC8 = 0x14000;

    {
        volatile u16 z;
        char *dst = (char *)func_02054d88();
        z = 0;
        MultiStore16(z, dst, 0x6000);
    }

    func_ov006_02124228(self);
}
