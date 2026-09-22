extern void _ZN5dCc_c5ClearEv(void *);
int func_ov072_02121640(char *c)
{
    *(char *)(c + 0x36e) = 0;
    *(int *)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv((char *)c + 0x160);
    *(int *)(c + 0x368) = 4;
    return 1;
}
