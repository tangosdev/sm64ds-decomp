/* func_ov002_020bd354 at 0x020bd354
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern unsigned short data_0209b274;

int func_ov002_020bd354(char *o, int unused, int r2) {
    if (r2 == data_0209b274) {
        *(unsigned char *)(o + 0x6ff) = 1;
        if (*(unsigned short *)(o + 0x6ae) == 0) {
            *(unsigned short *)(o + 0x6ae) = 0x198;
        }
    }
    *(int *)(o + 0xa8) = -0x20000;
    *(int *)(o + 0x684) = *(int *)(o + 0x60);
    return 1;
}
