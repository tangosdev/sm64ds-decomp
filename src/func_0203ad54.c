/* func_0203ad54 at 0x0203ad54
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

struct Inner { char pad[4]; unsigned char b; };
struct Outer { struct Inner *p; };

unsigned char func_0203ad54(struct Outer *o) {
    return o->p->b;
}
