// @symbol func_ov002_020c16ec
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
struct SurfaceInfo { char dummy; };


struct Obj {
    char pad554[0x554];
    int f554;   // 0x554
    int f558;   // 0x558
    int f55c;   // 0x55c
    char pad_to658[0x658 - 0x560];
    int f658;   // 0x658
    int f65c;   // 0x65c
    int f660;   // 0x660
    int f664;   // 0x664
    int f66c;   // 0x66c
    int f670;   // 0x670
};

extern int func_02037e38(unsigned int *p);
extern int func_02037e58(unsigned int *p);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(struct SurfaceInfo *s, struct Vector3 *out);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

void func_ov002_020c16ec(char *o, char *s)
{
    struct Vector3 v;
    *(int *)(o + 0x670) = func_02037e90((int *)(s + 4));
    *(int *)(o + 0x664) = func_02037e38((unsigned int *)(s + 4));
    *(int *)(o + 0x65c) = func_02037e48((unsigned int *)(s + 4));
    *(int *)(o + 0x660) = func_02037e68((unsigned int *)(s + 4));
    *(int *)(o + 0x66c) = func_02037e84((int *)(s + 4));
    if (*(int *)(o + 0x658) == 1)
        return;
    *(int *)(o + 0x658) = func_02037e58((unsigned int *)(s + 4));
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo *)(s + 4), &v);
    *(int *)(o + 0x554) = v.x;
    *(int *)(o + 0x558) = v.y;
    *(int *)(o + 0x55c) = v.z;
    *(short *)(o + 0x69a) = _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(o + 0x554), *(int *)(o + 0x55c));
}
