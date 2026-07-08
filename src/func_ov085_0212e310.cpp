//cpp
extern "C" {
typedef struct { int x, y, z; } Vector3;
struct Matrix4x3 { int m[12]; };

extern void *_ZN5Actor13ClosestPlayerEv(void);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int fix, bool loop);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, const Vector3 *pos, unsigned int d);
extern void Matrix4x3_FromRotationY(struct Matrix4x3 *m, int angY);
extern void MulVec3Mat4x3(const Vector3 *v, const struct Matrix4x3 *m, Vector3 *out);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 *cur, const Vector3 *tgt, int step);
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern void _Z14ApproachLinearRsss(short *cur, short tgt, short step);
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern int func_ov085_0212e728(void *c, void *p);

extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
extern char data_ov085_02130810[];

int func_ov085_0212e310(char *c)
{
    Vector3 in;
    Vector3 out;
    char *p;

    p = (char *)_ZN5Actor13ClosestPlayerEv();
    if (p == 0) {
        return 1;
    }

    *(int *)(p + 0x744) = *(int *)(c + 0x5c);
    *(int *)(p + 0x748) = *(int *)(c + 0x60);
    *(int *)(p + 0x74c) = *(int *)(c + 0x64);

    if (*(unsigned short *)(c + 0x100) == 0) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x14, 0x7f, 0x15666, 0);
    }

    *(unsigned int *)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int *)(c + 0x2e4), 3, 0x182, (Vector3 *)(c + 0x74), 0);

    in.x = 0;
    in.y = 0;
    in.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = 0xc8000;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, 0);
    MulVec3Mat4x3(&in, &MATRIX_SCRATCH_PAPER, &out);
    out.x += *(int *)(p + 0x5c);
    out.y += *(int *)(p + 0x60) + 0x70000;
    out.z += *(int *)(p + 0x64);

    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)(c + 0x5c), &out, 0x14000);
    _Z14ApproachLinearRsss((short *)(c + 0x94), Vec3_HorzAngle((Vector3 *)(c + 0x5c), &out), 0x800);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);

    if (Vec3_Dist((Vector3 *)(c + 0x5c), &out) < 0x14000) {
        *(void **)(c + 0x2a0) = p;
        func_ov085_0212e728(c, data_ov085_02130810);
    }

    return 1;
}
}
