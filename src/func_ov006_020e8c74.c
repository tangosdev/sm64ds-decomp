/* func_ov006_020e8c74 at 0x020e8c74
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020e8c74(char *r0, int r1) {
    int r3 = r1 * 0x14;
    int *p = (int *)(r0 + 0x5288 + r3);
    *p = *p - (*p >> 6);
    if (*p <= 0x900) {
        *(unsigned char *)(r0 + r3 + 0x5000 + 0x291) = 2;
    }
}
