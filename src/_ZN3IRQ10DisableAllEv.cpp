//cpp
extern "C" unsigned int _ZN3IRQ10DisableAllEv(void);
extern "C" asm unsigned int _ZN3IRQ10DisableAllEv(void)
{
    mrs r0, cpsr
    orr r1, r0, #0xc0
    msr cpsr_c, r1
    and r0, r0, #0xc0
    bx lr
}
