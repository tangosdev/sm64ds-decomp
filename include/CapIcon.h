#ifndef CAPICON_H
#define CAPICON_H

#include "types.h"

struct Actor;

/* The record an actor carries while it is holding one of the caps, and a node in
 * a per-character intrusive doubly-linked list. 0x1c bytes.
 *
 * The type had no name and no header: every user spelt it as a raw offset into
 * itself. It is reconstructed here from the four functions that are entirely
 * about it --
 *
 *   func_ov001_020ab3c4   construct: store the vtable, null both links, set bit 2
 *   func_ov001_020ab3a0   destruct:  store the vtable, then unlink
 *   func_ov001_020ab228   link into the list for a character
 *   func_ov001_020ab110   unlink, and reset every field
 *
 * -- and the last two are what pin the layout, because between them they read
 * and write every word.
 *
 * SIZE. Three independent facts agree on 0x1c, and none of them is the others:
 * CapEnemy holds one at 0x164 and is 0x180; Goomba derives from CapEnemy and its
 * first own member is at 0x180, and a derived member cannot begin inside its
 * base; and CapEnemy::GetCapState reads a field at 0x17f, which is 0x164 + 0x1b,
 * the last byte of this struct.
 *
 * PREV AND NEXT ARE THIS WAY ROUND, and the struct this replaces had them
 * swapped. func_ov001_020ab228 links a node by writing the old list head into
 * +0x10 and then back-patching that head's +0x0c to point at the new node, which
 * is only consistent with +0x0c = prev and +0x10 = next. func_ov001_020ab110
 * agrees when it unlinks: it writes this->next into prev->[0x10] and this->prev
 * into next->[0x0c].
 */
struct CapIcon {
    void **vtable;                /* 0x00 */
    /* The actor wearing the cap, and a copy of that actor's uniqueID taken at
       link time. func_ov001_020ab110 compares the owner against the live slot
       table before releasing it, which is what the copy is for: the slot can
       outlive the actor, and the id says whether it did. */
    struct Actor *mOwner;         /* 0x04 */
    u32 mOwnerUniqueID;           /* 0x08 */
    struct CapIcon *mPrev;        /* 0x0c */
    struct CapIcon *mNext;        /* 0x10 */
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
    /* Bit 0 is the cap bank, passed in by CapEnemy::AddCap. Bit 1 is tested by
       CapEnemy::GetCapState. Bit 2 means "not linked": the constructor sets it,
       the unlink sets it again on the way out, and func_ov001_020ab110 returns
       immediately if it is already set. */
    u8 mFlags;                    /* 0x1b */
};

typedef char CapIcon_size_must_be_0x1c[sizeof(struct CapIcon) == 0x1c ? 1 : -1];

#endif /* CAPICON_H */
