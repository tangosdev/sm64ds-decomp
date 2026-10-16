/* storage 0x02381FF8 runtime 0x037F9E90 size 0x7C + pool 0x94 wram ARM.
 * One-shot init guarded by the halfword flag: set flag, call init worker,
 * clear two data words, program two timer regs plus 0xC1, register callback,
 * run worker, clear trailing word. Six pools, three calls. */

extern unsigned short word_3806FD0; /* =0x03806FD0 */
extern int dword_3806FD4; /* =0x03806FD4 */
extern int dword_3806FD8[2]; /* =0x03806FD8 */
extern void fn_02381E94(int); /* =0x037F9D2C */
extern void fn_02380378(int, void *); /* =0x037F8810 */
extern void fn_023802C0(int); /* =0x037F8758 */
extern void fn_02381F60(void); /* =0x037F9DF8 */

short *fn_02381FF8(void) {
    short *result = (short *)&word_3806FD0;
    if (word_3806FD0 == 0) {
        word_3806FD0 = 1;
        fn_02381E94(0);
        dword_3806FD8[0] = 0;
        dword_3806FD8[1] = 0;
        *(volatile unsigned short *)0x04000102 = 0;
        *(volatile unsigned short *)0x04000100 = 0;
        *(volatile unsigned short *)0x04000102 = 0xC1;
        fn_02380378(8, fn_02381F60);
        fn_023802C0(8);
        dword_3806FD4 = 0;
        result = (short *)&dword_3806FD4;
    }
    return result;
}
