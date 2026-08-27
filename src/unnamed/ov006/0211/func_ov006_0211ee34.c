#include "types.h"
extern void func_ov006_0211f454(char *c, int i);
extern void func_ov006_0211f34c(char *c, int i);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *reg,
    unsigned short a, unsigned short b, unsigned short c, unsigned short d);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern short data_02082214[];

void func_ov006_0211ee34(char *c, int i)
{
    int k = i * 0x24;

    if (*(u16 *)(c + 0x4674 + k) != 0) {
        (*(u16 *)(c + 0x4674 + k))--;
        if (*(u16 *)(c + 0x4674 + k) == 0) {
            if (*(u8 *)(c + 0x4681 + k) != 0)
                _ZN5Sound12PlayBank2_2DEj(0x1f1);
        }
    }

    (*(u16 *)(c + 0x4670 + k))++;
    if (*(u16 *)(c + 0x4670 + k) >= 0xf) {
        *(u16 *)(c + 0x4670 + k) = 0;
        (*(u8 *)(c + 0x467d + k))++;
        if (*(u8 *)(c + 0x467d + k) >= 6)
            *(u8 *)(c + 0x467d + k) = 3;
    }

    func_ov006_0211f454(c, i);
    func_ov006_0211f34c(c, i);

    if (data_02082214[2 * (*(u16 *)(c + 0x466c + k) >> 4) + 1] >= 0)
        *(u8 *)(c + 0x467e + k) = 1;
    else
        *(u8 *)(c + 0x467e + k) = 0;

    if (*(u8 *)(c + 0x4676 + k) >= 0x10)
        return;

    (*(u16 *)(c + 0x466e + k))++;
    if (*(u16 *)(c + 0x466e + k) < 4)
        return;

    (*(u8 *)(c + 0x4676 + k))++;
    *(u8 *)(c + 0x467b + k) = 0;
    *(u16 *)(c + 0x466e + k) = 0;

    if (i == *(u8 *)(c + 0x4c22)) {
        _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4001050, 0, 1,
            *(u8 *)(c + 0x4676 + k), 0x10);
    }

    if (*(u8 *)(c + 0x4676 + k) < 0x10)
        return;

    *(u16 *)(c + 0x4672 + k) = 0x80;
    *(u16 *)(c + 0x466e + k) = 0;
    *(u8 *)(c + 0x4676 + k) = 0;
    *(u8 *)(c + 0x4678 + k) = 7;
}
