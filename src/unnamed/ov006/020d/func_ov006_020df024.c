#include "types.h"
typedef struct { u8 pad; u8 lo:4; u8 hi:4; } Entry094;
typedef struct { int a, b; } Pair6;

extern int data_0209e650;
extern Entry094 data_ov006_0213c094[];
extern s16 data_ov006_0213c0f4[];
extern Pair6 data_ov006_0213c0c0[];
extern Pair6 data_ov006_0213c0c4[];
extern Pair6 data_ov006_0213c0a8[];
extern Pair6 data_ov006_0213c0ac[];

extern int RandomIntInternal(int *seed);

void func_ov006_020df024(char *a)
{
    u32 rnd = (u32)RandomIntInternal(&data_0209e650);
    u32 r16 = rnd >> 0x10;
    int lr;
    int rem;

    if (r16 & 1) {
        lr = data_ov006_0213c094[*(u8*)(a + 0x5461)].hi;
    } else {
        lr = data_ov006_0213c094[*(u8*)(a + 0x5461)].lo;
    }

    lr = lr * 2;
    if (r16 & 2)
        lr += 1;

    *(s16*)(a + 0x545e) = data_ov006_0213c0f4[lr];

    rem = (rnd >> 0x18) % 3;
    *(int*)(a + 0x542c) = data_ov006_0213c0c0[rem].a;
    *(int*)(a + 0x5430) = data_ov006_0213c0c4[rem].a;

    *(int*)(a + 0x5400) = (data_ov006_0213c0a8[*(int*)(a + 0x5430)].a +
                            data_ov006_0213c0a8[*(int*)(a + 0x542c)].a) / 2;
    *(int*)(a + 0x5404) = (data_ov006_0213c0ac[*(int*)(a + 0x5430)].a +
                            data_ov006_0213c0ac[*(int*)(a + 0x542c)].a) / 2;
    *(int*)(a + 0x5408) = *(int*)(a + 0x5400) -
                           data_ov006_0213c0a8[*(int*)(a + 0x542c)].a;
    *(s16*)(a + 0x545c) = 0;

    {
        u8 kind = *(u8*)(a + 0x5461);
        if (kind == 7) {
            if (*(u8*)(a + 0x5460) == *(u8*)(a + 0x546c)) {
                goto set1;
            }
        }
        if (kind < 7) {
            goto set0;
        }
        {
            u32 r2 = (u32)RandomIntInternal(&data_0209e650);
            if (((r2 >> 0x10) & 7) != 0) {
                goto set0;
            }
        }
    set1:
        *(u8*)(a + 0x546a) = 1;
        return;
    set0:
        *(u8*)(a + 0x546a) = 0;
        return;
    }
}
