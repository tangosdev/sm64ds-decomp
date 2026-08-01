// @symbol func_02035860
/* recovered: shared common types */
#include "common.h"
void func_02035860(char *o, struct Vector3 *src)
{
    char *base = *(char **)(o + 0x14);
    struct Vector3 *d1 = (struct Vector3 *)(((long long)(int)(base + 0x5c)));
    d1->x = src->x;
    d1->y = src->y;
    d1->z = src->z;
    struct Vector3 *d2 = (struct Vector3 *)(((long long)(int)(base + 0x68)));
    d2->x = src->x;
    d2->y = src->y;
    d2->z = src->z;
}
