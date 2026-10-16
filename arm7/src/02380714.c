/* storage 0x02380714 runtime 0x037F85AC size 0x4C + 12 pool wram ARM.
 * Three pool data refs, one indirect call (MOV LR,PC / BX R2, no BL). */

extern int unk_3806DE4;
extern int cnt_27FFC3C;
extern int flg_380FFF8;

void fn_02380714(void) {
    void (*f)(void) = *(void (**)(void))((char *)&unk_3806DE4 + 0x60);
    ++*(volatile int *)&cnt_27FFC3C;
    if (f != 0)
        f();
    *(volatile int *)&flg_380FFF8 |= 1;
}
