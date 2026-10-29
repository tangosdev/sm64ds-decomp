#ifndef DMGTRMPOBJADAPTER_C_H
#define DMGTRMPOBJADAPTER_C_H

#include "types.h"

/* dMgTrmpObjAdapter_c, the jumper interface a drawn TrmpLine bounces (ROM RTTI
 * _ZTI19dMgTrmpObjAdapter_c 0x0213b19c, vtable 0x0213b3c4), and
 * dMgTrmpRingObjAdapter_c, the variant that also counts the item rings it hits
 * (_ZTI23dMgTrmpRingObjAdapter_c 0x0213b238, vtable 0x0213b3e0).
 * dMgTrmpln3DMario_c (Trampoline) is built on the first, dMgTrmpln2Mario_c
 * (Trampoline2) on the second.
 *
 * Both vtables are five slots: four zero words, then Clear (0x020cd720 /
 * 0x020cd6d8). The four zeros are the pure slots the Marios fill (their vtables
 * 0x0213b2c4 and 0x0213b2e0). Neither has a destructor slot, so the destructors are not
 * virtual; they are inline and empty, and each derived destructor ends by
 * storing the ring and then the base vptr, which is what they leave behind.
 * The constructors (C2 0x020cd72c and 0x020cd6f4) and both Clear methods are
 * out of line in src/minigames/d_mg_trmpln2_mario.cpp. Clear is each class's
 * key function, so that file emits the two vtables and typeinfos; its manifest
 * discards them against the cartridge's copies. */
struct dMgTrmpObjAdapter_c {
    dMgTrmpObjAdapter_c();
    ~dMgTrmpObjAdapter_c() {}

    virtual Vector3 *GetPos() = 0;      /* slot 0 */
    virtual Vector3 *GetPrevPos() = 0;  /* slot 1 */
    virtual Vector3 *GetVelocity() = 0; /* slot 2 */
    virtual int IsActive() = 0;         /* slot 3 */
    virtual void Clear();               /* slot 4 -- drops the contact */

    Vector3 mContactNormal; /* 0x04 -- the drawn line's normal */
    s32 mContactOffset;     /* 0x10 -- signed distance from the line's centre */
    s32 mContactHalfWidth;  /* 0x14 -- the line's half width */
    u16 mContact;           /* 0x18 -- 0 none, 1 hit, 2 hard hit, 3 the line's end */
    u8 pad_1a[0x2];         /* 0x1a */
};

struct dMgTrmpRingObjAdapter_c : dMgTrmpObjAdapter_c {
    dMgTrmpRingObjAdapter_c();
    ~dMgTrmpRingObjAdapter_c() {}

    virtual void Clear(); /* slot 4 -- also restarts mItemHitCount */

    u8 pad_1c[0x4];     /* 0x1c */
    u16 mItemHitCount;  /* 0x20 -- items hit; doubles the score (TrmpItem_UpdateWithJumper) */
    s16 mItemPushCount; /* 0x22 -- item-end pushes this frame (TrmpItem_ReflectOffLine) */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dMgTrmpObjAdapter_c_size_must_be_0x1c[sizeof(dMgTrmpObjAdapter_c) == 0x1c ? 1 : -1];
typedef char
    dMgTrmpRingObjAdapter_c_size_must_be_0x24[sizeof(dMgTrmpRingObjAdapter_c) == 0x24 ? 1 : -1];
#endif

#endif
