/* func_ov002_020f378c at 0x020f378c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f378c(unsigned char *r0, int r1) {
    unsigned char *base = r0 + r1 * 0x30;
    base[0x15e] = 0;
}
