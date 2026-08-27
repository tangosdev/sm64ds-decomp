extern unsigned int data_020a4d4c;

int func_0204f0d8(void)
{
    int i;
    int mask = 1;
    unsigned int v = data_020a4d4c;
    for (i = 0; i < 8; i++)
    {
        if ((v & mask) == 0)
        {
            data_020a4d4c |= mask;
            return i;
        }
        mask <<= 1;
    }
    return -1;
}
