typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern s16 data_02082214[];

void func_ov006_02102c3c(char *o, int x, int z, int d)
{
    int i;
    for (i = 0; i < 0x30; i++) {
        char *q = o + i * 0x40;
        if (*(u8 *)(q + 0x4698) == 0) {
            *(u8 *)(q + 0x4698) = 1;
            *(u8 *)(q + 0x4699) = 7;
            *(u16 *)(q + 0x4696) = 0x40;
            *(u8 *)(q + 0x469a) = 1;
            *(u8 *)(q + 0x469c) = 0;
            if (d != 0)
                *(u16 *)(q + 0x4694) = 0xd000;
            else
                *(u16 *)(q + 0x4694) = 0xb000;
            *(int *)(o + i * 0x40 + 0x4660) = x << 12;
            *(int *)(o + i * 0x40 + 0x4664) = (z - 0x12) << 12;
            *(int *)(o + i * 0x40 + 0x4668) = (int)(((s64)data_02082214[((*(u16 *)(o + i * 0x40 + 0x4694)) >> 4) * 2 + 1] * 0x2000 + 0x800) >> 12);
            *(int *)(o + i * 0x40 + 0x466c) = (int)(((s64)data_02082214[((*(u16 *)(o + i * 0x40 + 0x4694)) >> 4) * 2] * 0x2000 + 0x800) >> 12);
            *(int *)(o + i * 0x40 + 0x4684) = 0;
            _ZN5Sound12PlayBank2_2DEj(0x199);
            return;
        }
    }
}
