/* func_ov006_021048e4 at 0x021048e4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

extern unsigned char BOTTOM_SCREEN_RELATED[];

void func_ov006_021048e4(unsigned char *r0) {
    if (r0[0x4693] != 0) return;
    r0[0x4693] = 1;
    *(unsigned short *)(r0 + 0x4600 + 0x90) = 0;
    BOTTOM_SCREEN_RELATED[0] |= 1;
}
