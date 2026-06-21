/* func_ov002_020f5678 at 0x020f5678
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f5678(int r0, int r1) {
    int p = r0 + r1 * 0x4c;
    *(int*)(p + 0x28) = 0xccc;
    *(short*)(p + 0x2c) = 0x155;
}
