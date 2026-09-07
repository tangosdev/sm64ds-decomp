//cpp
#include "types.h"
// @symbol _ZN6Player21St_JumpQuicksand_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern char data_ov002_02110424;

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char *, char *);
extern void Player_AdvanceAnims(char *);
}

int Player::St_JumpQuicksand_Main()
{
    u8 t = mStateWork;
    (*(u8 *)(int)((char *)&mStateWork))++;
    if (t < 6) {
        (*(int *)(int)((char *)&mSinkDepth)) -=
            (int)((((7 - mStateWork) << 12) * 0xcccLL + 0x800) >> 12);
        if (mSinkDepth < 0x1000)
            mSinkDepth = 0x1100;
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_02110424);
    }
    Player_AdvanceAnims(((char *)this));
    return 1;
}
