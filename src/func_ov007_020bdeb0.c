/* func_ov007_020bdeb0 @ 0x20bdeb0 (ov007) -- tail-call veneer to func_ov007_020be098 (0x20be098).
 * ldr ip, [pc]; bx ip; .word 0x20be098
 */
extern void func_ov007_020be098(void);

void func_ov007_020bdeb0(void) {
    func_ov007_020be098();
}
