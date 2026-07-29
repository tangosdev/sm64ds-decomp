//cpp
typedef int s32;
typedef unsigned short u16;
typedef signed short s16;

struct Vector3 { s32 x, y, z; };
struct Actor { char pad[0x100]; };

extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern s32 Vec3_HorzLen(const Vector3 *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern s32 *Vec3_AsrInPlace(s32 *v, s32 sh);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int uniqueID, unsigned int effectID,
    s32 x, s32 y, s32 z,
    const void *dir, void *callback);

extern s16 data_02082214[];

struct Enemy
{
    char pad[0x100];

    void SpawnMegaCharParticles(Actor &a, char *p);
};

void Enemy::SpawnMegaCharParticles(Actor &a, char *p)
{
    char *self = (char *)this;
    char *ap = (char *)&a;
    s16 dirvec[3];
    Vector3 v;
    Vector3 dst, src;

    dst.x = *(s32 *)(self + 0x5c);
    dst.y = *(s32 *)(self + 0x60);
    {
        Vector3 *pv = (Vector3 *)(((long long)(int)(ap + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        dst.z = *(s32 *)(self + 0x64);
        src.x = pv->x;
        src.y = pv->y;
        src.z = pv->z;
    }

    {
        int t = ((int) * (u16 *)(self + 0x8e)) >> 4;
        dirvec[0] = data_02082214[t * 2];
        dirvec[1] = 0;
        t = ((int) * (u16 *)(self + 0x8e)) >> 4;
        dirvec[2] = data_02082214[t * 2 + 1];
    }

    if (p != 0) {
        Vector3 delta;
        s32 w = *(s32 *)(p + 4);
        s32 aX, aY;
        s32 iX, iY;
        s32 s;

        Vec3_Sub(&delta, &src, &dst);

        v.x = delta.x;
        v.y = delta.y;
        v.z = delta.z;

        aX = _ZN4cstd5atan2E5Fix12IiES1_(v.x, v.z);
        aY = _ZN4cstd5atan2E5Fix12IiES1_(v.y, Vec3_HorzLen(&v));

        iX = (u16)aX >> 4;
        iY = (u16)aY >> 4;

        s = (s32)(((long long)w * data_02082214[iY * 2 + 1] + 0x800) >> 12);
        dst.x += (s32)(((long long)s * data_02082214[iX * 2] + 0x800) >> 12);
        dst.y += (s32)(((long long)w * data_02082214[iY * 2] + 0x800) >> 12);
        dst.z += (s32)(((long long)s * data_02082214[iX * 2 + 1] + 0x800) >> 12);

        dst.y += (*(s32 *)(p + 8)) >> 1;
    } else {
        Vector3 delta;
        Vec3_Sub(&delta, &src, &dst);
        dst.x = delta.x;
        dst.y = delta.y;
        dst.z = delta.z;
        Vec3_AsrInPlace(&dst.x, 1);
        dst.y += 0x46000;
    }

    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x43, dst.x, dst.y, dst.z, 0, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x44, dst.x, dst.y, dst.z, 0, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0xbf, dst.x, dst.y, dst.z, dirvec, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0xc0, dst.x, dst.y, dst.z, 0, 0);
}
