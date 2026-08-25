/* Started life AUTO-GENERATED from matched-function evidence by
 * tools/gen_header.py; the field names below have since been recovered from
 * the bodies. Offsets/widths are observed, not guessed. Gaps are explicit
 * padding. Renaming cannot change codegen.
 *
 * THE BUTTERFLY PUZZLE. Actor 0x150. One butterfly with param1 bits 0xc0
 * clear is the "spawner": func_ov100_021412d8 spawns three more of actor
 * 0x150 with a kind in bits 0xc0, one of which is kind 1. The kind-1
 * butterfly is the one func_ov100_0214109c turns into actor 0x114 when it
 * has been chased long enough; the others fold their scale to 0 and switch
 * to the still model.
 *
 * mState IS A DISPATCH INDEX, NOT A MOOD. Butterfly::Behavior reads it, looks
 * data_ov100_02148628[mState] up as a pointer-to-member-function and calls
 * it, so every one of func_ov100_02140e44 / 0214109c / 0214117c / 021412d8 /
 * 02141470 / 021415bc / 02141800 / 02141848 -- all of them inside this
 * class's own address range, between _ZN9ButterflyD0Ev and
 * _ZN9Butterfly16CleanupResourcesEv -- is one of its states. Those eight are
 * where most of the names below come from; each of them writes mState and
 * zeroes mStateTimer to move on.
 *
 * 0x080..0x0a8 IS dActor_c's OWN LAYOUT, not this class's, and is named from
 * include/dActor_c.h by offset -- the same thing include/Tornado.h already
 * does over the same range. 0x0a4 and 0x0ac stay unk_ because dActor_c leaves
 * them unnamed too; this class writes the three of them together as an (x,
 * vert, z) velocity triple, which is a lead for a later pass on the base, not
 * a licence to name a shadow field something its own base contradicts.
 *
 * Provenance table: notes/butterfly-tornado-provenance.md. */
#ifndef BUTTERFLY_H
#define BUTTERFLY_H
#include "types.h"
#include "ModelAnim.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct Butterfly {
    u8  pad_000[0x80];
    /* dActor_c's own fields, named by offset out of include/dActor_c.h.
       Behavior writes all three scale words from mScale every frame. */
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e -- Behavior copies mPrevAngleY here and
                                 builds the render matrix from it */
    u8  pad_090[0x2];
    /* dActor_c declares these two s16; this class reads them UNSIGNED to index
       the sine table (`(u16 >> 4) << 1`), so the width stays u16 here -- an
       ldrh where the ROM has an ldrh. Renaming does not retype. */
    u16 mPrevAngleX;        /* 0x092 -- pitch: func_ov100_0214117c eases it
                                 toward -0x2000/0x2000 to climb or dive */
    u16 mPrevAngleY;        /* 0x094 -- heading: eased toward mWanderAngle, the
                                 home direction, or the player */
    u8  pad_096[0x2];
    s32 mHorzSpeed;         /* 0x098 */
    u8  pad_09c[0x8];
    s32 unk_0a4;            /* 0x0a4 -- dActor_c leaves it unnamed; Behavior
                                 writes it as the X of a velocity triple */
    s32 mVertSpeed;         /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac -- ditto, the Z of that triple */
    u8  pad_0b0[0x24];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_0f0 (+0x1c = mat4x3),
       mAnimation (+0x50 = the Animation base), which the header declared separately inside
       it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* Model member, named by _ZN5ModelD1Ev at +0x138 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_154 (+0x1c = mat4x3), which
       the header declared separately inside it. */
    Model mModel;            /* 0x138 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x188 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9ButterflyD0Ev.c] */
    ShadowModel mShadowModel1;            /* 0x188 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x1b0 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9ButterflyD0Ev.c] */
    ShadowModel mShadowModel2;            /* 0x1b0 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x1d8 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9ButterflyD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x1d8 */
    /* dCcAcPos_c member. The cartridge's own ~Butterfly calls _ZN10dCcAcPos_cD1Ev at
       +0x394 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dCcAcPos_c mdCcAcPos_c;            /* 0x394 */
    /* The position InitResources copies out of mPos before anything moves.
       func_ov100_02141470 and func_ov100_021415bc snap mPos back to it, and
       func_ov100_0214117c takes the horizontal angle from mPos to it as the
       heading to fly home on. */
    s32 mHomePosX;          /* 0x3d4 */
    s32 mHomePosY;          /* 0x3d8 */
    s32 mHomePosZ;          /* 0x3dc */
    s32 mScale;             /* 0x3e0 -- the single fix12 scale Behavior copies
                                 into all three of mScaleX/Y/Z. InitResources
                                 sets 0x1000; func_ov100_0214109c winds it from
                                 0 up to 0x800 in 0x40 steps, or drops it to 0
                                 for a butterfly that is not kind 1;
                                 func_ov100_02140e44 adds a sine-table wobble
                                 driven by mFlutterPhase. */
    s32 mState;             /* 0x3e4 -- index into the pointer-to-member table
                                 data_ov100_02148628 that Behavior dispatches
                                 through; see the header comment. 4 is the
                                 inert state -- Behavior skips the matrix work
                                 and Render draws nothing. */
    s32 mStateTimer;        /* 0x3e8 -- frames in the current state. Seeded to a
                                 random 0..99 by InitResources, then zeroed by
                                 every state that hands over; states compare it
                                 against 0x14, 0x3c, 0x6e, 0x78, 0x9d and 100. */
    s16 mWanderAngle;       /* 0x3ec -- a random heading func_ov100_021412d8
                                 rolls at spawn; after 0x3c frames
                                 func_ov100_0214117c steers mPrevAngleY toward
                                 it instead of toward home. */
    s16 mFlutterPhase;      /* 0x3ee -- a phase angle func_ov100_02140e44
                                 advances by 0x2710 or 0xfa0 a frame and feeds
                                 to the sine table to pump mScale. Zeroed by
                                 func_ov100_0214109c. */
    u8  mKind;              /* 0x3f0 -- 0 for the spawner, 1..3 for the three
                                 butterflies func_ov100_021412d8 spawns from
                                 param1 bits 0xc0. Only kind 1 becomes actor
                                 0x114 in func_ov100_0214109c; Behavior skips
                                 all movement while this is 0. */
    u8  mUseAnimModel;      /* 0x3f1 -- 1: draw mModelAnim with mShadowModel1
                                 and advance the animation. 0: draw mModel with
                                 mShadowModel2 and do not. Set by
                                 InitResources, cleared by
                                 func_ov100_0214109c. */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char Butterfly_size_must_be_0x3f4[sizeof(struct Butterfly) == 0x3f4 ? 1 : -1];

#endif
