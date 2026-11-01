/* storage 0x0238222C runtime 0x037FA0C4 size 0x7C wram ARM.
 * Conditional notify, take lock word, fill 6-slot record, run callback, unlock.
 * The comma store of t5 inside the check is load-bearing: with the init alone
 * the a5 load sinks late (0x74, PUSH R4-R8); with the comma alone it sits past
 * the first BEQ. Together b56 merges both into one early LDR into R6, which
 * recruits R9 and the SUB SP,#4 frame. */

extern void fn_02382B38(void); /* =0x037FA9D0 */
extern int fn_02382AC4(void); /* =0x037FA95C */
extern void fn_0238231C(int *, int, int); /* =0x037FA1B4 */
extern int fn_02382AD8(int); /* =0x037FA970 */

int fn_0238222C(int *a1, int a2, int a3, int a4, int a5, int a6, int a7) {
    int t5 = a5;
    int v11;
    if (a1 == 0 || (t5 = a5, *a1 != 0))
        fn_02382B38();
    v11 = fn_02382AC4();
    a1[7] = a4;
    a1[8] = t5;
    a1[9] = a2;
    a1[10] = a3;
    *a1 = a6;
    a1[1] = a7;
    fn_0238231C(a1, 0, 0);
    return fn_02382AD8(v11);
}
