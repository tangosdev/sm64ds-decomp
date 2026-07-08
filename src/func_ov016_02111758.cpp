//cpp
extern "C" {
struct Matrix4x3 { int m[12]; };
typedef struct { int x, y, z; } Vector3;
extern void Matrix4x3_FromRotationY(struct Matrix4x3 *m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *m, short angX);
extern void MulVec3Mat4x3(const Vector3 *v, const struct Matrix4x3 *m, Vector3 *out);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 *cur, const Vector3 *tgt, int step);
extern int _ZN9Animation8FinishedEv(void *self);
extern int func_ov018_02111bf0(void *self, void *arg);
extern unsigned char STAR_ID;
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
extern void *data_ov016_02114dac;

int func_ov016_02111758(char *c) {
    Vector3 in;
    Vector3 out;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    unsigned int t = (unsigned int)(*(int*)(c + 0x3a8)) << 4 >> 0x10;
    if (t >= 0x15 && t <= 0x3c) {
        if (STAR_ID == 1) {
            in.z = 0x2bc000;
        } else {
            in.z = 0x1f4000;
        }
        Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8c));
        MulVec3Mat4x3(&in, &MATRIX_SCRATCH_PAPER, &out);
        out.x += *(int*)(c + 0x3f0);
        out.y += *(int*)(c + 0x3f4);
        out.z += *(int*)(c + 0x3f8);
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3*)(c + 0x5c), &out, 0x14000);
    }
    if (_ZN9Animation8FinishedEv(c + 0x3a0)) {
        *(int*)(c + 0x98) = 0;
        func_ov018_02111bf0(c, &data_ov016_02114dac);
    }
    return 1;
}
}
