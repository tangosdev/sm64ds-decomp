//cpp
// @symbol _ZN10KingBobOmb8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "KingBobOmb.h"
struct C; typedef int (C::*PMF)();
struct C { char pad[0x420]; PMF *pp; };


struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); };
struct Derived { char pad[0x2cc]; Base base; };

extern "C" {
extern int _ZN8dActor_c13DistToCPlayerEv(void *self);
extern void _ZN14BlendModelAnim7AdvanceEv(void *self);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *self, void *clsn);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, void *wmc, unsigned int flags);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern int KingBobOmb_SetState(void *c, void *p);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);

extern void *data_0209f318;
extern char data_ov078_0212707c[];
extern char data_ov078_0212703c[];
extern char data_ov078_021270fc[];
}

int KingBobOmb::Behavior()
{
    char *self = (char *)((C *)this);

    if (_ZN8dActor_c13DistToCPlayerEv(((C *)this)) < 0x1770000) {
        *(C **)((char *)data_0209f318 + 0x114) = ((C *)this);
    }

    if (*(void **)((char *)((C *)this)->pp + 8) != 0) {
        PMF *p = ((C *)this)->pp + 1;
        (((C *)this)->**p)();
    }

    {
        Base *b = &((Derived *)self)->base;
        mBlendModelAnim.speed = mAnimSpeed << 0xc;
        b->v3();
    }
    _ZN14BlendModelAnim7AdvanceEv(self + 0x2cc);

    if ((char *)((C *)this)->pp == data_ov078_0212707c) {
        void *r1 = *(void **)(self + 0x494);
        int b;
        if (r1 != 0) {
            b = (*(int *)(self + 0xb0) & 0x4000) != 0;
            if (b != 0 && *(int *)((char *)r1 + 0xc8) != 0) {
                goto skip_de0;
            }
        }
        func_ov078_02125de0(self);
    skip_de0:
        func_ov078_02125c98(self);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(self + 0x100));
    DecIfAbove0_Byte(&mTimer505);
    DecIfAbove0_Byte(&mTimer504);

    if ((char *)((C *)this)->pp != data_ov078_021270bc) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(self, self + 0x33c);
    } else {
        _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(self, self + 0x33c);
    }

    if ((char *)((C *)this)->pp != data_ov078_021270bc || *(unsigned char *)(self + 0x499) == 1) {
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(self, self + 0x110, 0);
    }

    if ((char *)((C *)this)->pp == data_ov078_0212703c || (char *)((C *)this)->pp == data_ov078_021270fc) {
        if (_ZNK10dBgCh_Actr8IsOnWallEv(self + 0x110) != 0
            || _ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x110) == 0
            || (mArenaPosY - 0x28000) > mPosY) {
            KingBobOmb_SetState(self, data_ov078_021270bc);
        }
    }

    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(self + 0x33c, &v);
    }
    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(self + 0x37c, &v);
    }
    _ZN5dCc_c5ClearEv(self + 0x33c);
    _ZN5dCc_c6UpdateEv(self + 0x33c);
    _ZN5dCc_c5ClearEv(self + 0x37c);
    _ZN5dCc_c6UpdateEv(self + 0x37c);

    func_ov078_02125de0(self);
    func_ov078_02125c98(self);
    return 1;
}
