/* func_ov006_02104e70 at 0x02104e70
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02104e70(unsigned char *p) {
    p += 0x4000;
    p[0x677] = p[0xfde];
}
