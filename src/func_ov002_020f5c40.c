/* func_ov002_020f5c40 at 0x020f5c40
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f5c40(unsigned char *p) {
    if (p[0xdc] == 0) return;
    *(int*)(p + 0x98) = 0x80000;
    *(int*)(p + 0x9c) = 0x60000;
    *(short*)(p + 0xc8) = 0;
    p[0xde] = 1;
    *(int*)(p + 0xc0) = 0xccc;
    *(short*)(p + 0xc4) = 0x155;
}
