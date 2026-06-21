/* func_ov002_020f1578 at 0x020f1578
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int func_ov002_020dd8b8(char *r0);

int func_ov002_020f1578(char *self, char *p) {
    int ok = (*(unsigned short*)(p + 0xc) == 0xbf);
    int r;
    if (!ok) {
        return ok;
    }
    r = func_ov002_020dd8b8(p);
    if (r) {
        r = 1;
        *(unsigned char*)(self + 0x32c) = 1;
    }
    return r;
}
