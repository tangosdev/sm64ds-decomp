//cpp
/* _ZN4Heap7_SizeofEPv @ 0x203c274 (arm9) -- tail-call veneer to _ZN4Heap6SizeofEPv (0x203c454).
 * ldr ip, [pc]; bx ip; .word 0x203c454
 */
extern "C" {
extern void _ZN4Heap6SizeofEPv(void);
void _ZN4Heap7_SizeofEPv(void) {
    _ZN4Heap6SizeofEPv();
}
}
