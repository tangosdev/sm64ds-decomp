extern void func_ov006_0211f454(char *c, int i);
extern void func_ov006_0211f34c(char *c, int i);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *reg,
    unsigned short a, unsigned short b, int c, unsigned short d);
extern short data_02082214[];

void func_ov006_0211f224(char *c, int i)
{
    int k;
    func_ov006_0211f454(c, i);
    func_ov006_0211f34c(c, i);
    k = i * 0x24;
    if (data_02082214[2 * (*(unsigned short *)(c + 0x466c + k) >> 4) + 1] >= 0)
        *(unsigned char *)(c + 0x467e + k) = 1;
    else
        *(unsigned char *)(c + 0x467e + k) = 0;
    (*(unsigned short *)(c + 0x466e + k))++;
    if (*(unsigned short *)(c + 0x466e + k) < 4)
        return;
    (*(unsigned char *)(c + 0x4676 + k))++;
    *(unsigned short *)(c + 0x466e + k) = 0;
    if (i == 0)
        _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4001050, 0, 1,
            0x10 - *(unsigned char *)(c + 0x4676 + k), 0x10);
    if (*(unsigned char *)(c + 0x4676 + k) < 0x10)
        return;
    *(unsigned char *)(c + 0x4676 + k) = 0;
    *(unsigned char *)(c + 0x4678 + k) = 3;
    *(unsigned char *)(c + 0x467b + k) = 1;
}
