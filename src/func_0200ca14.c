extern unsigned char data_0209f250;

void func_0200ca14(int r0, unsigned int r1, int r2)
{
    if (r1 != (unsigned int)data_0209f250)
        return;

    if (r2 == 0)
        *(unsigned *)(r0 + 0x154) |= 0x20000;
    else
        *(unsigned *)(r0 + 0x154) |= 0x10000;
}
