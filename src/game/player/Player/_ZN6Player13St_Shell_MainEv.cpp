//cpp
#include "types.h"
// @symbol _ZN6Player13St_Shell_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_dBgPi.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_ov002_020eeca8(void* c, int arg);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* c);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int func_02037e58(u32* p);
extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern void func_ov002_020e25f0(char* c, int a);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void func_ov002_020e28d4(char* c, int a, int b);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int _ZN6Player6IsAnimEj(void* c, u32 anim);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern void Player_AdvanceAnims(char* c);

extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern u16 data_0209f49e[];
extern u16 data_0209f49c[];
extern int data_ov002_0211004c[];
extern int data_ov002_021104e4[];
}

int Player::St_Shell_Main()
{
    int flag = 0;
    void* res;

    if (func_02035638((u8*)((char*)&mMeshClsn))) {
        flag = 1;
        res = (void*)func_0203564c((int)((char*)&mMeshClsn));
        if (_ZNK5dBgPi9GetClsnIDEv(res) != -1) {
            void* a = _ZN8dActor_c10FindWithIDEj((u32)_ZNK5dBgPi9GetClsnIDEv(res));
            if (a) {
                u16 type = *(u16*)((char*)a + 0xc);
                if ((type == 0x14 ? flag : 0) || (type == 0x15 ? 1 : 0)) {
                    func_ov002_020eeca8((void*)((char*)&mMeshClsn), (int)((char*)this));
                    flag = 0;
                }
            }
        }
    }

    if (_ZNK10dBgCh_Actr8IsOnWallEv((void*)((char*)&mMeshClsn))) {
        int ang = _ZN4cstd5atan2E5Fix12IiES1_(mWallNormalX, mWallNormalZ);
        if (AngleDiff(ang, mPrevAngleY) > 0x6000) {
            if (func_02037e58((u32*)((char*)_ZNK10dBgCh_Actr13GetWallResultEv((void*)((char*)&mMeshClsn)) + 4)) != 1) {
                flag = 1;
            }
        }
    }

    if (flag) {
        func_ov002_020c1eb4(((char*)this), (s16)(mAngleY + 0x8000));
        return 1;
    }

    if (mStateStep == 0) {
        s16 target;
        int accel;
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            target = mDesiredAngleY;
            accel = 0x40000;
        } else {
            target = mAngleY;
            accel = 0x18000;
        }
        s16 angleSave = mPrevAngleY;
        ApproachAngle((short*)((char*)&mPrevAngleY), target, 8, 0x800, 0x100);
        mAngleY = mPrevAngleY;
        func_ov002_020cc660(((char*)this), angleSave);
        {
            int spd = mHorzSpeed;
            int step = 0x1100;
            if (spd > 0) step = 0x600;
            _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), accel, step);
        }
        func_ov002_020c04e0(((char*)this));
        if (mIsAirborne != 0) {
            (*(u8*)((char*)&mStateStep))++;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        {
            int off = data_020a0e40 * 0x18;
            if (*(u16*)((char*)data_0209f49e + off) & 2) {
                (*(u8*)((char*)&mStateStep))++;
                mVertSpeed = (mHorzSpeed >> 2) + 0x2a000;
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                func_ov002_020e25f0(((char*)this), 0);
                return 1;
            } else if (*(u16*)((char*)data_0209f49c + off) & 0x400) {
                if (func_ov002_020d674c(((char*)this))) {
                    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211004c);
                } else {
                    mStateStep = 0;
                    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021104e4);
                }
                return 1;
            }
        }
    } else {
        if (mIsAirborne == 0) {
            mStateStep = 0;
            unk_766 = 0x1000;
            unk_760 = unk_766;
        } else {
            unk_766 = 0;
            unk_760 = unk_766;
            func_ov002_020e28d4(((char*)this), 0x1800, 0x800);
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
            if (mVertSpeed >= 0) {
                mVertAccel = -0x8000;
                if (*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 2) {
                    mVertAccel = -0x3400;
                }
            } else {
                if ((*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 2) && mHasWings != 0) {
                    mVertAccel = -0x2000;
                    mTerminalVelocity = -0x25800;
                }
            }
        }
    }

    if (_ZN6Player12FinishedAnimEv(((char*)this)) && _ZN6Player6IsAnimEj(((char*)this), 0x35)) {
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x34, 0, 0x1000, 0);
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
