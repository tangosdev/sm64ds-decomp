extern void _ZN3IRQ7DisableEv(void);
extern void _ZN4CP1516WaitForInterruptEv(void);

void func_0201a028(void)
{
    for (;;)
    {
        _ZN3IRQ7DisableEv();
        _ZN4CP1516WaitForInterruptEv();
    }
}
