extern unsigned char data_0209f250;

void func_0200ca14(int r0, unsigned int r1, int r2)
{
    if (r1 != (unsigned int)data_0209f250)
        return;

    if (r2 == 0)
        *(unsigned *)(((long long)(int)(r0 + 0x154)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x20000;
    else
        *(unsigned *)(((long long)(int)(r0 + 0x154)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x10000;
}
