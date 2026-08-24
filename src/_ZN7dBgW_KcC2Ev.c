/* _ZN7dBgW_KcC2Ev - dBgW_Kc's base-subobject constructor at 0x020398c8,
 * renamed from func_020398c8 (2026-08-24). Byte-for-byte the C1 body, and
 * every ROM caller is a derived base step (inside dBgW_KcMbg's ctor pair),
 * so the variant label is C2 -- the section 4 rule in notes/ctor-migration.md.
 */
#include "types.h"
struct dBgW {
    void* vtable;             // 0x00
    void* actor;              // 0x04
    u32   actorUniqueID;      // 0x08
    Fix12i range;             // 0x0c
    Fix12i rangeOffsetY;      // 0x10
    u32   clsnID;             // 0x14
    void* beforeClsnCallback; // 0x18
    void* afterClsnCallback;  // 0x1c
};

struct dBgW_Kc {
    struct dBgW base; // 0x00..0x1f (size 0x20)
    void*  clsnFile;    // 0x20
    void*  clpsBlock;   // 0x24
    u32    unk28;       // 0x28
};

extern void _ZN4dBgWC2Ev(struct dBgW* self);
extern void func_02038228(void** ptr);
extern void* _ZTV7dBgW_Kc;

// dBgW_Kc C1 constructor - returns this
struct dBgW_Kc* _ZN7dBgW_KcC2Ev(struct dBgW_Kc* self) {
    _ZN4dBgWC2Ev(&self->base);
    self->base.vtable = &_ZTV7dBgW_Kc;
    func_02038228(&self->clpsBlock);
    self->clsnFile = 0;
    return self;
}
