/* storage 0x0238BD5C runtime 0x03803BF4 size 0x28 (listed 0x24 + pool) wram ARM.
 * Jump-table fill: 25 iterations; where (result & 1) set, table[v2] = a2.
 * The table base stays a global (hoisted by the compiler into R2); a v3
 * local takes R3 instead and breaks the STRNE form. do-while
 * (CMP #0x19 / BLT); returns result (shifted to 0). One address pool
 * (0x03805DF4, the dispatch table that case 0x41/0x42 arms with
 * fn_0238B1D4's address), no calls. IDA: ANDS+R3/STRNE, LSR, ADD, CMP, BLT. */

extern int unk_3805DF4[];

unsigned fn_0238BD5C(unsigned result, int a2) {
    int v2 = 0;
    do {
        if ((result & 1) != 0)
            unk_3805DF4[v2] = a2;
        result >>= 1;
        ++v2;
    } while (v2 < 25);
    return result;
}
