extern int RandomIntInternal(int *seed);
extern int data_0209e650;
void func_ov060_0211722c(char *c)
{
    unsigned int r;
    *(short*)(c+0x94) = (short)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
    r = (unsigned int)RandomIntInternal(&data_0209e650) >> 0x10;
    if (r % 10 < 2)
        *(int*)(c+0xa8) = 0x1e000;
    else
        *(int*)(c+0xa8) = 0xa000;
    *(int*)(c+0x98) = 0x5000;
    *(int*)(c+0x9c) = -0x1000;
    *(int*)(c+0x360) = (int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0xfff) + 0x1000;
    *(int*)(c+0x368) = 0x10;
}
