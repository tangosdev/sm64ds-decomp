/* func_ov002_020e9804 at 0x020e9804
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Obj { char pad[1]; };

extern void _ZN9Animation7AdvanceEv(void *anim);
extern void func_ov002_020e7fcc(struct Obj *self);
extern int _ZN9Animation8FinishedEv(void *anim);
extern void _ZN9ActorBase18MarkForDestructionEv(struct Obj *self);

void func_ov002_020e9804(struct Obj *self) {
    _ZN9Animation7AdvanceEv((char*)self + 0x35c);
    func_ov002_020e7fcc(self);
    if (_ZN9Animation8FinishedEv((char*)self + 0x35c)) {
        _ZN9ActorBase18MarkForDestructionEv(self);
    }
}
