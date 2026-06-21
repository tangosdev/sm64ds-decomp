/* func_02037940 at 0x02037940
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

void func_02037940(void *p, int val) {
    *(unsigned char *)((char *)p + 0x70) = val & ~0x1c;
}
