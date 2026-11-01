/* storage 0x02382EF8 runtime 0x037FAD90 size 0xC wram ARM.
 * Trampoline tail-call into fn_0238313C. One ABS32 pool word. */

extern int fn_0238313C(void);

int fn_02382EF8(int a1) {
    (void)a1;
    return fn_0238313C();
}
