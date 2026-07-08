typedef int Fix12i;
typedef short s16;

struct Vec3 { Fix12i x, y, z; };
struct Mtx43 { Fix12i a[12]; };

extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(struct Mtx43* m, int x, int y, int z);

extern struct Mtx43 MATRIX_SCRATCH_PAPER;

void func_ov002_020bafc0(char* self){
    struct Vec3 v;
    Vec3_Asr(&v, (struct Vec3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, 0x8c00, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&MATRIX_SCRATCH_PAPER,
        *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, -0x8c00, 0);
    *(struct Mtx43*)(self + 0xf0) = MATRIX_SCRATCH_PAPER;
}
