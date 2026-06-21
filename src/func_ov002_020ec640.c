/* func_ov002_020ec640 at 0x020ec640
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

unsigned char func_ov002_020ec640(unsigned char *r0) {
    return (r0[0x428] >> 2) & 0xf;
}
