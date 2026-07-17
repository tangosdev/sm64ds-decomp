typedef int Fix12;
typedef short s16;
typedef struct Vector3 { int x, y, z; } Vector3;

extern int Vec3_Dist(void *a, void *b);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *p);
extern short Vec3_HorzAngle(void *a, void *b);
extern void ApproachAngle(s16 *dst, s16 target, int a, int b, int c);
extern short Vec3_VertAngle(void *a, void *b);
extern void _Z14ApproachLinearRsss(void *dst, short a, short b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern void _Z14ApproachLinearRiii(void *dst, int a, int b);
extern int func_ov070_02120020(void *c, void *p);
extern void *_ZN5Actor22ClosestNonVanishPlayerEv(void *c);

extern char data_020a0e68[];
extern char data_ov070_0212359c[];
extern char data_ov070_021235ac[];

int func_ov070_0211fd98(char *c)
{
    int in[3];
    int out[3];
    Vector3 t;
    void *p;

    in[0] = 0; in[1] = 0; in[2] = 0;
    out[0] = 0; out[1] = 0; out[2] = 0;

    if (Vec3_Dist(c + 0x5c, c + 0x3c0) > 0x1f4000 ||
        _ZNK12WithMeshClsn8IsOnWallEv(c + 0x144)) {
        *(s16 *)(c + 0x300 + 0xe6) = Vec3_HorzAngle(c + 0x5c, c + 0x3c0);
        if (*(unsigned short *)(c + 0x100) < 0x14)
            *(unsigned short *)(c + 0x100) = 0x14;
    }
    ApproachAngle((s16 *)(c + 0x94), *(s16 *)(c + 0x300 + 0xe6), 0xa, 0x200, 0x100);

    _Z14ApproachLinearRsss(c + 0x92, Vec3_VertAngle(c + 0x5c, c + 0x3c0), 0x100);

    ApproachAngle((s16 *)(c + 0x96),
                  (*(s16 *)(c + 0x94) - *(s16 *)(c + 0x300 + 0xe6)) / 2,
                  0xa, 0x100, 0x50);

    in[2] = 0xa000;
    Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16 *)(c + 0x92));
    MulVec3Mat4x3(in, data_020a0e68, out);

    _Z14ApproachLinearRiii(c + 0xa4, out[0], 0x1000);
    _Z14ApproachLinearRiii(c + 0xa8, out[1], 0x800);
    _Z14ApproachLinearRiii(c + 0xac, out[2], 0x1000);

    if (*(unsigned short *)(c + 0x100) == 0) {
        func_ov070_02120020(c, data_ov070_0212359c);
        return 1;
    }
    if (*(unsigned short *)(c + 0x300 + 0xcc) != 0)
        return 1;
    if (Vec3_Dist(c + 0x5c, c + 0x3c0) < 0x5dc000) {
        p = _ZN5Actor22ClosestNonVanishPlayerEv(c);
        if (p) {
            int *pos = (int *)(((int)p + 0x5c) & 0xffffffffffffffffLL);
            t.x = pos[0];
            t.y = pos[1];
            t.z = pos[2];
            if (Vec3_Dist(c + 0x5c, &t) < 0x3e8000)
                func_ov070_02120020(c, data_ov070_021235ac);
        }
    }
    return 1;
}
