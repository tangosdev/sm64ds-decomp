unsigned short func_ov006_0211507c(unsigned char* base, int* pos)
{
    int a = pos[1];
    if (a >= 0 && a < 0xc0000) {
        int b = pos[0];
        int ai = (a >> 12) / 8;
        int bi = (b >> 12) / 8;
        unsigned short h = *(unsigned short*)(base + (ai * 32 + bi) * 2 + 0x5154);
        return (unsigned short)(h & 0x3ff);
    }
    if (a < -0x30000 && a >= -0xf0000) {
        int b = pos[0];
        int ai = ((a >> 12) + 0xf0) / 8;
        int bi = (b >> 12) / 8;
        unsigned short h = *(unsigned short*)(base + (ai * 32 + bi) * 2 + 0x4954);
        return (unsigned short)(h & 0x3ff);
    }
    return 0;
}
