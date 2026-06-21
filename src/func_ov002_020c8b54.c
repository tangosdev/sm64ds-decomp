/* func_ov002_020c8b54 at 0x020c8b54
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020c8b54(char *r0) {
    if (*(unsigned short*)(r0 + 0x6a4) == 0) {
        *(unsigned*)(r0 + 0x98) = 0xa000;
        *(unsigned char*)(r0 + 0x6e3) = 0xf;
    }
    return 0;
}
