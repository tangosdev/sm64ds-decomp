void func_ov060_021167c8(char *c)
{
    *(int *)(((int)c + 0x2e8)) |= 1;
    *(int *)(c + 0x360) = 0x2000;
    *(int *)(c + 0x368) = 0x10;
}
