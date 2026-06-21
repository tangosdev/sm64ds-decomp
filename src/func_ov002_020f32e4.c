/* func_ov002_020f32e4 at 0x020f32e4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f32e4(int *r0, int r1) {
    int *base = (int *)((char *)r0 + r1 * 0x30);
    *(unsigned short *)((char *)base + 0x100 + 0x58) = 0x10;
    base[0x150 / 4] = 0x1800;
    base[0x154 / 4] = 0x1800;
    *((unsigned char *)base + 0x15e) = 1;
}
