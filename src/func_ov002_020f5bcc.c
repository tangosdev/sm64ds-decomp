/* func_ov002_020f5bcc at 0x020f5bcc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f5bcc(char *r0) {
    int i = 0;
    do {
        i++;
        if (*(unsigned char*)(r0 + 0x44) != 0) {
            *(unsigned char*)(r0 + 0x4a) = 1;
        }
        r0 += 0x4c;
    } while (i < 4);
}
