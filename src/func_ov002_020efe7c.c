/* func_ov002_020efe7c at 0x020efe7c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020efe7c(char *r0) {
    unsigned char v = *(unsigned *)(r0 + 8) >> 0xa & 3;
    return v == 1;
}
