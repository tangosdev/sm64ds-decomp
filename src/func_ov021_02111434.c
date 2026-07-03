typedef int Fix12;
typedef short s16;

struct Quaternion;
struct Matrix4x3;

extern void Matrix4x3_FromQuaternion(const struct Quaternion *q, struct Matrix4x3 *mF);
extern void Vec3_Asr(void *d, void *s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void *a, void *b, void *c);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *mF, s16 angX);
extern void Matrix4x3_ApplyInPlaceToRotationZ(struct Matrix4x3 *mF, s16 angZ);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, s16 angY);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, Fix12 x, Fix12 y, Fix12 z);

struct M48 {
    int w[12];
};

extern struct M48 data_020a0e68;
extern const int data_ov021_02114a20[];
extern const s16 data_ov021_02114740[];

static asm unsigned conv_ov021(unsigned x) {
    ands r0, r0, #1
    bx lr
}

int func_ov021_02111434(char *c)
{
    struct M48 mtx;
    int i;
    char *src;
    char *tr;
    char *obj;
    unsigned rv;
    volatile int v[3];
    int tmp[3];
    int vo[3];

    Matrix4x3_FromQuaternion((struct Quaternion *)(c + 0xc4c), (struct Matrix4x3 *)(&mtx));
    Vec3_Asr(tmp, c + 0x5c, 3);
    Matrix4x3_FromTranslation((struct Matrix4x3 *)(&data_020a0e68), tmp[0], tmp[1], tmp[2]);
    MulMat4x3Mat4x3(&mtx, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToRotationX((struct Matrix4x3 *)(&data_020a0e68), *((s16 *)(c + 0x8c)));
    Matrix4x3_ApplyInPlaceToRotationZ((struct Matrix4x3 *)(&data_020a0e68), *((s16 *)(c + 0x90)));
    rv = 1;
    Matrix4x3_ApplyInPlaceToRotationY((struct Matrix4x3 *)(&data_020a0e68), *((s16 *)(c + 0x8e)));
    *((struct M48 *)(c + 0xf0)) = data_020a0e68;
    src = c + 0xf0;
    tr = (char *)(&data_ov021_02114a20[0]);
    i = 0;
    obj = c;
    for (; i < 4; i++) {
        int ny;

        v[0] = *((int *)tr);
        ny = *((int *)(tr + 4));
        v[rv] = ny;
        v[2] = *((int *)(tr + 8));
        if ((*((signed char *)((c + 0xc00) + 0x7a))) == i) {
            if ((*((unsigned char *)(c + 0xc7d))) == 0) {
                v[rv] = ny - 0x1e000;
            }
        }
        *((struct M48 *)(&data_020a0e68)) = *((struct M48 *)src);
        Vec3_Asr(vo, (void *)v, 3);
        Matrix4x3_ApplyInPlaceToTranslation((struct Matrix4x3 *)(&data_020a0e68), vo[0], vo[rv], vo[2]);
        Matrix4x3_ApplyInPlaceToRotationY((struct Matrix4x3 *)(&data_020a0e68), data_ov021_02114740[i]);
        *((struct M48 *)(obj + 0x33c)) = data_020a0e68;
        tr += 0xc;
        obj += 0x50;
    }

    rv = *((unsigned short *)((c + 0xc00) + 0x74));
    if (rv >= 0x2d)
        return rv;
    return conv_ov021(rv);
}