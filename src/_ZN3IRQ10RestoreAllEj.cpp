//cpp
extern "C" unsigned int _ZN3IRQ10RestoreAllEj(unsigned int);
extern "C" asm unsigned int _ZN3IRQ10RestoreAllEj(unsigned int)
{
    mrs r1, cpsr
    bic r2, r1, #0xc0
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0xc0
    bx lr
}
