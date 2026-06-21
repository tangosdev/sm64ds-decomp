/* func_ov002_020f16cc at 0x020f16cc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void _ZN5Event8ClearBitEj(unsigned int x);

int func_ov002_020f16cc(unsigned char *p) {
    _ZN5Event8ClearBitEj(p[0x10d]);
    return 1;
}
