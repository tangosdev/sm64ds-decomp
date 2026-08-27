/* func_ov063_02118ea0 @ 0x2118ea0 (ov063) -- tail-call veneer to func_ov063_02118eac (0x2118eac).
 * ldr ip, [pc]; bx ip; .word 0x2118eac
 */
extern void func_ov063_02118eac(void);

void func_ov063_02118ea0(void) {
    func_ov063_02118eac();
}
