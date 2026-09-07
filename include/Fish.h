#ifndef FISH_H
#define FISH_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"

/* daFish_c is the ROM RTTI alias. D1/D0 call ModelAnim's complete destructor at
 * +0xd4 and dActor_c's base destructor, while daFish_c_classInit constructs the same
 * subobject after dActor_c. Together those relocations establish the inheritance
 * and ownership below; dActor_c supplies uniqueID, param1, position, and angles.
 *
 * SM64DS RTTI names the implementation daFish_c. The reconstructed
 * factory daFish_c_classInit (historical alias
 * Fish_Spawn) constructs it for the FISH
 * registry profile.
 */
struct Fish : dActor_c {
    u8 pad_0d0[0x4];       /* 0x0d0 */
    ModelAnim mModelAnim;  /* 0x0d4 */
    s32 unk_138;           /* 0x138 */
    s32 mUniqueID_13c;     /* 0x13c */
    s32 mTopY;             /* 0x140 */
    u8 pad_144[0x8];       /* 0x144 */
    s32 mState;            /* 0x14c */
    s32 unk_150;           /* 0x150 */
    u8 pad_154[0x5];       /* 0x154 */
    u8 mHidden;            /* 0x159 */
    u8 pad_15a;            /* 0x15a */
    u8 mModelIndex;        /* 0x15b */
    u8 mVariant;           /* 0x15c */
    u8 pad_15d[0x3];       /* 0x15d */

    virtual ~Fish();
    virtual void OnPendingDestroy(); /* slot 12 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Fish_size_must_be_0x160[sizeof(struct Fish) == 0x160 ? 1 : -1];

#endif
