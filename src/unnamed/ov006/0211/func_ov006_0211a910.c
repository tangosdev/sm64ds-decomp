void func_ov006_0211a910(char* c, int i)
{
    int n = i * 0x24;
    int v;
    *(int*)(c + 0x51b0 + n) = *(int*)(c + 0x51b0 + n) + *(int*)(c + 0x51b8 + n);
    *(int*)(c + 0x51b4 + n) = *(int*)(c + 0x51b4 + n) + *(int*)(c + 0x51bc + n);
    v = *(int*)(c + 0x51b8 + n);
    if (v > 0x20) {
        *(int*)(c + 0x51b8 + n) = v - 0x20;
    } else if (v < -0x20) {
        *(int*)(c + 0x51b8 + n) = v + 0x20;
    }
    *(int*)(c + 0x51bc + n) = *(int*)(c + 0x51bc + n) - 0x40;
    *(unsigned short*)(c + 0x51c8 + n) = *(unsigned short*)(c + 0x51c8 + n) + 1;
    if (*(unsigned short*)(c + 0x51c8 + n) >= 0x28) {
        *(unsigned short*)(c + 0x51c8 + n) = 0;
        *(unsigned char*)(c + n + 0x5000 + 0x1d1) = 2;
        *(unsigned char*)(c + n + 0x5000 + 0x1d3) = 0;
        *(int*)(c + 0x51b8 + n) = 0xc00;
    }
}
