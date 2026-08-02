//cpp
#include "types.h"
// @symbol _ZN6Player18St_LevelEnter_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef struct RaycastGround {
    char pre[0x10];
    char result[0x34];
    int clsnY;
    char post[0x8];
} RaycastGround;

extern "C" {
int _ZN8SaveData16HasPlayerLostCapEv(void);
void _ZN6Player7SetAnimEji5Fix12IiEj(void* self, u32 anim, int a, int fix, u32 b);
int _ZNK6Player14GetBodyModelIDEjb(void* self, u32 a, int b);
void _ZN9Animation8SetFlagsEi(void* self, int flags);
void Player_DisableInteraction(char* self);
void _ZN13RaycastGroundC1Ev(RaycastGround* self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, Vector3* pos, void* actor);
int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
int func_ov002_020c7cbc(char* self);
void func_02012790(int a);
void _ZN6Player16InitWingFeathersEb(void* self, int b);
void _ZN13RaycastGroundD1Ev(RaycastGround* self);
}

extern u8 data_0209f2d8;
extern s16 data_02082214[];
extern signed char data_0209f2f8;
extern u8 data_0209f264;
extern u8 data_0209f2fc;

int Player::St_LevelEnter_Init()
{
    RaycastGround rg;

    u32 anim = data_ov002_0210a7e8[mStateStep];
    if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
        if (anim == 0x85) anim = 0xa5;
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), anim, 0, 0x1000, 0);

    u8 st = mStateStep;
    if (st != 3 && st != 0xa && st != 0) {
        _ZN9Animation8SetFlagsEi(
            *(char**)((((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), (u8)param1, 0) * 4)) + 0xdc) + 0x50,
            0x40000000);
        if (mStateStep == 2) {
            char* p = *(char**)((((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), (u8)param1, 0) * 4)) + 0xdc) + 0x50;
            *(int*)(p + 8) = 0x14000;
        }
    }

    unk_719 = -1;
    Player_DisableInteraction(((char*)this));
    mJumpComboStage = 0;
    mHorzSpeed = 0;
    mStateArg = 0;
    mPrevAngleY = mAngleY;
    mStateWork = 0;
    _ZN13RaycastGroundC1Ev(&rg);

    switch (mStateStep) {
    case 0: {
        Vector3 pos;
        int hit;
        int y = mPosY;
        int z = mPosZ;
        int x = mPosX;
        int w = y + 0xc8000;
        pos.x = x;
        pos.y = w;
        pos.z = z;
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, ((char*)this));
        hit = 0x80000000;
        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) hit = rg.clsnY;
        mPosY = hit;
        mStateWork = 2;
        {
            int b = (data_0209f2d8 == 2);
            if (b == 0) mStateWaitTimer = 0x1e;
            else mStateWaitTimer = 0;
        }
        break;
    }
    case 1:
        mVertAccel = 0;
        mStateWork = 4;
        mIsUnderwater = 1;
        break;
    case 5:
    case 15:
        mStateArg = 1;
        break;
    case 11:
    case 12: {
        s32 idx;
        s16 cosv, sinv;
        if (func_ov002_020c7cbc(((char*)this)) != 0) mStateStep = 0x12;
        *(int*)(((long long)(int)((char*)&mPosY))) += 0x64000;
        idx = ((s32)(u16)(s16)(mAngleY + 0x8000) >> 4) * 2;
        cosv = data_02082214[idx];
        sinv = data_02082214[idx + 1];
        *(int*)(((long long)(int)((char*)&mPosX))) += (int)(((s64)0xa0000 * cosv + 0x800) >> 12);
        *(int*)(((long long)(int)((char*)&mPosZ))) += (int)(((s64)0xa0000 * sinv + 0x800) >> 12);
        mStateTimer = 0x20;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        break;
    }
    case 8:
    case 9:
        if (func_ov002_020c7cbc(((char*)this)) != 0) mStateStep = 0x10;
        mStateTimer = 2;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        break;
    case 13:
    case 17: {
        u8 st2 = mStateStep;
        if (st2 == 0x11) mStateTimer = 2;
        else mStateTimer = 0x20;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        if (data_0209f2f8 == 0x18) func_02012790(0x16);
        else if (data_0209f2f8 == 0x19) func_02012790(0x17);
        break;
    }
    case 2:
        if (param1 != 0) {
            mStateStep = 0xa;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a7e8[mStateStep], 0, 0x1000, 0);
        } else {
            _ZN6Player16InitWingFeathersEb(((char*)this), 0);
            mStateWork = 3;
            mVertSpeed = 0;
            mVertAccel = 0;
        }
        break;
    default:
        break;
    }

    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    {
        u8 c2 = data_0209f264;
        if (data_0209f2f8 == 2) {
            if (c2 == 0 || (c2 == 0xe && data_0209f2fc != 1)) {
                func_02012790(0x1d);
            }
        } else if (data_0209f2f8 == 1 && (c2 == 6 || c2 == 7 || (u32)(c2 - 9) <= 1)) {
            func_02012790(0x1d);
        }
    }

    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
