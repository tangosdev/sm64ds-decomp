/* func_ov006_020e38b0 at 0x020e38b0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern void func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);

void func_ov006_020e38b0(char *r0)
{
    int i;
    char *p = r0;
    for (i = 0; i < 0x3c; i++) {
        unsigned char mode = *(unsigned char*)(p + 0x4ff5);
        if (mode != 0) {
            int a = *(int*)(p + 0x4fe0) >> 0xc;
            int b = *(int*)(p + 0x4fe4) >> 0xc;
            int c = *(unsigned short*)(p + 0x4ff2);
            if (mode == 1) {
                func_ov004_020b1ea4(a, b, c, -1, -1, 0, 0);
            } else {
                func_ov004_020b1ea4(a, b, c, -1, -1, 0, 0x32);
            }
        }
        p += 0x18;
    }
}
