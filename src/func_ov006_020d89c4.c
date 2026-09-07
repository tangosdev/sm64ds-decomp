#include "types.h"
extern void func_ov006_020d836c(char* c);
extern int func_ov004_020adbc0(void);
extern void func_ov004_020adb1c(int a);
extern void SetBg0Offset(int a, int b);

#pragma opt_strength_reduction off
void func_ov006_020d89c4(char* self)
{
    int j;

    func_ov006_020d836c(self);

    if (*(u16*)(self + 0x62e8) != 0) {
        (*(u16*)(self + 0x62e8))--;
        if (*(s16*)(self + 0x62e8) < 0)
            *(u16*)(self + 0x62e8) = 0;
        return;
    }

    for (j = 0; j < 0x70; j++) {
        u8 *row = (u8*)self + j*0x40;
        u8 *q = (u8*)(row + 0x4698);
        if (*q == 2) {
            if (((u8 (*)[0x40])(self + 0x4000))[j][0x697] == 6
             && ((u8 (*)[0x40])(self + 0x4000))[j][0x69b] == 4) {
                *q = 0;
                ((u8 (*)[0x40])(self + 0x4000))[j][0x699] = 0;
            }
        }
    }

    if (*(u8*)(self + 0x62f8) != 0) {
        *(u16*)(self + 0x62e8) = 0x60;
        *(int*)(self + 0x62d0) = 4;
        *(int*)(self + 0x62d4) = 0;
        func_ov004_020adb1c(*(u16*)(self + 0x62ee) + func_ov004_020adbc0());
    } else {
        func_ov004_020adb1c(*(u16*)(self + 0x62ee) + func_ov004_020adbc0());
        *(int*)(self + 0x62d0) = 2;
        *(int*)(self + 0x62d4) = 0;
        *(u8*)(self + 0x62f8) = 0;
        *(u8*)(self + 0x62fb) = 0;
    }
    *(u8*)(self + 0x62f9) = 0;
    *(u16*)(self + 0x62ee) = 0;
    SetBg0Offset(0, 0);
}
