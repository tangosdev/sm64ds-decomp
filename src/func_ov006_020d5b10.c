#include "types.h"
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern void func_ov006_020d8904(char* p);
extern void SetBg2Offset(int a, int b);

void func_ov006_020d5b10(char *c)
{
    u8 state = *(u8*)(c+0x62f4);

    if (state == 0) {
        return;
    }

    if (state == 1) {
        s32 v;
        s32 shifted;
        *(s32*)(((long long)(int)(c + 0x62dc))) += 0x3000;
        v = *(s32*)(c+0x62dc);
        shifted = v >> 0xc;
        if (shifted >= 0xc0) {
            *(s32*)(c+0x62dc) = 0xc0000;
            *(u8*)(c+0x62f4) = 2;
            shifted = 0xc0;
            *(u16*)(c+0x62f2) = shifted;
            _ZN5Sound12PlayBank2_2DEj(0x1de);
            func_ov006_020d8904(c);
        }
        SetBg2Offset(0, shifted);
        return;
    }

    if (state != 2) {
        return;
    }

    if (*(u16*)(c+0x62f2) != 0) {
        *(u16*)(((long long)(int)(c + 0x62f2))) -= 1;
        if (*(u16*)(c+0x62f2) != 0) {
            return;
        }
        _ZN5Sound12PlayBank2_2DEj(0x1df);
        return;
    }

    {
        s32 v;
        s32 shifted;
        *(s32*)(((long long)(int)(c + 0x62dc))) -= 0x3000;
        v = *(s32*)(c+0x62dc);
        shifted = v >> 0xc;
        if (shifted <= 0) {
            shifted = 0;
            *(s32*)(c+0x62dc) = 0;
            *(u8*)(c+0x62f4) = 0;
            _ZN5Sound12PlayBank2_2DEj(0x1de);
        }
        SetBg2Offset(0, shifted);
    }
}
