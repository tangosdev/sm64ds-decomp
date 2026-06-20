/* func_ov006_0211f51c at 0x0211f51c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_0211f51c(unsigned char *r0) {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (r0[0x4000 + 0x677] != 0) {
            if (r0[0x4000 + 0x678] == 1) {
                r0[0x4000 + 0x678] = 2;
            }
        }
        r0 += 0x24;
    }
}
