//cpp
/* _ZN4Heap11_DeallocateEPv @ 0x203c280 (arm9) -- tail-call veneer to _ZN4Heap10DeallocateEPv (0x203c538).
 * ldr ip, [pc]; bx ip; .word 0x203c538
 */
extern "C" {
extern void _ZN4Heap10DeallocateEPv(void);
void _ZN4Heap11_DeallocateEPv(void) {
    _ZN4Heap10DeallocateEPv();
}
}
