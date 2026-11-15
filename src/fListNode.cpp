//cpp
/* List-node destructors, the frame loop over the process lists, and the
 * pending lifecycle-sequence helpers fBase_c calls during cleanup.
 * arm9 .text 0x020440e8..0x020443c8.
 *
 * leftovers:
 * - defer_codegen is off for the whole file, so definitions run low
 *   address first.
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

extern void *data_020a4b68;
extern int data_02099f24;
extern const int data_02082178[6];
namespace Absorb02043fdc {
struct ProcessListDescriptor;
void *func_02043fdc(ProcessListDescriptor *list);
}

/* If this node is the one currently executing, clear that pointer. */
fLiNdBaPr_c::~fLiNdBaPr_c() {
    if (data_020a4b68 == this)
        data_020a4b68 = 0;
}

fLiNdBa_c::~fLiNdBa_c() {
    if (data_020a4b68 == this)
        data_020a4b68 = 0;
}

/* The process tree's sentinel: the root scene node, then the callback
 * func_020441cc runs on each node's owner. */
namespace ProcessTree {
struct Thing {
    fBase_c::SceneNode *head;          // 0
    fBase_c::ProcessFunction callback; // 4

    Thing(fBase_c::ProcessFunction &p) {
        callback = p;
        head = 0;
    }
    int func_020441cc();
};
}

/* The five process-list heads func_02044120 walks. The tree sentinel is three
 * words (head, then the callback). Each list sentinel is four (head, tail,
 * then the callback). The callbacks are the ROM's pointer-to-member constants;
 * copying them is not a constant expression, so mwcc emits __sinit_fListNode.cpp
 * (was the handwritten __sinit_02075154) in this definition order. No destructor
 * is registered. ActorBase.cpp still reaches the heads by their address names. */
extern fBase_c::ProcessFunction data_02099f60;
extern fBase_c::ProcessFunction data_02099f70, data_02099f48, data_02099f68, data_02099f50;

struct ProcessList {
    fLiNdBaPr_c *head;
    fLiNdBaPr_c *tail;
    fBase_c::ProcessFunction callback;

    ProcessList(fBase_c::ProcessFunction &p) {
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
void func_02044120(void) {
    data_02099f24 = 4;
    Absorb02043fdc::func_02043fdc((Absorb02043fdc::ProcessListDescriptor *)&data_020a4ba8);
    data_02099f24 = 2;
    Absorb02043fdc::func_02043fdc((Absorb02043fdc::ProcessListDescriptor *)&data_020a4b88);
    data_02099f24 = 3;
    Absorb02043fdc::func_02043fdc((Absorb02043fdc::ProcessListDescriptor *)&data_020a4b78);
    data_02099f24 = 5;
    Absorb02043fdc::func_02043fdc((Absorb02043fdc::ProcessListDescriptor *)&data_020a4b98);
    data_02099f24 = 1;
    data_020a4b6c.func_020441cc();
    data_02099f24 = 0;
}

/* Table entry for frame-loop phase i (data_02099f24). The crash screen
 * prints it. */
int func_020441bc(int i) {
    return data_02082178[i];
}

/* ActorBase_SceneNode.cpp defines it at file scope. */
fBase_c::SceneNode *func_0203b394(fBase_c::SceneNode *node);

namespace ProcessTree {
/* Walk a tree list, calling the descriptor's callback on each owner.
 * Unlike func_02043fdc, it does not set data_020a4b68. */
int Thing::func_020441cc() {
    fBase_c::SceneNode *node;
    if (callback == 0)
        return 1;
    node = head;
    while (node) {
        fBase_c::SceneNode *next = func_0203b394(node);
        (node->owner->*callback)();
        node = next;
    }
    return 1;
}
}

/* The pending lifecycle sequence fBase_c holds at +0x48. Slot 2 runs one
 * table entry and returns the new state; slot 3 is called on each entry
 * already run when the sequence is drained. The layout is ROM-proven; the
 * class name is not. LifecycleSequence is coined from its role: fBase_c's
 * lifecycleState member holds it and BeforeCleanupResources/
 * AfterCleanupResources step and drain it. fBase_c declares the member
 * void *, nothing in the tree stores to it, and no ROM vtable or RTTI is tied
 * to it, so v00..v08 and m3 stay placeholders until the class is found. */
struct LifecycleSequence {
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

int func_02044284(LifecycleSequence *self, int flag);

/* Step a pending sequence once. Returns 0 while it is still pending. */
extern "C" int func_0204424c(char *c) {
    LifecycleSequence *self = (LifecycleSequence *)c;
    if (self->state == 1)
        self->state = func_02044284(self, 0);
    if (self->state == 1)
        return 0;
    return 1;
}

/* Run table entries from idx while each returns state 2. With flag == 0,
 * run at most one. */
int func_02044284(LifecycleSequence *self, int flag) {
    void **arr = self->arr;
    s16 i;

    if (arr == 0 || self->state == 2)
        return 2;

    i = self->idx;
    while (self->arr[self->idx] != 0) {
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

/* Drain a started sequence: call slot 3 on each entry already run, newest
 * first. */
void func_02044334(LifecycleSequence *self) {
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
