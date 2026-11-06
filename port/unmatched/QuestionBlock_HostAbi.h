/* PORT_HOST_ABI. QuestionBlock's own fields, put back where the ROM keeps
 * them. Force-included ahead of src/game/actors/d_a_obj_hatena_block.cpp (main
 * folded the ten src/_ZN13QuestionBlock*.cpp translation units into it) so
 * it addresses the ROM's member offsets instead of MSVC's. Nothing in src/
 * or include/ changes. Run link100, lane CRASH8.
 *
 * The mechanism is the one port/unmatched/MgSmartball_HostAbi.h and
 * port/unmatched/Fader_HostAbi.h already use: pre-define a header's own guard
 * macro, so the src file still says #include "daObjHatenaBlock_c.h", the include
 * still happens, and the body is skipped because the guard is already defined.
 *
 * ---- WHAT IS WRONG WITHOUT THIS ----------------------------------------
 *
 * MSVC does not let a derived class reuse its base class's TAIL PADDING. The
 * Itanium ABI that mwccarm implements does. dBgActor_c has a DATA size of
 * 0x31e and a sizeof of 0x320, so mwccarm starts QuestionBlock's own fields at
 * 0x31e, include/daObjHatenaBlock_c.h's `u8 pad_31e[0x2]` fills 0x31e..0x31f, and
 * ModelAnim mModelAnim lands on the ROM's 0x320. MSVC starts them at 0x320
 * instead, the same two-byte pad fills 0x320..0x321, and mModelAnim is pushed
 * to 0x324. Every field of this class from mModelAnim down is then four bytes
 * high, and sizeof becomes 0x3fc against the 0x3f8 the ROM's own factory asks
 * fBase_c::operator new for.
 *
 * DROPPING THE PAD IS THE WHOLE FIX. With no pad_31e, MSVC starts the own
 * fields at sizeof(dBgActor_c) = 0x320, which is already four-aligned, so
 * mModelAnim lands on 0x320 and every later field follows: the member sizes
 * themselves agree between the two compilers (measured below), so one
 * four-byte correction at the base boundary realigns the entire class and
 * sizeof comes back to 0x3f8.
 *
 * ---- MEASURED IN THE SHIPPED ARTIFACT, NOT INFERRED --------------------
 *
 * The factory and the class disagree in the bytes. Both quoted from
 * build/port/walk_window.exe at fd50368ad (md5 4c0c76c8):
 *
 *   _daObjHatenaBlock_c_classInit_HATENA_BLOCK  00521c00, from the C TU
 *   src/d_a_obj_hatena_block_hatena_block.c, which uses the ROM's literals:
 *     00521c1a  lea ecx, [esi + 0x320]   -> __ZN9ModelAnimC1Ev
 *     00521c2c  lea eax, [esi + 0x384]   -> __ZN11ShadowModelC1Ev
 *
 *   ?InitResources@QuestionBlock@@UAEHXZ  00530240, the C++ TU:
 *     0053026b  lea esi, [edi + 0x324]   -> ModelBase::SetFile
 *     00530325  lea ecx, [edi + 0x388]   -> ShadowModel::InitCuboid
 *
 * So ModelAnim is CONSTRUCTED at 0x320 and USED at 0x324. Word 0 of the used
 * address is still the zero the allocator left, and
 * ?SetFile@ModelBase@@QAEHPAUBMD_File@@HH@Z's first act is
 * `mov eax,[ecx] / jmp dword ptr [eax+8]`, so the boot faults at 004b7986
 * with eax = 0 on the first question block the level loader spawns.
 *
 * The ROM's own code sits on the 0x320 side, which is what makes the C++ TU
 * the outlier rather than the factory: _func_ov102_02149da8 at 0052c620, the
 * state setter matched from ROM bytes, does `mov dword ptr [eax + 0x3e8], edx`
 * where the C++ TUs read mState at 0x3ec. Fixing only the crash would have
 * left the state machine writing into a hole.
 *
 * THE FULL SHIFT, every displacement the ten C++ TUs compute, read out of the
 * shipped bytes with tmp/crash8/qbscan.py. Base members agree, own members are
 * four high:
 *     mModel        0x0d4 = 0x0d4     mMeshCollider 0x124 = 0x124
 *     mClsnMat      0x2ec = 0x2ec     (all inherited from dBgActor_c)
 *     mModelAnim    0x320 -> 0x324    mShadowModel  0x384 -> 0x388
 *     mShadowMat    0x3ac -> 0x3b0    mHomePosY     0x3e0 -> 0x3e4
 *     mState        0x3e8 -> 0x3ec    mStarTracked  0x3f0 -> 0x3f4
 *     mStarId       0x3f1 -> 0x3f5    mHitterParam  0x3f2 -> 0x3f6
 *     mContentType  0x3f3 -> 0x3f7
 *
 * ---- WHY THIS SHAPE ----------------------------------------------------
 *
 * THIS IS A CLASS-SHAPED DEFECT AND THIS IS A ONE-CLASS FIX. port/CMakeLists
 * already records the same mechanism biting dScMgSmartball_c ("it neither
 * reuses the base's tail padding nor skips a vfptr for a base that declares no
 * virtual of its own; measured 0x47d0 against the ROM's 0x47c8"), and any
 * header whose first own field follows a `u8 pad_NNN[k]` with NNN not
 * four-aligned has it. The complete fix belongs in include/daObjHatenaBlock_c.h and
 * its peers, as
 *
 *     #ifndef _MSC_VER
 *         u8 pad_31e[0x2];
 *     #endif
 *
 * which is byte-neutral on the NDS side because mwccarm does not define
 * _MSC_VER. That is a decomp-side edit, which this lane does not own, so it is
 * written up in the lane report and handed over instead.
 *
 * Everything below is include/daObjHatenaBlock_c.h's own C++ declaration
 * verbatim, with pad_31e removed and nothing else touched, so the member
 * types, the member order, the class name and every method signature are
 * unchanged and no mangled name moves.
 *
 * PORTSYNC2: re-copied from main's header after the 2026-10 merge. Main
 * renamed include/QuestionBlock.h to include/daObjHatenaBlock_c.h (guard
 * DAOBJHATENABLOCK_C_H), named the old pads (mBounceYOffs, mFloorY,
 * mBounceAng, mBounceTimer, mHeldActor), added the func_ov102_* members and
 * the State / Content enums. The old shadow still defined QUESTIONBLOCK_H, so
 * the real header was no longer skipped and the class was defined twice.
 */
