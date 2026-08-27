void func_ov006_0211a314(char* c, int i)
{
    int off = i * 0x24;
    int v = *(int*)(c + 0x51c4 + off);
    if (v > 0x800) {
        *(int*)(c + 0x51c4 + off) = v - 0x60;
        if (*(int*)(c + 0x51c4 + off) < 0x800)
            *(int*)(c + 0x51c4 + off) = 0x800;
    }
    *(unsigned short*)(c + 0x51c8 + off) = *(unsigned short*)(c + 0x51c8 + off) + 1;
    if (*(unsigned short*)(c + 0x51c8 + off) >= 0x20) {
        *(unsigned char*)(c + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1cd) = 0;
    }
}
