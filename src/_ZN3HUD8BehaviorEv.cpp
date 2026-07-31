//cpp
#include "types.h"
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};

extern "C" {
extern VObj *data_0209f5bc;
extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern u8 data_ov002_02111178;
extern u8 data_0209f2fc;
int NumStars();
}

struct HUD {
    int Behavior();
    void UpdateVsTimer();
    void UpdateHealthMeter();
};

int HUD::Behavior()
{
    int t;

    if (data_0209f5bc->v5() == 0)
        goto ret;
    t = (data_0209f2d8 == 1);
    if (t != false)
        goto body;
    if ((data_0209caa0[2] & 0x80) != 0)
        goto body;
ret:
    return 1;
body:
    if (t != false) {
        UpdateVsTimer();
    } else {
        int flags;
        int n;

        UpdateHealthMeter();
        n = NumStars();
        flags = 0;
        if (n > 0x63)
            n = 0x1b;
        else if (n > 9)
            n = 0x12;
        else
            n = 9;

        switch (data_ov002_02111178) {
        case 2:
            data_ov002_02111178 = 3;
            break;
        case 3:
            if (*(s16 *)((char *)this + 0x6e) >= 0x10) {
                *(s16 *)((char *)this + 0x6e) = 0x10;
                flags = 1;
            } else {
                *(s16 *)(((int)this + 0x6e) & 0xFFFFFFFFFFFFFFFFULL) += 4;
            }
            if (data_0209f2fc == 1) {
                if (*(s16 *)((char *)this + 0x70) <= 0xf0) {
                    *(s16 *)((char *)this + 0x70) = 0xf0;
                    flags |= 2;
                } else {
                    *(s16 *)(((int)this + 0x70) & 0xFFFFFFFFFFFFFFFFULL) -= 4;
                }
            } else {
                flags |= 2;
            }
            if (flags == 3)
                data_ov002_02111178 = 4;
            break;
        case 5:
            if (*(u16 *)((char *)this + 0x6a) == 0) {
                if (*(s16 *)((char *)this + 0x6e) < -0x38) {
                    flags = 1;
                } else {
                    *(s16 *)(((int)this + 0x6e) & 0xFFFFFFFFFFFFFFFFULL) -= 4;
                }
                if (data_0209f2fc == 1) {
                    if (*(s16 *)((char *)this + 0x70) <= n + 0x120) {
                        *(s16 *)(((int)this + 0x70) & 0xFFFFFFFFFFFFFFFFULL) += 4;
                    } else {
                        flags |= 2;
                    }
                } else {
                    flags |= 2;
                }
                if (flags == 3)
                    data_ov002_02111178 = 6;
            }
            break;
        case 6:
            if (*(s16 *)((char *)this + 0x6e) >= 0x10) {
                *(s16 *)((char *)this + 0x6e) = 0x10;
                flags = 1;
            } else {
                *(s16 *)(((int)this + 0x6e) & 0xFFFFFFFFFFFFFFFFULL) += 4;
            }
            if (data_0209f2fc == 1 || *(u8 *)((char *)this + 0x78) != 0) {
                if (*(s16 *)((char *)this + 0x70) <= 0xf0) {
                    *(s16 *)((char *)this + 0x70) = 0xf0;
                    flags |= 2;
                } else {
                    *(s16 *)(((int)this + 0x70) & 0xFFFFFFFFFFFFFFFFULL) -= 4;
                }
            } else {
                flags |= 2;
            }
            if (flags == 3) {
                data_ov002_02111178 = 0;
                *(u8 *)((char *)this + 0x78) = 0;
            }
            break;
        }
    }
    return 1;
}
