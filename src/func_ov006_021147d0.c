/* func_ov006_021147d0 at 0x021147d0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

int func_ov006_021147d0(unsigned char *r0) {
    int i = 0;
    do {
        unsigned char *p = r0 + 0x5000;
        if (p[0x9bc] == 1) {
            return 1;
        }
        i++;
        r0 += 0x24;
    } while (i < 0x40);
    return 0;
}
