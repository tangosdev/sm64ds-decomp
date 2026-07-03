#define LAUNDER_ADDR(x) ((int)(((long long)(int)(x)) & 0xffffffffffffffffLL))

int func_ov078_021250d0(char *c)
{
    int *flags = (int *)LAUNDER_ADDR(c + 0x354);
    *flags |= 2;
    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0x98) = 0;
    *(unsigned char *)(c + 0x499) = 0;
    return 1;
}
