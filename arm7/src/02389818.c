/* storage 0x02389818 runtime 0x038016B0 size 0x30 wram ARM, no pools.
 * One BL (fn_02383024 = 0x037FAEBC). Constants homed in callee-saved regs. */

extern int fn_02383024(int a1, int a2, int a3);

int fn_02389818(int a1) {
    int save = a1;
    int eight = 8;
    int zero = 0;
    int r;
    do {
        r = fn_02383024(eight, save, zero);
    } while (r != 0);
    return r;
}
