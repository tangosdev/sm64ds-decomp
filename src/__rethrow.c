// @symbol __rethrow
// MSL C++ runtime rethrow entry. mwccarm emits `bl __rethrow` itself for a bare `throw;`
// inside a catch block, same proof as __end__catch above: the probe object's undefined
// symbol table carries __rethrow and the source never names it. The ROM's own rethrow
// calls land here (notes/mwccarm-codegen.md 9a), and src/func_02073300.cpp,
// src/func_020733a8.c and src/func_02073470.cpp all branch to this address.
// Was named for the old symbol func_020717c0. The file follows the symbol, the same
// move __end__catch needed: tools/srcpath.py keys a source to its file stem, so under
// the old stem config declared a function at 0x020717c0 that no source resolved to,
// and tools/nearmiss_db.py resync-names would have relabelled the stored row to
// __rethrow and then lost its link to this source.
// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. Nintendo shipped this as an
// assembly primitive, so there is no original C to recover and no match to chase. Counts as
// done under the asm-primitive policy - see notes/arm9-endgame.md.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
extern void func_02071be8(void);

asm void __rethrow(void) {
    mov ip, sp
    sub sp, sp, #0x70
    str r4, [sp, #0x2c]
    str r5, [sp, #0x30]
    str r6, [sp, #0x34]
    str r7, [sp, #0x38]
    str r8, [sp, #0x3c]
    str r9, [sp, #0x40]
    str r10, [sp, #0x44]
    str r11, [sp, #0x48]
    str ip, [sp, #0x14]
    str ip, [sp, #0x5c]
    str lr, [sp, #0x10]
    mov ip, #0
    str ip, [sp]
    str ip, [sp, #4]
    str ip, [sp, #8]
    mov r0, sp
    b func_02071be8
}
