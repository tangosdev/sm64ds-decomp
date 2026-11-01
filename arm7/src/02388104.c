/* storage 0x02388104 runtime 0x037FFF9C size 0xC (+4 pool) wram ARM.
 * Tail call through r12 into mid-function label loc_37FF6A4. */

extern int loc_37FF6A4(int);

int fn_02388104(int **a1) {
    return loc_37FF6A4((*a1)[1]);
}
