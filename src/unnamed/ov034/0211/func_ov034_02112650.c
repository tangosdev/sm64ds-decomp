int func_ov034_02112650(int a, int *b, int *c)
{
    volatile int pad[4];
    int y = b[0x10 / 4];
    int x = c[0xa8 / 4];
    (void)&pad;
    if (x < 0 && y < 0 && y > x)
        return 1;
    return 0;
}
