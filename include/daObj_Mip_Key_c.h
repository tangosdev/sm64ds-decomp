#ifndef DAOBJ_MIP_KEY_C_H
#define DAOBJ_MIP_KEY_C_H

#include "types.h"

/* ROM RTTI at ov085:0x02130194 identifies dEnemyBase_c as the base.
 * Constructor/destructor calls place Model at 0x110 and ShadowModel at 0x160;
 * the factory allocates 0x1a0 bytes. */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ShadowModel.h"

struct daObj_Mip_Key_c;
typedef int (daObj_Mip_Key_c::*MipKeyPMF)();
// Inferred state shape: sinit copies two 8-byte PMFs into each 16-byte object.
// Field names describe their observed use, not recovered source spellings.
struct MipKeyState {
    MipKeyPMF enter;
    MipKeyPMF execute;
};

struct daObj_Mip_Key_c : dEnemyBase_c {
    Model mModel;                         /* 0x110 */
    ShadowModel mShadowModel;             /* 0x160 */
    MipKeyState * mState;                 /* 0x188 */
    Player * mPlayer;                     /* 0x18c */
    s32 unk_190;                          /* 0x190 */
    s32 mTalkState;                       /* 0x194 */
    s32 mWaitForTalk;                     /* 0x198 */
    s32 unk_19c;                          /* 0x19c */

    /* --- vtable --- */
    virtual ~daObj_Mip_Key_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObj_Mip_Key_c_size_must_be_0x1a0[sizeof(daObj_Mip_Key_c) == 0x1a0 ? 1 : -1];
#endif

#endif /* DAOBJ_MIP_KEY_C_H */
