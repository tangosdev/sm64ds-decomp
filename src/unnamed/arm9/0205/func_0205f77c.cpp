//cpp
struct IRQ {
    static unsigned int Disable();
    static unsigned int DisableIRQs(unsigned int);
    static unsigned int SetIRQs(unsigned int);
    static void Restore(unsigned int);
};
struct CP15 { static void WaitForInterrupt(); };
extern "C" void func_02059d8c(int x);
extern "C" int func_0205fc44(unsigned short a, unsigned short b);
extern "C" int func_0205f68c(int mode, int arg1, int arg2, int arg3);
extern "C" int func_0205f958(void);
extern "C" int func_0206a4a0(void);

extern "C" int func_0205f77c(int mode, int arg1, int arg2)
{
    unsigned int saved_ime;
    unsigned int saved_irq;
    unsigned int flag = 0;
    unsigned short old_ime;
    unsigned short dummy;

    old_ime = *(volatile unsigned short *)0x4000208;
    *(volatile unsigned short *)0x4000208 = 0;
    saved_ime = IRQ::Disable();
    saved_irq = IRQ::DisableIRQs(0x3fffff);
    IRQ::SetIRQs(0x40000);
    IRQ::Restore(saved_ime);

    *(volatile unsigned short *)0x4000208;
    *(volatile unsigned short *)0x4000208 = 1;
    if (mode & 8) {
        if (*(volatile unsigned short *)0x27ffc40 == 2) mode &= ~8;
    }
    if (mode & 0x10) {
        if (func_0206a4a0() == 0) mode &= ~0x10;
    }
    while (func_0205fc44(mode, arg1 | arg2) != 0)
        ;

    CP15::WaitForInterrupt();
    func_02059d8c(0x332340);
    if (mode & 8) {
        if (*(volatile unsigned int *)0x4000214 & 0x100000) flag = 1;
    }
    if (flag == 0) {
        func_0205f68c(1, 1, 1, 0);
    }
    IRQ::Disable();
    IRQ::SetIRQs(saved_irq);
    IRQ::Restore(saved_ime);

    dummy = *(volatile unsigned short *)0x4000208;
    *(volatile unsigned short *)0x4000208 = old_ime;
    if (flag != 0) {
        return func_0205f958();
    }
    return dummy;
}
