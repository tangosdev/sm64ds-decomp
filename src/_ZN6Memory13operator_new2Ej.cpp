//cpp
/* _ZN6Memory13operator_new2Ej @ 0x203cbd8 (arm9) -- tail-call veneer to _Znwj (0x203cc0c).
 * ldr ip, [pc]; bx ip; .word 0x203cc0c
 */
extern "C" {
extern void _Znwj(void);
void _ZN6Memory13operator_new2Ej(void) {
    _Znwj();
}
}
