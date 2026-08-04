#include "types.h"
void func_ov006_0211d224(char* c, int i) {
    int b = i * 0x1c;
    int t;

    (*(u16*)(c + 0x4bdc + b))++;
    if (*(u16*)(c + 0x4bdc + b) >= 4) {
        *(u16*)(c + 0x4bdc + b) = 0;
        (*(u8*)(c + 0x4be3 + b))++;
        if (*(u8*)(c + 0x4be3 + b) >= 4) {
            *(u8*)(c + 0x4be3 + b) = 0;
        }
    }

    *(s32*)(c + 0x4bcc + b) = *(s32*)(c + 0x4bcc + b) + *(s32*)(c + 0x4bd4 + b);
    t = *(s32*)(c + 0x4bcc + b) >> 0xc;
    if (t <= 0x40) {
        if (*(s32*)(c + 0x4bd4 + b) <= -0x800) {
            *(s32*)(c + 0x4bd4 + b) = *(s32*)(c + 0x4bd4 + b) + 0x80;
        }
    } else {
        if (*(s32*)(c + 0x4bd4 + b) >= -0x7000) {
            *(s32*)(c + 0x4bd4 + b) = *(s32*)(c + 0x4bd4 + b) - 0x400;
        }
    }
    if (t > -0x20) {
        return;
    }
    *(s32*)(c + 0x4bcc + b) = -0x20000;
    *(s32*)(c + 0x4bd4 + b) = 0;
    *(u8*)(c + 0x4be1 + b) = 4;
    *(u16*)(c + 0x4bde + b) = 8;
    *(u8*)(c + 0x4be2 + b) = 0;
    if (*(u8*)(c + 0x4be5 + b) != 0) {
        *(u8*)(c + 0x4be1 + b) = 6;
    }
}
