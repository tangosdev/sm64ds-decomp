//cpp
#include "types.h"
// @symbol _ZN6Player16St_DebugFly_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" int func_ov002_020bf224(int a, int b, int c);
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(char *self, void *st);
extern "C" void Player_AdvanceAnims(char *self);

extern u8 data_020a0e40;
extern char data_0209f4a0[];
extern char data_0209f49c[];
extern char data_020a0e5a[];
extern int data_ov002_0211013c;

int Player::St_DebugFly_Main()
{
    u32 idx;
    u16 flags;

    mHorzSpeed = 0;
    mVertSpeed = 0;
    mClsnFlags = 0;

    if (*(s16 *)(data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        mPrevAngleY = mDesiredAngleY;
        mHorzSpeed = func_ov002_020bf224((int)((char *)this), 0x50000, 0);
    }

    idx = data_020a0e40;
    flags = *(u16 *)(data_0209f49c + idx * 0x18);
    if (flags & 2) {
        *(int *)((char *)&mPosY) += 0x28000;
    } else if (flags & 1) {
        *(int *)((char *)&mPosY) -= 0x28000;
    }

    if (*(u16 *)(data_020a0e5a + idx * 4) & 0x400) {
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
    }

    Player_AdvanceAnims(((char *)this));
    return 1;
}
