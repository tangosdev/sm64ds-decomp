// @symbol func_ov100_021415bc
/* recovered: shared common types */
#include "common.h"
#pragma opt_common_subs off
#define M(p) (p)

extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern void Vec3_Sub(struct Vector3 *out, void *a, void *b);
extern int Vec3_HorzLen(struct Vector3 *v);
extern short Vec3_HorzAngle(const void *a, const void *b);
extern short Vec3_VertAngle(const void *a, const void *b);
extern void _Z14ApproachLinearRsss(short *dst, short target, short step);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cyl);
extern short data_02082214[];

void func_ov100_021415bc(char *c)
{
    char *player;
    struct Vector3 v;
    struct Vector3 d;

    *(int *)(c + 0x5c) = *(int *)(c + 0x3d4);
    *(int *)(c + 0x60) = *(int *)(c + 0x3d8);
    *(int *)(c + 0x64) = *(int *)(c + 0x3dc);

    player = (char *)_ZN5Actor13ClosestPlayerEv(c);
    if (player != 0) {
        int *pp;
        int k = 0x5000;
        int py;
        int t;
        int five = 5;
        Vec3_Sub(&d, c + 0x5c, player + 0x5c);
        v.x = d.x;
        v.y = d.y;
        v.z = d.z;
        if (Vec3_HorzLen(&v) > 0x4b0000)
            *(int *)(c + 0x3e4) = 3;

        pp = (int *)(int)M(player + 0x5c);
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);

        v.x = pp[0];
        v.y = pp[1];
        v.z = pp[2];
        t = *(int *)(c + 0x3e8) * k;
        v.x = v.x + t / 4;
        t = *(int *)(c + 0x3e8) * k;
        v.z = v.z + t / 4;
        _Z14ApproachLinearRsss((short *)(c + 0x94),
            Vec3_HorzAngle(c + 0x5c, &v), 0x300);

        pp = (int *)(int)M(player + 0x5c);
        v.x = pp[0];
        py = pp[1];
        v.y = py;
        v.z = pp[2];
        v.y = py + ((*(int *)(c + 0x3e8) * five + 0x100) << 12) / 4;
        _Z14ApproachLinearRsss((short *)(c + 0x92),
            Vec3_VertAngle(c + 0x5c, &v), 0x500);

        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

        {
        int *p = (int *)(c + 0x60);
        *p = *p - ((int)(((long long)*(int *)(c + 0x98)
            * data_02082214[(*(unsigned short *)(c + 0x92) >> 4) * 2] + 0x800) >> 12)
            + (short)data_02082214[
            ((unsigned short)(short)((*(int *)(c + 0x3e8) << 16) / 100) >> 4) * 2 + 1]
            * (short)20 / 4);
        int *cnt = (int *)(c + 0x3e8);
        *cnt = *cnt + 1;
        if (*(int *)(c + 0x3e8) > 100)
            *(int *)(c + 0x3e8) = 0;
        }
        return;
    }
    *(int *)(c + 0x5c) = *(int *)(c + 0x68);
    *(int *)(c + 0x60) = *(int *)(c + 0x6c);
    *(int *)(c + 0x64) = *(int *)(c + 0x70);
}
