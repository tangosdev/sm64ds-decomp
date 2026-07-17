typedef unsigned char u8;
typedef unsigned short u16;

extern void func_ov006_020d5d90(char *o, int i);

#pragma opt_common_subs off
#pragma opt_strength_reduction off

void func_ov006_020d64c8(char *o, int i)
{
    *(u16 *)(o + 0x6268 + i * 16) += 1;
    if (*(u16 *)(o + i * 16 + 0x6268) < 5)
        return;
    *(u16 *)(o + i * 16 + 0x6268) = 0;
    *(u8 *)(o + 0x626f + i * 16) += 1;
    if (i == 0) {
        u8 v = *(u8 *)(o + i * 16 + 0x626f);
        if (v == 2 || v == 4) {
            int *w = (int *)(((int)o + 0x62b4) & 0xFFFFFFFFFFFFFFFF);
            *w += 0x8000;
        }
    }
    if (*(u8 *)(o + i * 16 + 0x626f) == 3 && i == 1)
        func_ov006_020d5d90(o, i);
    if (*(u8 *)(o + i * 16 + 0x626f) >= 4)
        *(u8 *)(o + i * 16 + 0x626c) = 2;
}
