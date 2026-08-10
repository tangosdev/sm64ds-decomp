//cpp
#include "types.h"
// @symbol _ZN6Player20St_StomachSlide_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020bf90c(void* c);
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void func_ov002_020c06fc(void* c, u32 arg);
extern int func_ov002_020dd2f4(void* c);
extern void func_ov002_020c0364(void* c, u32 arg);
extern int func_ov002_020c0688(void* c);
extern int func_ov002_020e2ea0(void* c);
extern int _ZN6Player6IsAnimEj(void* c, u32 anim);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern void func_ov002_020c18b0(void* c, u32 a);
extern void func_ov002_020e25f0(void* c, int a);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN12CylinderClsn6UpdateEv(void* c);
extern void func_ov002_020dc560(void* c);
extern int func_ov002_020e0ccc(void* c, short* st);
extern Fix12i _ZN4cstd5atan2E5Fix12IiES1_(Fix12i a, Fix12i b);
extern void _Z15ApproachLinear2Rsss(short* cur, short target, short step);
extern void Player_AdvanceAnims(void* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern u8 data_0209ee90;
extern int data_ov002_021105bc[];
extern int data_ov002_0211031c[];
extern int data_ov002_0211013c[];
extern int data_ov002_021101b4[];
}

int Player::St_StomachSlide_Main()
{
    void* light0 = *(void**)((char*)&mHeldObj);
    if (light0 != 0) {
        int* p = (int*)((char*)light0 + 0xb0);
        *p |= 0x4000;
    }

    switch (unk_6e6) {
    case 0:
        func_ov002_020bf90c(((char*)this));
        if (mIsAirborne == 0) {
            mIsSlidingOnGround = 1;
            if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_021105bc)) {
                if (mHeldObj != 0) {
                    mHorzSpeed = 0;
                    goto case0_e0;
                }
            }
            {
                u32 arg = 0x4000;
                if (mSlideType == 0) arg = 0x8000;
                func_ov002_020c06fc(((char*)this), arg);
            }
            func_ov002_020dd2f4(((char*)this));
            if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_021105bc) &&
                (u16)(mStateFlags & 1)) {
                func_ov002_020c0364(((char*)this), 2);
            }
        }
    case0_e0:
        if (mHorzSpeed == 0) {
            if (mSlideType == 0) {
                mStateStep = 0;
                unk_6e6 = 2;
                goto end;
            }
            {
                u8* p = (u8*)((char*)&mSlideStoppedTimer);
                *p = (u8)(*p + 1);
            }
            if (mSlideStoppedTimer >= 0x1e) {
                mStateStep = 0;
                unk_6e6 = 2;
                goto end;
            }
        } else {
            mSlideStoppedTimer = 0;
        }

        if (func_ov002_020c0688(((char*)this))) {
            if (func_ov002_020e2ea0(((char*)this))) {
                return 1;
            }
            if (_ZN6Player6IsAnimEj(((char*)this), 0x40) && _ZN6Player12FinishedAnimEv(((char*)this))) {
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x43, 0x40000000, 0x1000, 0);
            }
            {
                u16 flags = *(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18);
                if ((flags & 1) || (flags & 2)) {
                    if (!(u16)(mStateFlags & 1)) {
                        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101fc);
                    }
                }
            }
            func_ov002_020c18b0(((char*)this), 0);
        } else {
            if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211031c)) {
                unk_6e6 = 1;
                mStateStep = 0;
                if (*(int*)((char*)&data_0209ee90 + 0x244) != 0) {
                    if ((mStateArg | mSlideType) != 0) {
                        mVertSpeed = 0x15000;
                    }
                }
                func_ov002_020e25f0(((char*)this), 0);
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x54, 0x40000000, 0x1000, 0);
            }
        }
        func_ov002_020dba0c(((char*)this));
        _ZN12CylinderClsn5ClearEv((char*)&mAttackClsn);
        _ZN12CylinderClsn6UpdateEv((char*)&mAttackClsn);
        goto end;

    case 1:
        func_ov002_020dba0c(((char*)this));
        _ZN12CylinderClsn5ClearEv((char*)&mAttackClsn);
        _ZN12CylinderClsn6UpdateEv((char*)&mAttackClsn);
        if (mIsAirborne == 0) {
            unk_6e6 = 0;
            mVertSpeed = 0;
            mStateStep = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x43, 0x40000000, 0x1000, 0);
            goto end;
        }
        func_ov002_020dc560(((char*)this));
        {
            u8* p = (u8*)((char*)&mStateStep);
            *p = (u8)(*p + 1);
        }
        if (mStateStep <= 0x1e) goto end;
        if (mPosY - mGroundY <= 0x1f4000) goto end;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);
        return 1;

    case 2:
        mAngleZ = 0;
        mAngleX = mAngleZ;
        if (mStateStep == 0) {
            void* light = *(void**)((char*)&mHeldObj);
            int haveLight = (light != 0);
            if (haveLight) {
                u32 animId = 0x18;
                int b1 = (*(int*)((char*)light + 0xb0) & 0x8000) != 0;
                if (b1) {
                    animId = 0x8b;
                    if (param1 == 2) {
                        int b2 = (*(u16*)((char*)light + 0xc) == 0xce);
                        if (b2) {
                            animId = 0x18;
                        }
                    }
                }
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), animId, 0x40000000, 0x1000, 0);
            } else {
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x3e, 0x40000000, 0x1000, 0);
            }
            mPrevAngleY = mAngleY;
            {
                u8* p = (u8*)((char*)&mStateStep);
                *p = (u8)(*p + 1);
            }
            goto end;
        }
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            if (func_ov002_020e0ccc(((char*)this), *(short**)((char*)&mHeldObj))) {
                return 1;
            }
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
        goto end;
    }

end:
    if (mIsAirborne != 0) {
        s16 v = _ZN4cstd5atan2E5Fix12IiES1_(mHorzSpeed >> 8, mVertSpeed >> 8) - 0x4000;
        if (v < 0) v = 0;
        if (v >= 0x2aaa) v = 0x2aaa;
        _Z15ApproachLinear2Rsss((short*)((char*)&mAngleX), v, 0x200);
    }
    Player_AdvanceAnims(((char*)this));
    mStateArg = mSlideType;
    return 1;
}
