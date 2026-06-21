/* func_ov006_020e8cb0 at 0x020e8cb0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020e8cb0(char *r0, int idx) {
    short *p = (short *)(r0 + 0x528c + idx * 0x14);
    unsigned short v = *(unsigned short *)p;
    if (v != 0) {
        *p = v - 1;
        if (*p < 0) *p = 0;
        return;
    }
    *(unsigned char *)(r0 + idx * 0x14 + 0x5000 + 0x292) = 1;
    *(unsigned char *)(r0 + idx * 0x14 + 0x5000 + 0x291) = 1;
}
