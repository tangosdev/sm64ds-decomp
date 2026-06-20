/* func_ov006_02100734 at 0x02100734
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02100734(unsigned char *r0, int r1) {
    unsigned char *b = r0 + (r1 << 6);
    if (b[0x5000 + 0x294] == 0) {
        return;
    }
    *(short *)(b + 0x5200 + 0x90) = 0;
    *(int *)(b + 0x5000 + 0x270) = 0;
    *(short *)(b + 0x5200 + 0x92) = 0x40;
    b[0x5000 + 0x296] = 0xc;
}
