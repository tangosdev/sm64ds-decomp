// NONMATCHING: missing logic (ROM does more) (div=38). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12i;
struct Matrix4x3 { Fix12i a[12]; };
struct Vector3 { Fix12i x, y, z; };
struct Player;
struct Actor;

extern void *func_ov002_020e496c(struct Player *p, struct Vector3 v);
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(struct Matrix4x3 *mF, short x, short y, short z);
extern void Vec3_Asr(struct Vector3 *vF, const struct Vector3 *v, int amount);
extern void Vec3_Lsl(struct Vector3 *vF, const struct Vector3 *v, int amount);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;

void _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(struct Actor *self, struct Player *player, const struct Vector3 *vec)
{
    char *r8;
    void *o;
    struct Vector3 zero;
    struct Vector3 asr, lsl;
    zero.x = 0; zero.y = 0; zero.z = 0;
    o = func_ov002_020e496c(player, zero);
    r8 = (char*)*(void**)((char*)o + 0x14);
    MATRIX_SCRATCH_PAPER = *(struct Matrix4x3*)((char*)o + 0x1c);
    MulMat4x3Mat4x3((struct Matrix4x3*)(r8 + 0x2a0), &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    *(short*)((char*)self + 0x94) = *(short*)((char*)player + 0x8e);
    *(short*)((char*)self + 0x8e) = *(short*)((char*)self + 0x94);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
        *(Fix12i*)((char*)&MATRIX_SCRATCH_PAPER + 0x24),
        *(Fix12i*)((char*)&MATRIX_SCRATCH_PAPER + 0x28),
        *(Fix12i*)((char*)&MATRIX_SCRATCH_PAPER + 0x2c));
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(short*)((char*)self + 0x8c), *(short*)((char*)self + 0x8e), *(short*)((char*)self + 0x90));
    Vec3_Asr(&asr, vec, 3);
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, asr.x, asr.y, asr.z);
    *(Fix12i*)((char*)self + 0x5c) = *(Fix12i*)((char*)&MATRIX_SCRATCH_PAPER + 0x24);
    *(Fix12i*)((char*)self + 0x60) = *(Fix12i*)((char*)&MATRIX_SCRATCH_PAPER + 0x28);
    *(Fix12i*)((char*)self + 0x64) = *(Fix12i*)((char*)&MATRIX_SCRATCH_PAPER + 0x2c);
    Vec3_Lsl(&lsl, (struct Vector3*)((char*)self + 0x5c), 3);
    *(Fix12i*)((char*)self + 0x5c) = lsl.x;
    *(Fix12i*)((char*)self + 0x60) = lsl.y;
    *(Fix12i*)((char*)self + 0x64) = lsl.z;
}
