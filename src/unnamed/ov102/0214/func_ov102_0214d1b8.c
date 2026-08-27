int func_ov102_0214d1b8(char *c)
{
    *(int *)(c + 0x98) = 0;

    if (*(unsigned char *)(c + 0x3c5) != 0) {
        *(int *)(((int)c + 0xb0)) |= 0x80;
        *(int *)(((int)c + 0x12c)) |= 0x1000;
        *(int *)(c + 0x9c) = 0;
    }

    return 1;
}
