void _ZN5Sound12PlayBank2_2DEj(unsigned int);

#pragma opt_strength_reduction off
void func_ov006_020e1608(char *this) {
    int i;
    for (i = 0; i < 5; i++) {
        char *base = this + (i << 4);
        if (*(unsigned char*)(base + 0x4748) == 0) continue;
        if (*(unsigned short*)(base + 0x4746) == 0) continue;
        {
            volatile unsigned short *t = (volatile unsigned short*)(((unsigned int)base + 0x4746) & 0xFFFFFFFFFFFFFFFFull);
            *t = *t - 1;
            if (*t != 0) continue;
        }
        *(unsigned char*)(base + 0x4749) = 1;
        _ZN5Sound12PlayBank2_2DEj(0x1bc);
    }
}
