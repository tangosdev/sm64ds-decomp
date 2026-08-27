//cpp
/* _ZN13HeapAllocator7DestroyEv @ 0x204e3b4 (arm9) -- tail-call veneer to _ZN13HeapAllocator6RemoveEv (0x204df38).
 * ldr ip, [pc]; bx ip; .word 0x204df38
 */
extern "C" {
extern void _ZN13HeapAllocator6RemoveEv(void);
void _ZN13HeapAllocator7DestroyEv(void) {
    _ZN13HeapAllocator6RemoveEv();
}
}
