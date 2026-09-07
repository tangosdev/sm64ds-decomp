#include "types.h"
struct dActor_c {
    char pad[0x49e];
    unsigned char obj; /* 0x49e */
};

extern int _ZN8dActor_c13SpawnSoundObjEj(struct dActor_c *thiz, u32 id);

void func_ov002_020e7e24(struct dActor_c *thiz)
{
    if (thiz->obj != 0xff)
        return;
    if (_ZN8dActor_c13SpawnSoundObjEj(thiz, 6))
        thiz->obj = 0x78;
}
