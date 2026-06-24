typedef int Fix12i;
typedef short s16;

struct Vec3 { Fix12i x, y, z; };
struct Mtx43 { Fix12i a[12]; };

extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN9ModelBase12ApplyOpacityEj(void* self, unsigned int a, int b);
extern void MulMat4x3Mat4x3(struct Mtx43* dst, struct Mtx43* a, struct Mtx43* b);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* self, void* sm, struct Mtx43* m, Fix12i fx, int t, unsigned int u);

extern struct Mtx43 data_020a0e68;

void func_ov094_021361d8(char* self){
    struct Mtx43 out;
    struct Vec3 v;
    Vec3_Asr(&v, (struct Vec3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    _ZN9ModelBase12ApplyOpacityEj(self + 0x30c, *(unsigned char*)(self + 0x3e4), 1);
    *(struct Mtx43*)(self + 0x328) = data_020a0e68;
    MulMat4x3Mat4x3((struct Mtx43*)(*(char**)(self + 0x320) + 0x30),
        (struct Mtx43*)(self + 0x328), &out);
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(Fix12i*)(self + 0x5c) >> 3,
        (*(Fix12i*)(self + 0x60) - 0x38000) >> 3,
        *(Fix12i*)(self + 0x64) >> 3);
    *(struct Mtx43*)(self + 0x398) = data_020a0e68;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, self + 0x370, (struct Mtx43*)(self + 0x398), 0x64000, 0x320000, 0xf);
}
