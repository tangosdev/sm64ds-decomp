// @symbol func_ov002_020f6f48
/* recovered: shared common types */
#include "common.h"
typedef int s32;
typedef short s16;
typedef long long s64;

extern s16 Vec3_VertAngle(const struct Vector3* v1, const struct Vector3* v0);
extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern void ApproachAngle(s16* target, s16 cur, int a, int step, int flag);

void func_ov002_020f6f48(char* c, struct Vector3* v, int amt)
{
    int* p;
    ApproachAngle((s16*)(c+0x8c), Vec3_VertAngle((struct Vector3*)(c+0x5c), v), amt, 0x4000, 0);
    ApproachAngle((s16*)(c+0x8e), Vec3_HorzAngle((struct Vector3*)(c+0x5c), v), amt, 0x4000, 0);
    p = (int*)(*(char**)(c+0xe0) + 0x1c);
    *(int*)(c+0xa4) = (int)(((s64)*(int*)(c+0xf4) * *(int*)((char*)p + 0x18) + 0x800) >> 12);
    *(int*)(c+0xa8) = (int)(((s64)*(int*)(c+0xf4) * *(int*)((char*)p + 0x1c) + 0x800) >> 12);
    *(int*)(c+0xac) = (int)(((s64)*(int*)(c+0xf4) * *(int*)((char*)p + 0x20) + 0x800) >> 12);
}
