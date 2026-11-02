//cpp
/* List-node destructors, the frame loop over the process lists, and the
 * pending lifecycle-sequence helpers fBase_c calls during cleanup.
 * arm9 .text 0x020440e8..0x020443c8.
 *
 * leftovers:
 * - defer_codegen is off for the whole file, so definitions run low
 *   address first. func_02044284 needs opt_common_subs off and the other
 *   bodies need it on; under deferred codegen one setting covers the file.
 * - data_020a4b68 is the currently executing process node. It stays
 *   void *: ActorBase.cpp and ShowCrashScreen.c spell it that way, and
 *   the fLiNdBa_c destructor's old fLiNdBa_c * spelling was a second claim
 *   on the same word.
 * - The five list heads are defined here. ActorBase.cpp still spells them
 *   int []; that spelling is banked. decl_common.h spells data_020a4b6c as
 *   int, which is the plurality.
 * - The address-named helpers keep their linker names. Their roles are
 *   read from the bytes; the original names are not known.
 */
#include "fListNode.h"
#include "fBase_c.h"

#pragma defer_codegen off

extern "C" {
extern void *data_020a4b68;
extern int data_02099f24;
extern int data_02082178[];
extern void *func_02043fdc(void *list);
}

/* If this node is the one currently executing, clear that pointer. */
// @symbol _ZN11fLiNdBaPr_cD1Ev
fLiNdBaPr_c::~fLiNdBaPr_c() {
    if (data_020a4b68 == this)
        data_020a4b68 = 0;
}

// @symbol _ZN9fLiNdBa_cD1Ev
fLiNdBa_c::~fLiNdBa_c() {
    if (data_020a4b68 == this)
        data_020a4b68 = 0;
}

/* func_020441cc's own view of a tree node and the descriptor that names the
 * callback. The namespace keeps this Obj apart from the pending-sequence Obj
 * further down. */
namespace ProcessTree {
class Obj {
public:
    int dummy;
};
typedef void (Obj::*PMF)();

struct Node {
    char pad[0x10];
    Obj *obj; // 0x10
};

struct Thing {
    Node *head;   // 0
    PMF callback; // 4

    Thing(PMF &p) {
        callback = p;
        head = 0;
    }
    void *func_020441cc();
};
}

/* The five process-list heads func_02044120 walks. The tree sentinel is three
 * words (head, then the callback). Each list sentinel is four (head, tail,
 * then the callback). The callbacks are the ROM's pointer-to-member constants;
 * copying them is not a constant expression, so mwcc emits __sinit_fListNode.cpp
 * (was the handwritten __sinit_02075154) in this definition order. No destructor
 * is registered. ActorBase.cpp still reaches the heads by their address names. */
struct fBase_c;
typedef int (fBase_c::*ProcessCallback)();

extern ProcessTree::PMF data_02099f60;
extern ProcessCallback data_02099f70, data_02099f48, data_02099f68, data_02099f50;

struct ProcessList {
    fLiNdBaPr_c *head;
    fLiNdBaPr_c *tail;
    ProcessCallback callback;

    ProcessList(ProcessCallback &p) {
        head = 0;
        tail = 0;
        callback = p;
    }
};

ProcessTree::Thing data_020a4b6c(data_02099f60);
ProcessList data_020a4b88(data_02099f70);
ProcessList data_020a4b78(data_02099f48);
ProcessList data_020a4b98(data_02099f68);
ProcessList data_020a4ba8(data_02099f50);

/* One frame: walk each process list, recording the phase number in
 * data_02099f24 while it runs. */
// @symbol func_02044120
extern "C" void func_02044120(void) {
    data_02099f24 = 4;
    func_02043fdc(&data_020a4ba8);
    data_02099f24 = 2;
    func_02043fdc(&data_020a4b88);
    data_02099f24 = 3;
    func_02043fdc(&data_020a4b78);
    data_02099f24 = 5;
    func_02043fdc(&data_020a4b98);
    data_02099f24 = 1;
    data_020a4b6c.func_020441cc();
    data_02099f24 = 0;
}

/* Table entry for frame-loop phase i (data_02099f24). The crash screen
 * prints it. */
// @symbol func_020441bc
extern "C" int func_020441bc(int i) {
    return data_02082178[i];
}

namespace ProcessTree {
/* local extern: ActorBase_SceneNode.cpp defines it; no header declares it.
   Node is this file's view of the same fBase_c::SceneNode. */
extern "C" fBase_c::SceneNode *func_0203b394(fBase_c::SceneNode *node);

/* Walk a tree list, calling the descriptor's callback on each owner.
 * Unlike func_02043fdc, it does not set data_020a4b68. */
// @symbol _ZN11ProcessTree5Thing13func_020441ccEv
void *Thing::func_020441cc() {
    Node *node;
    if (this->callback == 0)
        return (void *)1;
    node = this->head;
    while (node) {
        Node *next = (Node *)func_0203b394((fBase_c::SceneNode *)node);
        (node->obj->*this->callback)();
        node = next;
    }
    return (void *)1;
}
}

/* The pending lifecycle sequence fBase_c holds at +0x48. Slot 2 runs one
 * table entry and returns the new state; slot 3 is called on each entry
 * already run when the sequence is drained. The layout is ROM-proven; the
 * class name is not. fBase_c declares the member void *, nothing in the tree
 * stores to it, and no ROM vtable or RTTI is tied to it, so v00..v08 and m3
 * stay placeholders until the class is found. */
struct Obj {
    virtual void v00();
    virtual void v04();
    virtual int v08(void *p);
    virtual void m3(void *p);

    s16 state;  // 0x4
    s16 idx;    // 0x6
    void **arr; // 0x8
    int f0c;    // 0xc
    s16 f10;    // 0x10
    s16 f12;    // 0x12
};

extern "C" int func_02044284(Obj *self, int flag);

/* Step a pending sequence once. Returns 0 while it is still pending. */
// @symbol func_0204424c
extern "C" int func_0204424c(char *c) {
    Obj *self = (Obj *)c;
    if (self->state == 1)
        self->state = func_02044284(self, 0);
    if (self->state == 1)
        return 0;
    return 1;
}

/* Run table entries from idx while each returns state 2. With flag == 0,
 * run at most one. */
#pragma push
#pragma opt_common_subs off
// @symbol func_02044284
extern "C" int func_02044284(Obj *self, int flag) {
    void **arr = self->arr;
    s16 i;

    if (arr == 0 || self->state == 2)
        return 2;

    i = self->idx;
    while (arr[i] != 0) {
        self->state = self->v08(arr[i]);
        if (self->state != 2)
            break;
        self->idx++;
        self->f12 = 0;
        if (flag == 0)
            break;
        i = self->idx;
        arr = self->arr;
    }
    return self->state;
}
#pragma pop

/* Drain a started sequence: call slot 3 on each entry already run, newest
 * first. */
// @symbol func_02044334
extern "C" void func_02044334(Obj *self) {
    if (self->arr == 0)
        return;
    if (self->state == 0)
        return;
    self->idx--;
    if (self->idx < 0)
        return;
    do {
        self->m3(self->arr[self->idx]);
        self->idx--;
    } while (self->idx >= 0);
}
