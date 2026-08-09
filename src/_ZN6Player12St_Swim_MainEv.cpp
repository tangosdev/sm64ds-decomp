//cpp
// @symbol _ZN6Player12St_Swim_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Swimming, per frame. Metal cap diverts to the metal-underwater state and
 * surfacing to the walking one. mStateStep selects the stroke phase; each
 * phase drives the stroke animation, the body model's animation block at
 * +0x50, and the forward impulse, with a kick sound on the frame the
 * animation crosses its trigger.
 */
#include "Player.h"
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern int _ZN6Player9GetHealthEv(void);
extern void func_ov002_020c5dec(void*, int);
extern int func_ov002_020cede0(void*);
extern int func_ov002_020cec2c(void*);
extern void func_ov002_020cd71c(void*);
extern void func_ov002_020cd550(void*);
extern unsigned int func_02022d44(unsigned int, unsigned int, int, int, int, void*);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern void* _ZNK12WithMeshClsn14GetFloorResultEv(void*);
extern void* func_02037dc4(void*);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int, int);
extern void Vec3_RotateYAndTranslate(int*, void*, short, int*);
extern void _Z14ApproachLinearRiii(int*, int, int);
extern int _ZN6Player6IsAnimEj(void*, unsigned int);
extern int _ZN6Player12FinishedAnimEv(void*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned char, int);
extern int func_ov002_020ce324(void*);
extern void func_ov002_020cd364(void*);
extern void func_ov002_020cd3e0(void*);
extern int func_ov002_020cedb0(void*, int);
extern void func_ov002_020cd39c(void*);
extern int _ZNK9Animation12WillHitFrameEi(void*, int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
extern void func_ov002_020cd2c4(void*);
extern void func_ov002_020cd308(void*);
extern void func_ov002_020dbf4c(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern void func_ov002_020bd928(void*, unsigned int);
extern void func_ov002_020bd8c0(void*, unsigned int);
extern void Player_ReleaseHeldActor(void*);
extern void func_ov002_020daa74(void*);
extern void func_ov002_020cd218(void*, int, void*, void*);
extern int func_ov002_020ceaf4(void*);
extern int func_ov002_020ce5f8(void*);
extern void Player_AdvanceAnims(void*);

extern char data_ov002_021106ac[];
extern char data_ov002_021101b4[];
extern unsigned char data_020a0e40;
extern unsigned short data_0209f49c;
extern unsigned short data_0209f49e;
extern char data_0209ee90[];

int Player::St_Swim_Main()
{
    int t4;
    volatile int pos[3];
    int* fr;
    int ang;
    int out[3];
    int vec[3];
    short yaw;
    int var_r2;
    unsigned short a4;
    int var_r6;
    int var_r1;
    int has;
    int* p;
    int* sub;
    unsigned int t5;


    if (mIsMetal != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_021106ac);
        return 1;
    }
    if (_ZN6Player9GetHealthEv() == 0) {
        func_ov002_020c5dec(this, 8);
        return 1;
    }
    if (func_ov002_020cede0(this) == 0) {
        mIsUnderwater = 0;
        _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_021101b4);
        return 1;
    }
    mPeakY = mPosY;
    if (func_ov002_020cec2c(this) != 0) return 1;

    t4 = (*(unsigned short*)((char*)&data_0209f49c + data_020a0e40 * 0x18)) & 0x802;
    unk_69e = mAngleZ;
    func_ov002_020cd71c(this);
    func_ov002_020cd550(this);

    if (mJumpComboTimer != 0) {
        int eid = 0xcd;
        int y, z;
        pos[0] = mPosX;
        y = mPosY;
        pos[1] = y;
        z = mPosZ;
        pos[2] = z;
        y = y + 0x32000;
        pos[1] = y;
        *(unsigned int*)&unk_630 = func_02022d44(*(volatile unsigned int*)(&unk_630), eid, pos[0], pos[1], z, 0);
    }

    if (mPrevVertSpeed == 0 && _ZNK12WithMeshClsn10IsOnGroundEv(&mMeshClsn) != 0) {
        fr = (int*)func_02037dc4((char*)_ZNK12WithMeshClsn14GetFloorResultEv(&mMeshClsn) + 4);
        ang = fr[1];
        if (ang < 0x720 && ang > 0x400 && mPosY >= unk_64c - 0xc8000) {
            int at;
            mPosY = unk_54c - 0x8000;
            at = _ZN4cstd5atan2E5Fix12IiES1_(fr[0], fr[2]);
            vec[0] = 0;
            vec[1] = 0;
            at = at + 0x8000;
            yaw = (short)at;
            vec[2] = (*(int*)((char*)data_0209ee90 + 0x244)) << 0xc;
            Vec3_RotateYAndTranslate(out, &mPosX, yaw, vec);
            mPosX = out[0];
            mPosY = out[1];
            mPosZ = out[2];
        }
    }

    switch (mStateStep) {
    case 0:
        mPrevAngleX = -0x4000;
        mPrevAngleZ = 0;
        mAngleX = 0;
        mAngleZ = 0;
        mAngleYSpeed = 0;
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), 0, 0x5000);
        if (_ZN6Player6IsAnimEj(this, 0xa8) == 0 && _ZN6Player12FinishedAnimEv(this) != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xa8, 0, 0x1000, 0);
        }
        if (mPrevVertSpeed == 0) {
            mStateStep = 1;
            mPrevAngleX = 0;
            mStateTimer = 0x1e;
        }
        break;
    case 1:
        var_r2 = 0x800;
        if (mStateTimer != 0) var_r2 = 0x200;
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), 0, var_r2);
        if (_ZN6Player6IsAnimEj(this, 0xa8) == 0) {
            if (_ZN6Player12FinishedAnimEv(this) != 0) {
                _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xa8, 0, 0x1000, 0);
            }
        } else if (mAngleX >= 0x1000) {
            sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x3000) {
                sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x3000;
            }
        } else if (mAngleYSpeed != 0) {
            sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x2000) {
                sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x2000;
            }
        } else {
            sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x1000) {
                sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x1000;
            }
        }
        if (func_ov002_020ce324(this) != 0) return 1;
        if ((*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18)) & 1) {
            func_ov002_020cd364(this);
        } else if (t4 != 0) {
            func_ov002_020cd3e0(this);
        }
        break;
    case 2:
        a4 = mStateTimer;
        if (a4 >= 0xa) var_r6 = 0x800;
        else if (a4 <= 4) var_r6 = 0x1800;
        else var_r6 = 0;
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), func_ov002_020cedb0(this, 0x10000), var_r6);
        if (mStateTimer < 0xa && t4 == 0) {
            *(unsigned char*)(((unsigned int)(char*)this + 0x6e5) & 0xFFFFFFFFFFFFFFFFULL) |= 1;
        }
        if (mStateTimer == 0 && _ZN6Player6IsAnimEj(this, 0xa9) != 0 && _ZN6Player12FinishedAnimEv(this) != 0) {
            if (!(mStateWork & 1)) {
                mStateStep = 3;
            } else {
                func_ov002_020cd39c(this);
            }
        }
        break;
    case 3:
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), func_ov002_020cedb0(this, 0xc000), 0x200);
        if (func_ov002_020ce324(this) != 0) return 1;
        if (t4 != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xa6, 0, 0x1000, 0);
            if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc) + 0x50), 0) != 0) {
                _ZN5Sound9PlayBank0EjRK7Vector3(0x15, &mCamSpacePosX);
            }
        } else {
            func_ov002_020cd39c(this);
        }
        break;
    case 4:
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), 0, 0x400);
        if (func_ov002_020ce324(this) != 0) return 1;
        if (mStateTimer == 0 && t4 != 0) {
            func_ov002_020cd3e0(this);
        }
        if (_ZN6Player12FinishedAnimEv(this) != 0 && _ZN6Player6IsAnimEj(this, 0xaa) != 0) {
            func_ov002_020cd2c4(this);
        }
        break;
    case 5:
        if (mStateTimer == 0) {
            func_ov002_020cd308(this);
        }
        break;
    case 6:
        func_ov002_020dbf4c(this);
        var_r1 = func_ov002_020cedb0(this, 0x10000);
        a4 = mStateTimer;
        if (a4 >= 5) var_r2 = 0x1000;
        else if (a4 == 0) { var_r2 = 0x800; var_r1 = 0; }
        else var_r2 = 0;
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), var_r1, var_r2);
        if (_ZN6Player12FinishedAnimEv(this) != 0) {
            has = (mHeldObj != 0) ? 1 : 0;
            if (has != 0) {
                mStateStep = 8;
                mStateTimer = 0xf0;
                p = (int*)(((mHeldObj + 0xb0) & 0xFFFFFFFFFFFFFFFFULL));
                *p |= 0x4000;
                _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xb0, 0, 0x1000, 0);
            } else {
                func_ov002_020cd2c4(this);
            }
        }
        _ZN12CylinderClsn5ClearEv(&mAttackClsn);
        sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
        t5 = ((unsigned int)(*(int*)((char*)sub + 8)) << 4) >> 0x10;
        if ((int)t5 >= 5 && (int)t5 <= 7) {
            _ZN12CylinderClsn6UpdateEv(&mAttackClsn);
        }
        break;
    case 7:
        if (_ZN6Player12FinishedAnimEv(this) != 0) {
            mStateStep = 8;
            mStateTimer = 0xf0;
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xb0, 0, 0x1000, 0);
            p = (int*)(((mHeldObj + 0xb0) & 0xFFFFFFFFFFFFFFFFULL));
            *p |= 0x4000;
            func_ov002_020bd928(this, 0x33);
            unk_6f7 = 1;
        }
        break;
    case 8:
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), func_ov002_020cedb0(this, 0x1e000), 0x400);
        if ((*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18)) & 1) {
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xb1, 0x40000000, 0x1000, 0);
            mStateStep = 9;
            func_ov002_020bd8c0(this, 0x33);
        }
        if (mStateTimer != 0) {
            has = (mHeldObj != 0) ? 1 : 0;
            if (has == 0) goto case8_block107;
        } else {
        case8_block107:
            Player_ReleaseHeldActor(this);
            func_ov002_020cd2c4(this);
            func_ov002_020bd8c0(this, 0x33);
            unk_6f7 = 0;
        }
        if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc) + 0x50), 0) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0x15, &mCamSpacePosX);
        }
        break;
    case 9:
        if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc) + 0x50), 0xd) != 0) {
            func_ov002_020daa74(this);
        }
        if (_ZN6Player12FinishedAnimEv(this) != 0) {
            func_ov002_020cd2c4(this);
        }
        break;
    }

    func_ov002_020cd218(this, mPrevVertSpeed, &mHorzSpeed, &mVertSpeed);
    {
        int* pa8 = (int*)(((unsigned int)(&mVertSpeed)) & 0xFFFFFFFFFFFFFFFFULL);
        *pa8 = *pa8 + func_ov002_020ceaf4(this);
    }
    if (func_ov002_020ce5f8(this) != 0) return 1;
    Player_AdvanceAnims(this);
    return 1;
}
}
