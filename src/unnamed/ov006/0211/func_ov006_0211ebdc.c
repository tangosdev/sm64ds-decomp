#include "types.h"
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern void func_ov006_0211f454(char *c, int i);
extern void func_ov006_0211f34c(char *c, int i);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile unsigned short *p, int a, int b, int c, int d);
extern short data_02082214[];

void func_ov006_0211ebdc(char *c, int i)
{
    u8 *s;
    int o = i * 0x24;
    u8 *fade;

    if (*(u16 *)(c + 0x4674 + o) != 0) {
        (*(u16 *)((c + 0x4674) + o))--;
        if (*(u16 *)((c + 0x4674) + o) == 0)
            _ZN5Sound12PlayBank2_2DEj(0x1f1);
    }

    s = (u8 *)(c + 0x467d + o);
    if (*s != 6) {
        u16 *p70 = (u16 *)(c + 0x4670 + o);
        *p70 = *p70 + 1;
        if (*p70 >= 0xf) {
            *p70 = 0;
            (*s)++;
            if (*s >= 6)
                *s = 3;
        }
    }

    func_ov006_0211f454(c, i);
    func_ov006_0211f34c(c, i);

    {
        char *row = c + o;
        if (data_02082214[((*(u16 *)(row + 0x466c) >> 4) << 1) + 1] >= 0)
            *(u8 *)(row + 0x467e) = 1;
        else
            *(u8 *)(row + 0x467e) = 0;
    }

    if (*(u16 *)(((char *)c + o) + 0x4672) != 0) {
        u16 *p72 = (u16 *)((c + 0x4672) + o);
        *p72 = *p72 - 1;
        if (*p72 != 0)
            return;
        *(u16 *)((c + 0x466c) + o) += 0x8000;
        *s = 6;
        if (*(u8 *)(((char *)c + o) + 0x4681) != 0)
            _ZN5Sound12PlayBank2_2DEj(0x1f4);
        return;
    }

    {
        u16 *p6e = (u16 *)(c + 0x466e + o);
        *p6e = *p6e + 1;
        if (*p6e < 4)
            return;
        fade = (u8 *)((c + 0x4676) + o);
        *fade = *fade + 1;
        *p6e = 0;
        if (i == *(u8 *)(c + 0x4c22)) {
            _ZN3G2x13SetBlendAlphaEPVttttt((volatile unsigned short *)0x4001050, 0, 4,
                0x10 - *fade, 0x10);
        }
        if (*fade >= 0x10) {
            *(u8 *)(c + 0x4677 + o) = 0;
            *(u8 *)(c + 0x467a + o) = 0;
        }
    }
}
