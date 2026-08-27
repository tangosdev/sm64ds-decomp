#include "types.h"
struct BF { u16 pad : 7; u16 b7 : 1; u16 b8 : 1; u16 b9 : 1; u16 rest : 6; };

enum { false, true };

extern int NumVsStarsObtained(void);
extern void _ZN9PowerStar13AddStarMarkerEv(char *self);
extern void func_02012790(int id);
extern int _Z14ApproachLinearRiii(int *v, int target, int step);
extern void _ZN8dActor_c11UntrackStarERa(char *self, char *p);
extern void func_ov002_020e7e14(char *self);
extern void func_ov002_020e930c(char *self);
extern int data_0209b454;

void func_ov002_020ea420(char *c) {
    int spd;
    int spd2;
    int en;
    int lim;
    int step;

    if (*(int *)(c + 0x43c) == 6) {
        en = 0;
        if ((unsigned int)(*(u16 *)(c + 0x4a2) << 24) >> 31 == 0 && NumVsStarsObtained() == 5) {
            lim = 0x14;
            step = 0x100;
            en = 1;
        } else {
            if (((struct BF *)(c + 0x4a2))->b7 != 0 && ((struct BF *)(c + 0x4a2))->b8 != 0) {
                lim = 5;
                step = 0x200;
                en = 1;
            }
        }
        if (en != 0) {
            if (*(int *)(c + 0x80) != 0x1000 && (data_0209b454 & 0x4000000) == 0) {
                *(int *)(c + 0xb0) |= 0x4000000;
                data_0209b454 |= 0x4000000;
            } else if (*(int *)(c + 0x80) == 0x1000) {
                *(u16 *)(c + 0x492) = lim + 0xb;
                *(u16 *)(c + 0x4a2) |= 0x200;
                _ZN9PowerStar13AddStarMarkerEv(c);
                *(int *)(c + 0x128) &= ~1;
            }
            if (*(u16 *)(c + 0x492) < (unsigned int)lim) {
                *(u16 *)(c + 0x492) += 1;
                if (*(u16 *)(c + 0x492) == lim) {
                    if (*(u16 *)(c + 0x496) == 0xffff)
                        *(u16 *)(c + 0x496) = 0x64;
                }
            } else if (*(int *)(c + 0x80) != 0x1000) {
                spd = *(int *)(c + 0x80);
                if (*(u16 *)(c + 0x492) >= lim + 0xa) {
                    if (_Z14ApproachLinearRiii(&spd, 0x1000, step) != 0) {
                        _ZN9PowerStar13AddStarMarkerEv(c);
                        *(int *)(c + 0x128) &= ~1;
                    }
                } else {
                    *(u16 *)(c + 0x492) += 1;
                }
                {
                    int v = spd;
                    *(int *)(c + 0x80) = v;
                    *(int *)(c + 0x84) = v;
                    *(int *)(c + 0x88) = v;
                }
            } else {
                if ((unsigned int)(*(u16 *)(c + 0x4a2) << 24) >> 31 == 0)
                    *(u16 *)(c + 0x492) = 0x3d;
                else
                    *(u16 *)(c + 0x492) = 0;
            }
            if (*(u16 *)(c + 0x492) >= lim + 0xa &&
                (unsigned int)(*(u16 *)(c + 0x4a2) << 22) >> 31 == 0) {
                *(u16 *)(c + 0x4a2) |= 0x200;
                func_02012790(0x41);
            }
            *(u16 *)(c + 0x100) = 0;
        } else {
            *(int *)(c + 0x128) |= 1;
            if (*(u16 *)(c + 0x492) != 0) {
                *(u16 *)(c + 0x492) -= 1;
            } else if (*(int *)(c + 0x80) != 0) {
                int step2;
                if (*(u16 *)(c + 0x100) == 0)
                    func_02012790(0x42);
                spd2 = *(int *)(c + 0x80);
                if (*(u16 *)((int)(c + 0x100)) <= 0xf) {
                    *(u16 *)((int)(c + 0x100)) += 1;
                    step2 = 0;
                } else {
                    step2 = 0x100;
                }
                if (_Z14ApproachLinearRiii(&spd2, 0, step2) != 0) {
                    *(u16 *)(c + 0x4a2) &= ~0x200;
                    _ZN8dActor_c11UntrackStarERa(c, c + 0x498);
                }
                {
                    int v2 = spd2;
                    *(int *)(c + 0x80) = v2;
                    *(int *)(c + 0x84) = v2;
                    *(int *)(c + 0x88) = v2;
                }
                if ((data_0209b454 & 0x4000000) == 0) {
                    *(int *)(c + 0xb0) |= 0x4000000;
                    data_0209b454 |= 0x4000000;
                    *(u16 *)(c + 0x496) = 0x64;
                }
            }
        }
    } else {
        int t = *(u16 *)(c + 0xc);
        t = t == 0xb3;
        if (t != false) {
            if (*(int *)(c + 0x60) <= *(int *)(c + 0x458)) {
                *(int *)(c + 0x60) = *(int *)(c + 0x458);
                *(int *)(c + 0xa8) = 0x10000;
            }
        } else {
            func_ov002_020e7e14(c);
        }
    }
    func_ov002_020e930c(c);
}
