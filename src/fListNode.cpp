//cpp
/* List-node destructors, the frame loop over the process lists
 * (fManager_c::mainLoop, include/fManager_c.h), and the pending
 * lifecycle-sequence helpers fBase_c calls during cleanup.
 * arm9 .text 0x020440e8..0x020443c8.
 *
 * leftovers:
 * - defer_codegen is off for the whole file, so definitions run low
 *   address first.
 * - data_020a4b68 is the currently executing process node. It stays
 *   void *: ActorBase.cpp and ShowCrashScreen.c spell it that way, and
 *   the fLiNdBa_c destructor's old fLiNdBa_c * spelling was a second claim
 *   on the same word. NSMBW's fManager_c has no such member, so it keeps
 *   its address name.
 * - The address-named helpers keep their linker names. Their roles are
 *   read from the bytes; the original names are not known.
 */
#include "fListNode.h"
#include "fBase_c.h"

#pragma defer_codegen off

extern void *data_020a4b68;
extern const int data_02082178[6];
namespace Absorb02043fdc {
struct ProcessListDescriptor {
    void *func_02043fdc();
};
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

/* fManager_c's five managers, the heads mainLoop walks. The tree sentinel is three
 * words (head, then the callback). Each list sentinel is four (head, tail,
 * then the callback). The callbacks are the ROM's pointer-to-member constants;
 * copying them is not a constant expression, so mwcc emits __sinit_fListNode.cpp
 * (was the handwritten __sinit_02075154) in this definition order. No destructor
 * is registered. */
extern fBase_c::ProcessFunction data_02099f60;
extern fBase_c::ProcessFunction data_02099f70, data_02099f48, data_02099f68, data_02099f50;

#include "fManager_c.h"

ProcessTree::Thing fManager_c::m_connectManage(data_02099f60);
ProcessList fManager_c::m_createManage(data_02099f70);
ProcessList fManager_c::m_executeManage(data_02099f48);
ProcessList fManager_c::m_drawManage(data_02099f68);
ProcessList fManager_c::m_deleteManage(data_02099f50);

/* One frame: walk each process list, recording the phase in m_nowLoopProc
 * while it runs. */
void fManager_c::mainLoop() {
    m_nowLoopProc = DELETE;
    ((Absorb02043fdc::ProcessListDescriptor *)&m_deleteManage)->func_02043fdc();
    m_nowLoopProc = CREATE;
    ((Absorb02043fdc::ProcessListDescriptor *)&m_createManage)->func_02043fdc();
    m_nowLoopProc = EXECUTE;
    ((Absorb02043fdc::ProcessListDescriptor *)&m_executeManage)->func_02043fdc();
    m_nowLoopProc = DRAW;
    ((Absorb02043fdc::ProcessListDescriptor *)&m_drawManage)->func_02043fdc();
    m_nowLoopProc = CONNECT;
    m_connectManage.func_020441cc();
    m_nowLoopProc = NONE;
}

/* Table entry for frame-loop phase i (fManager_c::m_nowLoopProc). The crash screen
 * prints it. */
int ProcessList::func_020441bc(int i) {
    return data_02082178[i];
}

/* ActorBase_SceneNode.cpp defines the method. The return type is not in the mangled name. */
struct TreeNd {
    fBase_c::SceneNode *func_0203b394();
};

namespace ProcessTree {
/* Walk a tree list, calling the descriptor's callback on each owner.
 * Unlike func_02043fdc, it does not set data_020a4b68. */
int Thing::func_020441cc() {
    fBase_c::SceneNode *node;
    if (callback == 0)
        return 1;
    node = head;
    while (node) {
        fBase_c::SceneNode *next = ((TreeNd *)node)->func_0203b394();
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

    int func_02044284(int flag);
    int func_0204424c();
    void func_02044334();
};

/* Step a pending sequence once. Returns 0 while it is still pending. */
int LifecycleSequence::func_0204424c() {
    if (state == 1)
        state = func_02044284(0);
    if (state == 1)
        return 0;
    return 1;
}

/* Run table entries from idx while each returns state 2. With flag == 0,
 * run at most one. */
int LifecycleSequence::func_02044284(int flag) {
    void **arr = this->arr;
    s16 i;

    if (arr == 0 || this->state == 2)
        return 2;

    i = this->idx;
    while (this->arr[this->idx] != 0) {
        this->state = this->v08(arr[i]);
        if (this->state != 2)
            break;
        this->idx++;
        this->f12 = 0;
        if (flag == 0)
            break;
        i = this->idx;
        arr = this->arr;
    }
    return this->state;
}

/* Drain a started sequence: call slot 3 on each entry already run, newest
 * first. */
void LifecycleSequence::func_02044334() {
    if (arr == 0)
        return;
    if (state == 0)
        return;
    idx--;
    if (idx < 0)
        return;
    do {
        m3(arr[idx]);
        idx--;
    } while (idx >= 0);
}
