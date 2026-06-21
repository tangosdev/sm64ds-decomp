/* func_ov002_020f6c24 at 0x020f6c24
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020f6c24(unsigned char *r0, unsigned char *r1) {
    r0[0x102] = *r1;
    return 1;
}
