#pragma opt_common_subs off
#pragma opt_strength_reduction off
void func_ov006_020f1318(char *c, int idx)
{
    char *base = c + 0x506c;
    int twice = idx * 2;
    unsigned short h = *(unsigned short *)(base + twice);
    unsigned char *q;

    *(short *)(base + twice) = (short)(h - 1);

    if (*(short *)(c + (idx << 1) + 0x5000 + 0x6c) < 0)
        *(short *)(c + (idx << 1) + 0x5000 + 0x6c) = 0;

    if ((((unsigned short)*(unsigned short *)(c + (idx << 1) + 0x5000 + 0x6c) >> 2) & 1) != 0) {
        char *p = c + 0x53dd;
        p[idx] = 0;
        q = (unsigned char *)(p + idx);
        goto after_flag;
    }
    {
        char *p = c + 0x53dd;
        p[idx] = 1;
        q = (unsigned char *)(p + idx);
    }
after_flag:
    if (*(unsigned short *)(c + (idx << 1) + 0x5000 + 0x6c) != 0)
        return;
    *(unsigned char *)(c + idx + 0x5000 + 0x1fd) = 0;
    *q = 1;
}
