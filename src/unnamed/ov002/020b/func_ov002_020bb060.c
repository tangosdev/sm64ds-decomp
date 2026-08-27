struct Vec3 { int x, y, z; };
struct M43 { int w[12]; };

extern char *func_ov002_020e496c(void *p);
extern int _ZN6Player14IsFrontSlidingEv(void *p);
extern int _ZN6Player17LostGrabbedObjectEv(void *p);
extern void _Z14ApproachLinearRsss(short *dst, short target, short step);
extern void MulMat4x3Mat4x3(void *a, void *b, void *c);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void Vec3_Lsl(struct Vec3 *d, struct Vec3 *s, int sh);
extern struct M43 data_020a0e68;
extern short data_ov002_020ff0d0[];
extern int data_ov002_020ff0d4[];
extern int data_ov002_020ff0d8[];
extern int data_ov002_020ff0dc[];

void func_ov002_020bb060(char *self)
{
    struct Vec3 v;
    struct Vec3 lo;
    char *result = func_ov002_020e496c(*(void **)(self + 0x59c));
    char *m2 = *(char **)(result + 0x14);
    int r4 = 0;

    if (_ZN6Player14IsFrontSlidingEv(*(void **)(self + 0x59c)))
        r4 = 1;
    if (_ZN6Player17LostGrabbedObjectEv(*(void **)(self + 0x59c))) {
        if ((unsigned int)(*(int *)(result + 0x58) << 4) >> 0x10 < 0xe)
            r4 = 1;
    }

    _Z14ApproachLinearRsss((short *)(self + 0x8c), data_ov002_020ff0d0[r4], 0x1000);

    ((int *)&v)[0] = 0;
    ((int *)&v)[1] = 0;
    ((int *)&v)[2] = 0;
    data_020a0e68 = *(struct M43 *)(result + 0x1c);
    MulMat4x3Mat4x3(m2 + 0x2a0, &data_020a0e68, &data_020a0e68);
    v.x = data_020a0e68.w[9];
    v.y = data_020a0e68.w[10];
    v.z = data_020a0e68.w[11];

    *(short *)(self + 0x94) = *(short *)(*(char **)(self + 0x59c) + 0x8e);
    *(short *)(self + 0x8e) = *(short *)(self + 0x94);

    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x8c00, 0);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(short *)(self + 0x8c), *(short *)(self + 0x8e), *(short *)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -0x8c00, 0);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, *(int *)((char *)data_ov002_020ff0d4 + r4 * 0xc), *(int *)((char *)data_ov002_020ff0d8 + r4 * 0xc), *(int *)((char *)data_ov002_020ff0dc + r4 * 0xc));

    *(int *)(self + 0x5c) = data_020a0e68.w[9];
    *(int *)(self + 0x60) = data_020a0e68.w[10];
    *(int *)(self + 0x64) = data_020a0e68.w[11];
    Vec3_Lsl(&lo, (struct Vec3 *)(self + 0x5c), 3);
    *(int *)(self + 0x5c) = lo.x;
    *(int *)(self + 0x60) = lo.y;
    *(int *)(self + 0x64) = lo.z;
    *(struct M43 *)(self + 0xf0) = data_020a0e68;
}
