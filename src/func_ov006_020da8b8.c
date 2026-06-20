/* func_ov006_020da8b8 at 0x020da8b8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

int func_ov006_020da8b8(unsigned char *r0, int r1) {
    int i = 0;
    do {
        if (r1 == r0[0x2b]) {
            return i;
        }
        i++;
        r0 += 0x30;
    } while (i < 5);
    return -1;
}
