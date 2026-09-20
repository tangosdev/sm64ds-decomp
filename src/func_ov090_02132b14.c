// @symbol func_ov090_02132b14
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(const int* a, const int* b, int* dst);

extern struct Matrix4x3 data_020a0e68;

void func_ov090_02132b14(char* self){
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    *(struct Matrix4x3*)(self + 0x328) = data_020a0e68;
    *(int*)(self + 0x39c) = 0;
    *(int*)(self + 0x3a0) = 0;
    *(int*)(self + 0x3a4) = 0;
    data_020a0e68 = *(struct Matrix4x3*)(self + 0x328);
    MulMat4x3Mat4x3((const int*)(*(char**)(self + 0x320) + 0x90),
        data_020a0e68.m, data_020a0e68.m);
    {
        int* p9 = (int*)((unsigned long long)((int)(self) + 0x39c));
        int* p10 = (int*)((unsigned long long)((int)(self) + 0x3a0));
        int* p11 = (int*)((unsigned long long)((int)(self) + 0x3a4));
        *(int*)(self + 0x39c) = data_020a0e68.m[9];
        *(int*)(self + 0x3a0) = data_020a0e68.m[10];
        *(int*)(self + 0x3a4) = data_020a0e68.m[11];
        *p9 <<= 3;
        *p10 <<= 3;
        *p11 <<= 3;
    }
}
