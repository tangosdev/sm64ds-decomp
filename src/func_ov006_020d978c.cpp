//cpp
typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern "C" {
extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern int _Z15ApproachLinear2Rsss(s16 *a, s16 b, s16 c);
extern int func_ov006_020da8e4(void);
}

struct Obj {
    virtual void m00();
    virtual void m04();
    virtual int m08();
};

extern "C" void func_ov006_020d978c(void *arg0)
{
    char *c = (char *)arg0;

    if (*(u8 *)(c + 0x2b) == 0)
        return;

    if (*(u8 *)(c + 0x2c) != 0)
        _Z14ApproachLinearRiii((int *)(c + 0x24), 0x4000, 0x200);
    else
        _Z14ApproachLinearRiii((int *)(c + 0x24), 0, 0x200);

    u8 state = *(u8 *)(c + 0x2b);

    if (state == 5) {
        int r5 = _Z14ApproachLinearRiii((int *)(c + 8), *(s32 *)(c + 0x10), *(s32 *)(c + 0x18));
        int r0 = _Z14ApproachLinearRiii((int *)(c + 4), *(s32 *)(c + 0xc), *(s32 *)(c + 0x14));
        if (r5 == 0)
            return;
        if (r0 != 0)
            *(u8 *)(c + 0x2b) = 1;
        return;
    }

    if (state == 4 && *(s32 *)(c + 0x24) == 0) {
        int v = ((struct Obj *)c)->m08();
        int r0 = _Z14ApproachLinearRiii((int *)(c + 8), v, 0x8000);
        if (r0 == 0)
            return;
        *(s32 *)(c + 4) = 0x80000;
        *(u8 *)(c + 0x2b) = 6;
        *(u8 *)(c + 0x2a) = func_ov006_020da8e4();
        return;
    }

    if (state == 7) {
        int r0 = _Z14ApproachLinearRiii((int *)(c + 4), *(s32 *)(c + 0x1c), 0x6000);
        if (r0 != 0)
            *(u8 *)(c + 0x2b) = 8;
        return;
    }

    if (state == 9) {
        _Z14ApproachLinearRiii((int *)(c + 8), 0x10000, 0x6000);
        return;
    }

    if (state == 1) {
        if (*(s16 *)(c + 0x28) != 0) {
            _Z15ApproachLinear2Rsss((s16 *)(c + 0x28), 0, 1);
            return;
        }
        _Z14ApproachLinearRiii((int *)(c + 8), *(s32 *)(c + 0x10), *(s32 *)(c + 0x18));
        _Z14ApproachLinearRiii((int *)(c + 4), *(s32 *)(c + 0xc), *(s32 *)(c + 0x14));
        return;
    }

    if (state != 0xa)
        return;

    if (_Z15ApproachLinear2Rsss((s16 *)(c + 0x28), 0, 1) == 0)
        return;

    _Z14ApproachLinearRiii((int *)(c + 4), *(s32 *)(c + 0x1c), *(s32 *)(c + 0x14));
}
