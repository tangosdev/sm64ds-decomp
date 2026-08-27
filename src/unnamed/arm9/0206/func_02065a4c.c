int func_02065a4c(int x)
{
    int i;
    for (i = 0; i < 16; i++)
        if ((x >> i) & 1)
            return i;
    return -1;
}
