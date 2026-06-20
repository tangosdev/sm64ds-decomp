/* func_ov006_020e95a4 at 0x020e95a4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020e95a4(unsigned char *r0) {
    int i;
    for (i = 0; i < 5; i++) {
        *(int *)(r0 + 0x5000 + 0x208) = 0;
        *(int *)(r0 + 0x5000 + 0x20c) = 0;
        *(short *)(r0 + 0x5200 + 0x14) = 0;
        r0[0x5000 + 0x218] = 0;
        r0[0x5000 + 0x21a] = 0;
        r0[0x5000 + 0x21b] = 0;
        r0[0x5000 + 0x21c] = 0;
        r0 += 0x18;
    }
}
