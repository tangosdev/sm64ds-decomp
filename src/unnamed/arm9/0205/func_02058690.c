extern int func_02057020(void);
extern unsigned int _ZN3IRQ7DisableEv(void);
extern int func_020570b8(unsigned short* p);
extern int func_02057158(int x);
extern int func_02057178(int a);
extern void _ZN3IRQ7RestoreEj(unsigned int flags);

int func_02058690(void)
{
    int result;
    int done = 0;
    int x;
    int res;
    unsigned int irq;

    x = (unsigned short)func_02057020();
    result = done;

    do {
        res = -1;
        irq = _ZN3IRQ7DisableEv();
        if ((func_020570b8((unsigned short*)0x027fffe8) & 0x40) != 0 || (res = func_02057158(x)) == 0) {
            if (*(unsigned int*)0x8000000 == 0x544e494e && *(unsigned int*)0x8000004 == 0x4f444e45)
                result = 0x1000000;
            else
                result = 0x2000000;
            if (res == 0) {
                func_02057178(x);
                done = 1;
            }
        }
        _ZN3IRQ7RestoreEj(irq);
    } while (done == 0);

    return result;
}
