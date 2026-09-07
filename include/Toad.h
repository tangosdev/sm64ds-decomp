/* Toad, reconstructed from the ov085 RTTI record, vtable, factory,
 * destructor pair, state table, and matched methods.
 *
 * The cartridge calls this class `daKinopio_c`: its __si_class_type_info at
 * 0x0212fe50 points directly at dActor_c, and its 31-slot vtable at
 * 0x0212feb8 overrides only the usual fBase_c lifecycle slots. The readable
 * `Toad` spelling is retained for compatibility with configured function
 * symbols; metadata emitted by isolated destructor objects is discard-only.
 *
 * SIZE 0x210 is the literal passed by Toad_Spawn to fBase_c::operator new.
 * Member subobjects are pinned independently by both constructor and
 * destructor calls: dCcAc_c at 0x0d4, ModelAnim at 0x108, and ShadowModel at
 * 0x16c. The two Matrix4x3 objects and state fields are addressed by the
 * class's own nonvirtual methods and pointer-to-member state table.
 *
 * SM64DS proves this class as daKinopio_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daKinopio_c_classInit at 0x02129cd0 (historical alias Toad_Spawn)
 * allocates 0x210 and installs this class's cartridge vtable. It backs the
 * KINOPIO registry profile, whose descriptor at 0x0212fe6c is reconstructed
 * as g_profile_KINOPIO.
 */
#ifndef TOAD_H
#define TOAD_H

#include "types.h"

#ifdef __cplusplus

#include "dActor_c.h"
#include "dCcAc_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "math/Matrix.h"

struct Player;

struct Toad : dActor_c {
    typedef void (Toad::*StateFunc)();

    struct State {
        StateFunc init;
        StateFunc main;
        u32 unk_10;
    };

    u8 mPadD0[0x4];               /* 0x0d0 */
    dCcAc_c mCollider;             /* 0x0d4 */
    ModelAnim mModelAnim;          /* 0x108 */
    ShadowModel mShadowModel;      /* 0x16c */
    Matrix4x3 mCapTransform;       /* 0x194 */
    Matrix4x3 mShadowTransform;    /* 0x1c4 */
    u32 mCapUniqueID;              /* 0x1f4 */
    Player *mTalkPlayer;           /* 0x1f8 */
    s32 mState;                    /* 0x1fc */
    s16 mHeadYaw;                  /* 0x200 */
    s16 mHeadYawTarget;            /* 0x202 */
    s16 mHeadPitch;                /* 0x204 */
    s16 mHeadPitchTarget;          /* 0x206 */
    u16 mMessageID;                /* 0x208 */
    u8 mTalkFinished;              /* 0x20a */
    u8 mVariant;                   /* 0x20b */
    u8 mStarID;                    /* 0x20c */
    u8 mOpacity;                   /* 0x20d */
    u8 mTargetOpacity;             /* 0x20e */
    u8 mStarReqIndex;              /* 0x20f */

    /* Inline is load-bearing: explicit use from the two destructor sources
     * makes mwccarm emit D1/D0 in ROM order without a homeless D2. */
    virtual ~Toad() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    u16 GetMessageID();
    void St_Talk_Main();
    void St_Talk_Init();
    void St_Idle_Main();
    void St_Idle_Init();
    void SetState(s32 state);
    void RunState();
    void UpdateModelPose();
};

typedef char Toad_State_size_must_be_0x14[
    sizeof(Toad::State) == 0x14 ? 1 : -1];

#else

/* No C source needs the reconstructed fields. Keep a flat compatibility
 * view so incidental C includes still agree on the allocation size. */
struct Toad {
    u8 bytes[0x210];
};

#endif

typedef char Toad_size_must_be_0x210[
    sizeof(struct Toad) == 0x210 ? 1 : -1];

#endif /* TOAD_H */
