//cpp
// @symbol func_ov006_02106fdc
#include "dScMgPanel_c.h"

typedef void (dScMgPanel_c::*PMF)(int);
extern PMF data_ov006_02142840[];

extern "C" void func_ov006_021050bc(dScMgPanel_c* c);

extern "C" void func_ov006_02106fdc(dScMgPanel_c* c)
{
    u8 idx;
    int found;
    int i;
    func_ov006_021050bc(c);
    found = 0;
    c->unk_4fe9 = 1;
    for (i = 0; i < c->unk_4cb8; i++) {
        idx = c->unk_4efa[i];
        (c->*data_ov006_02142840[idx])(i);
        if (idx != 0)
            found++;
    }
    if (found == 0) {
        c->unk_4ca8 = 4;
        c->unk_4fdf = 1;
        c->unk_4ec4 = 0x40;
    }
}
