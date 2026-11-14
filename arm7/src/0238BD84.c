/* storage 0x0238BD84 runtime 0x03803C1C size 0x48 (no pool) wram ARM.
 * Retry loop: while (fn_02383024(13, a1, 0) != 0) fn_02388CD8(1).
 * Entry MOVs (R7 = a1; R6 = 1, R5 = 13, R4 = 0 declared in reverse so the
 * allocator lands them so) then B into the check; loop body falls through
 * into the check (MOV R0,R5 / MOV R1,R7 / MOV R2,R4 / BL / CMP #0 / BNE).
 * No pools, two BLs (0x03800B70, 0x037FAEBC). IDA audit confirms. */

extern int fn_02388CD8(int a1);
extern int fn_02383024(int a1, int a2, int a3);

void fn_0238BD84(int a1) {
    int zero = 0;
    int thirteen = 13;
    int one = 1;
    while (fn_02383024(thirteen, a1, zero) != 0)
        fn_02388CD8(one);
}
