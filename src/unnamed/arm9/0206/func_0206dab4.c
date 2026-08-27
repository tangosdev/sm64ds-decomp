// HAND-ASM PRIMITIVE: byte-faithful asm-block match. The `swp` atomic swap has no C
// equivalent under mwccarm (predates C11 _Atomic), so this was assembly in the original --
// the asm block is the faithful source. Counts as matched (asm-primitive policy).
asm void func_0206dab4(void) { swp r0, r0, [r1]; bx lr }
