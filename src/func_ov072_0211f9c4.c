int func_ov072_0211f9c4(char *c)
{
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    {
        int *p = (int *)(((int)c + 0xb0));
        *p &= ~1;
    }
    *(int *)(c + 0x388) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x394) = 2;
    return 1;
}
