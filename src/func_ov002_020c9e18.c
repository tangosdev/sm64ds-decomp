void func_ov002_020c9e18(char *c)
{
    if (*(unsigned char *)(c + 0x709) != 1) return;
    *(unsigned char *)(c + 0x709) = 0;
    *(unsigned int *)(((long long)(int)(c + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) &= ~4;
}
