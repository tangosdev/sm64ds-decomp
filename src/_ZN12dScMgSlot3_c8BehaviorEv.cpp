//cpp
// @symbol _ZN12dScMgSlot3_c8BehaviorEv
#include "decl_common.h"
#include "dScMgSlot3_c.h"
/* dScMgSlot3_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the two slots where this class's table
 * differs from dScMgSingle3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * Steps the reel state machine through the pointer-to-member table at
 * data_ov006_02142bdc, ticks the frame counter at 0x503f, spins the two reel
 * offsets while state 1 is running, and flips the blend bit in data_0209d45c off a
 * coin toss so the machine's lights flicker.
 *
 * `Obj` stays a bare non-polymorphic stand-in rather than dScMgSlot3_c: a PMF on a
 * polymorphic class is a different representation, so the shape here is codegen and
 * not decoration -- the same reason src/_ZN15daObjMarioCap_c8BehaviorEv.cpp keeps
 * its own. Its `idx` at 0x5000 is the header's mState, h5018/h501a the header's
 * mLamp1Angle/mLamp2Angle, and c503f its mFrameCounter.
 *
 * The three globals stay outside `extern "C"`, exactly as the pre-migration file
 * had them: a variable at global namespace scope is not mangled, so the spelling
 * that already linked is kept rather than churned. */

struct Obj {
    char pad0[0x4f38];
    int field_4f38;
    char pad1[0x5000 - 0x4f3c];
    int idx;
    char pad2[0x5018 - 0x5004];
    unsigned short h5018;
    unsigned short h501a;
    char pad3[0x503f - 0x501c];
    unsigned char c503f;
};

typedef void (Obj::*ObjFn)();

extern ObjFn data_ov006_02142bdc[];
extern int data_0209e650;
extern unsigned char data_0209d45c;

extern "C" int RandomIntInternal(int *seed);

s32 dScMgSlot3_c::Behavior()
{
    Obj *self = (Obj *)this;
    int i;
    unsigned char t;

    unsigned char *pc;
    unsigned short *ph1;
    unsigned short *ph2;

    (self->*data_ov006_02142bdc[self->idx])();

    pc = (unsigned char *)((char *)self + 0x503f);
    *pc = *pc + 1;
    func_ov006_020c2144((char *)self + 0x4f38);

    for (i = 0; i < 3; i++) {
        if (self->idx == 1) {
            ph1 = (unsigned short *)((char *)self + 0x5018);
            ph2 = (unsigned short *)((char *)self + 0x501a);
            *ph1 = *ph1 - 0x200;
            *ph2 = *ph2 - 0x400;
            break;
        }
    }

    t = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 1;
    if (t)
        data_0209d45c |= 2;
    else
        data_0209d45c &= ~2;

    return 1;
}
