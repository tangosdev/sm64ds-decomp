//cpp
#include "types.h"
struct C; typedef void (C::*PMF)();
extern PMF data_ov002_0211075c[];
struct Obj { char pad[0x118]; void *f118; };
extern Obj *data_0209f318;
extern "C" void Player_AdvanceAnims(C *c);

struct C { char pad[0x10000]; };

extern "C" int func_ov002_020c6f3c(C *c)
{
    u8 *f = (u8 *)c;
    switch (f[0x6e3]) {
    case 8: case 9: case 11: case 12: case 16: case 18:
        data_0209f318->f118 = c;
        break;
    case 10: case 13: case 14: case 15: case 17:
    default:
        break;
    }
    (c->*data_ov002_0211075c[f[0x6e5]])();
    if (f[0x70c] == 0) Player_AdvanceAnims(c);
    return 1;
}
