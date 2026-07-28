//cpp
typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern "C" int _Z15ApproachLinear2Rsss(s16 &v, s16 target, s16 step);
extern "C" void Math_Function_0203b0fc(int *p, int target, int scale, int max);
extern "C" void func_ov006_020ec134(void *self);

struct Self;
typedef void (Self::*Pmf)();

struct Self {
    char pad10[0x10];
    Pmf cb;
};

extern "C" void func_ov006_020eb0c8(char *self) {
    if (_Z15ApproachLinear2Rsss(*(s16 *)(self + 0x90), 0, 1)) {
        *(u8 *)(self + 0x94) = 0;
        *(u8 *)(self + 0x93) = 1;
        *(s16 *)(self + 0x8e) = 0x21;
        *(s32 *)(self + 0x44) = 0x1000;
        *(s16 *)(self + 0x86) = 0x900;
        *(u8 *)(self + 0x95) = 6;
    } else {
        (((Self *)self)->*(((Self *)self)->cb))();
        if (*(s16 *)(self + 0x90) == 0x30) {
            if (*(u8 *)(self + 0x95) != 4) *(u8 *)(self + 0x95) = 3;
        }
    }

    if (*(u8 *)(self + 0x95) != 6) return;

    int *cnt = (int *)(self + 0x60);
    *cnt = *cnt + 1;

    int i = 0;
    int scale = 0x200;
    int *q = (int *)(self + 0x4c);
    do {
        if (*(s32 *)(self + 0x60) > (5 - i) * 3) {
            Math_Function_0203b0fc(q, 0x9800, 0x400, scale);
        }
        scale += 0x140;
        q += 1;
        i++;
    } while (i < 5);

    func_ov006_020ec134(self);
}
