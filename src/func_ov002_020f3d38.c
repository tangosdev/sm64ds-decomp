/* func_ov002_020f3d38 at 0x020f3d38
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f3d38(char *r0, int r1) {
    int ip = r1 * 0x4c;
    unsigned short h = *(unsigned short *)(r0 + 0x30 + ip);
    char *r3 = r0 + 0x30 + ip;
    if (h != 0) {
        *(unsigned short *)r3 = h - 1;
        return;
    }
    *(int *)(r0 + ip) = -0x10000;
    *(int *)(r0 + ip + 4) = -0x10000;
    *(int *)(r0 + ip + 8) = 0x2000;
    *(unsigned short *)(r0 + ip + 0x2e) = 0;
    *(unsigned short *)r3 = 0;
    {
        unsigned char *b = (unsigned char *)(r0 + 0x47);
        b[ip] = b[ip] + 1;
    }
}
