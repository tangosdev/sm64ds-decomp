extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *p, unsigned short a, unsigned short b, int c, unsigned short d);
extern unsigned char data_0209d454;
extern void func_0201ef50(unsigned char v);

#pragma opt_common_subs off

void func_ov002_020f2bf4(char *c, int idx)
{
    unsigned char cnt;

    *(unsigned short *)(c + 0x1c4 + idx * 8) += 1;
    *(unsigned short *)(c + 0x1c6 + idx * 8) += 1;

    if (*(unsigned short *)(c + idx * 8 + 0x100 + 0xc6) >= 2) {
        *(unsigned short *)(c + idx * 8 + 0x100 + 0xc6) = 0;
        *(unsigned char *)(c + 0x1ca + idx * 8) += 1;
        cnt = *(unsigned char *)(c + idx * 8 + 0x1ca);
        _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4001050, 1, 0x36, 0x10 - cnt, cnt);
    }

    if (*(unsigned short *)(c + idx * 8 + 0x100 + 0xc4) < 30) {
        return;
    }
    *(unsigned short *)(c + idx * 8 + 0x100 + 0xc4) = 0;
    *(unsigned char *)(c + 0x1c9 + idx * 8) += 1;
    *(unsigned char *)(c + 0x1cb + idx * 8) += 1;
    *(unsigned char *)(c + idx * 8 + 0x1ca) = 0;
    *(unsigned short *)(c + idx * 8 + 0x100 + 0xc6) = 0;
    data_0209d454 &= ~1;
    func_0201ef50(*(unsigned char *)(c + idx * 8 + 0x1cb));
}
