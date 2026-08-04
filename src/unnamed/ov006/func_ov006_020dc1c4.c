extern void func_ov006_020ddeb0(void *thiz);

void func_ov006_020dc1c4(void *thiz, int idx)
{
    unsigned char *c = (unsigned char *)thiz;
    int off = idx * 0x1c;

    *(int *)(c + 0x51ac + off) = *(int *)(c + 0x51ac + off) + *(int *)(c + 0x51b4 + off);
    *(int *)(c + 0x51b4 + off) = *(int *)(c + 0x51b4 + off) - 0x100;
    *(unsigned short *)(c + 0x51b8 + off) = *(unsigned short *)(c + 0x51b8 + off) + 0x400;

    if ((*(int *)(c + 0x51ac + off) >> 0xc) > -0xf0) return;

    *(int *)(c + 0x51ac + off) = -0xf0000;
    *(unsigned char *)(c + 0x51bf + off) = 2;
    *(unsigned short *)(c + 0x51ba + off) = 0x30;
    func_ov006_020ddeb0(c);
}
