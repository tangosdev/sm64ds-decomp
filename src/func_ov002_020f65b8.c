/* func_ov002_020f65b8 at 0x020f65b8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Obj {
    char pad[0x7c];
    void *seq;   // 0x7c
};

extern void _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc);

void func_ov002_020f65b8(struct Obj *self) {
    void *seq = self->seq;
    if (seq == 0) return;
    _ZN15TextureSequence6UpdateER15ModelComponents(seq, (char*)self + 8);
}
