//cpp
// @symbol _ZN6Player21St_JumpQuicksand_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern char data_ov002_02110424;

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char *, char *);
extern void func_ov002_020bedd4(char *);
}

int Player::St_JumpQuicksand_Main()
{
    u8 t = mStateWork;
    (*(u8 *)(int)(((long long)(int)((char *)&mStateWork))))++;
    if (t < 6) {
        (*(int *)(int)(((long long)(int)((char *)&mSinkDepth)))) -=
            (int)((((7 - mStateWork) << 12) * 0xcccLL + 0x800) >> 12);
        if (mSinkDepth < 0x1000)
            mSinkDepth = 0x1100;
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_02110424);
    }
    func_ov002_020bedd4(((char *)this));
    return 1;
}
