#ifndef DASOUNDOBJ_C_H
#define DASOUNDOBJ_C_H

/* The positional sound emitter (SOUND_OBJ), actor 0x167, ov002.
 *
 * RTTI names the class daSoundObj_c; the SOUND_OBJ registry profile and
 * the daSoundObj_c_classInit factory spelling are reconstructed.
 */

#include "types.h"
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* The fields are witnessed by InitResources, which loads all four from
 * one row of the 0xc-stride table at data_ov002_0210c080, and by the
 * two starter callbacks, which read them back: mSoundID/mVolume go
 * straight to Sound::PlaySub as its soundID/vol arguments, mVolume's
 * complement drives the distance as (0x7f - vol) << 12, and mLoop is
 * PlaySub's loop argument. mCounter runs from 0 to mCounterLimit.
 * pad_0e1 reaches the factory's 0xe4; it is padding to a known total,
 * not evidence of a field boundary. */
struct daSoundObj_c : dActor_c {
    u8  pad_0d0[0x4];
    s32 mSoundID;                /* 0x0d4 -- PlaySub arg 1 */
    s32 mVolume;                 /* 0x0d8 -- PlaySub arg 2 */
    u16 mCounter;                /* 0x0dc */
    u16 mCounterLimit;           /* 0x0de */
    u8  mLoop;                   /* 0x0e0 -- PlaySub arg 5 */
    u8  pad_0e1[0x3];

    /* INLINE: out of line, mwccarm emits D0 before D1 (the reverse of the
       ROM's 0x020f934c/0x020f9370 pair) plus a homeless D2, and objisolate
       refuses the TU. Safe because this class is a leaf: _ZTI12daSoundObj_c
       occurs exactly once in the image, in its own vtable header. */
    virtual ~daSoundObj_c() {}          /* slots 16 (D1), 17 (D0) */

    virtual int InitResources();        /* slot 0 */
    virtual int Behavior();             /* slot 6 */

    /* Leaf adapter until fBase_c::operator new(unsigned long) lands (#2570).
       `return new daSoundObj_c()` then routes through the retail allocator. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daSoundObj_c_size_must_be_0xe4[sizeof(daSoundObj_c) == 0xe4 ? 1 : -1];
#endif

#endif /* DASOUNDOBJ_C_H */
