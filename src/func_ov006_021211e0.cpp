//cpp
extern "C" {
int func_ov006_020e6e78(char* self);
void SetBg2Offset(int a, int b);
int func_ov004_020b04c0(void);
}
extern "C" unsigned char TOP_SCREEN_RELATED;

extern "C" int func_ov006_021211e0(char* self)
{
    if (!func_ov006_020e6e78(self))
        return 0;
    if (*(int*)(self + 0x4628) == 0) {
        if (*(unsigned short*)(self + 0x4664) == 0) {
            TOP_SCREEN_RELATED &= ~8;
            SetBg2Offset(0, *(int*)(self + 0x5d94) + *(int*)(self + 0x5da0));
            *(volatile int*)0x4000018 = (*(int*)(self + 0x5d94) + *(int*)(self + 0x5da0)) << 16 & 0x1ff0000;
        } else {
            TOP_SCREEN_RELATED |= 8;
            SetBg2Offset(0, *(int*)(self + 0x5d94) + 0xc0 + *(int*)(self + 0x5da0) + func_ov004_020b04c0());
            *(volatile int*)0x4000018 = (*(int*)(self + 0x5d94) + 0xc0 + *(int*)(self + 0x5da0) + func_ov004_020b04c0()) << 16 & 0x1ff0000;
        }
    }
    return 1;
}
