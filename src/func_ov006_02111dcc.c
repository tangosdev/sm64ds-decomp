/* func_ov006_02111dcc at 0x02111dcc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

int func_ov006_02111dcc(int *r0, int r1) {
    if (*((unsigned char *)r0 + 0x30) == 0) return 0;
    if (r0[0x10] > 0) return 0;
    r0[0x10] = r1;
    return 1;
}
