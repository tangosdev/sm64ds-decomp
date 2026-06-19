//cpp
extern "C" unsigned int _ZN3IRQ7RestoreEj(unsigned int);
extern "C" asm unsigned int _ZN3IRQ7RestoreEj(unsigned int)
{
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr
}
