#ifndef CYLINDERCLSN_H
#define CYLINDERCLSN_H

#include "types.h"

/* The cylinder-collision root, vtable _ZTV12CylinderClsn at 0x0208e6ec
 * (also reachable as data_0208e6ec, kept as an alias so existing consumers
 * and their relocations stay untouched -- the #972 pattern).
 *
 * VTABLE, 4 slots, read out of the ROM:
 *
 *   slot 0  0x020150a8  ~CylinderClsn (D1)
 *   slot 1  0x0201507c  ~CylinderClsn (D0)
 *   slot 2  0x00000000  null - pure; GetPos() in every derived table
 *   slot 3  0x00000000  null - pure; GetOwnerID() in every derived table
 *
 * A ROOT, not a derived class. Its typeinfo at 0x0208e660 is the 8-byte
 * __class_type_info kind, which carries no base pointer; the ROM's own name
 * for it is dCc_c. The three children are __si_class_type_info records
 * pointing back at it: dCcPos_c (CylinderClsnWithPos), dCcAc_c
 * (MovingCylinderClsn) and, one level further down, dCcAcPos_c
 * (MovingCylinderClsnWithPos, based on dCcAc_c).
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h. D0/D1/D2 stay
 * self-contained translation units.
 *
 * func_02014fa4 IS NOT A BASE DESTRUCTOR, whatever the old comments in the
 * D1/D2 files said. There is no base. It is the intrusive-list unlink that
 * D0/D1/D2 all tail into, and its body says so:
 *
 *   ldr r2,[r0,#0x28] / strne r1,[r2,#0x2c]   prev->next = next
 *   ldr r2,[r0,#0x2c] / strne r1,[r2,#0x28]   next->prev = prev
 *
 * with the list head at data_0209cee8 patched when this node is the head,
 * then both links zeroed.
 *
 * Update() is the matching link-in: it stores the old head into next, points
 * the old head's prev back at this, and makes this the new head. C2 zeroes
 * both links, which is what an unlinked node looks like.
 *
 * LAYOUT is 0x30 bytes. Offsets come from Init (0x04/0x08/0x18/0x1c), Clear
 * (0x0c/0x10/0x14/0x20/0x24), Update and the unlink above (0x28/0x2c), and
 * the derived classes start their own fields at exactly 0x30.
 *
 * Field NAMES below are the ones Process() had already recovered and carried
 * as a local struct; hoisting them here is what removes that duplicate. The
 * names are a reading, not pinned by the ROM the way the offsets are.
 */

#ifdef __cplusplus

struct CylinderClsn {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    Fix12i radius;          /* 0x04 - Init arg 1 */
    Fix12i height;          /* 0x08 - Init arg 2 */
    Vector3 pushback;       /* 0x0c - cleared by Clear */
    u32 flags;              /* 0x18 - Init arg 3; bit 0 makes Update bail */
    u32 vulnFlags;          /* 0x1c - Init arg 4 */
    u32 hitFlags;           /* 0x20 - cleared by Clear */
    u32 otherOwner;         /* 0x24 - cleared by Clear */
    CylinderClsn *prev;     /* 0x28 - intrusive list, zeroed by C2 */
    CylinderClsn *next;     /* 0x2c - intrusive list, zeroed by C2 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~CylinderClsn();            /* slots 0 (D1), 1 (D0) */
    virtual Vector3 &GetPos() = 0;      /* slot 2 - null in the ROM table */
    virtual u32 GetOwnerID() = 0;       /* slot 3 - null in the ROM table */

    /* --- non-virtual --- */
    void Init(Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
    void Clear();
    void Update();
    static void Process();
};

typedef char CylinderClsn_size_must_be_0x30[sizeof(CylinderClsn) == 0x30 ? 1 : -1];

#else

struct CylinderClsn {
    void **vtable;             /* 0x00 */
    s32 radius;                /* 0x04 */
    s32 height;                /* 0x08 */
    s32 pushbackX;             /* 0x0c */
    s32 pushbackY;             /* 0x10 */
    s32 pushbackZ;             /* 0x14 */
    u32 flags;                 /* 0x18 */
    u32 vulnFlags;             /* 0x1c */
    u32 hitFlags;              /* 0x20 */
    u32 otherOwner;            /* 0x24 */
    struct CylinderClsn *prev; /* 0x28 */
    struct CylinderClsn *next; /* 0x2c */
};

#endif /* __cplusplus */

#endif /* CYLINDERCLSN_H */
