/* func_ov006_02106758 at 0x02106758
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02106758(unsigned char *r0) {
    int j;
    int i;
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 0xa; i++) {
            *(unsigned char *)(r0 + i + 0x4000 + 0xec8) = 0xff;
        }
        r0 += 0xa;
    }
}
