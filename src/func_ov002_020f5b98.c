/* func_ov002_020f5b98 at 0x020f5b98
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct E {
    unsigned char pad44[2];
    unsigned char f46;
    unsigned char f47;
    unsigned char pad48[2];
    unsigned char f4a;
    unsigned char pad[0x4c - 0x4b];
};

void func_ov002_020f5b98(unsigned char *p) {
    int i = 0;
    do {
        if (p[0x44]) {
            p[0x46] = 2;
            p[0x47] = 0;
            p[0x4a] = 0;
        }
        i++;
        p += 0x4c;
    } while (i < 4);
}
