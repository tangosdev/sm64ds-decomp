//cpp
extern "C" unsigned int _ZN3IRQ6EnableEv(void);
extern "C" asm unsigned int _ZN3IRQ6EnableEv(void)
{
    mrs r0, cpsr
    bic r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}
