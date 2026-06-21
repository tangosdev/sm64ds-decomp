/* func_ov002_020d225c at 0x020d225c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020d225c(int r0) {
    if (*(int*)(r0 + 8) == 3 &&
        *(unsigned char*)(r0 + 0x721) == 2 &&
        *(unsigned char*)(r0 + 0x6e6) == 0)
        return 1;
    return 0;
}
