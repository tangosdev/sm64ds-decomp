#include "types.h"
// dBgW_Kc::dBgW_Kc() - C1 constructor returning this
// Address: 0x020398c8
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
struct dBgW_Kc* func_020398c8(struct dBgW_Kc* self) {
    _ZN4dBgWC2Ev(&self->base);
    self->base.vtable = &_ZTV7dBgW_Kc;
    func_02038228(&self->clpsBlock);
    self->clsnFile = 0;
    return self;
}