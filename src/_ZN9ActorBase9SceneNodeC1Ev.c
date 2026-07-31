// @symbol _ZN9ActorBase9SceneNodeC1Ev
/* recovered: named members + shared header */
#include "ActorBase.h"
// ActorBase::SceneNode::SceneNode() - C1 constructor
// Address: 0x0203b4c4

typedef unsigned int u32;

struct SceneNode {
    struct SceneNode* parent;      // 0x00
    struct SceneNode* firstChild;  // 0x04
    struct SceneNode* prevSibling; // 0x08
    struct SceneNode* nextSibling; // 0x0c
    void*             actor;       // 0x10
};

extern void _ZN9ActorBase9SceneNode5ResetEv(struct SceneNode* self);

struct SceneNode* _ZN9ActorBase9SceneNodeC1Ev(struct SceneNode* self) {
    _ZN9ActorBase9SceneNode5ResetEv(self);
    return self;
}