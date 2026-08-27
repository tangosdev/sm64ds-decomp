extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *p, unsigned short a, unsigned short b, unsigned short c, unsigned short d);

void func_ov006_0211d608(char *c)
{
    *(unsigned char *)(c + 0x4be0) = 1;
    *(int *)(c + 0x4bcc) = 0x80000;
    *(int *)(c + 0x4bd0) = 0x100000;
    *(int *)(c + 0x4bd4) = 0;
    *(int *)(c + 0x4bd8) = -0x1800;
    *(unsigned short *)(c + 0x4bdc) = 0;
    *(unsigned short *)(c + 0x4bde) = 0;
    *(unsigned char *)(c + 0x4be1) = 0;
    *(unsigned char *)(c + 0x4be3) = 8;
    *(unsigned char *)(c + 0x4be4) = 0;
    *(unsigned char *)(c + 0x4be5) = 0;
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4000050, 0, 0xc, 0xc, 0x10);
}
