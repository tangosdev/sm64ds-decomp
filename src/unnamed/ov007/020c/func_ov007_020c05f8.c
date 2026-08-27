/* func_ov007_020c05f8 @ 0x20c05f8 (ov007) -- tail-call veneer to func_ov007_020c3d1c (0x20c3d1c).
 * ldr ip, [pc]; bx ip; .word 0x20c3d1c
 */
extern void func_ov007_020c3d1c(void);

void func_ov007_020c05f8(void) {
    func_ov007_020c3d1c();
}
