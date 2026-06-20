/* func_ov006_02104558 at 0x02104558
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02104558(unsigned char *r0) {
    int i;
    for (i = 0; i < 0x40; i++) {
        r0[0x4000 + 0x6b8] = 0;
        r0[0x4000 + 0x6bd] = 0;
        r0 += 0x18;
    }
}
