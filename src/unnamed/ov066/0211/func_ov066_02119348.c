extern int _ZN4dBgW9IsEnabledEv(void *base);
extern void _ZN4dBgW7DisableEv(void *base);

int func_ov066_02119348(void *c)
{
    if (_ZN4dBgW9IsEnabledEv((char *)c + 0x674) != 0) {
        _ZN4dBgW7DisableEv((char *)c + 0x674);
    }
    *(int *)((char *)c + 0x494) = 0;
    *(int *)((char *)c + 0x498) = 0;
    *(short *)((char *)c + 0x4d0) = 0;
    *(int *)((char *)c + 0x4a0) = 0;
    return 1;
}