#ifndef PORT_QUESTIONBLOCK_HOSTABI_H
#define PORT_QUESTIONBLOCK_HOSTABI_H

#include "types.h"
#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "dExtShadowModel_c.h"

/* shadows include/daObjHatenaBlock_c.h */
#define DAOBJHATENABLOCK_C_H

struct daObjHatenaBlock_c : dBgActor_c {
    /* u8 pad_31e[0x2] lives here on the NDS side. See the header comment:
       MSVC has already spent those two bytes as the base's tail padding. */
    ModelAnim mModelAnim;             /* 0x320 -- only HATENA_BLOCK (20) loads and draws it */
    dExtShadowModel_c mShadowModel;         /* 0x384 */
    /* Seeded from mModel.mat4x3 by InitResources, then rebuilt every frame
       by func_ov102_02149ea4. DropShadowScaleXYZ takes this pairing. 0x3ac + 0x30 = 0x3dc. */
    Matrix4x3 mShadowMat;             /* 0x3ac */
    /* Bounce squash Y added onto mPosY when writing the model/collider
       translation (func_ov102_02149ff0 / 02149e38 / 021498e0). Fix12:
       0x1000 = 1 unit. */
    s32 mBounceYOffs;                 /* 0x3dc */
    s32 mHomePosY;                    /* 0x3e0 -- mPosY at InitResources; Behavior clamps mPosY up to it after UpdatePos (not while POPPED) */
    /* Floor Y from the ground raycast (func_ov102_02149610); shadow height. */
    s32 mFloorY;                      /* 0x3e4 */
    s32 mState;                       /* 0x3e8 -- State */
    /* Bounce phase: starts at 0x4000 (a quarter turn) when the bounce begins
       and grows by 0x1000 (1/16 turn) per frame; its sine, from
       data_02082214, drives the squash scale and mBounceYOffs. */
    u16 mBounceAng;                   /* 0x3ec */
    /* Frame countdown, ticked by DecIfAbove0_Short: 7 when the bounce begins
       (func_ov102_02149c78); 300 (0x12c) when the block pops
       (func_ov102_021498c4), where it is the delay before the block can come
       back. */
    u16 mBounceTimer;                 /* 0x3ee */
    u8 mStarTracked;                  /* 0x3f0 -- TrackStar's result (CONTENT_STAR only); passed back to UntrackStar */
    u8 mStarId;                       /* 0x3f1 -- param1 bits 8..15 (CONTENT_STAR only) */
    /* Truncated from the hitter's param1 by every combat callback, then
       func_ov102_02149da8(this, STATE_BOUNCING). A prize column is picked
       with it, clamped to 0..3. */
    u8 mHitterParam;                  /* 0x3f2 */
    u8 mContentType;                  /* 0x3f3 -- Content */
    /* A SECRET_COIN that registered itself with this block: daSCoin_c::
       LinkToBlock stores itself here when it is within 200 units of
       an actor 20 / 21 block. Bounce-end (func_ov102_021498e0) collects it
       (actorID 0x149, SECRET_COIN) and clears this. Not written by this
       class. */
    dActor_c *mHeldActor;             /* 0x3f4 */

