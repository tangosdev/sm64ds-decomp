void func_ov006_0211ac30(char *self, int idx)
{
    int i = idx * 0x24;
    *(int*)(self + 0x51b0 + i) = *(int*)(self + 0x51b0 + i) + *(int*)(self + 0x51b8 + i);
    *(int*)(self + 0x51b4 + i) = *(int*)(self + 0x51b4 + i) + *(int*)(self + 0x51bc + i);
    if (*(int*)(self + 0x51c4 + i) > 0x800) {
        *(int*)(self + 0x51c4 + i) = *(int*)(self + 0x51c4 + i) - 0x100;
        if (*(int*)(self + 0x51c4 + i) < 0x800)
            *(int*)(self + 0x51c4 + i) = 0x800;
    }
    *(int*)(self + 0x51bc + i) = *(int*)(self + 0x51bc + i) - 0x20;
    {
        int d = (*(int*)(self + 0x51c0 + i) - *(int*)(self + 0x51b4 + i)) >> 12;
        if (d >= 0x38) {
            *(unsigned char*)(self + 0x51cc + i) = 0;
            *(unsigned char*)(self + 0x51cd + i) = 0;
        }
    }
}
