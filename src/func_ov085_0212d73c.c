typedef signed short s16;
typedef struct Vector3 { int x, y, z; } Vector3;

extern void _ZN6Camera9SetFlag_3Ev(void *cam);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int h, unsigned int a, unsigned int b, Vector3 *v, unsigned int d);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
extern void _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
extern void _Z14ApproachLinearRiii(int *cur, int tgt, int step);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 *cur, Vector3 *tgt, int step);
extern void _ZN6Camera9SetLookAtERK7Vector3(void *cam, Vector3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(void *cam, Vector3 *v);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern int LenVec3(Vector3 *v);
extern void func_ov085_0212e728(char *c, void *p);

extern void *CAMERA;
extern Vector3 data_ov085_0213084c;
extern void *data_ov085_021307a0;

int func_ov085_0212d73c(char *c)
{
    Vector3 v[3];
    void *cam;
    int spd;
    int len;

    cam = CAMERA;
    _ZN6Camera9SetFlag_3Ev(cam);
    *(unsigned int *)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int *)(c + 0x2e4), 3, 0x182, (Vector3 *)(c + 0x74), 0);
    _Z14ApproachLinearRsss((s16 *)(c + 0x8e), Vec3_HorzAngle((Vector3 *)(c + 0x5c), &data_ov085_0213084c), 0x200);
    _Z14ApproachLinearRsss((s16 *)(c + 0x8c), Vec3_VertAngle((Vector3 *)(c + 0x5c), &data_ov085_0213084c), 0x200);
    _Z14ApproachLinearRiii((int *)(c + 0x98), 0x28000, 0x2000);
    _Z14ApproachLinearRsss((s16 *)(c + 0x94), Vec3_HorzAngle((Vector3 *)(c + 0x5c), &data_ov085_0213084c), (s16)*(int *)(c + 0x2cc));
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
    *(int *)(((int)c + 0x2cc) & 0xFFFFFFFFFFFFFFFF) += 5;
    if (*(int *)(c + 0x2cc) > 0x800)
        *(int *)(c + 0x2cc) = 0x800;
    *(int *)(((int)c + 0x2c8) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (*(int *)(c + 0x2c8) < 0x19)
        return 1;
    v[1].x = -0x50c000;
    v[1].y = 0x115000;
    v[1].z = 0x1d15000;
    spd = *(int *)(c + 0x98) >> 1;
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)(c + 0x2b0), (Vector3 *)(c + 0x5c), spd);
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)(c + 0x2bc), &v[1], spd);
    _ZN6Camera9SetLookAtERK7Vector3(cam, (Vector3 *)(c + 0x2b0));
    _ZN6Camera6SetPosERK7Vector3(cam, (Vector3 *)(c + 0x2bc));
    Vec3_Sub(&v[2], (Vector3 *)(c + 0x5c), &data_ov085_0213084c);
    len = LenVec3(&v[2]);
    if (len == 0 || len < 0x7d0000)
        func_ov085_0212e728(c, (void *)&data_ov085_021307a0);
    return 1;
}
