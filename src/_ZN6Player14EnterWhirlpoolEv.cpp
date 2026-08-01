//cpp
// @symbol _ZN6Player14EnterWhirlpoolEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* _ZN6Player14EnterWhirlpoolEv @ 0x20c5cc8 (ov002) -- tail-call veneer to func_ov002_020c5dec (0x20c5dec) with r1=9.
 */
extern "C" {
extern void func_ov002_020c5dec(void*, int);
}

void Player::EnterWhirlpool()
{
    func_ov002_020c5dec(((void*)this), 9);
}
