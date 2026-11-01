/* storage 0x02380A20 runtime 0x037F88B8 size 0x18 (+8 pool) wram ARM.
 * Clear word_3806DE0 (halfword) and dword_27FFC3C, return the latter. */

extern short word_3806DE0;
extern int dword_27FFC3C;

int *fn_02380A20(void) {
    word_3806DE0 = 0;
    dword_27FFC3C = 0;
    return &dword_27FFC3C;
}
