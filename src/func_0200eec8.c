// Long-branch veneer with argument load: load r0 with a fixed global pointer, then
// tail-call RunKuppaScript through ip. Hand-asm because it is a pure relocation
// wildcard thunk (ldr ip,[pc]; ldr r0,[pc]; bx ip; .word target; .word arg).
extern void RunKuppaScript(void);
extern char data_02088610[];

asm void func_0200eec8(void)
{
    ldr ip, [pc, #4]
    ldr r0, [pc, #4]
    bx ip
    dcd RunKuppaScript
    dcd data_02088610
}
