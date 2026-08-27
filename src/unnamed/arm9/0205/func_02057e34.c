// func_02057e34 - idle loop: enable IRQ once, then wait for interrupt forever

extern void _ZN3IRQ6EnableEv(void);
extern void _ZN4CP1516WaitForInterruptEv(void);

void func_02057e34(void) {
    _ZN3IRQ6EnableEv();
    for (;;) {
        _ZN4CP1516WaitForInterruptEv();
    }
}