    /* Inline is load-bearing: out-of-line emits D0 before D1.
       The two-arm pair from include/daObjHatenaBlock_c.h, which this view
       replaces: without it the compiler vtable's slot 16 named
       dBgActor_c::Destructor1, so deleting a block never ran this
       destructor and its mShadowModel stayed on the shadow list. The
       virtual destructor stays, so the slot after Kill keeps its place. */
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    virtual ~daObjHatenaBlock_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    void OnGroundPounded(dActor_c &other);     /* slot 21 */
    int OnAttacked1(dActor_c &other);          /* slot 22 */
    void OnKicked(dActor_c &other);            /* slot 24 */
    void OnHitByMegaChar(Player &player);      /* slot 27 */
    int OnHitFromUnderneath(dActor_c &other);  /* slot 28 */

    /* func_ov102_* helpers keep linker names -- no semantic names are known.
       All are ordinary members; func_ov102_02149684 is the one exception,
       staying free (the block is its second argument). */
    int func_ov102_02149078();
    void func_ov102_02149100(Vector3 *pos, int n, unsigned int speed, short baseAngle);
    void *func_ov102_02149220();
    void func_ov102_02149288();
    void func_ov102_021492d4();
    void *func_ov102_02149384();
    void func_ov102_021493dc();
    void func_ov102_02149428();
    void func_ov102_02149478();
    void func_ov102_021494cc();
    void func_ov102_0214953c(int p1, int p2);
    int func_ov102_02149610();
    int func_ov102_02149878();
    void func_ov102_021498c4();
    void func_ov102_021498e0();
    void func_ov102_02149c78();
    void func_ov102_02149ccc();
    void func_ov102_02149d80();
    void func_ov102_02149da8(int i);
    void func_ov102_02149df0();
    void func_ov102_02149e38();
    void func_ov102_02149ea4();
    void func_ov102_02149ff0();

    /* mState: an index into the {enter, update} pair table at
       data_ov102_0214e890. func_ov102_02149da8 stores the index and runs the
       row's enter routine; func_ov102_02149df0 (the top of Behavior) runs its
       update routine. Rows, read from the ROM:
         IDLE     enter 02149d80  update 02149ccc
         BOUNCING enter 02149c78  update 021498e0
         POPPED   enter 021498c4  update 02149878
       The names are descriptive, taken from what the routines do; the ROM
       stores none. */
    enum State {
        STATE_IDLE = 0,     /* waiting to be hit */
        STATE_BOUNCING = 1, /* hit: squash-and-stretch for a few frames, then spawn the prize */
        STATE_POPPED = 2    /* prize spawned: Render draws nothing; if the block survives the
                               prize (coins, star and 1-up kill it), back to IDLE after a
                               300-frame delay once the player is over 100 units away.
                               Behavior keeps the mesh collider off in every state but IDLE. */
    };

    /* mContentType, the low byte of param1 (0xff reads as 0; the VS block
       forces 0). Row index into the prize table data_ov102_0214e8c0
       (data_ov102_0214e870 for the VS block); each row has four columns,
       picked by mHitterParam clamped to 0..3 (ITEM_BLOCK always takes column
       0). The names say which actor the row spawns; the loaders in
       InitResources / CleanupResources key off the same numbers. */
    enum Content {
        CONTENT_COINS = 0,         /* a spray of COIN actors; count is param1 bits 8..15 (0xff = 1) */
        CONTENT_STAR = 1,          /* STAR; star id is param1 bits 8..15 (0xff = 0) */
        CONTENT_ONEUP = 2,         /* ONEUPKINOKO */
        CONTENT_SHELL = 3,         /* SHELL */
        CONTENT_SCALEUP_KINOKO = 4, /* SCALEUP_KINOKO */
        CONTENT_POWER_UP_5 = 5,    /* column 0 FEATHER, columns 1..3 POWER_UP_ITEM */
        CONTENT_POWER_UP_6 = 6,    /* POWER_UP_ITEM in every column */
        CONTENT_POWER_UP_7 = 7     /* column 0 BOMBHEI (func_ov102_02149220), 1..3 POWER_UP_ITEM */
    };
};

#endif
