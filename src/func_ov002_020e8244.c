// NONMATCHING: register allocation (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
struct M48 { int w[12]; };
struct V3 { int x, y, z; };

extern int func_0203d024(struct V3* a, struct V3* b);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* mF, s16 angY);
extern void MulMat4x3Mat4x3(void* out, void* a, void* b);
extern void SubVec3(struct V3* a, struct V3* b, struct V3* c);
extern void Vec3_LslInPlace(void* v, int sh);
extern void AddVec3(struct V3* a, struct V3* b, struct V3* c);

extern struct M48 MATRIX_SCRATCH_PAPER;

void func_ov002_020e8244(int* out, char* b)
{
    struct M48 local;
    struct V3 zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    if (func_0203d024((struct V3*)(b + 0x4a8), &zero) != 0) {
        out[0] = *(int*)(b + 0x4a8);
        out[1] = *(int*)(b + 0x4ac);
        out[2] = *(int*)(b + 0x4b0);
        return;
    }
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)(b + 0x5c), *(int*)(b + 0x60), *(int*)(b + 0x64));
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(b + 0x94));
    local = *(struct M48*)(*(char**)(b + 0x320));
    MulMat4x3Mat4x3(&local, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    *(int*)(b + 0x4a8) = MATRIX_SCRATCH_PAPER.w[9];
    *(int*)(b + 0x4ac) = MATRIX_SCRATCH_PAPER.w[10];
    *(int*)(b + 0x4b0) = MATRIX_SCRATCH_PAPER.w[11];
    SubVec3((struct V3*)(b + 0x5c), (struct V3*)(b + 0x4a8), (struct V3*)(b + 0x4a8));
    Vec3_LslInPlace((void*)(b + 0x4a8), 3);
    AddVec3((struct V3*)(b + 0x4a8), (struct V3*)(b + 0x5c), (struct V3*)(b + 0x4a8));
    *(int*)(b + 0x4ac) = *(int*)(*(char**)(b + 0x31c) + 0xc) * 0xd + *(int*)(b + 0x4ac);
    out[0] = *(int*)(b + 0x4a8);
    out[1] = *(int*)(b + 0x4ac);
    out[2] = *(int*)(b + 0x4b0);
}
