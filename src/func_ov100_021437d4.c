// @symbol func_ov100_021437d4
/* recovered: shared common types */
#include "common.h"
typedef short s16;
typedef long long s64;

extern short data_02082214[];
extern char data_020a0e68[];
extern void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
extern void MulVec3Mat4x3(struct Vector3* in, void* m, struct Vector3* out);
extern void Vec3_Add(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int Vec3_HorzLen(struct Vector3* v);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* mF, int angX);
extern void Vec3_Sub(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern void Vec3_MulScalar(struct Vector3* out, const struct Vector3* in, int scale);
#define M(p) (p)


void func_ov100_021437d4(char* thisx)
{
    struct Vector3 in, out, cur, delta, added, v48, v54, v60;
    struct Vector3 *pos, *vel, *src;
    int *heights;
    int loop;
    int angOff;
    int idx, val, ang, hlen, angz;

    pos = (struct Vector3*)(thisx + 0x6d8);
    vel = (struct Vector3*)(thisx + 0x720);
    heights = (int*)(thisx + 0x78c);

    in.z = -0xfa000; in.x = 0; in.y = 0;
    out.x = 0; out.y = 0; out.z = 0;

    Matrix4x3_FromRotationXYZExt(data_020a0e68, *(s16*)(thisx + 0x8c), *(s16*)(thisx + 0x8e), *(s16*)(thisx + 0x90));
    MulVec3Mat4x3(&in, data_020a0e68, &out);
    Vec3_Add(&added, (struct Vector3*)(thisx + 0x5c), &out);

    {
        int ax = added.x, ay = added.y, az = added.z;
        int *p6a0;
        int zin;
        loop = 0;
        cur.x = ax; cur.y = ay; cur.z = az;
        *(int*)(thisx + 0x6d8) = ax;
        ay = cur.y;
        p6a0 = (int*)(int)M(thisx + 0x6a0);
        *(int*)(thisx + 0x6dc) = ay;
        {
            int cz = cur.z;
            zin = 0x32000;
            *(int*)(thisx + 0x6e0) = cz;
            {
                int c = *p6a0;
                *p6a0 = c + 1;
            }
        }
        in.z = zin;
        angOff = loop;
        in.x = 0;
        in.y = 0;
    }

    for (; loop < 6; ) {
        if (loop != 0) src = (struct Vector3*)((char*)pos - 0xc);
        else src = &cur;
        delta.x = vel->x + (pos->x - src->x);
        delta.z = vel->z + (pos->z - src->z);
        idx = ((unsigned short)(short)(angOff + (*(int*)(thisx + 0x6a0) << 12))) >> 4;
        val = (pos->y + vel->y) - 0x2000
            + (int)(((s64)data_02082214[idx * 2] * (-0xa000) + 0x800) >> 12);
        if (val <= *heights) val = *heights;
        delta.y = val - src->y;
        ang = _ZN4cstd5atan2E5Fix12IiES1_(delta.x, delta.z);
        hlen = Vec3_HorzLen(&delta);
        angz = (s16)(-_ZN4cstd5atan2E5Fix12IiES1_(delta.y, hlen));
        Matrix4x3_FromRotationY(data_020a0e68, ang);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, angz);
        MulVec3Mat4x3(&in, data_020a0e68, &out);
        vel->x = pos->x; vel->y = pos->y; vel->z = pos->z;
        Vec3_Add(&v48, src, &out);
        pos->x = v48.x; pos->y = v48.y; pos->z = v48.z;
        Vec3_Sub(&v54, pos, vel);
        Vec3_MulScalar(&v60, &v54, 0xbb8);
        vel->x = v60.x; vel->y = v60.y; vel->z = v60.z;
        *heights = *(int*)(thisx + 0x60) - 0xc8000;
        if (*heights - pos->y > 0xc8000) *heights = pos->y;
        angOff += 0x2000;
        vel = (struct Vector3*)((char*)vel + 0xc);
        pos = (struct Vector3*)((char*)pos + 0xc);
        loop++;
        heights = heights + 1;
    }
}
