//cpp
#include "types.h"
// @symbol _ZN6Player8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct C3;
typedef int (C3::*PMF)();
struct G_ee90 {
    char pad[0x26c];
    s32 flag26c;
};

#define LAU(p) (p)

extern "C" {
extern u16 DecIfAbove0_Short(u16 *p);
extern s16 GetAngleToCamera(u8 playerID);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 charID, u32 soundID, const Vector3 *pos);
extern int _ZN6Player11ChangeStateERNS_5StateE(void *self, Player::State *s);
extern int _ZN6Player7IsStateERNS_5StateE(void *self, Player::State *s);
extern void _ZN9Animation7AdvanceEv(void *self);
extern void func_02035684(void *p, s32 v);
extern void func_0203568c(void *p, s32 v);
extern void func_ov002_020ce8bc(void *self, s32 arg);
extern void func_ov002_020d6790(void *self);
extern void func_ov002_020d80d0(void *self);

extern u8 data_0209f250;
extern s8 data_0209f284;
extern u8 data_0209f2d8;
extern s8 data_0209f2f8;
extern s32 data_0209fc48;
extern u8 data_0209fc5c[];
extern s32 data_0209fc68;
extern u8 data_020a0e40;
extern G_ee90 data_0209ee90;

extern Player::State data_ov002_0210ffec;
extern Player::State data_ov002_021101b4;
extern Player::State data_ov002_0211031c;
extern Player::State data_ov002_02110334;
extern Player::State data_ov002_02110454;
extern Player::State data_ov002_021104b4;
extern Player::State data_ov002_021104e4;
extern Player::State data_ov002_021104fc;
extern Player::State data_ov002_02110514;
extern Player::State data_ov002_021105bc;
extern Player::State data_ov002_0211061c;
extern Player::State data_ov002_02110634;
extern Player::State data_ov002_021106f4;
}

int Player::Behavior()
{
    s32 r0;
    s32 scale;
    s32 r2;
    s32 temp;
    s32 mul;
    s32 fallFlag;
    volatile Vector3 v0;
    Vector3 v1;

    if (data_0209f2d8 == 1)
        r0 = 1;
    else
        r0 = 0;
    if (r0 != 0 && data_0209fc5c[mPlayerNo] == 0)
        return 1;

    scale = 0x1000;
    r2 = scale;
    if (mIsMega != 0) {
        scale = 0x3000;
        r2 = 0x4200;
    } else if (mIsBalloon != 0) {
        scale = 0x1400;
        r2 = 0x2000;
    }
    temp = (s32)(((s64)r2 * 0x32000 + 0x800) >> 12);
    func_0203568c(((char *)this) + 0x380, temp);
    func_02035684(((char *)this) + 0x380, temp);
    mMovingCylinderClsnWithPos.radius = scale * 0x28;
    mul = 0x96;
    if (_ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_021104e4)
        || _ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_02110514)
        || _ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_02110334)
        || _ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_0211031c)
        || _ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_021105bc)
        || _ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_0211061c)
        || _ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_02110634)) {
        mul = 0x5a;
    }
    mMovingCylinderClsnWithPos.height = mul * scale;

    if (mIsMega != 0)
        *(u32 *)LAU((char *)&mMovingCylinderClsnWithPos.flags) |= 0x10;
    else
        *(u32 *)LAU((char *)&mMovingCylinderClsnWithPos.flags) &= ~0x10;

    if (data_0209fc68 == 0) {
        if (data_0209fc48 != 0)
            r0 = 1;
        else
            r0 = 0;
        if (r0 == 0) {
            data_020a0e40 = 0;
            goto after_player_slot;
        }
    }
    data_020a0e40 = mPlayerNo;
