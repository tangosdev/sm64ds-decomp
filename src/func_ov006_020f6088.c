typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;

extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern s16 data_02082214[];
extern u16* data_ov006_0213d338[];

void func_ov006_020f6088(char* self, int i)
{
    int i2 = i * 2;
    u16* row = data_ov006_0213d338[*(u8*)(self + 0x540a)];
    int i18 = i * 0x18;
    int dx, dy, a, b;
    a = row[i2];
    dx = a - (*(int*)(self + 0x51a8 + i18) >> 12);
    b = row[i2 + 1];
    dy = b - (*(int*)(self + 0x51ac + i18) >> 12);

    *(u16*)(self + 0x51b4 + i18) = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    {
        s16 tv = data_02082214[((*(u16*)(self + 0x51b4 + i18) >> 4) << 1) + 1];
        *(int*)(self + 0x51a8 + i18) += (int)(((s64)tv * *(int*)(self + 0x51b0 + i18) + 0x800) >> 0xc);
    }
    {
        s16 tv = data_02082214[(*(u16*)(self + 0x51b4 + i18) >> 4) << 1];
        *(int*)(self + 0x51ac + i18) += (int)(((s64)tv * *(int*)(self + 0x51b0 + i18) + 0x800) >> 0xc);
    }

    if (dx < -6)
        return;
    if (dx > 6)
        return;
    if (dy < -6)
        return;
    if (dy > 6)
        return;

    *(int*)(self + 0x51a8 + i18) = a << 12;
    *(int*)(self + 0x51ac + i18) = b << 12;
    *(u8*)(self + i18 + 0x51bc) = 1;
}
