//cpp
// @symbol _ZN6Player24St_BowserEarthquake_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern void _Z14ApproachLinearRiii(int* p, int value, int speed);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* state);
extern void func_ov002_020bedd4(char* self);
extern short data_02082214[];
extern char data_ov002_0211013c[];

int Player::St_BowserEarthquake_Main()
{
    if (mStateStep == 0) {
        _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), 0, 0x1800);
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
    } else {
        int t = mStateTimer;
        int m = t % 16;
        int idx = (unsigned short)(short)(m << 12) >> 4;

        mHorzSpeed = 0;

        int radius = 0x4000 + ((0x6000 - ((t << 12) / 8)) << 3);

        mVertSpeed = 0;

        int sinVal = data_02082214[idx * 2];

        if (sinVal >= 0) {
            mPosY = mGroundY + (int)(((long long)radius * sinVal + 0x800) >> 12);
        } else {
            mPosY = mGroundY - (int)(((long long)radius * sinVal + 0x800) >> 12);
        }

        if (mStateTimer == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
    }

    func_ov002_020bedd4(((char*)this));
    return 1;
}
