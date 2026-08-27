//cpp
// @symbol _ZN4Door8BehaviorEv
// recovered name: Door::Behavior
/* recovered: renamed to Class_Method, vtable slot 6 */
/* Door::Behavior -- vtable slot 6, ov100 0x02145550. This is the real virtual
 * override declared in include/Door.h rather than a free function that only
 * borrows the method's mangled symbol.
 *
 * FOLDED ONTO include/Door.h alongside the rest of the class: `mCallbackNode`
 * rather than a raw `c + 0x140`. Byte-exact under the pinned 2004/b56 before
 * and after.
 *
 * THE CALLBACK IS A Door MEMBER FUNCTION, and that is measured rather than
 * assumed. This file used to declare the pointer-to-member as
 * `void (Base::*)(int)` over a local empty `struct Base {}` and cast the
 * object to it -- a shadow standing in for a class nobody had identified.
 * Spelt honestly as `void (Door::*)(int)` and called on the Door directly,
 * mwcc emits the identical 0x50 bytes, including the full member-pointer
 * dispatch the ROM does at 0x02149d48..0x02149d5c: `asr #1` on the stored
 * offset, bit 0 tested for "this is a vtable index rather than a direct
 * address", both arms present. A cheaper representation would have collapsed
 * that sequence, so the bytes do positively distinguish this shape -- Door
 * being polymorphic with a single base is what makes it the general form.
 * The shadow class is gone.
 *
 * The node itself keeps a local struct: only its +0x8 word (the member
 * pointer's first half, which is what the null test reads) and +0xc are
 * touched here, and it is written by src/func_ov100_021453d8.cpp, which is
 * out of this slice. */
#include "Door.h"
typedef void (Door::*PMF)(int);
struct CallbackNode {
    char pad[8];
    PMF callback;
};
extern "C" {
extern int func_ov100_02145370(Door *self);
}
int Door::Behavior() {
    int res = func_ov100_02145370(this);
    CallbackNode *node = (CallbackNode *)mCallbackNode;
    if (*(int*)&node->callback != 0) {
        (this->*(node->callback))(res);
    }
    return 1;
}
