#include "types.h"
struct Actor {
    char pad[0x49e];
    unsigned char obj; /* 0x49e */
};

extern int _ZN5Actor13SpawnSoundObjEj(struct Actor *thiz, u32 id);

void func_ov002_020e7e24(struct Actor *thiz)
{
    if (thiz->obj != 0xff)
        return;
    if (_ZN5Actor13SpawnSoundObjEj(thiz, 6))
        thiz->obj = 0x78;
}
