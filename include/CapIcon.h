#ifndef CAPICON_H
#define CAPICON_H

#include "types.h"

struct dActor_c;

/* The record an actor carries while it is holding one of the caps, and a
 * node in a per-character intrusive doubly-linked list. 0x1c bytes.
 *
 * RTTI names dCapIcon_c. Prev is +0x0c and next is +0x10: the link
 * helper writes the old list head into +0x10 and back-patches that
 * head's +0x0c. Unlink agrees.
 *
 * deslop leftovers:
 * - unk_19 / unk_1a: this class's TU stores the 4th link argument into
 *   unk_19 and special-cases it against 3; Unlink zeros unk_19. Original
 *   names are not in the cartridge.
 * - Bit 0 of mFlags is the cap bank (AddCap). Bit 1 is tested by
 *   GetCapState. Bit 2 means "not linked".
 */
#ifdef __cplusplus

extern "C" void _ZN6Memory16operator_delete2EPv(void *ptr);

/* Lifecycle declarations have no inline bodies: owners must emit calls
 * to the ROM-owned constructor and destructor rather than duplicating
 * their vtable stores and unlink logic. */
struct dCapIcon_c {
    /* The compiler-owned vptr occupies 0x00. */
    struct dActor_c *mOwner;      /* 0x04 */
    u32 mOwnerUniqueID;           /* 0x08 */
    struct dCapIcon_c *mPrev;     /* 0x0c */
    struct dCapIcon_c *mNext;     /* 0x10 */
    /* Index into the global slot table data_0209f3e8, or -1 for "not held". */
    s32 mSlot;                    /* 0x14 */
    /* Which character's list this node is in. Unlink parks it at 3. */
    u8 mCharacter;                /* 0x18 */
    u8 unk_19;                    /* 0x19 */
    u8 unk_1a;                    /* 0x1a */
    u8 mFlags;                    /* 0x1b */

    dCapIcon_c();
    virtual ~dCapIcon_c();
    void Unlink();
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
};

typedef dCapIcon_c CapIcon;
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dCapIcon_c_size_must_be_0x1c[sizeof(dCapIcon_c) == 0x1c ? 1 : -1];
#endif

#else

struct CapIcon {
    void **vtable;                /* 0x00 */
    struct dActor_c *mOwner;      /* 0x04 */
    u32 mOwnerUniqueID;           /* 0x08 */
    struct CapIcon *mPrev;        /* 0x0c */
    struct CapIcon *mNext;        /* 0x10 */
    s32 mSlot;                    /* 0x14 */
    u8 mCharacter;                /* 0x18 */
    u8 unk_19;                    /* 0x19 */
    u8 unk_1a;                    /* 0x1a */
    u8 mFlags;                    /* 0x1b */
};
typedef struct CapIcon CapIcon;

#endif

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char CapIcon_size_must_be_0x1c[sizeof(CapIcon) == 0x1c ? 1 : -1];
#endif

#endif /* CAPICON_H */
