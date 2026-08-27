extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);
extern void OS_SleepThread(void *);
extern char data_020a8180[];

int func_0205ffd0(void)
{
    int *g = (int *)data_020a8180;
    unsigned int saved = _ZN3IRQ7DisableEv();
    if (g[13] & 4) {
        do {
            OS_SleepThread((char *)g + 0xd4);
        } while (g[13] & 4);
    }
    _ZN3IRQ7RestoreEj(saved);
    return *(int *)g[0] == 0;
}
