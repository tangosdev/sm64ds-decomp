//cpp
/* _Znwj @ 0x203cbe4 (arm9) -- tail-call veneer to func_0203cc0c (0x203cc0c).
 * ldr ip, [pc]; bx ip; .word 0x203cc0c
 */
extern "C" {
extern void func_0203cc0c(void);
void _Znwj(void) {
    func_0203cc0c();
}
}
