#include "types.h"
// @symbol func_ov002_020de428
/* recovered: shared common types */
#include "common.h"
struct Actor {
    char pad74[0x74];
    struct Vector3 vec; /* 0x74 */
    char pad[0x70c - 0x80];
    unsigned char flag; /* 0x70c */
};

extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 soundID, const struct Vector3 *pos);

void func_ov002_020de428(struct Actor *thiz)
{
    if (thiz->flag)
        return;
    _ZN5Sound9PlayBank0EjRK7Vector3(0xbd, &thiz->vec);
    thiz->flag = 0x1e;
}
