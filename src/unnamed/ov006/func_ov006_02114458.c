#include "types.h"
extern void func_ov006_02114738(char *o);

void func_ov006_02114458(char *o)
{
    int i;
    func_ov006_02114738(o);
    *(int *)(o + 0x114) = *(int *)(o + 8);
    *(int *)(o + 0x118) = *(int *)(o + 0xc);
    *(int *)(o + 0x20) = 0;
    *(int *)(o + 0x24) = 0;
    for (i = 0; i < 0x20; i++) {
        char *p = o + i;
        p[0x4c] = 0;
        p[0x6c] = 0;
        p[0xac] = 0;
        p[0x8c] = 0;
    }
    for (i = 0; i < 0x19; i++) {
        char *p = o + i;
        p[0xce] = 0;
    }
    o[0xcc] = 0;
    o[0xcd] = 0;
    o[0x38] = 0;
    o[0x39] = 0;
    *(int *)(o + 0x34) = 0;
    o[0x3a] = 0;
    o[0x3b] = 0;
    *(int *)(o + 0x3c) = 0;
    *(int *)(o + 0xf8) = -1;
    *(int *)(o + 0xfc) = 0;
    o[0x100] = 1;
    *(int *)(o + 0x104) = 0;
    *(int *)(o + 0x40) = 0;
    o[0x110] = 0;
    *(int *)(o + 0x11c) = 0;
    o[0x120] = 0;
    o[0x121] = 1;
    if ((*(int *)(*(int *)(o + 4) + 8) & 0xff) && *(int *)(o + 0x2c) >= 9)
        o[0x121] = 0;
    else
        o[0x121] = 1;
    *(int *)(o + 0x124) = 0;
    o[0x128] = 0;
    o[0x129] = 0;
}
