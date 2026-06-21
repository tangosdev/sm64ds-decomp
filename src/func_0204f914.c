/* func_0204f914 at 0x0204f914
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

struct S { char pad[0x40]; unsigned char flag; };

void func_0204f914(struct S **pp, unsigned char v) {
    struct S *p = *pp;
    if (p != 0)
        p->flag = v;
}
