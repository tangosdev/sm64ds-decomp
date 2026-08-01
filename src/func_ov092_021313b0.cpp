//cpp
#include "types.h"
// @symbol func_ov092_021313b0
/* recovered: shared common types */
#include "common.h"
#define LA(p) (((long long)(int)(p)))

struct PathPtr {
    int GetNode(Vector3 &v, unsigned int i) const;
};
extern "C" s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);

extern "C" void func_ov092_021313b0(void *t) {
    char *c = (char*)t;
    Vector3 old;
    old.x = *(int*)(c+0x580);
    old.y = *(int*)(c+0x584);
    old.z = *(int*)(c+0x588);
    (*(int*)LA(c+0x57c))++;
    if (*(int*)(c+0x57c) >= *(int*)(c+0x578))
        *(int*)(c+0x57c) = 0;
    ((PathPtr*)(c+0x58c))->GetNode(*(Vector3*)(c+0x580), (unsigned int)*(int*)(c+0x57c));
    if (*(int*)(c+0x580) == old.x && *(int*)(c+0x588) == old.z) {
        *(int*)(c+0x568) = 1;
        return;
    }
    u16 a = (u16)Vec3_HorzAngle(&old, (Vector3*)(c+0x580));
    if (a >= 0x2000 && a < 0x6000) *(int*)(c+0x568) = 5;
    else if (a >= 0x6000 && a < 0xa000) *(int*)(c+0x568) = 3;
    else if (a >= 0xa000 && a < 0xe000) *(int*)(c+0x568) = 4;
    else *(int*)(c+0x568) = 2;
}
