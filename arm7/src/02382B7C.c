/* storage 0x02382B7C runtime 0x037FAA14 size 0x38 + 4 pool wram ARM.
 * One pool data ref (word_3806FFC), one call (fn_02382B38, noreturn in practice). */

extern unsigned short word_3806FFC;
extern void fn_02382B38(void);

void fn_02382B7C(int a1, int a2) {
    if ((((unsigned int)a2 & 0x7F00) << 8 >> 16) == 16) {
        word_3806FFC = 1;
        return;
    }
    fn_02382B38();
}
