/* func_ov002_020c924c at 0x020c924c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_0201f32c(int r0);

int func_ov002_020c924c(char *r0) {
    char *r4 = r0;
    if (*(unsigned short*)(r4 + 0x600 + 0xa4) == 0) {
        func_0201f32c((int)(short)*(int*)(r4 + 0x688));
        *(unsigned char*)(r4 + 0x6e3) = 0x15;
    }
    return 0;
}
