/* func_ov002_020f30d4 at 0x020f30d4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f30d4(char *r0) {
    int i = 0;
    do {
        r0[0x170] = 0;
        i++;
        r0[0x171] = 0;
        r0 += 0x14;
    } while (i < 5);
}
