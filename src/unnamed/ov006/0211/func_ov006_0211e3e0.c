extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_0211e3e0(char *c)
{
    if (*(unsigned char *)(c + 0x4c20) == 0) return;
    if (*(unsigned char *)(c + 0x4c1c) != 0) return;
    if (*(unsigned int *)(c + 0xbc) >= 0xf)
        *(unsigned short *)(c + 0x4c14) = 2;
    else
        *(unsigned short *)(c + 0x4c14) = 3;
    *(unsigned char *)(c + 0x4c1a) = 0;
    *(unsigned char *)(c + 0x4c1b) = 1;
    *(unsigned char *)(c + 0x4c1c) = 1;
    _ZN5Sound12PlayBank2_2DEj(0xa7);
}
