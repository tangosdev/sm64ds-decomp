#include "types.h"
extern int Vec3_HorzLen(void *v);
extern int AngleDiff(int a, int b);
extern int func_ov002_020c031c(void *c);
extern int func_ov002_020f035c(u32 sel, int r1);
extern int _ZN6Player7IsStateERNS_5StateE(void *thiz, void *st);

extern char data_ov002_0211010c;
extern char data_ov002_02110304;

int func_ov002_020c04e0(char *c)
{
    int horzLen;
    int angDiff;
    int sel;
    int accelBase;
    int speed;
    int accel;
    int ret;

    horzLen = Vec3_HorzLen(c + 0x554);
    angDiff = AngleDiff(*(s16 *)(c + 0x69a), *(s16 *)(c + 0x94));

    if (*(u16 *)(c + 0x6b8) != 0) {
        if (*(int *)(c + 0x658) == 0)
            return *(int *)(c + 0x658);
    }

    sel = func_ov002_020c031c(c);
    ret = func_ov002_020f035c(sel, *(int *)(c + 0x558));
    if (ret == 0)
        return ret;

    sel = 0;
    if (!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211010c))
        sel = func_ov002_020c031c(c);
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110304)) {
        if (sel == 4)
            sel = 0;
    }

    switch (sel) {
    case 4:
    case 5:
        accelBase = 0x54cc;
        break;
    case 3:
        accelBase = 0x2b33;
        break;
    case 0:
    default:
        accelBase = 0x1b33;
        break;
    case 1:
    case 2:
        accelBase = 0;
        break;
    }

    speed = *(int *)(c + 0x98) - 0x13000;
    if (speed < 0x1000)
        speed = 0x1000;
    if (speed >= 0x1800)
        speed = 0x1800;

    accel = (int)(((long long)accelBase * speed + 0x800) >> 12);

    if (angDiff < 0x4000) {
        int *p = (int *)(((int)(c + 0x98)));
        int v = *p + (int)(((long long)accel * horzLen + 0x800) >> 12);
        *p = v;
        return v;
    } else {
        int *p = (int *)(((int)(c + 0x98)));
        int v = *p - (int)(((long long)accel * horzLen + 0x800) >> 12);
        *p = v;
        return v;
    }
}
