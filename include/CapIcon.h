#ifndef CAPICON_H
#define CAPICON_H

#include "types.h"

struct dActor_c;

/* The record an actor carries while it is holding one of the caps, and a node in
 * a per-character intrusive doubly-linked list. 0x1c bytes.
 *
 * The tree originally had no name or header for it: every user spelt it as a
 * raw offset into itself. ROM RTTI at ov001 0x020ad450 now proves the original
 * name dCapIcon_c, and the class is reconstructed here from the four functions
 * that are entirely about it --
 *
 *   dCapIcon_c::dCapIcon_c construct: store the vtable, null both links, set bit 2
 *   dCapIcon_c::~dCapIcon_c destruct: store the vtable, then unlink
 *   func_ov001_020ab228   link into the list for a character
 *   func_ov001_020ab110   unlink, and reset every field
 *
 * -- and the last two are what pin the layout, because between them they read
 * and write every word.
 *
 * SIZE. Three independent facts agree on 0x1c, and none of them is the others:
 * dCapEnemy_c holds one at 0x164 and is 0x180; daKrb_c derives from dCapEnemy_c and its
 * first own member is at 0x180, and a derived member cannot begin inside its
 * base; and dCapEnemy_c::GetCapState reads a field at 0x17f, which is 0x164 + 0x1b,
 * the last byte of this struct.
 *
 * PREV AND NEXT ARE THIS WAY ROUND, and the struct this replaces had them
 * swapped. func_ov001_020ab228 links a node by writing the old list head into
 * +0x10 and then back-patching that head's +0x0c to point at the new node, which
 * is only consistent with +0x0c = prev and +0x10 = next. func_ov001_020ab110
 * agrees when it unlinks: it writes this->next into prev->[0x10] and this->prev
 * into next->[0x0c].
 */
#ifdef __cplusplus

extern "C" void _ZN6Memory16operator_delete2EPv(void *ptr);

/* RTTI at ov001 0x020ad450 supplies the original class name. Its two-slot
 * vtable at 0x020ad470 identifies the complete/deleting destructor pair at
 * 0x020ab3a0/0x020ab374, while the adjacent 0x020ab3c4 routine is the
 * constructor used by every owning actor. These declarations deliberately
 * have no inline bodies: owners must emit calls to those ROM-owned lifecycle
 * functions rather than duplicating their vtable stores and unlink logic. */
struct dCapIcon_c {
    /* The compiler-owned vptr occupies 0x00. */
    /* The actor wearing the cap, and a copy of that actor's uniqueID taken at
       link time. func_ov001_020ab110 compares the owner against the live slot
       table before releasing it, which is what the copy is for: the slot can
       outlive the actor, and the id says whether it did. */
    struct dActor_c *mOwner;         /* 0x04 */
    u32 mOwnerUniqueID;           /* 0x08 */
    struct dCapIcon_c *mPrev;     /* 0x0c */
    struct dCapIcon_c *mNext;     /* 0x10 */
    /* Index into the global slot table data_0209f3e8, or -1 for "not held".
       Both the constructor and the unlink reset it, the unlink to -1. */
    s32 mSlot;                    /* 0x14 */
    /* Which character's list this node is in -- the index into the list-head
       array data_ov001_020ad634 and the per-character count data_ov001_020ad630.
       func_ov001_020ab110 parks it at 3 on unlink, so 3 is the "none" value for
       a three-entry table. */
    u8 mCharacter;                /* 0x18 */
    u8 unk_19;                    /* 0x19 */
    u8 unk_1a;                    /* 0x1a */
    /* Bit 0 is the cap bank, passed in by dCapEnemy_c::AddCap. Bit 1 is tested by
       dCapEnemy_c::GetCapState. Bit 2 means "not linked": the constructor sets it,
       the unlink sets it again on the way out, and func_ov001_020ab110 returns
       immediately if it is already set. */
    u8 mFlags;                    /* 0x1b */

    dCapIcon_c();
    virtual ~dCapIcon_c();
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
};

/* Compatibility spelling retained while callers move to the ROM name. */
typedef dCapIcon_c CapIcon;
typedef char dCapIcon_c_size_must_be_0x1c[sizeof(dCapIcon_c) == 0x1c ? 1 : -1];

#else

/* Flat C view of the same polymorphic object. */
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

typedef char CapIcon_size_must_be_0x1c[sizeof(CapIcon) == 0x1c ? 1 : -1];

#endif /* CAPICON_H */
