//cpp
// @symbol _ZN6Player18St_TurnAround_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
extern Fix12 Player_ScaleByCharFactor(void* c, Fix12 a);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int _ZN6Player6IsAnimEj(void* c, u32 a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, s16 a5);
extern void Player_AdvanceAnims(void* c);

extern u16 data_0209f49e[];
extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern int data_ov002_021101b4[];
extern int data_ov002_0211013c[];
}

int Player::St_TurnAround_Main()
{
    int r5v;
    s16 angle;
    int sel;
    int raw;
    int tmp;

    raw = mDesiredAngleY;
    if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        angle = (s16)(raw + 0x8000);
        sel = (mHorzSpeed < 0) ? 0x1800 : 0x8000;
        r5v = func_ov002_020bf56c(((char*)this), sel);
        ApproachAngle((s16*)((char*)&mTargetAngleY), angle, 0x10, 0x1000, 0x200);
        tmp = Player_ScaleByCharFactor(((char*)this), 0x28000);
        _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), -tmp, r5v);
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }

    ApproachAngle((s16*)((char*)&mAngleY), mTargetAngleY, 8, 0x2000, 0x800);

    if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101e4);
    } else if (mIsAirborne != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);
    } else if (mHorzSpeed <= 0x12000) {
        if (!_ZN6Player6IsAnimEj(((char*)this), 0x45))
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x45, 0x40000000, 0x1000, 0);
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            func_ov002_020d3498(((char*)this));
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
    } else {
        func_ov002_020bf88c(((char*)this));
        mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2, (int)((char*)&mCamSpacePos), mHorzSpeed, 0);
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
