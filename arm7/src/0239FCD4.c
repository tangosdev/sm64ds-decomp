extern int *dat_0380FFF4;

int fn_0239FCD4(int a1)
{
    unsigned short v = *(unsigned short *)(*(int *)(a1 + 8) + 4) & 0xFF;
    if (v == 0)
        ++*(int *)((int)dat_0380FFF4 + 0x560);
    else
        *(int *)((int)dat_0380FFF4 + 0x55C) += v;
    ++*(unsigned short *)(a1 + 4);
    *(unsigned short *)a1 = 0;
    return a1;
}
