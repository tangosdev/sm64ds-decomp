/* func_ov006_0211de7c at 0x0211de7c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

int func_ov006_0211de7c(unsigned char *r0) {
    int found = 0;
    int i = 0;
    do {
        if (r0[0x4a70] != 0) {
            if (r0[0x4a73] != 2) {
                found++;
            }
        }
        i++;
        r0 = r0 + 0x14;
    } while (i < 0x10);
    return found != 0;
}
