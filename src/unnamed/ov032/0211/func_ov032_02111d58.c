int func_ov032_02111d58(char *p)
{
    *(short *)(p + 0x100) = 300;
    *(int *)(p + 0x3a8) = 8192;
    *(int *)(p + 0x98) = 40960;
    return 1;
}
