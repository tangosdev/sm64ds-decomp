extern int func_ov064_02117a44(void *c);

int func_ov064_02117bdc(void *c)
{
    unsigned char b = *(unsigned char*)((char*)c + 0x33c);
    if (b == 1) {
        unsigned short v = *(unsigned short*)((char*)c + 0x338);
        if (v < 0x14) return v;
    }
    int r = func_ov064_02117a44(c);
    if (r == -1) {
        r = 2;
        *(unsigned char*)((char*)c + 0x33b) = r;
    }
    return r;
}
