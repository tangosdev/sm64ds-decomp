typedef unsigned char u8;
typedef unsigned short u16;

#pragma opt_common_subs off

void func_ov006_020f10ec(char *q)
{
    int i;
    for (i = 0; i < 0x10; i++, q += 0x14) {
        if (*(u8 *)(q + 0x4670) != 0) {
            if (*(u8 *)(q + 0x4672) == 0) {
                if (*(u16 *)(q + 0x466c) != 0) {
                    *(u16 *)(((int)q + 0x466c) & 0xFFFFFFFFFFFFFFFF) -= 1;
                    if (*(short *)(q + 0x466c) < 0)
                        *(u16 *)(q + 0x466c) = 0;
                    *(int *)(((int)q + 0x4664) & 0xFFFFFFFFFFFFFFFF) += *(int *)(q + 0x4668);
                    *(int *)(((int)q + 0x4668) & 0xFFFFFFFFFFFFFFFF) += 0x100;
                } else {
                    *(u16 *)(q + 0x466c) = 0x40;
                    *(u8 *)(((int)q + 0x4672) & 0xFFFFFFFFFFFFFFFF) += 1;
                }
            } else {
                if (*(u16 *)(q + 0x466c) != 0) {
                    *(u16 *)(((int)q + 0x466c) & 0xFFFFFFFFFFFFFFFF) -= 1;
                    if (*(short *)(q + 0x466c) < 0)
                        *(u16 *)(q + 0x466c) = 0;
                } else {
                    *(u16 *)(q + 0x466c) = 0;
                    *(u8 *)(q + 0x4670) = 0;
                    *(u8 *)(q + 0x4671) = 0;
                }
            }
        }
    }
}
