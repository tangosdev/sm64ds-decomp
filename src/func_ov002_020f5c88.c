/* func_ov002_020f5c88 at 0x020f5c88
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f5c88(unsigned char *p) {
    if (!p[0x90]) return;
    *(int*)(p + 0x4c) = 0x80000;
    *(int*)(p + 0x50) = 0x60000;
    *(unsigned short*)(p + 0x7c) = 0;
    p[0x92] = 1;
    *(int*)(p + 0x74) = 0xccc;
    *(unsigned short*)(p + 0x78) = 0x155;
}
