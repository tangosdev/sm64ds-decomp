/* func_ov002_020f65ec at 0x020f65ec
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Anim { int dummy; };
extern void _ZN9Animation7AdvanceEv(void *a);

void func_ov002_020f65ec(unsigned char *p) {
    void *a = *(void**)(p + 0x7c);
    if (a == 0) return;
    _ZN9Animation7AdvanceEv(a);
}
