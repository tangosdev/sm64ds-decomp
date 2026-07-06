typedef int s32;
typedef unsigned short u16;
typedef signed short s16;
typedef struct { s32 x, y, z; } Vector3;

extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern s32 Vec3_HorzLen(const Vector3 *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, s32 x, s32 y, s32 z);

extern s16 data_02082214[];

void func_ov074_02120bb8(char *self, char *p1, char *p2, int mode)
{
    Vector3 v, dst, src, delta;
    s32 aX, aY;
    s32 iX, iY;
    s32 s, w;

    dst.x = *(s32*)(self + 0x5c);
    dst.y = *(s32*)(self + 0x60);
    dst.z = *(s32*)(self + 0x64);
    {
        Vector3 *pv = (Vector3*)(((long long)(int)(p1 + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        src.x = pv->x;
        src.y = pv->y;
        src.z = pv->z;
    }
    {
        s32 dy = *(s32*)(p2 + 8);
        w = *(s32*)(p2 + 4);
        dst.y += dy;
    }
    src.y += 0x46000;
    Vec3_Sub(&delta, &src, &dst);

    v.x = delta.x;
    v.y = delta.y;
    v.z = delta.z;
    aX = _ZN4cstd5atan2E5Fix12IiES1_(v.x, v.z);
    aY = _ZN4cstd5atan2E5Fix12IiES1_(v.y, Vec3_HorzLen(&v));

    iY = (u16)aY >> 4;
    iX = (u16)aX >> 4;

    s = (s32)(((long long)w * data_02082214[iY * 2 + 1] + 0x800) >> 12);
    dst.x += (s32)(((long long)s * data_02082214[iX * 2] + 0x800) >> 12);
    dst.y += (s32)(((long long)w * data_02082214[iY * 2] + 0x800) >> 12);
    dst.z += (s32)(((long long)s * data_02082214[iX * 2 + 1] + 0x800) >> 12);

    switch (mode) {
    case 0:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xf6, dst.x, dst.y, dst.z);
        break;
    case 1:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, dst.x, dst.y, dst.z);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, dst.x, dst.y, dst.z);
        break;
    }
}
