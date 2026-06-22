int func_ov002_020f92e4(char *p)
{
    unsigned char b = *(unsigned char *)(p + 0x36d);
    if (b != 0 && b != 4)
        return 5;
    return 0;
}
