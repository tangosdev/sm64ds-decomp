//cpp
// @symbol _ZN6Player20St_InYoshiMouth_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
}

int Player::St_InYoshiMouth_Init()
{
    unsigned int r3;
    char *slot;
    unsigned int r1;

    func_ov002_020bdb50(((char *)this), 0);
    r3 = 0;
    mStateWork = (unsigned char)r3;
    unk_6e6 = (unsigned char)r3;
    slot = (char *)(((long long)(int)((char *)&mBodyClsnFlags)));
    r1 = *(unsigned int *)slot;
    r1 |= 2u;
    *(unsigned int *)slot = r1;
    mIsBodyClsnEnabled = (unsigned char)r3;
    return 1;
}
