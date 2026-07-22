extern unsigned short func_0203dae4(int i);
extern unsigned char data_020a0e44;
extern unsigned short data_020a0e48[4];
extern unsigned short data_020a0e50[4];
extern unsigned short data_020a0e58[8];

void func_0203bc7c(void)
{
    unsigned short *p;
    int i;
    unsigned short cur;
    unsigned short old;

    p = data_020a0e58;
    data_020a0e44 = 0;
    for (i = 0; i < 4; i++) {
        cur = func_0203dae4(i);
        if ((cur & 0x30) == 0x30)
            cur &= ~0x30;
        if ((cur & 0xc0) == 0xc0)
            cur &= ~0xc0;
        if (cur == 0x30c)
            data_020a0e44 = 1;
        old = data_020a0e50[i];
        p[1] = cur & (cur ^ old);
        data_020a0e48[i] = old & (cur ^ old);
        data_020a0e50[i] = cur;
        *p = cur;
        p += 2;
    }
}
