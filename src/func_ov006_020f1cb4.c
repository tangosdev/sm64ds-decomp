#include "types.h"
extern int data_ov006_0212e8c8[];
extern s16 data_02082214[];

void func_ov006_020f1dbc(char* c, int i);

void func_ov006_020f1cb4(char* c, int i) {
    u8* counter = (u8*)(c + 0x5275);
    if (counter[i] == 0) {
        *(u16*)(c + i * 2 + 0x4f7c) = 0x6000;
        counter[i]++;
        return;
    }
    {
        u8* spd = (u8*)(c + 0x5365);
        int* xs = (int*)(c + 0x47f8);
        int a = (int)*(u16*)(c + i * 2 + 0x4f7c) >> 4;
        xs[i] = xs[i] + (int)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e8c8[spd[i]] + 0x800) >> 12);
        int* ys = (int*)(c + 0x49d8);
        a = (int)*(u16*)(c + i * 2 + 0x4f7c) >> 4;
        ys[i] = ys[i] + (int)(((s64)data_02082214[a * 2] * data_ov006_0212e8c8[spd[i]] + 0x800) >> 12);
    }
    func_ov006_020f1dbc(c, i);
}
