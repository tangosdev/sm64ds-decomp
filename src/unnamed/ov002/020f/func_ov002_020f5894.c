void func_ov002_020f5894(char *a, int i)
{
    int off;
    unsigned short *p34;
    short s2c;
    if (i != 0) return;
    off = i * 0x4c;
    p34 = (unsigned short*)(a + 0x34 + off);
    *p34 = *p34 + 1;
    if (*(int*)(a + 0x28 + off) != 0x1000) {
        unsigned short v = *p34;
        if (v == 0xf || v == 0x1e) {
            *(int*)(a + 0x28 + off) = *(int*)(a + 0x28 + off) + 0x199;
            if (*(int*)(a + 0x28 + off) >= 0x1000) *(int*)(a + 0x28 + off) = 0x1000;
        }
    }
    s2c = *(short*)(a + 0x2c + off);
    if (s2c != 0x800) {
        if (*(unsigned short*)(a + off + 0x3e) >= 5) {
            if ((*p34 & 3) == 3) {
                *(short*)(a + 0x2c + off) = s2c + 0x199;
                if (*(short*)(a + 0x2c + off) >= 0x800) *(short*)(a + 0x2c + off) = 0x800;
            }
        }
    }
    if (*p34 < 0x1e) return;
    *p34 = 0;
    {
        unsigned short *p3e = (unsigned short*)(a + 0x3e + off);
        *p3e = *p3e + 1;
    }
}
