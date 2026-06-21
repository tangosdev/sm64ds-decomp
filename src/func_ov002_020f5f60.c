/* func_ov002_020f5f60 at 0x020f5f60
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void _ZN6Memory16operator_delete2EPv(void *p);

void func_ov002_020f5f60(void *self) {
    void *p = *(void **)((char *)self + 0x510);
    if (p) {
        _ZN6Memory16operator_delete2EPv(p);
        *(void **)((char *)self + 0x510) = 0;
    }
}
