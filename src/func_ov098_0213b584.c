struct Vec3 { int x, y, z; };
struct Mtx43 { int w[12]; };

void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
void Matrix4x3_FromTranslation(struct Mtx43 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Mtx43 *m, short angY);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, struct Mtx43 *m, int radHeight, int a, unsigned int b);

extern struct Mtx43 MATRIX_SCRATCH_PAPER;

void func_ov098_0213b584(char *c)
{
    struct Vec3 v;
    Vec3_Asr(&v, (struct Vec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    *(struct Mtx43*)(c + 0x378) = MATRIX_SCRATCH_PAPER;
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8e));
    *(struct Mtx43*)(c + 0x31c) = MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (void*)(c + 0x350), (struct Mtx43*)(c + 0x378),
        *(int*)(c + 0x80) * 0xa0, 0x3e8000, 6);
}
