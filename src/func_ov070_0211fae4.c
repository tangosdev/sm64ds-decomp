typedef struct { int x, y, z; } Vector3;

extern void *_ZN5Actor22ClosestNonVanishPlayerEv(void *self);
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern short Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
extern void ApproachAngle(short *cur, short target, int a, int b, int c);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(const void *a, const void *m, void *out);
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern int RandomIntInternal(int *seed);
extern int func_ov070_02120020(void *c, void *p);

extern signed char data_0209f2f8;
extern int data_ov070_021235dc[];
extern int data_020a0e68[];
extern int data_ov070_0212359c[];
extern int data_0209e650;
extern int data_ov070_021235cc[];
extern int data_ov070_0212358c[];

int func_ov070_0211fae4(void *arg)
{
    char *c = (char *)arg;
    char *player;
    Vector3 vin;
    Vector3 vb;
    Vector3 vc;
    Vector3 vd;

    player = (char *)_ZN5Actor22ClosestNonVanishPlayerEv(c);
    if (player == 0) {
        if (data_0209f2f8 != 0x16) {
            *(int *)(c + 0x3c0) = *(int *)(c + 0x5c);
            *(int *)(c + 0x3c4) = *(int *)(c + 0x60);
            *(int *)(c + 0x3c8) = *(int *)(c + 0x64);
            *(int *)(((int)c + 0x3c4) & 0xFFFFFFFFFFFFFFFFLL) += 0xc8000;
        }
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
        *(short *)(c + 0x92) = 0;
        *(short *)(c + 0x100) = 0;
        *(int *)(c + 0xa8) = 0;
        func_ov070_02120020(c, data_ov070_021235dc);
        return 1;
    }

    vin.x = 0;
    vin.y = 0;
    vin.z = 0;

    {
    int *q = (int *)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
    vb.x = q[0];
    vb.y = q[1];
    vb.z = q[2];
    }
    vb.y += 0xc8000;
    vc.x = vb.x;
    vc.y = vb.y;
    vc.z = vb.z;

    *(short *)(c + 0x3e6) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &vc);
    ApproachAngle((short *)(c + 0x94), *(short *)(c + 0x3e6), 1, 0x500, 0x500);

    vd.x = vb.x;
    vd.y = vb.y;
    vd.z = vb.z;
    ApproachAngle((short *)(c + 0x92), Vec3_VertAngle((Vector3 *)(c + 0x5c), &vd), 1, 0x500, 0x500);

    ApproachAngle((short *)(c + 0x96),
                  (*(short *)(c + 0x94) - *(short *)(c + 0x3e6)) / 2,
                  0xa, 0x100, 0x50);

    vin.z = 0xf000;
    Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short *)(c + 0x92));
    MulVec3Mat4x3(&vin, data_020a0e68, c + 0xa4);

    if (Vec3_Dist((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x3c0)) > 0x5dc000) {
        func_ov070_02120020(c, data_ov070_0212359c);
        return 1;
    }

    {
    int dist = Vec3_Dist((Vector3 *)(c + 0x5c), &vb);
    int thresh = 0x258000;
    if (data_0209f2f8 == 0x16) thresh = 0x384000;
    if (dist < thresh) {
        *(int *)(c + 0xa4) = 0;
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0xac) = 0;
        if (*(int *)(c + 0x3e0) == 0 ||
            (((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 1) == 0) {
            func_ov070_02120020(c, data_ov070_021235cc);
        } else {
            func_ov070_02120020(c, data_ov070_0212358c);
        }
    }
    }

    return 1;
}
