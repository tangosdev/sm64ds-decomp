/* func_ov007_020c2bf8 at 0x020c2bf8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern void _ZN6Player17St_EndingFly_MainEv(void *);

struct S {
    int state;        /* +0 */
    int flags;        /* +4 */
    int pad08;
    int pad0c;
    int pad10;
    int pad14;
    void *p18;        /* +0x18 */
    void *p1c;        /* +0x1c */
    void *p20;        /* +0x20 */
    void *p24;        /* +0x24 */
    void *p28;        /* +0x28 */
    void *p2c;        /* +0x2c */
};

void func_ov007_020c2bf8(struct S *s)
{
    switch (s->state) {
    case 0:
        _ZN6Player17St_EndingFly_MainEv(s->p24);
        _ZN6Player17St_EndingFly_MainEv(s->p28);
        _ZN6Player17St_EndingFly_MainEv(s->p1c);
        break;
    case 1:
        _ZN6Player17St_EndingFly_MainEv(s->p2c);
        break;
    }
    _ZN6Player17St_EndingFly_MainEv(s->p18);
    if (s->flags & 1)
        _ZN6Player17St_EndingFly_MainEv(s->p20);
    _ZN6Player17St_EndingFly_MainEv(s);
}
