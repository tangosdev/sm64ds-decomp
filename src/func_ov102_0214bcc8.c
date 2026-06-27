/* func_ov102_0214bcc8 at 0x0214bcc8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov102).
 */
extern void func_ov102_0214b3b8(void *c);
extern void func_ov102_0214c0b8(char *c);
extern void _ZN9Animation7AdvanceEv(void *a);

void func_ov102_0214bcc8(char *c) {
    int f = *(int *)(c + 0xb0);
    int a = (f & 0x400) ? 1 : 0;
    if (a != 0) {
        func_ov102_0214b3b8(c);
    } else {
        int b = (f & 0x100) ? 1 : 0;
        if (b == 0) {
            func_ov102_0214c0b8(c);
        }
    }
    _ZN9Animation7AdvanceEv(c + 0x350);
}
