/* func_0204ebb8 @ 0x204ebb8 (arm9) -- tail-call veneer to _ZN13HeapAllocator6RemoveEv (0x204df38).
 * ldr ip, [pc]; bx ip; .word 0x204df38
 */
extern void _ZN13HeapAllocator6RemoveEv(void);

void func_0204ebb8(void) {
    _ZN13HeapAllocator6RemoveEv();
}
