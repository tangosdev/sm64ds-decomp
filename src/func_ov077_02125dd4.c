extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *);
int func_ov077_02125dd4(char *c)
{
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x98) = 0x4000;
    *(int *)(c + 0xa8) = 0x19000;
    _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)c + 0x1e4);
    *(int *)(c + 0x3d8) = 0;
    return 1;
}
