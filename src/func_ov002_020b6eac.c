extern signed char LEVEL_ID;
int func_ov002_020b6eac(char *c)
{
    signed char v;
    *(short *)(c + 0xd4) = 0x24;
    v = LEVEL_ID;
    if (v != 0x16) {
        if (v == 0x21)
            *(short *)(c + 0xd4) = 0xeb;
    } else {
        *(short *)(c + 0xd4) = 0x71;
    }
    return 1;
}
