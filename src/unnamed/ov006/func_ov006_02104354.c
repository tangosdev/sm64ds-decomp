#include "types.h"
#pragma opt_common_subs off

void func_ov006_02104354(char *q)
{
    int i;
    for (i = 0; i < 0x40; i++, q += 0x18) {
        if (*(u8 *)(q + 0x46b8) != 0) {
            if (*(u8 *)(q + 0x46b9) == 0) {
                *(int *)(((int)q + 0x46a8)) += *(int *)(q + 0x46b0);
                *(int *)(((int)q + 0x46ac)) += *(int *)(q + 0x46b4);
                *(int *)(((int)q + 0x46b4)) -= 0x300;
                if (*(int *)(q + 0x46b0) > 0)
                    *(int *)(((int)q + 0x46b0)) += 0xc00;
                else
                    *(int *)(((int)q + 0x46b0)) -= 0xc00;
            }
            *(u8 *)(((int)q + 0x46bc)) += 1;
            if (*(u8 *)(q + 0x46bc) >= 4) {
                *(u8 *)(((int)q + 0x46bb)) += 1;
                if (*(u8 *)(q + 0x46bb) >= 5) {
                    *(u8 *)(q + 0x46b8) = 0;
                    *(u8 *)(q + 0x46bd) = 0;
                }
            }
        }
    }
}
