/* func_02014a20 @ 0x2014a20 (arm9) -- tail-call veneer to _ZN18MovingCylinderClsn10GetOwnerIDEv (0x201493c).
 * ldr ip, [pc]; bx ip; .word 0x201493c
 */
extern void _ZN18MovingCylinderClsn10GetOwnerIDEv(void);

void func_02014a20(void) {
    _ZN18MovingCylinderClsn10GetOwnerIDEv();
}
