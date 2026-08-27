void func_ov006_0211f5d4(char *c, int idx)
{
    int off = idx * 0x24;
    char *e = c + off;
    unsigned char t;
    *(unsigned char *)(e + 0x4000 + 0x67a) = 1;
    *(short *)(e + 0x4600 + 0x6e) = 0x60;
    *(unsigned char *)(e + 0x4000 + 0x679) = 0;
    *(unsigned char *)(e + 0x4000 + 0x678) = 1;
    *(int *)(c + 0x4668 + off) = 0xc00;
    t = *(unsigned char *)(e + 0x4000 + 0x67f);
    if (t == 0)
        return;
    if (t == 8)
        *(int *)(c + 0x4668 + off) = *(int *)(c + 0x4668 + off) - 0x200;
    else
        *(int *)(c + 0x4668 + off) = *(int *)(c + 0x4668 + off) + ((t - 1) << 9);
}
