extern void _ZN5dCc_c5ClearEv(void *);
int func_ov077_02124394(char *c)
{
    *(int *)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv((char *)c + 0x1c4);
    *(int *)(c + 0x3f4) = 2;
    return 1;
}
