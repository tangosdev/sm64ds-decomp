//cpp
// @symbol _ZN6Player16St_DebugFly_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern void func_ov002_020c9e40(void *);

int Player::St_DebugFly_Init()
{
    func_ov002_020c9e40(((char *)this));
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mVertAccel = 0;
    return 1;
}
