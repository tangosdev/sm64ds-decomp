typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern void func_ov006_020d634c(char *o, int i);
extern s16 data_02082214[];

void func_ov006_020d795c(char *o, int i)
{
    int b, v;
    {
        s16 tv = data_02082214[((*(u16 *)(o + i * 0x40 + 0x468c)) >> 4) * 2 + 1];
        *(int *)((char *)(((int)o + 0x4660) & 0xFFFFFFFFFFFFFFFF) + i * 0x40) +=
            (int)(((s64)tv * *(int *)(o + i * 0x40 + 0x4670) + 0x800) >> 12);
    }
    {
        s16 tv = data_02082214[((*(u16 *)(o + i * 0x40 + 0x468c)) >> 4) * 2];
        *(int *)((char *)(((int)o + 0x4664) & 0xFFFFFFFFFFFFFFFF) + i * 0x40) +=
            (int)(((s64)tv * *(int *)(o + i * 0x40 + 0x4670) + 0x800) >> 12);
    }
    b = *(u8 *)(o + i * 0x40 + 0x4696);
    v = *(int *)(o + i * 0x40 + 0x4660) >> 12;
    if (b != 0) {
        if (v >= 0x110) {
            *(u8 *)(o + i * 0x40 + 0x469b) = 2;
            *(int *)(o + i * 0x40 + 0x4660) = 0x80000;
            *(int *)(o + i * 0x40 + 0x4664) = -0xf0000;
        }
    } else {
        if (v <= -0x10) {
            *(u8 *)(o + i * 0x40 + 0x469b) = 2;
            *(int *)(o + i * 0x40 + 0x4660) = 0x80000;
            *(int *)(o + i * 0x40 + 0x4664) = -0xf0000;
        }
    }
    func_ov006_020d634c(o, *(u8 *)(o + i * 0x40 + 0x4696));
}
