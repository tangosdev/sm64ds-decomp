typedef unsigned char u8;
typedef unsigned short u16;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern void func_ov006_02102d6c(char *o, int i);

void func_ov006_02103608(char *o, int i)
{
    int v660, v664, t660;
    *(int *)((char *)(((int)o + 0x4660) & 0xFFFFFFFFFFFFFFFF) + i * 0x40) += *(int *)(o + i * 0x40 + 0x4668);
    *(int *)((char *)(((int)o + 0x4664) & 0xFFFFFFFFFFFFFFFF) + i * 0x40) += *(int *)(o + i * 0x40 + 0x466c);
    *(int *)((char *)(((int)o + 0x4684) & 0xFFFFFFFFFFFFFFFF) + i * 0x40) += 0x20;
    *(int *)((char *)(((int)o + 0x466c) & 0xFFFFFFFFFFFFFFFF) + i * 0x40) += *(int *)(o + i * 0x40 + 0x4684);
    *(u16 *)((char *)(((int)o + 0x4690) & 0xFFFFFFFFFFFFFFFF) + i * 0x40) += *(u16 *)(o + i * 0x40 + 0x4692);
    if (*(int *)(o + i * 0x40 + 0x466c) >= 0x8000)
        *(int *)(o + i * 0x40 + 0x466c) = 0x8000;
    t660 = *(int *)(o + i * 0x40 + 0x4664);
    v660 = *(int *)(o + i * 0x40 + 0x4660) >> 12;
    v664 = t660 >> 12;
    if (v664 <= -0x140)
        *(u8 *)(o + i * 0x40 + 0x4699) = 6;
    if (v660 >= 0x140 || v660 <= -0x40)
        *(u8 *)(o + i * 0x40 + 0x4699) = 6;
    if (v664 >= 0xa8)
        func_ov006_02102d6c(o, i);
}
