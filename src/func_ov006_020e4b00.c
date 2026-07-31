void _ZN5Sound12PlayBank2_2DEj(unsigned int);

#pragma opt_strength_reduction off
void func_ov006_020e4b00(char *this) {
    int i;
    for (i = 0; i < 5; i++) {
        char *base = this + (i << 4);
        volatile unsigned short *t;
        if (*(unsigned char*)(base + 0x487c) == 0) continue;
        if (*(unsigned short*)(base + 0x487a) == 0) continue;
        t = (volatile unsigned short*)(((int)base + 0x487a));
        *t = *t - 1;
        if (*t != 0) continue;
        *(unsigned char*)(base + 0x487d) = 1;
        _ZN5Sound12PlayBank2_2DEj(0x1bc);
    }
}
