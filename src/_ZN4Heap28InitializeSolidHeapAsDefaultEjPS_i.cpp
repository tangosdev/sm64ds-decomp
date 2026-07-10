//cpp
/* _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i @ 0x203c2d8 (arm9) -- tail-call veneer to _ZN4Heap23SetupSolidHeapAsDefaultEjPS_i (0x203c2e4).
 * ldr ip, [pc]; bx ip; .word 0x203c2e4
 */
extern "C" {
extern void _ZN4Heap23SetupSolidHeapAsDefaultEjPS_i(void);
void _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i(void) {
    _ZN4Heap23SetupSolidHeapAsDefaultEjPS_i();
}
}
