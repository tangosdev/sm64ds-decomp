#ifndef DABGSNMBDY_C_H
#define DABGSNMBDY_C_H

#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "PathPtr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* The big rolling snowman's BODY actor.
 *
 * NAME. The cartridge names this class itself: ov072 0x0212278c holds the
 * length-prefixed Itanium _ZTS payload "12daBgSnmBdy_c" NUL-terminated,
 * _ZTI12daBgSnmBdy_c at 0x02122780 points its +4 word at that string, and the
 * vtable's -4 header word at 0x021227bc points back at the _ZTI. `SnowmanBody`
 * was a coined readable spelling with no cartridge attestation; it is retired.
 *
 * BASE. dActor_c directly. _ZTI12daBgSnmBdy_c is a three-word
 * __si_class_type_info whose +8 word is 0x0208e390 == _ZTI8dActor_c, so
 * dActor_c is the sole base at offset zero. The vtable at 0x021227c0 is 31
 * slots -- exactly dActor_c's own length, which a dBgActor_c child could not
 * be (dBgActor_c adds a 32nd slot). Seven of the 31 are overridden: 0, 3, 6,
 * 9, 12, 16 and 17, read word-by-word out of the cartridge table.
 *
 * SIZE. 0x3a8. daBgSnmBdy_c_classInit at 0x0211fedc opens with
 * fBase_c::operator new(936), and the five subobjects it constructs after the
 * dActor_c base -- Model at 0xd4, ShadowModel at 0x124, dCcAc_c at 0x14c,
 * dBgCh_Actr at 0x180 and PathPtr at 0x380 -- are declared typed below in that
 * same order, which is what makes the compiler emit that exact call sequence.
 *
 * THE SIX STATE PAIRS are not guesses from code adjacency. The static
 * initializer at 0x02122018 copies twelve ROM pointer-to-member constants into
 * data_ov072_02122b64 in the exact pairs documented below. SetState indexes
 * that table at state * 0x10 and CallStateInit invokes the first PMF of the
 * pair; CallStateBehavior invokes the second. Descriptive original names are
 * absent, so the ROM-evidenced indices are retained: State<N>/InitState<N>
 * claim the INDEX and nothing more.
 *
 * daBgSnmBdy_c_classInit and g_profile_BIG_SNOWMAN_BODY are reconstructed
 * source-style spellings (historical aliases SnowmanBody_Spawn and
 * SnowmanBody_SpawnInfo). The cartridge proves the factory's address, its
 * behavior, and the BIG_SNOWMAN_BODY registry relationship; it does not
 * preserve either identifier.
 */
struct daBgSnmBdy_c : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    Model mModel;                       /* 0x0d4 */
    ShadowModel mShadowModel;           /* 0x124 */
    dCcAc_c mCylinder;                  /* 0x14c */
    dBgCh_Actr mWithMeshClsn;           /* 0x180 */

    s32 mHomePosX;                      /* 0x33c */
    s32 mHomePosY;                      /* 0x340 */
    s32 mHomePosZ;                      /* 0x344 */
    s16 mHomeAngleX;                    /* 0x348 */
    s16 mHomeAngleY;                    /* 0x34a */
    s16 mHomeAngleZ;                    /* 0x34c */
    u8 mPad34e[0x2];                    /* 0x34e */
    Matrix4x3 mShadowMat;               /* 0x350 */
    PathPtr mPath;                      /* 0x380 */
    u32 mPathNode;                      /* 0x388 */

    typedef int (daBgSnmBdy_c::*StateFunc)();
    StateFunc *mStateFuncs;             /* 0x38c */
    Player *mTalkPlayer;                /* 0x390 */
    s32 mStateValue;                    /* 0x394 */
    s32 mRadius;                        /* 0x398 */
    u32 mSoundID;                       /* 0x39c */
    u16 mStateTimer;                    /* 0x3a0 */
    u8 mSubstate;                       /* 0x3a2 */
    u8 mPlayerReachedPath;              /* 0x3a3 */
    u8 unk_3a4;                         /* 0x3a4 */
    u8 mPad3a5[0x3];                    /* 0x3a5 */

    /* --- overrides, in dActor_c's own vtable order. --- */
    virtual int InitResources();        /* slot  0 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Behavior();             /* slot  6 */
    virtual int Render();               /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    int InitState0();
    int State0();
    int InitState1();
    int State1();
    int InitState2();
    int State2();
    int InitState3();
    int State3();
    int InitState4();
    int State4();
    int InitState5();
    int State5();

    int IsPlayerNearCenter();
    void UpdateRollAngle();
    int AdvancePath();
    int HurtPlayer();
    /* The mangled spelling P10dBgCh_Actr asserts a POINTER parameter. The bytes
       cannot distinguish a pointer from a reference here -- R10dBgCh_Actr would
       have matched equally well -- so the parameter type is a disclosed guess,
       not a recovered fact. */
    void UpdateGroundCollision(dBgCh_Actr *collision);
    void UpdateModel();
    void CallStateBehavior();
    void CallStateInit();
    /* The Ei suffix chooses int for the state index. The table access and
       call sites do not establish the original parameter type; an enum
       spelling is not ruled out. This is an inferred declaration. */
    void SetState(int state);

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* Declared last and inline so class instantiation emits the retail D1/D0
       pair in that order, at the bottom of the run, with no separate D2 body.
       Keep the brace on the signature line -- check_header_offsets only arms
       its body skip when the signature line itself carries the `{`. */
    virtual ~daBgSnmBdy_c() {}          /* slots 16 (D1), 17 (D0) */
};

typedef char daBgSnmBdy_c_size_must_be_0x3a8[
    sizeof(daBgSnmBdy_c) == 0x3a8 ? 1 : -1];

#endif
