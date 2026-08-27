#pragma opt_strength_reduction off

extern short data_ov003_020b1774[];
extern unsigned char data_0209d45c[];
extern unsigned char data_0209d454[];

void func_ov003_020b060c(char *c)
{
    unsigned char s = *(unsigned char *)(c + 0x94);
    int i;
    int ip;
    if (s == 0)
        goto case0;
    if (s == 1)
        goto case1;
    return;

case0:
    i = 0;
    ip = 0xa;
    do {
        char *e = c + (i << 1);
        if (*(unsigned short *)(e + 0x70) != 0) {
            *(unsigned short *)((long long)(int)(e + 0x70)) =
                (unsigned short)(*(unsigned short *)((long long)(int)(e + 0x70)) - 1);
        } else {
            short tgt = data_ov003_020b1774[i];
            if (*(short *)(e + 0x50) != tgt) {
                if (i < 4) {
                    short *q = (short *)(e + 0x50);
                    *q = (short)(*q + 0xc);
                    if (*(short *)(e + 0x50) >= tgt) {
                        *(short *)(e + 0x50) = tgt;
                        *(unsigned short *)(e + 0x70) = (unsigned short)ip;
                    }
                } else {
                    short *q = (short *)(e + 0x50);
                    *q = (short)(*q - 0xc);
                    if (*(short *)(e + 0x50) <= tgt) {
                        *(short *)(e + 0x50) = tgt;
                        *(unsigned short *)(e + 0x70) = (unsigned short)ip;
                        *(unsigned char *)(c + 0x94) = 1;
                    }
                }
            }
        }
        i = i + 1;
        ip = ip + 4;
    } while (i < 8);
    return;

case1:
    if (*(unsigned short *)(c + 0x70) != 0) {
        unsigned short *p = (unsigned short *)(c + 0x70);
        *p = (unsigned short)(*p - 1);
        return;
    }
    *(unsigned char *)(c + 0x94) = 2;
    data_0209d45c[0] |= 1;
    data_0209d454[0] |= 3;
}
