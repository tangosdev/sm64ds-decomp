#include "types.h"
// @symbol func_ov002_020e84ec
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
struct Flags { unsigned short b0 : 1; };

extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void* m, int angle);

void func_ov002_020e84ec(char* self)
{
    struct Vector3 v;
    s16* ang;
    int t;

    if (*(int*)(self + 0x43c) == 8) {
        Vec3_Asr(&v, (struct Vector3*)(self + 0x5c), 3);
        Matrix4x3_FromTranslation(self + 0x328, v.x, v.y, v.z);
    } else if (*(void**)(self + 0xc8) != 0) {
        *(struct Matrix4x3*)(self + 0x328) = *(struct Matrix4x3*)(*(char**)(self + 0xc8));
    } else if (!((struct Flags*)(self + 0x4a2))->b0) {
        ang = (s16*)((int)(self + 0x8e));
        t = *ang + 0xc00;
        *ang = t;
        Matrix4x3_FromRotationY(self + 0x328, *(s16*)(self + 0x8e));
        *(int*)(self + 0x34c) = *(int*)(self + 0x5c) >> 3;
        *(int*)(self + 0x350) = *(int*)(self + 0x60) >> 3;
        *(int*)(self + 0x354) = *(int*)(self + 0x64) >> 3;
    } else {
        Matrix4x3_FromRotationY(self + 0x328, *(s16*)(self + 0x94));
        *(int*)(self + 0x34c) = *(int*)(self + 0x5c) >> 3;
        *(int*)(self + 0x350) = (*(int*)(self + 0x60) + 0x32000) >> 3;
        *(int*)(self + 0x354) = *(int*)(self + 0x64) >> 3;
    }

    *(struct Matrix4x3*)(self + 0x38c) = *(struct Matrix4x3*)(self + 0x328);
    func_ov002_020e8398(self);
}
