#include "types.h"
// MeshCollider::MeshCollider() - C1 constructor returning this
// Address: 0x020398c8
struct MeshColliderBase {
    void* vtable;             // 0x00
    void* actor;              // 0x04
    u32   actorUniqueID;      // 0x08
    Fix12i range;             // 0x0c
    Fix12i rangeOffsetY;      // 0x10
    u32   clsnID;             // 0x14
    void* beforeClsnCallback; // 0x18
    void* afterClsnCallback;  // 0x1c
};

struct MeshCollider {
    struct MeshColliderBase base; // 0x00..0x1f (size 0x20)
    void*  clsnFile;    // 0x20
    void*  clpsBlock;   // 0x24
    u32    unk28;       // 0x28
};

extern void _ZN16MeshColliderBaseC2Ev(struct MeshColliderBase* self);
extern void func_02038228(void** ptr);
extern void* _ZTV12MeshCollider;

// MeshCollider C1 constructor - returns this
struct MeshCollider* func_020398c8(struct MeshCollider* self) {
    _ZN16MeshColliderBaseC2Ev(&self->base);
    self->base.vtable = &_ZTV12MeshCollider;
    func_02038228(&self->clpsBlock);
    self->clsnFile = 0;
    return self;
}