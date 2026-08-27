extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int x);
extern void func_02065a84(int x);
extern void func_02065ad0(int v);
extern char *func_02065b94(void);
extern int func_02065af0(int *thiz);
extern void func_02065be0(void);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);
extern int data_020a94c0;

void func_02065170(void)
{
    int saved = _ZN3IRQ7DisableEv();
    char *p;
    int i;
    int local[0x28];
    if (data_020a94c0 == 0) {
        _ZN3IRQ7RestoreEj(saved);
        return;
    }
    data_020a94c0 = 0;
    func_02065a84(-1);
    func_02065ad0(0);
    p = func_02065b94();
    *(int *)(p + 0) = 0;
    *(short *)(p + 8) = 0;
    *(short *)(p + 0xa) = 0;
    *(int *)(p + 0x10) = 0;
    *(int *)(p + 0x138) = 0;
    *(short *)(p + 0x140) = 0;
    *(short *)(p + 0x142) = 0;
    *(int *)(p + 0x148) = 0;
    i = 0;
    do {
        *(unsigned char *)(p + 0x1d7) = (unsigned char)0;
        i++;
        p += 0x68;
    } while (i < 0x10);
    local[0] = 0;
    func_02065af0(local);
    func_02065be0();
    func_02065ba0(-1);
    func_02065bb0(-1);
    _ZN3IRQ7RestoreEj(saved);
}
