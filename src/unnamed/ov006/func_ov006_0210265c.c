extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_0210265c(char *c)
{
    unsigned char *q;
    unsigned short *h;
    unsigned short t;
    if (*(unsigned short *)(c + 0x5674) == 0) return;
    q = (unsigned char *)(((int)c + 0x5679));
    *q += 1;
    if (*(unsigned char *)(c + 0x5679) < 0x3c) return;
    *(unsigned char *)(c + 0x5679) = 0;
    h = (unsigned short *)(((int)c + 0x5674));
    *h -= 1;
    t = *(unsigned short *)(c + 0x5674);
    if (t > 0xa)
        _ZN5Sound12PlayBank2_2DEj(0xa8);
    else if (t > 3)
        _ZN5Sound12PlayBank2_2DEj(0xa7);
    else
        _ZN5Sound12PlayBank2_2DEj(0xa6);
    if (*(unsigned short *)(c + 0x5674) == 0)
        *(unsigned char *)(c + 0x567a) = 0;
}
