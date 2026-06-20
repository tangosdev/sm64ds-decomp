/* func_ov006_0210e4c8 at 0x0210e4c8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

int func_ov006_0210e4c8(int *r0) {
    int i = 0;
    do {
        if ((r0 + i)[0x6c / 4] > 0x168) {
            return 1;
        }
        i++;
    } while (i < 8);
    return 0;
}