after_player_slot:

    if (func_ov002_020c4188(((char *)this)) != 0)
        return 1;

    DecIfAbove0_Short((u16 *)((char *)&mStateTimer));
    DecIfAbove0_Short((u16 *)((char *)&mStateWaitTimer));
    DecIfAbove0_Short((u16 *)((char *)&mInvincibleTimer));
    DecIfAbove0_Short((u16 *)((char *)&mJumpComboTimer));
    DecIfAbove0_Short((u16 *)((char *)&unk_6aa));
    DecIfAbove0_Short((u16 *)((char *)&unk_6ac));
    DecIfAbove0_Short((u16 *)((char *)&unk_6b0));
    DecIfAbove0_Short((u16 *)((char *)&unk_6b2));
    DecIfAbove0_Short((u16 *)((char *)&unk_6b4));
    DecIfAbove0_Short((u16 *)((char *)&unk_6b6));
    DecIfAbove0_Short((u16 *)((char *)&unk_6b8));
    DecIfAbove0_Short((u16 *)((char *)&unk_6ba));
    DecIfAbove0_Short((u16 *)((char *)&unk_6bc));
    DecIfAbove0_Short((u16 *)((char *)&unk_6c4));
    DecIfAbove0_Short((u16 *)((char *)&unk_6c8));

    if (unk_6c8 == 1)
        data_0209f284 = 0;

    if (mIsControlDisabled == 0) {
        DecIfAbove0_Short((u16 *)((char *)&mBalloonTimer));
        if (DecIfAbove0_Short((u16 *)((char *)&unk_6be)) == 0)
            func_ov002_020d80d0(((char *)this));
        if (DecIfAbove0_Short((u16 *)((char *)&unk_6ae)) == 0) {
            func_ov002_020e032c(((char *)this));
            func_ov002_020bdef0(((char *)this));
            func_ov002_020bdd9c(((char *)this));
        }
        if (DecIfAbove0_Short((u16 *)((char *)&unk_6c2)) == 0)
            func_ov002_020bdd2c(((char *)this));

        if (unk_6a2 != mAreaId) {
            unk_6a2 = mAreaId;
            unk_6c6 = 0;
        }
        if (DecIfAbove0_Short((u16 *)((char *)&unk_6c6)) == 0 && mObjInMouth != 0)
            func_ov002_020d6790(((char *)this));
    }

    {
        s16 ang = mDesiredAngleY;
        s32 stride = 0x18;
        unk_6d4 = ang;
        mDesiredAngleY = *(s16 *)((char *)&data_0209f4a6 + data_020a0e40 * stride);
        if (data_0209fc68 == 0) {
            s16 add = GetAngleToCamera(0);
            s16 *p = (s16 *)LAU((char *)&mDesiredAngleY);
            *p = (s16)(*p + add);
        } else {
            s16 add = GetAngleToCamera(mPlayerNo);
            s16 *p = (s16 *)LAU((char *)&mDesiredAngleY);
            *p = (s16)(*p + add);
        }
    }

    func_ov002_020c2e78(((char *)this));
    func_ov002_020ca940(((char *)this));
    func_ov002_020d8158(((char *)this));

    {
        State *st = mState;
        if (*(void **)&st->mMain != 0)
            (this->*(st->mMain))();
    }

    if ((u16)(mStateFlags & 0x80) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_021104fc);
        *(u16 *)LAU((char *)&mStateFlags) &= ~0x80;
    }

    if (mIsNoControl != 0)
        fallFlag = 1;
    else
        fallFlag = 0;
    if (data_0209f2f8 == 0x1f
        && _ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_021104b4) != 0)
        fallFlag = 0;

    if (mIsAirborne == 0 || fallFlag != 0 || mPeakY < mPosY)
        mPeakY = mPosY;

    if (mIsAirborne == 0 && mIsFallScreaming != 0) {
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x2e, (const Vector3 *)((char *)&mCamSpacePosX));
        mIsFallScreaming = 0;
    }
    if (mIsAirborne != 0 && mIsFallScreaming == 0
        && _ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_02110454) == 0
        && (mPeakY - mPosY) > 0x47e000) {
        mIsFallScreaming = 1;
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x1d, (const Vector3 *)((char *)&mCamSpacePosX));
    }

    if (mIsInShallowWater != 0 && mIsUnderwater == 0 && mIsAirborne == 0)
        func_ov002_020ce8bc(((char *)this), mHorzSpeed);

    unk_6d6 = mAngleY;
    *(u16 *)LAU((char *)&mStateFlags) &= ~0x440;

    if (mHasWings != 0)
        mModelAnim4.Advance();

    unk_548 = mPosX;
    unk_54c = mPosY;
    unk_550 = mPosZ;
    func_ov002_020bf36c(((char *)this), ((char *)this) + 0x2d4);
    func_ov002_020bf13c(((char *)this));

    {
        s32 *p = (s32 *)LAU((int)&v0);
        s32 z = 0;
        p[0] = z;
        p[1] = z;
        {
            State *st = &data_ov002_0210ffec;
            p[2] = z;
            if (_ZN6Player7IsStateERNS_5StateE(((char *)this), st) != 0) {
                if (mIsMega != 0) {
                    s32 t = 0x12c000;
                    v0.y = -t;
                } else {
                    s32 t = 0x64000;
                    v0.y = -t;
                }
            } else if (_ZN6Player7IsStateERNS_5StateE(((char *)this), &data_ov002_021106f4) != 0) {
                v0.y = 0x64000;
            }
        }
    }
    v1.x = v0.x;
    v1.y = v0.y;
    v1.z = v0.z;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char *)this) + 0x2d4, &v1);
    func_ov002_020c2db8(((char *)this));

    if (mIsInShallowWater != 0)
        mGroundSoundType = 3;

    if (data_0209ee90.flag26c != 0) {
        data_0209ee90.flag26c = 0;
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_021101b4);
        mStateStep = 0;
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0210ffec);
    }

    func_ov002_020db704(((char *)this));
    func_ov002_020d869c(((char *)this));

    {
        u16 t = mInvincibleTimer;
        if (t != 0) {
            if (t == 1)
                *(u32 *)LAU((char *)&mMovingCylinderClsnWithPos.vulnFlags) |= 0x10000000;
            else
                *(u32 *)LAU((char *)&mMovingCylinderClsnWithPos.vulnFlags) &= ~0x10000000;
        }
    }

    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsnWithPos);
    if (mIsBodyClsnEnabled != 0)
        _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsnWithPos);

    if (data_0209fc68 == 0)
        data_020a0e40 = 0;
    else
        data_020a0e40 = data_0209f250;

    func_ov002_020e4bb8(((char *)this));

    {
        s32 r2 = unk_654;
        if (r2 != 0) {
            u8 ch = mUseAltBodyModel;
            char *t4 = (char *)(*(int *)(*(int *)(((char *)this) + ((ch << 2) + 3) * 4 + 0x154) + 0x14) + 0x90);
            if (r2 < 0)
                r2 = -r2;
            *(s32 *)(t4 + 0x2c) = r2 >> 3;
        }
    }

    data_0209f254 = 0;
    if (param1 == 3) {
        void *p = *(void **)(*(void **)((char *)&mHeldObjQueue));
        if (p != 0)
            data_0209f254 = *(u8 *)((char *)p + 0x428);
    }

    return 1;
}
