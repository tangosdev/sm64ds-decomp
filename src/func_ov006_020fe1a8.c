/* func_ov006_020fe1a8 at 0x020fe1a8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020fe1a8(char *r0) {
    int i = 0;
    unsigned char v = i;
    do {
        *(unsigned char *)(r0 + 0x4000 + 0xf0c) = v;
        i++;
        *(unsigned char *)(r0 + 0x4000 + 0xf0e) = v;
        r0 += 0x38;
    } while (i < 0x30);
}
