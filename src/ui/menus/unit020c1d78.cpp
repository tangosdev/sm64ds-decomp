//cpp
/* unit020c1d78 -- ov007 unit 0x020c1d78..0x020c1db0: 3 functions, .text only.
 *
 * A table of ten int flags. ov007 is the title and file-select menu program
 * that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or vtable, so
 * no class name is claimed and the file keeps the unit<addr> name.
 *
 * What the code does. func_ov007_020c1d78 clears flag i, func_ov007_020c1d8c
 * sets it to 1 and func_ov007_020c1da0 reads it. The table is the 0x28-byte
 * bss block at 0x02104bec. Menu code across the overlay calls the three
 * functions, and three sites load func_ov007_020c1d78 or 020c1d8c as a
 * function pointer.
 *
 * Edges: the bss block is referenced by these three functions and nothing
 * else in the overlay. It sits in link order between the bss words
 * unit020c1404's functions use (0x02104bdc..0x02104be8) and the one
 * unit020c3ce4's use (0x02104c14). The neighbours
 * func_ov007_020c19cc and func_ov007_020c1db0 (the touch-record update)
 * reference none of it.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * Text only: the table stays ROM-owned and is reached by extern. The
 * functions keep their ROM names under C linkage.
 */

extern "C" {

extern int data_ov007_02104bec[10];

int func_ov007_020c1da0(int i) {
    return data_ov007_02104bec[i];
}

void func_ov007_020c1d8c(int i) {
    data_ov007_02104bec[i] = 1;
}

void func_ov007_020c1d78(int i) {
    data_ov007_02104bec[i] = 0;
}

} // extern "C"
