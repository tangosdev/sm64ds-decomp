/* func_ov002_020d80d0 at 0x020d80d0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_ov002_020bd984(char *r0, int r1);

void func_ov002_020d80d0(char *r0) {
    if (*(unsigned char*)(r0 + 0x6f8) != 1) return;
    *(unsigned short*)(r0 + 0x600 + 0xbe) = 0;
    *(unsigned char*)(r0 + 0x6f8) = 0;
    *(unsigned char*)(r0 + 0x6f4) = 0;
    *(unsigned char*)(r0 + 0x714) = 0;
    func_ov002_020bd984(r0, 0x32);
}
