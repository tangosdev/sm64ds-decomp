/* storage 0x02380B14 runtime 0x037F89AC size 0xC (+4 pool) wram ARM.
 * Absolute tail call: fn_02382B28(0x1000). */

extern int fn_02382B28(int);

int fn_02380B14(void) {
    return ((int (*)(int))fn_02382B28)(0x1000);
}
