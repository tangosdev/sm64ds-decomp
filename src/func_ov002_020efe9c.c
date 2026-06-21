/* func_ov002_020efe9c at 0x020efe9c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020efe9c(int *r0) {
    unsigned char v = ((unsigned int)r0[2] >> 0xc) & 3;
    return (v == 1) ? 1 : 0;
}
