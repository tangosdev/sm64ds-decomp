// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
extern void func_02071be8(void);

asm void func_020717c0(void) {
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
