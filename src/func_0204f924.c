/* func_0204f924 at 0x0204f924
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

struct S { unsigned char b[0x42]; };
void func_0204f924(struct S **pp, unsigned char v) {
    struct S *p = *pp;
    if (p) p->b[0x41] = v;
}
