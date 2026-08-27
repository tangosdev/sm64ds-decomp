// @symbol func_ov007_020c2390
/* recovered: shared common types */
#include "common.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void NormalizeVec3(struct Vector3 *a, struct Vector3 *b);

void func_ov007_020c2390(char *t)
{
    struct Vector3 v;
    SubVec3((struct Vector3 *)(t + 4), (struct Vector3 *)(t + 0x10), &v);
    NormalizeVec3(&v, &v);
    *(short *)(t + 0x1c) = v.x;
    *(short *)(t + 0x1e) = v.y;
    *(short *)(t + 0x20) = v.z;
    if (*(short *)(t + 0x1c) > 0xfff) *(short *)(t + 0x1c) = 0xfff;
    if (*(short *)(t + 0x1e) > 0xfff) *(short *)(t + 0x1e) = 0xfff;
    if (*(short *)(t + 0x20) > 0xfff) *(short *)(t + 0x20) = 0xfff;
}
