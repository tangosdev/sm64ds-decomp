/* func_ov002_020f6424 at 0x020f6424
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_ov002_020f5f0c(int arg0, unsigned char arg1);

int func_ov002_020f6424(char *o) {
    func_ov002_020f5f0c(*(int *)(o + 0xd8), *(unsigned char *)(o + 0x100));
    return 1;
}
