void func_ov006_02119d50(char* c, int i)
{
    int off = i * 0x24;
    int v = *(int*)(c + 0x51c4 + off);
    if (v > 0xd80) {
        *(int*)(c + 0x51c4 + off) = v - 0x60;
        if (*(int*)(c + 0x51c4 + off) < 0xd80)
            *(int*)(c + 0x51c4 + off) = 0xd80;
    }
    *(unsigned short*)(c + 0x51c8 + off) = *(unsigned short*)(c + 0x51c8 + off) + 1;
    if (*(unsigned short*)(c + 0x51c8 + off) >= 0x18) {
        *(unsigned char*)(c + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(c + off + 0x5000 + 0x1cd) = 0;
    }
}
