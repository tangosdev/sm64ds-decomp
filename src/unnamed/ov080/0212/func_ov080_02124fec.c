int func_ov080_02124fec(int *c)
{
    int *r = (int *)(((int)c + 0xb0));
    int one = 1;
    *(int *)((char *)c + 0x370) = one;
    *r &= ~3;
    return one;
}
