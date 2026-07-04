extern void func_ov006_020f3c0c(char *c);
void func_ov006_020f49ac(char *c)
{
    int i;
    char *e;
    unsigned short *t;
    if (*(unsigned short *)(c + 0x5300 + 0x22) != 0) {
        t = (unsigned short *)(((int)c + 0x5322) & 0xFFFFFFFFFFFFFFFF);
        *t = *t - 1;
        if (*(short *)(c + 0x5300 + 0x22) > 0)
            return;
        func_ov006_020f3c0c(c);
        return;
    }
    e = c;
    for (i = 0; i < 0xc; i++) {
        *(unsigned char *)(e + 0x5000 + 0x1bc) = 2;
        e += 0x18;
    }
    *(int *)(c + 0x5000 + 0x318) = 0;
    *(int *)(c + 0x5000 + 0x314) = 2;
}
