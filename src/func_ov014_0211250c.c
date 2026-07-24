typedef struct { int x, y, z; } Vec3;

extern void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(Vec3 *v, void *m, Vec3 *dst);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int Vec3_HorzLen(Vec3 *v);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Vec3_MulScalar(Vec3 *out, Vec3 *v, int s);

extern int data_020a0e68[];

void func_ov014_0211250c(char *c)
{
    int yoff;
    Vec3 dvec;
    Vec3 head;
    Vec3 rotated;
    Vec3 saved;
    Vec3 tmp;
    Vec3 sum;
    Vec3 diff;
    Vec3 mul;
    int scale;
    Vec3 *seg;
    Vec3 *dst;
    int *bound;
    int state;
    short angY;
    short angX;
    Vec3 *prev;
    int i;

    head.x = 0;
    head.y = 0;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;
    head.z = -0xc8000;

    /* setup order is load-bearing: seg before state so add/cmp schedule matches */
    seg = (Vec3 *)(c + 0x530);
    dst = (Vec3 *)(c + 0x584);
    state = *(int *)(c + 0x610);
    bound = (int *)(c + 0x5d0);
    scale = 0xb68;
    yoff = -0x5000;

    if (state == 2 || state == 4) {
        if (*(unsigned char *)(c + 0x605) == 0) {
            scale += 0x7800;
            yoff = 0;
        } else {
            scale += 0x190;
        }
    }

    Matrix4x3_FromRotationXYZExt(
        data_020a0e68,
        *(short *)(c + 0x8c),
        *(short *)(c + 0x8e),
        *(short *)(c + 0x90));
    MulVec3Mat4x3(&head, data_020a0e68, &rotated);
    Vec3_Add(&tmp, (Vec3 *)(c + 0x5c), &rotated);
    saved.x = tmp.x;
    saved.y = tmp.y;
    saved.z = tmp.z;
    *(int *)(c + 0x524) = tmp.x;
    *(int *)(c + 0x528) = saved.y;
    *(int *)(c + 0x52c) = saved.z;

    head.z = *(int *)(c + 0x5f8);
    head.x = 0;
    head.y = 0;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;
    i = 1;

    for (; i < 7; i++, bound++) {
        if (i == 0)
            prev = &saved;
        else
            prev = (Vec3 *)((char *)seg - 0xc);

        dvec.x = (seg->x - prev->x) + dst->x;
        dvec.z = (seg->z - prev->z) + dst->z;
        {
            int t = (seg->y + dst->y) + yoff;
            if (t <= *bound)
                t = *bound;
            dvec.y = t - prev->y;
        }

        angY = _ZN4cstd5atan2E5Fix12IiES1_(dvec.x, dvec.z);
        angX = (short)(-_ZN4cstd5atan2E5Fix12IiES1_(dvec.y, Vec3_HorzLen(&dvec)));
        Matrix4x3_FromRotationY(data_020a0e68, angY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, angX);
        MulVec3Mat4x3(&head, data_020a0e68, &rotated);

        dst->x = seg->x;
        dst->y = seg->y;
        dst->z = seg->z;
        Vec3_Add(&sum, prev, &rotated);
        seg->x = sum.x;
        seg->y = sum.y;
        seg->z = sum.z;
        Vec3_Sub(&diff, seg, dst);
        Vec3_MulScalar(&mul, &diff, scale);
        dst->x = mul.x;
        dst->y = mul.y;
        dst->z = mul.z;

        if (*bound <= *(int *)(c + 0x5f0))
            *bound = *(int *)(c + 0x5f0) + 0x28000;

        seg = (Vec3 *)((char *)seg + 0xc);
        dst = (Vec3 *)((char *)dst + 0xc);
    }
}
