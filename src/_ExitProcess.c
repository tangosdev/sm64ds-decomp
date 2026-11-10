/* _ExitProcess (0x02071784): the Metrowerks NITRO runtime's exit, a tail
 * call to the SYS_EXIT semihosting call sys_exit -- pret pokediamond's
 * arm9/asm/RUNTIME_NITRO_eabi_init.s, which is this function alone.
 * ldr ip, [pc]; bx ip; .word sys_exit */
extern void sys_exit(void);

void _ExitProcess(void) {
    sys_exit();
}
