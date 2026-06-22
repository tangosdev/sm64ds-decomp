extern unsigned int data_020a4d48;
int func_0204f138(unsigned int mask)
{
    if (mask & data_020a4d48)
        return 0;
    data_020a4d48 = data_020a4d48 | mask;
    return 1;
}
