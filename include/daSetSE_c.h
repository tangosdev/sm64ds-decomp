#ifndef DASETSE_C_H
#define DASETSE_C_H

/* The level's ambient sound emitter (SET_SE), ov002.
 *
 * RTTI names the class daSetSE_c; the SET_SE registry profile and the
 * daSetSE_c_classInit factory spelling are reconstructed.
 */

#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daSetSE_c : dActor_c {
    u8  pad_0d0[0x4];
    u32 mSoundHandle;       /* 0x0d4: Sound::PlayLong recycled handle, never
                               initialized -- PlayLong treats an unknown
                               handle as not playing and starts a new voice */

    /* INLINE: out of line, mwccarm emits D0 before D1 (the reverse of the
       ROM's 0x020f198c/0x020f19b0 pair) plus a homeless D2, and objisolate
       refuses the TU. Safe because this class is a leaf: _ZTI9daSetSE_c
       occurs exactly once in the image, in its own vtable header. */
    virtual ~daSetSE_c() {}         /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();        /* slot 0 */
    virtual s32 CleanupResources();     /* slot 3 */
    virtual s32 Behavior();             /* slot 6 */
    virtual s32 Render();               /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    /* Leaf adapter until fBase_c::operator new(unsigned long) lands (#2570).
       `return new daSetSE_c()` then routes through the retail allocator. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daSetSE_c_size_must_be_0xd8[
    sizeof(daSetSE_c) == 0xd8 ? 1 : -1];
#endif

#endif /* DASETSE_C_H */
