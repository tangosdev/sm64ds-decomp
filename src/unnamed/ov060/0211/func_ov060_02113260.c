void func_ov060_02113260(char *c)
{
    int y = *(int *)(c + 0x60);

    *(int *)(c + 0x5c) = 0;
    *(int *)(c + 0x60) = y;
    *(int *)(c + 0x64) = 0;
    *(int *)(c + 0x80) = 0;
    *(int *)(c + 0x84) = 0;
    *(int *)(c + 0x88) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0x9c) = 0;
    *(unsigned char *)(c + 0x426) = 0;
    *(int *)(((int)c + 0x378)) |= 1;
}
