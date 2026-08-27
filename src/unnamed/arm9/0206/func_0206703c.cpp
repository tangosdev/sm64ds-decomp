//cpp
extern "C" {
extern int data_020a9db8;
extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(int saved);
extern int func_02065edc(unsigned int x);

int func_0206703c(unsigned int arg0, int sel) {
    int val;
    int code;
    int saved = _ZN3IRQ7DisableEv();
    switch (sel) {
    case 0: code = 0xa; val = 4; break;
    case 1: code = 0xa; val = 3; break;
    case 2: code = 0xe; val = 2; break;
    case 3: code = 7;   val = 5; break;
    default:
        _ZN3IRQ7RestoreEj(saved);
        return 0;
    }
    if (data_020a9db8 == 0) goto fail;
    if (func_02065edc(arg0) == 0) goto fail;
    {
        unsigned int i = arg0 - 1;
        if (*(int*)((char*)data_020a9db8 + i * 4 + 0x14e8) != code) goto fail;
        *(short*)((char*)data_020a9db8 + i * 2 + 0x1754) = (short)val;
    }
    _ZN3IRQ7RestoreEj(saved);
    return 1;
fail:
    _ZN3IRQ7RestoreEj(saved);
    return 0;
}
}
