typedef struct { int x, y, z; } Vector3;
typedef long long s64;

extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int, unsigned int, unsigned int, const Vector3 *, unsigned int);
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern void MulVec3Mat4x3(void *v, void *m, void *out);
extern int Vec3_ApproachHorz(void *out, void *a, int maxStep);
extern short Vec3_HorzAngle(const void *v0, const void *v1);
extern void _Z14ApproachLinearRsss(short *, short, short);
extern int func_ov073_0211f61c(void *c);
extern int func_ov073_0212157c(void *c, void *p);

extern int MATRIX_SCRATCH_PAPER[];
extern short SINE_TABLE[];
extern int data_ov073_02123340[];
extern int data_ov073_021233f0[];

int func_ov073_02120910(char *c)
{
    Vector3 in;
    Vector3 out;

    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;

    if (*(unsigned short *)(c + 0x100) == 0) {
        *(c + 0x4cb) = 1;
        func_ov073_0212157c(c, data_ov073_02123340);
        *(c + 0x4ca) = 1;
        *(short *)(c + 0x94) = *(short *)(c + 0x8e);
        return 1;
    }

    *(unsigned int *)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int *)(c + 0x500), 3, 0x16e, (Vector3 *)(c + 0x74), 0);

    in.z = 0;
    in.z = 0x78000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, *(short *)(c + 0x8e) + 0x8000);
    MulVec3Mat4x3(&in, MATRIX_SCRATCH_PAPER, &out);
    in.x = *(int *)(c + 0x4a8) + out.x;
    in.y = *(int *)(c + 0x4ac);
    in.z = *(int *)(c + 0x4b0) + out.z;
    Vec3_ApproachHorz(c + 0x5c, &in, 0x14000);
    *(short *)(c + 0x8e) = Vec3_HorzAngle(c + 0x5c, c + 0x3d8);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e) + 0x8000;
    *(int *)(((int)c + 0x4b4) & 0xFFFFFFFFFFFFFFFF) += 0x500;
    _Z14ApproachLinearRsss((short *)(c + 0x8c),
        ((s64)*(int *)(c + 0x4d0) * SINE_TABLE[((unsigned short)(short)*(int *)(c + 0x4b4) >> 4) * 2] + 0x800) >> 12,
        0x400);
    if (func_ov073_0211f61c(c)) {
        func_ov073_0212157c(c, data_ov073_021233f0);
    }
    return 1;
}
