extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_020d7e7c(char *c, int i)
{
    char *b = c + i * 64;
    if (*(unsigned char *)(b + 0x469d) == 0) return;
    if (*(unsigned char *)(b + 0x4695) != 0) return;
    if (*(unsigned short *)(b + 0x468e) != 1) return;
    _ZN5Sound12PlayBank2_2DEj(0x1e0);
    *(short *)(c + 0x62f0) = 0x60;
}
