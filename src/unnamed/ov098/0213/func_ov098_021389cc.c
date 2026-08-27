void func_ov098_021389cc(char *c)
{
    *(int *)(c + 0x98) = 0;
    *(int *)(((int)c + 0x57c)) &= ~0x2000;
    *(int *)(c + 0x5f0) = 0;
    *(int *)(c + 0x5f4) = 0;
}
