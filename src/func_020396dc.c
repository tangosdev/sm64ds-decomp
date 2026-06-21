/* func_020396dc at 0x020396dc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

struct Inner { char pad[8]; unsigned int v; };
struct Outer { char pad[0x20]; struct Inner *p; };

unsigned int func_020396dc(struct Outer *o, unsigned int x) {
    return (x - o->p->v) >> 4;
}
