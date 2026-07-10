//cpp
/* _Znwj @ 0x203cbe4 (arm9) -- tail-call veneer to _Znwj (0x203cc0c).
 * ldr ip, [pc]; bx ip; .word 0x203cc0c
 */
extern "C" {
extern void _Znwj(void);
void _Znwj(void) {
    _Znwj();
}
}
