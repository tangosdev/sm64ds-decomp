/* func_ov002_020f5a6c at 0x020f5a6c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f5a6c(unsigned char* r0) {
    int i = 0;
    do {
        r0[0x44] = 0;
        r0[0x45] = 0;
        r0[0x4a] = 0;
        i++;
        r0 += 0x4c;
    } while (i < 4);
}
