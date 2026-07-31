//cpp
// @symbol _ZN6Player13InitFireYoshiEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef unsigned short u16;

extern "C" void _ZN6Player11ChangeStateERNS_5StateE(char *c, void *st);
extern int data_ov002_0211004c;

void Player::InitFireYoshi()
{
    u16 *p = (u16 *)(((int)((char *)this) + 0x6ce) & 0xFFFFFFFFFFFFFFFFLL);
    *p |= 0x1000;
    if (mObjInMouth == 0)
        return;
    _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211004c);
}
