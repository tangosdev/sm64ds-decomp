/* func_ov002_020c9cc0 at 0x020c9cc0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern signed char data_02092110;

void func_ov002_020c9cc0(unsigned char* p) {
    int v;
    *(unsigned short*)(p + 0x600 + 0xa6) = 3;
    p[0x6e3] = 6;
    p[0x716] = 1;
    p[0x713] = 0;
    v = *(int*)(p + 0xa8);
    if (v >= 0x14000) {
        *(int*)(p + 0xa8) = 0x14000;
    }
    {
        signed char g = data_02092110;
        if (g == 0x26 || g == 0x12) {
            p[0x6f5] = 0;
        }
    }
}
