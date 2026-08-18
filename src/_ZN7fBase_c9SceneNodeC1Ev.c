#include "types.h"
// @symbol _ZN7fBase_c9SceneNodeC1Ev
/* recovered: named members + shared header */
#include "fBase_c.h"
// fBase_c::SceneNode::SceneNode() - C1 constructor
// Address: 0x0203b4c4
struct SceneNode {
    struct SceneNode* parent;      // 0x00
    struct SceneNode* firstChild;  // 0x04
    struct SceneNode* prevSibling; // 0x08
    struct SceneNode* nextSibling; // 0x0c
    void*             actor;       // 0x10
};

extern void _ZN7fBase_c9SceneNode5ResetEv(struct SceneNode* self);

struct SceneNode* _ZN7fBase_c9SceneNodeC1Ev(struct SceneNode* self) {
    _ZN7fBase_c9SceneNode5ResetEv(self);
    return self;
}