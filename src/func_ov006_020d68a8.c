typedef unsigned char u8;
typedef unsigned short u16;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

void func_ov006_020d68a8(char *o, int a1)
{
    int i;
    u8 *cur = (u8 *)(((int)(o + a1 * 0x40) + 0x469a) & 0xFFFFFFFFFFFFFFFF);
    for (i = 0; i < 0x70; i++) {
        char *q = o + i * 0x40;
        if (*(u8 *)(q + 0x4698) != 0) {
            if (i != a1) {
                u8 n = *cur;
                if (n == 0) {
                    u8 *st = (u8 *)(((int)q + 0x4697) & 0xFFFFFFFFFFFFFFFF);
                    if (*st != 5) {
                        *st = 3;
                        *(u16 *)(q + 0x4690) = 0x20;
                    }
                } else {
                    u8 *st = (u8 *)(((int)q + 0x4697) & 0xFFFFFFFFFFFFFFFF);
                    if (*st == 5) {
                        if (*(u8 *)(q + 0x4696) + 1 == n) {
                            *st = 3;
                            *(u16 *)(q + 0x4690) = 0x20;
                        }
                    }
                    if (*st <= 1) {
                        *st = 3;
                        *(u16 *)(o + i * 0x40 + 0x4690) = 0x20;
                    }
                }
            }
        }
    }
    if (*(u8 *)(o + 0x62f8) != 0)
        return;
    *(u8 *)(o + 0x62f8) = *(u8 *)(o + a1 * 0x40 + 0x469a) + 1;
    *(u16 *)(o + 0x62e8) = 0x60;
    *(int *)(o + 0x62d0) = 3;
    *(int *)(o + 0x62d4) = 0;
}
