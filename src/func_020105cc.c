// @symbol func_020105cc
/* recovered: shared common types */
#include "common.h"
/* func_020105cc at 0x020105cc
 * Plays a sound effect based on flags bitmask.
 * If flags & 0x380: play sound bank0 ID 0xa at camSpacePos.
 * Else if flags & 0x40: play sound bank0 ID 9 at camSpacePos.
 */

typedef unsigned int u32;
typedef int Fix12i;



struct Actor {
    char pad[0x74];
    struct Vector3 camSpacePos; /* 0x74 */
};

extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 soundID, const struct Vector3 *pos); /* 0x0201264c */

void func_020105cc(struct Actor *thiz, u32 flags)
{
    if (flags & 0x380)
    {
        _ZN5Sound9PlayBank0EjRK7Vector3(0xa, &thiz->camSpacePos);
        return;
    }
    if (flags & 0x40)
    {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, &thiz->camSpacePos);
        return;
    }
}
