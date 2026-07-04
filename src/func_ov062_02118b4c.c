typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern void func_ov062_02117724(void *c, int a, int b, int d, int e);
extern int func_ov062_02117b60(void *c);
extern int _Z14ApproachLinearRiii(int *val, int target, int step);
extern int _Z14ApproachLinearRsss(s16 *val, int target, int step);
extern void func_ov062_02117994(char *c, int idx);

void func_ov062_02118b4c(char *self) {
    if (*(u8 *)(self + 0x398) == 3)
        func_ov062_02117724(self, 2, 5, 8, 0xb);

    if (*(int *)(self + 0x394) != 0) {
        if (*(u16 *)(self + 0x100) > 0x1e && func_ov062_02117b60(self) > 0x320000) {
            if (_Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x1000) == 0)
                return;
            *(int *)(self + 0x38c) = 1;
            func_ov062_02117994(self, 2);
            return;
        }
        _Z14ApproachLinearRsss((s16 *)(self + 0x94), *(s16 *)(self + 0x3c0), 0x400);
        _Z14ApproachLinearRiii((int *)(self + 0x98), 0x11000, 0x1000);
        return;
    }

    if (*(int *)(self + 0x3b8) >= 0x61a8000) {
        *(s16 *)(((int)self + 0x3c0) & 0xFFFFFFFFFFFFFFFF) += 0x8000;
        *(int *)(self + 0x3b8) = 0;
    }
    if (*(u16 *)(self + 0x100) > 0x1e && *(int *)(self + 0x3b8) > 0x320000) {
        if (_Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x1000) == 0)
            return;
        *(int *)(self + 0x38c) = 1;
        func_ov062_02117994(self, 2);
        return;
    }
    _Z14ApproachLinearRsss((s16 *)(self + 0x94), (s16)(*(s16 *)(self + 0x3c0) + 0x8000), 0x400);
    _Z14ApproachLinearRiii((int *)(self + 0x98), 0x11000, 0x1000);
}
