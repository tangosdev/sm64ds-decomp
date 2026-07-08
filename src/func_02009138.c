struct Vec3 { int x, y, z; };
struct Mtx43 { int m[12]; };
extern void Vec3_Sub(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int LenVec3(struct Vec3 *v);
extern int Math_Function_0203b14c(int *p, int a, int b, int c, int d);
extern void Matrix4x3_FromTranslation(struct Mtx43 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Mtx43 *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Mtx43 *m, short ang);
extern void MulVec3Mat4x3(struct Vec3 *v, struct Mtx43 *m, struct Vec3 *res);
extern struct Mtx43 MATRIX_SCRATCH_PAPER;

int func_02009138(int *thiz, int arg)
{
    struct Vec3 v;
    int len;
    int r;
    Vec3_Sub(&v, (struct Vec3*)((char*)thiz + 0x8c), (struct Vec3*)((char*)thiz + 0x80));
    len = LenVec3(&v);
    r = Math_Function_0203b14c(&len, arg, 0x300, 0x60000, 0x200);
    v.x = 0;
    v.y = 0;
    v.z = len;
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, thiz[0x20], thiz[0x21], thiz[0x22]);
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)((char*)thiz + 0x17c));
    Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(short*)((char*)thiz + 0x17e));
    MulVec3Mat4x3(&v, &MATRIX_SCRATCH_PAPER, (struct Vec3*)((char*)thiz + 0x8c));
    return (r == 0) ? 1 : 0;
}
