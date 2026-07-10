/* func_ov007_020c05f8 @ 0x20c05f8 (ov007) -- tail-call veneer to _ZN6Player17St_EndingFly_MainEv (0x20c3d1c).
 * ldr ip, [pc]; bx ip; .word 0x20c3d1c
 */
extern void _ZN6Player17St_EndingFly_MainEv(void);

void func_ov007_020c05f8(void) {
    _ZN6Player17St_EndingFly_MainEv();
}
