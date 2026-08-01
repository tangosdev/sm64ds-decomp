// @symbol func_ov007_020c30ac
/* recovered: shared common types */
#include "common.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
int func_ov007_020c30ac(struct Vector3 *arr, int count, struct Vector3 *target, int arg4, int *out)
{
    struct Vector3 diff; long long d2; int i; int bestIdx;
    long long best = 0x7fffffffffffffffLL;
    for (i = 0; i < count; i++) {
        SubVec3(arr, target, &diff);
        d2 = (long long)diff.x * diff.x + (long long)diff.y * diff.y + (long long)diff.z * diff.z;
        arr = (struct Vector3 *)((char *)arr + 0xc);
        if (d2 < best) { bestIdx = i; best = d2; }
    }
    if (arg4 < 0 || (long long)arg4 * arg4 >= best) {
        unsigned short ime = *(volatile unsigned short *)0x4000208;
        *(volatile unsigned short *)0x4000208 = 0;
        *(volatile unsigned short *)0x40002b0 = 1;
        *(volatile unsigned long long *)0x40002b8 = (unsigned long long)best << 2;
        *(volatile unsigned short *)0x4000208;
        *(volatile unsigned short *)0x4000208 = ime;
        if (out != 0) { while (*(volatile unsigned short *)0x40002b0 & 0x8000) ; *out = (*(volatile int *)0x40002b4 + 1) >> 1; }
        return bestIdx;
    }
    return -1;
}
