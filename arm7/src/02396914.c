/* storage 0x02396914 runtime 0x027C888C size 0x44 main.
 * Runs fn_023965A8, then returns fn_023830B4(0, state base + 0x4F4, 180).
 * Pool: unk_380FFF4, 0x4F4. */

extern int fn_023965A8(void); /* =0x027C8520 */
extern int fn_023830B4(int a1, void *a2, int a3); /* =0x037FAB4C */
extern unsigned int unk_380FFF4; /* =0x0380FFF4 */

int fn_02396914(void) {
    volatile int zero;
    fn_023965A8();
    zero = 0;
    return fn_023830B4(zero, (void *)(*(int *)&unk_380FFF4 + 0x4F4), 180);
}
