typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int Fix12i;
typedef long long s64;

typedef struct { Fix12i x, y, z; } Vector3;

extern u16 data_0209b274;
extern void *data_0209f318;
extern s16 data_02082214[];

extern unsigned int ReadUnalignedInt(unsigned char *p);
extern int func_02008b4c(void *a0, short *p1, int *p2, char *tbl);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern Fix12i Vec3_HorzLen(const Vector3 *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);

int func_ov002_020f7bb8(char *c, unsigned char *p, u16 id)
{
    Vector3 v0, v1;
    int src[3];
    Fix12i dist;
    s16 vertAngle, heading;
    char *obj;
    unsigned int tbl1, tbl2;
    int i;

    *(u8 *)(c + 0x102) = 0;
    if (id == data_0209b274) {
        *(int *)(c + 0xf4) = 0;
        *(u16 *)(c + 0xfc) = 0;
    }

    tbl1 = ReadUnalignedInt(p);
    tbl2 = ReadUnalignedInt(p + 4);

    if (func_02008b4c(&v1, (short *)(c + 0xfc), (int *)(c + 0xf4), (char *)tbl1) != 0) {
        return 0;
    }
    if (func_02008b4c(&v0, (short *)(c + 0xfc), (int *)(c + 0xf4), (char *)tbl2) != 0) {
        return 0;
    }

    *(u8 *)(c + 0x102) = 0x1f;
    obj = data_0209f318;

    AddVec3(&v0, (Vector3 *)(obj + 0x8c), &v0);
    dist = Vec3_Dist(&v0, (Vector3 *)(obj + 0x8c));

    {
        s16 t1 = *(s16 *)(obj + 0x17e);
        vertAngle = Vec3_VertAngle(&v0, (Vector3 *)(obj + 0x8c));
        vertAngle += t1;
    }
    {
        s16 t2 = *(s16 *)(obj + 0x17c);
        heading = Vec3_HorzAngle(&v0, (Vector3 *)(obj + 0x8c));
        heading += t2;
    }

    src[0] = 0;
    src[1] = (int)(((s64)dist * data_02082214[(((u16)vertAngle) >> 4) * 2] + 0x800) >> 12);
    src[2] = (int)(((s64)dist * data_02082214[(((u16)vertAngle) >> 4) * 2 + 1] + 0x800) >> 12);

    Vec3_RotateYAndTranslate((int *)(c + 0x5c), (int *)(obj + 0x8c), heading, src);

    *(s16 *)(c + 0x8c) = _ZN4cstd5atan2E5Fix12IiES1_(v1.y, Vec3_HorzLen(&v1));
    *(s16 *)(c + 0x8e) = _ZN4cstd5atan2E5Fix12IiES1_(v1.x, v1.z);

    return 1;
}
