#include "types.h"
#define CNT16(b,i) (*(u16*)((b) + 0x4bdc + (i)*0x1c))
#define CNT8(b,i)  (*(u8 *)((b) + 0x4be3 + (i)*0x1c))
#define VEL(b,i)   (*(int*)((b) + 0x4bd4 + (i)*0x1c))
#define ACC(b,i)   (*(int*)((b) + 0x4bcc + (i)*0x1c))
#define F15(b,i)   (*(u8 *)((b) + 0x4be1 + (i)*0x1c))
#define F12(b,i)   (*(u16*)((b) + 0x4bde + (i)*0x1c))
#define F16(b,i)   (*(u8 *)((b) + 0x4be2 + (i)*0x1c))
#define F19(b,i)   (*(u8 *)((b) + 0x4be5 + (i)*0x1c))

void func_ov006_0211d0f8(char* base, int i)
{
    int v;
    CNT16(base, i)++;
    if (CNT16(base, i) >= 4) {
        CNT16(base, i) = 0;
        CNT8(base, i)++;
        if (CNT8(base, i) >= 4)
            CNT8(base, i) = 0;
    }
    ACC(base, i) += VEL(base, i);
    v = ACC(base, i) >> 0xc;
    if (v >= 0xc0) {
        if (VEL(base, i) >= 0x800)
            VEL(base, i) -= 0x80;
    } else {
        if (VEL(base, i) <= 0x7000)
            VEL(base, i) += 0x400;
    }
    if (v >= 0x120) {
        VEL(base, i) = 0;
        ACC(base, i) = 0x120000;
        F15(base, i) = 4;
        F12(base, i) = 8;
        F16(base, i) = 0;
        if (F19(base, i) != 0)
            F15(base, i) = 6;
    }
}
