/* func_ov006_020faeec at 0x020faeec
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020faeec(char *r0) {
    int i = 0;
    do {
        *(unsigned char *)(r0 + 0x5000 + 0xbd4) = 0;
        i++;
        *(unsigned char *)(r0 + 0x5000 + 0xbd5) = 0;
        r0 += 0xc;
    } while (i < 4);
}
