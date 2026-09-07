//cpp
#include "types.h"
struct C;
typedef void (C::*PMF0)();
typedef void (C::*PMF1)(int);

extern "C" {
extern PMF0 data_ov006_02141978[];
extern PMF1 data_ov006_021419d8[];
}

extern "C" void func_ov006_020e6354(char* c)
{
    if (*(u16*)(c + 0x55b6) != 0) {
        u16* q = (u16*)(c + 0x55b6);
        *q = *q - 1;
        return;
    }
    if (*(u8*)(c + 0xc4) == 0) {
        *(u8*)(c + 0xc3) = 1;
        *(u8*)(c + 0xc4) = 1;
        *(u16*)(c + 0xc0) = 0;
    }
    if (*(u8*)(c + 0x55bd) != 0) {
        u8* q = (u8*)(c + 0x55bd);
        *q = *q - 1;
    }
    (((C*)c)->*data_ov006_02141978[*(u8*)(c + 0x55b8)])();

    {
        int count = 0;
        int i = 0;
        char* p = c;
        for (; i < 0xb; i++, p += 0x30) {
            if (*(u8*)(p + 0x4689) != 0) {
                *(int*)(p + 0x466c) = *(int*)(p + 0x4660);
                *(int*)(p + 0x4670) = *(int*)(p + 0x4664);
                if (*(u16*)(p + 0x4680) != 0) {
                    u16* q = (u16*)(p + 0x4680);
                    *q = *q - 1;
                }
                (((C*)c)->*data_ov006_021419d8[*(u8*)(p + 0x4688)])(i);
                if (*(u8*)(p + 0x4688) != 2) count++;
            }
        }
        if (count != 0) return;
    }
    *(int*)(c + 0x5580) = 2;
    *(u16*)(c + 0x55b6) = 0x40;
}
