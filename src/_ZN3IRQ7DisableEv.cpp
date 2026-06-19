//cpp
extern "C" unsigned int _ZN3IRQ7DisableEv(void);
extern "C" asm unsigned int _ZN3IRQ7DisableEv(void)
{
    mrs r0, cpsr
    orr r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}
