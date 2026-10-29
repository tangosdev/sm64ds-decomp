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
 * - Bit 0 of mFlags is the cap bank (AddCap). Bit 1 is tested by
 *   GetCapState. Bit 2 means "not linked".
 */
#ifdef __cplusplus

namespace Memory {
void operator_delete2(void *);
}

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
    /* The link helper's 4th argument (daObjMarioCap_c passes its mIconKind);
       Unlink zeros it. Kind 3 raises request flag 8 on link, kind 4 is the
       one the reset pass commits, and the dispatchers keep the highest kind
       of a list as its priority. */
    u8 mIconKind; /* 0x19 */
    /* Zeroed on link. The commit (func_ov001_020aa6e4) stores 0x3c here and
       copies it into gCapIconFlashTimer, the HUD flash countdown. */
    u8 mFlashFrames;              /* 0x1a */
    u8 mFlags;                    /* 0x1b */

    dCapIcon_c();
    virtual ~dCapIcon_c();
    void Unlink();
    void func_ov001_020ab228(char *a1, int idx, int a3, unsigned char a5);
    void operator delete(void *ptr) {
        Memory::operator_delete2(ptr);
    }
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
    u8 mIconKind;                 /* 0x19 */
    u8 mFlashFrames;              /* 0x1a */
    u8 mFlags;                    /* 0x1b */
};
typedef struct CapIcon CapIcon;

#endif

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char CapIcon_size_must_be_0x1c[sizeof(CapIcon) == 0x1c ? 1 : -1];
#endif

#endif /* CAPICON_H */
