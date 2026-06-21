/* func_02037fc0 at 0x02037fc0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

int func_02037fc0(void *p) {
    return *(unsigned short *)((char *)p + 0x1a) != 0x18;
}
