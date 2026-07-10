//cpp
/* _ZN4Heap9_AllocateEji @ 0x203c29c (arm9) -- tail-call veneer to _ZN4Heap8AllocateEji (0x203c6cc).
 * ldr ip, [pc]; bx ip; .word 0x203c6cc
 */
extern "C" {
extern void _ZN4Heap8AllocateEji(void);
void _ZN4Heap9_AllocateEji(void) {
    _ZN4Heap8AllocateEji();
}
}
