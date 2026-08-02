//cpp
// @symbol _ZN6Player16InitWingFeathersEb
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020bd9ec(void* c, unsigned r1);
extern void func_ov002_020c43c4(void* c, int r1);
extern unsigned char data_0209f2d8[];
}

void Player::InitWingFeathers(bool b_)
{
    int b = (int)b_;

    int t0 = (data_0209f2d8[0] == 1);
    if (!t0) {
        if (param1 != 0) return;
    }
    func_ov002_020bdd2c(((char*)this));
    mHasWings = 1;
    *(short*)(((char*)this)+0x600+0xae) = 0x708;
    {
        int t1 = (data_0209f2d8[0] == 1);
        if (!t1) func_ov002_020bd9ec(((char*)this), 0x33);
        else func_ov002_020bd9ec(((char*)this), 0x4e);
    }
    if (b == 0) return;
    func_ov002_020c43c4(((char*)this), 1);
}
