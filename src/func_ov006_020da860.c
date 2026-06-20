/* func_ov006_020da860 at 0x020da860
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

int func_ov006_020da860(unsigned char *r0, int r1) {
    int i = 0;
    do {
        if (r1 != r0[0x2b]) {
            return 0;
        }
        i++;
        r0 += 0x30;
    } while (i < 5);
    return 1;
}
