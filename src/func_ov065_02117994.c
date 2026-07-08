typedef int Fix12i;
typedef short s16;

struct Vec3 { Fix12i x, y, z; };
struct Mtx43 { Fix12i a[12]; };

extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* self, void* sm, struct Mtx43* m, Fix12i fx, int t, unsigned int u);

extern struct Mtx43 MATRIX_SCRATCH_PAPER;

void func_ov065_02117994(char* self){
    struct Vec3 v;
    Vec3_Asr(&v, (struct Vec3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    if (*(unsigned char*)(self + 0x43c) == 1) {
        *(struct Mtx43*)(self + 0x31c) = MATRIX_SCRATCH_PAPER;
    } else {
        *(struct Mtx43*)(self + 0x380) = MATRIX_SCRATCH_PAPER;
    }
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
        *(Fix12i*)(self + 0x5c) >> 3,
        (*(Fix12i*)(self + 0x60) - 0x18000) >> 3,
        *(Fix12i*)(self + 0x64) >> 3);
    *(struct Mtx43*)(self + 0x3f0) = MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, self + 0x3c8, (struct Mtx43*)(self + 0x3f0), 0x32000, 0x258000, 0xf);
}
