void func_ov006_0211a578(char* c, int i)
{
    int off = i * 0x24;
    *(int*)(c + 0x51b4 + off) = *(int*)(c + 0x51b4 + off) + *(int*)(c + 0x51bc + off);
    *(int*)(c + 0x51bc + off) = *(int*)(c + 0x51bc + off) + 0x400;
    if (*(int*)(c + 0x51bc + off) > 0) {
        *(int*)(c + 0x51bc + off) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1d1) = 2;
    }
}
