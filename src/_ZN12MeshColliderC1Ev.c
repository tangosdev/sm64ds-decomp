#include "types.h"
/* _ZN12MeshColliderC1Ev - MeshCollider constructor.
 * Attempt 3: constructor returns self (C++ C1 ctors return this).
 * ROM has mov r0,r4 before final str+epilogue - this is "return self" pattern.
 * Adding return self fixes: pool offset (0x18->0x1c) AND mov r0,r4 at +0x20.
 */
struct MeshColliderBase { u32 _pad[8]; }; /* sizeof=0x20 */

extern void _ZN16MeshColliderBaseC2Ev(struct MeshColliderBase* self);

extern void func_02038228(u32* obj);

extern void* MeshCollider_vtable;

struct MeshCollider {
    void* vtable;                 /* 0x00 */
    u32 pad04[7];                 /* 0x04 - 0x1f */
    u32 fld20;                    /* 0x20 */
    u32 fld24;                    /* 0x24 */
    u32 pad28[10];               /* 0x28 - 0x4f */
};

struct MeshCollider* _ZN12MeshColliderC1Ev(struct MeshCollider* self)
{
    _ZN16MeshColliderBaseC2Ev((struct MeshColliderBase*)self);
    self->vtable = &MeshCollider_vtable;
    func_02038228(&self->fld24);
    self->fld20 = 0;
    return self;
}