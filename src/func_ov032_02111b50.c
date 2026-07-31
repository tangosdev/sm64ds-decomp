int func_ov032_02111b50(char *c)
{
    *(short *)(c + 0x430) = -0x4000;
    *(int *)(((int)c + 0x128)) |= 2;
    *(int *)(((int)c + 0x168)) |= 2;
    *(unsigned char *)(c + 0x428) = 0;
    *(int *)(c + 0x98) = 0xa000;
    *(int *)(c + 0x424) = 0;
    return 1;
}
