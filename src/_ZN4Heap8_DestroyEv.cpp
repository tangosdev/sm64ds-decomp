//cpp
/* _ZN4Heap8_DestroyEv @ 0x203c74c (arm9) -- tail-call veneer to _ZN4Heap7DestroyEv (0x203c758).
 * ldr ip, [pc]; bx ip; .word 0x203c758
 */
extern "C" {
extern void _ZN4Heap7DestroyEv(void);
void _ZN4Heap8_DestroyEv(void) {
    _ZN4Heap7DestroyEv();
}
}
