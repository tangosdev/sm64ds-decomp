extern signed char data_0209f2f8;
int _ZN18PoppingLavaBubbles13InitResourcesEv(char *c)
{
    signed char v;
    *(short *)(c + 0xd4) = 0x24;
    v = data_0209f2f8;
    if (v != 0x16) {
        if (v == 0x21)
            *(short *)(c + 0xd4) = 0xeb;
    } else {
        *(short *)(c + 0xd4) = 0x71;
    }
    return 1;
}
