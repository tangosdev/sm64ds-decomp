typedef int Fix12i;
typedef short s16;

struct Vec3 { Fix12i x, y, z; };
struct Mtx43 { Fix12i a[12]; };

extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(struct Mtx43* dst, struct Mtx43* a, struct Mtx43* b);

extern struct Mtx43 MATRIX_SCRATCH_PAPER;

void func_ov090_02132b14(char* self){
    struct Vec3 v;
    Vec3_Asr(&v, (struct Vec3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    *(struct Mtx43*)(self + 0x328) = MATRIX_SCRATCH_PAPER;
    *(int*)(self + 0x39c) = 0;
    *(int*)(self + 0x3a0) = 0;
    *(int*)(self + 0x3a4) = 0;
    MATRIX_SCRATCH_PAPER = *(struct Mtx43*)(self + 0x328);
    MulMat4x3Mat4x3((struct Mtx43*)(*(char**)(self + 0x320) + 0x90),
        &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    {
        int* p9 = (int*)(((unsigned long long)((int)(self) + 0x39c)) & 0xFFFFFFFFFFFFFFFFULL);
        int* p10 = (int*)(((unsigned long long)((int)(self) + 0x3a0)) & 0xFFFFFFFFFFFFFFFFULL);
        int* p11 = (int*)(((unsigned long long)((int)(self) + 0x3a4)) & 0xFFFFFFFFFFFFFFFFULL);
        *(int*)(self + 0x39c) = MATRIX_SCRATCH_PAPER.a[9];
        *(int*)(self + 0x3a0) = MATRIX_SCRATCH_PAPER.a[10];
        *(int*)(self + 0x3a4) = MATRIX_SCRATCH_PAPER.a[11];
        *p9 <<= 3;
        *p10 <<= 3;
        *p11 <<= 3;
    }
}
