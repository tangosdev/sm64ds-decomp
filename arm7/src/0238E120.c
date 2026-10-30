extern unsigned char dat_27D6A84[];
extern int dat_27D7A84[];

int *fn_0238E120(unsigned char a1) {
    int i;
    unsigned char *base;
    unsigned char *p;
#pragma scheduling off
    i = 0;
    base = dat_27D6A84;
#pragma scheduling reset
    do {
        p = base + i;
        p += 0x1000;
        p[0x614] = a1;
        i++;
    } while (i < 16);
    dat_27D7A84[393] = 0;
    return dat_27D7A84;
}
