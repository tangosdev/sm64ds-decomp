//cpp
// @symbol func_ov006_02121fa4
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgTrampoline_c.h"

extern "C" {
// recovered name: dScMgTrampoline_c_OnYoshiTryEat_02121fa4
/* recovered: renamed to Class_Method */
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *reg,
    unsigned short a, unsigned short b, int c, unsigned short d);
extern void func_ov006_020d0b04(int a);
extern int GetGameLanguage(void);
extern int RandomIntInternal(int *seed);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);

extern volatile short data_020a0dbc[];
extern int data_0209e650;

void func_ov006_02121fa4(char *o)
{
    struct dScMgTrampoline_c *self = (struct dScMgTrampoline_c *)(void *)o;
    volatile unsigned short fill;
    int q;

    _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4000050, 1, 0x3e, 0x10, 0x10);

    self->mTouchStartX = data_020a0dbc[0];
    self->mTouchStartY = data_020a0dbc[1];
    self->mTouchX = data_020a0dbc[0];
    self->mTouchY = data_020a0dbc[1];

    data_ov006_02142f60 = 0;
    self->unk_0bc = 0;
    if ((unsigned int)self->unk_0bc > 0x270e)
        self->unk_0bc = 0x270e;
    /* unk_0bc is u32 on dScMgBase_c (dScMg3DEsp_c's evidence). This function
       divides and takes a remainder of it, where signedness IS codegen -- it
       reads the field SIGNED, which is what this header used to declare
       locally. Cast at the use site rather than retype the shared base. */
    q = ((((s32)self->unk_0bc % 5) << 12)) / 4;
    self->mScrollY =
        (int)(((long long)(0x1000 - q) * 0x20 + 0x800) >> 12) +
        (int)(((long long)q * 0x50 + 0x800) >> 12);
    self->mScrollTargetY = self->mScrollY;
    self->mScrollHoldTimer = 0;
    func_ov006_020cd424(self->unk_0bc, self->mScrollY);

    func_ov006_020d0b04(self->unk_0bc);
    func_ov006_02120ca0();
    func_ov006_020c8a9c(0, data_ov006_0213fb18[GetGameLanguage()]);

    func_ov006_02120a44(o + 0x5d84);

    self->mInputEnabled = 0;
    self->unk_5dc2 = 0;
    self->unk_5dbc =
        (((int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13)
            * 0x2d0) >> 12) + 0x2d0;
    self->unk_5dbe = 0;
    self->unk_5dc0 = 1;
    self->mArrow1X = 0;
    self->mArrow2X = 0;

    func_ov006_02121750(o, 0);

    {
        char *dst = (char *)func_02054d88();
        fill = 0;
        MultiStore16(fill, dst, 0x6000);
    }

    func_ov006_02121f04(o);
}
}
