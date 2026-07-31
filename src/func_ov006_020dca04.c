typedef unsigned char u8;
typedef unsigned short u16;

#pragma opt_common_subs off
#pragma opt_strength_reduction off

void func_ov006_020dca04(char *o)
{
    int i;
    char *q = o;
    for (i = 0; i < 0x20; i++, q += 0x18) {
        if (*(u8 *)(q + 0x4d28) != 0) {
            if (*(u16 *)(q + 0x4d24) != 0) {
                *(u16 *)(((int)q + 0x4d24)) -= 1;
            } else {
                *(u8 *)(o + i * 0x18 + 0x4d28) = 0;
                *(u8 *)(o + i * 0x18 + 0x4d29) = 0;
                return;
            }
            *(int *)(((int)q + 0x4d14)) += *(int *)(q + 0x4d1c);
            *(int *)(((int)q + 0x4d18)) += *(int *)(q + 0x4d20);
            *(int *)(((int)q + 0x4d20)) += 0x180;
            if (*(int *)(q + 0x4d1c) > 0x300)
                *(int *)(((int)q + 0x4d1c)) -= 0x180;
            else if (*(int *)(q + 0x4d1c) < -0x300)
                *(int *)(((int)q + 0x4d1c)) += 0x180;
        }
    }
}
