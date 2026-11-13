//cpp
/* daStar_c / daStarBase_c -- the Power Star, the silver star, and the marker
 * that spawns one.
 *
 * ROM evidence: _ZTS8daStar_c at ov002:0x0210aa00, _ZTI8daStar_c at
 * 0x0210aa24, base word _ZTI12dEnemyBase_c at 0x021081c0.
 * _ZTS12daStarBase_c at 0x0210aa30, _ZTI12daStarBase_c at 0x0210aa18,
 * base word _ZTI8dActor_c at 0x0208e390. The run is 0x020e6c40..0x020ebe5c.
 *
 * daStar_c's out-of-line destructor is the key function. Under
 * `#pragma defer_codegen off` it emits D1 at 0x020e6c40 and D0 at
 * 0x020e6c90, then a D2 the cartridge does not keep. daStarBase_c's
 * destructor is inline; the odr-use at the end of this file emits D1 at
 * 0x020e6cf4 then D0 at 0x020e6d34. That odr-use is not in the ROM.
 * defer_codegen stays on for the rest, so those bodies are written in
 * reverse source order and come out in ROM order. Do not reorder.
 *
 * READABILITY PASS (byte-neutral): daStar_c's fields, State, Kind and flag
 * bits are named in daStar_c.h and daStarBase_c.h (layout unchanged, sizeof
 * 0x4c4 and 0x1dc asserted); the handlers read through members, dCamera_c,
 * daSoundObj_c, dBgCh_Gnd and dBgCh_Actr instead of offset arithmetic, and each
 * function says what it does.
 *
 * Units: positions, speeds and scales are Fix12 (0x1000 = 1.0; the comments
 * give decimal values); angles are s16 (0x10000 = a full turn); a model or
 * shadow matrix takes the position >> 3. Hit-flag bits and sound ids stay
 * numbers; the sound calls are func_02012694 (positional, bank 3) and
 * func_02012790 (2D, bank 2).
 *
 * Leftover. The first three items were tried in their clean form and moved
 * the bytes; the rest are names or types the evidence does not give:
 * - StateCollectTalk keeps the `*(u16 *)((int)&mStarFlags)` halfword
 *   stores and the LA label; a plain mBits.answer store loses the address
 *   rematerialization. The `& ~0ULL` identity mask that used to sit on that
 *   address is gone and the sentence no longer claims it.
 * - Render keeps its gotos, and mStateTimer keeps its (u16) casts.
 * - func_ov002_020e8244 stays a free function; its first parameter is the
 *   output vector, not the star.
 * - Still unnamed: the dActor_c word at +0xc8, the bone word at +0xc and
 *   the table entry at data_02082714 + 0x56.
 * - Fix12-by-value callees stay as mangled extern "C" names, and `this`
 *   goes to some externs as (char *)this to keep their declared signatures.
 * - The Kind names are numbers (KIND_n): only what the code evidences is
 *   said about each one.
 * Methods are named for what their bodies do (and, for the State handlers and the
 * launch/arc/land sequence, the N64 star_spawn behaviour); the two whose job is
 * unclear keep their address.
 *
 */

#include "Particle__System.h"
#include "ParticleIds.h"

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daStar_c_AssetId {
    ASSET_STAR_BASE_BMD = 0x04a0
};

#include "daStar_c.h"
#include "daStarBase_c.h"
#include "daObjIceBlock_c.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "common.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "decl_Actor.h"
#include "daPly_c.h"
#include "Message.h"
#include "SaveData.h"
#include "SharedFilePtr.h"
#include "dExtFrameCtrl_c.h"
#include "dCamera_c.h"
#include "fBase_c.h"
#include "daSoundObj_c.h"
#include "dExtSimpleModel_c.h"
#include "Sound.h"
#include "private/mtx43.h"
#include "cstd.h"
#include "Event.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "LevelRng.h"
#include "nitro/fx/fx_trig.h"

/* Plain-data stand-ins. Vector3 and Matrix4x3 carry declared destructors, so
 * a local or a struct copy of either one adds cleanup code the cartridge does
 * not have; NitroSDK VecFx32 and these same-layout PODs keep the bytes
 * identical. Whole matrices are copied through Mtx43. */
struct Vec1 { s32 a; };                /* one word, copied component by component */

/* A state handler: data_ov002_021109d8 holds one pointer-to-member per
   daStar_c::State. */
typedef void (daStar_c::*StateHandler)();

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
   func_02017ab4. Animation handles construct through SharedFilePtr::Construct
   and destroy through SharedFilePtr_Destruct_Anim. The manifest aliases those
   undefined members onto the ROM symbols. */
struct StarModelFilePtr : SharedFilePtr {
    StarModelFilePtr(u32 fileID);
    ~StarModelFilePtr();
};

struct StarAnimationFileHandle : SharedFilePtr {
    StarAnimationFileHandle(u32 fileID);
    ~StarAnimationFileHandle();
};

extern StarAnimationFileHandle data_ov002_02110944;

/* Actor IDs this file spawns or searches for. */
enum {
    ACTOR_ICE_BLOCK_LL   = 0x12,
    ACTOR_CAMERA_MARKER  = 0xb1,  /* StarCamera: where the camera stands for this star id */
    ACTOR_STAR           = 0xb2,  /* daStar_c, the Power Star */
    ACTOR_SILVER_STAR    = 0xb3,  /* daStar_c, the silver star */
    ACTOR_STAR_MARKER    = 0xb4   /* daStarBase_c */
};

/* Set in an actor's mFlags, in the touching player's mFlags and in the global
   word gNextActorUpdateMask for as long as a star's collection cutscene runs. */
enum { CUTSCENE_FLAG = 0x4000000 };

extern "C" {
/* dExtAnmModel_c::SetAnim, called with a scalar speed. */
extern char data_02082714[];
extern int func_0203d024(struct Vector3*, struct Vector3*);
extern void DeathTable_ClearBit(int);
extern void FUN_0202a130(void);
extern void UnloadSilverStarAndNumber(void);
extern StarAnimationFileHandle data_ov002_02110934;
extern unsigned int gNextActorUpdateMask;
extern int data_ov002_0210da28[];
extern signed char gNumStageStars[];
extern signed char NumRedCoins(void);
extern unsigned char gGameMode;
extern void func_02035860(void* o, void* src);
extern dCamera_c *gCamera;
extern unsigned char IsAreaShowing(int idx);
extern void func_02012694(unsigned int a, void *p);
extern void func_ov002_020e8244(void *out, daStar_c *b);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* mF, s16 angY);
/* local extern: math/MtxStack.h takes Matrix4x3 pointers, and
   func_ov002_020e8244 passes an Mtx43: its scratch copy cannot be a Matrix4x3,
   whose Vector3 member brings a destructor (_ZN9Matrix4x3D1Ev) into the TU. */
extern void MTX_Concat43(void *out, void *a, void *b);
extern int *Vec3_LslInPlace(void *v, int sh);
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern int SurfaceInfo_TestFlag0x20(void* p);
extern int data_0209f32c;
extern signed char gCurrentSublevel;
extern void SetStarMarker(int i, dActor_c *actor, int v2);
extern int IsStarCollectedInCurLevel(int starID);
extern dActor_c *STAR_MARKERS[12];
extern u8 data_0209f208;
extern unsigned char gCurrentEntranceId;
void GiveVsStars(int idx, int delta);
void CollectStarInCurLevel(int i);
int SublevelToLevel(int i);
s16 NumCoins(void);
extern u8 gCollectedStarId;
extern u8 gNewStarCollected;
extern s16 gNumCoins[];
extern u8 data_0209d684;
extern s16 GetAngleToCamera(int i);
extern u8 *data_0209f344;
}
/* local extern: the header KuppaScript.h would renumber this TU's compiler temps (@NNN pins in its manifest) */
namespace KuppaScript {
void End(void);
}
extern "C" {
extern u8 data_0209d660;
extern s8 gNumStageStars[];
extern signed char gNumStageStars[];
extern int NumVsStarsObtained(void);
extern StarModelFilePtr data_ov002_0211092c;
extern StarModelFilePtr data_ov002_0211093c;
extern StarAnimationFileHandle data_ov002_02110924;
extern StarAnimationFileHandle data_ov002_02110964;
extern SharedFilePtr data_ov002_0210d9a8;
/* Numbering pin: func_ov002_020d718c is now a daPly_c member (see daPly_c.h);
   no call in this TU uses the free spelling anymore. The declaration stays,
   spelled exactly as it was, because each named parameter advances mwccarm's
   @NNN counter: removing or un-naming it renumbers this TU's .data temps
   (measured on daYegg_c, same mechanism). */
extern void func_ov002_020d718c(void *p);
extern int data_ov002_0210aa0c[3];
extern StateHandler data_ov002_021109d8[];
extern u8 gCurrentStarId;
extern s32 data_0209cef0;
extern s32 data_02092138;
extern StarModelFilePtr data_ov002_0211094c;
extern StarModelFilePtr data_ov002_02110954;
extern StarModelFilePtr data_ov002_0211095c;
extern void LoadSilverStarAndNumber(void);
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, void *actor, s32 a, s32 b, void *p1, void *p2);
extern s32 IsStarCollected(s32 level, s32 idx);
}

/* The two destructor bodies are defined first so their D1/D0 groups lead the
 * object in ROM order. Under defer_codegen off each out-of-line destructor
 * emits its D1, D0 and a homeless D2 at the definition. With the deferred
 * queue still empty here both groups land before every deferred function;
 * placed later, the second class's group instead rides the deferred flush and
 * ends the object. (d_a_kpa.cpp precedent.) */
#pragma defer_codegen off

/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct daStar_c : dEnemyBase_c` and the members that declaration now types:
 * its own vptr, then dExtShadowModel_c (0x3d4), dExtAnmModel_c (0x370), dExtAnmModel_c (0x30c),
 * dBgCh_Actr (0x150),
 * dCcAcPos_c (0x110)
 * in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `daStar_c_classInit_STAR` constructs the
 * same types at the same offsets.
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Nobody writes that; declaring `~daStar_c()`
 * is enough, because mwcc emits D2, D0 and D1 together and objisolate keeps the
 * one this file is bound to. The deallocation is an inline operator delete --
 * dEnemyBase_c's, reached because dEnemyBase_c is this class's IMMEDIATE base.
 */
#ifndef _MSC_VER
daStar_c::~daStar_c()
{
}
#endif

#pragma defer_codegen on

/* Spawn-time set-up for the Power Star (actor 0xb2) and the silver star
 * (0xb3). Loads the shared animation files and the star models, then picks the
 * first State from the Kind in param1 bits 4..7: kinds 0, 5, 6 and 7 idle,
 * kind 1 bounces out (32.0 up), kinds 2 and 4 launch, anything else waits for
 * a marker. Kind 6 spawns its own marker (param 0x40) 10.0 above itself and
 * starts at full size only when NumVsStarsObtained() is 5. In a VS match
 * (gGameMode == 1) and for the silver star, a marker with param 0x50 is
 * spawned at the star's own position instead. Returns 0 to cancel the spawn
 * when a model, a marker or the shadow fails; a few level rules (level 0x1d
 * with the star already collected, sublevel 8 star 7, sublevel 7 star 2)
 * destroy the star here. */
s32 daStar_c::InitResources()
{
    s32 ret;
    s32 b;
    u32 p;
    u32 k;
    s32 kind;
    char *sp;
    s32 *q;
    VecFx32 v;
    VecFx32 v2;

    ret = 1;
    mStarFlags = 0;
    mSoundObjMode = 0;
    mCamSeq = 0xffff;
    mSavedAreaId = (u8)mAreaId;
    mMarkerSlot = -1;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mMarkerID = 0;
    mSoundObjID = 0;
    mParticle[3] = 0;
    mParticle[2] = mParticle[3];
    mParticle[1] = mParticle[2];
    mParticle[0] = mParticle[1];
    mAppearTimer = 0;
    mSeqTimer = 0;
    mSoundObj6State = 0xff;
    mInitPosX = mPosX;
    mInitPosY = mPosY;
    mInitPosZ = mPosZ;
    mMusicTimer = 0;
    dExtFrameCtrl_c::LoadFile(data_ov002_02110944);
    dExtFrameCtrl_c::LoadFile(data_ov002_02110924);
    dExtFrameCtrl_c::LoadFile(data_ov002_02110964);
    dExtFrameCtrl_c::LoadFile(data_ov002_02110934);

    b = (s32)(actorID == ACTOR_STAR);
    if (b != 0) {
        if (mAnmModel1.SetFile((BMD_File *)data_ov002_0211094c.filePtr, 1, 1) == 0 ||
            mAnmModel2.SetFile((BMD_File *)data_ov002_0211095c.filePtr, 1, 0x18) == 0)
            ret = 0;
    } else {
        LoadSilverStarAndNumber();
        if (mAnmModel1.SetFile((BMD_File *)data_ov002_02110954.filePtr, 1, 1) == 0 ||
            mAnmModel2.SetFile((BMD_File *)data_ov002_02110954.filePtr, 1, 1) == 0)
            ret = 0;
    }

    p = param1;
    k = p & 0x7f;
    if (k == 0x7f) {
        InitKind8();
        return ret;
    }
    if (k == 0x6f) {
        InitKind9();
        return ret;
    }
    mKind = (s32)((p >> 4) & 0xf);

    b = (s32)(actorID == ACTOR_STAR);
    if (b != 0) {
        if (gCurrentStarId == (param1 & 0xf) || SublevelToLevel(gCurrentSublevel) > 0xe)
            mMarkerType = 2;
        else
            mMarkerType = 0;
        if (mKind == KIND_6) {
            LoadSilverStarAndNumber();
            mFlags |= CUTSCENE_FLAG;
        }
    } else {
        mMarkerType = 1;
    }

    mAnmModel1.SetAnim((BCA_File *)data_ov002_02110964.filePtr, 0x40000000, 0x1000, 0);
    mAnmModel2.SetAnim((BCA_File *)data_ov002_02110964.filePtr, 0x40000000, 0x1000, 0);
    if (mShadowModel.InitCylinder() == 0)
        return 0;

    v2.x = data_ov002_0210aa0c[0];
    v2.y = data_ov002_0210aa0c[1];
    v2.z = data_ov002_0210aa0c[2];
    mdCc_c.Init(this, *(Vector3 *)&v2, 0x64000, 0x96000, 0x100002, 0x8000);
    mWithMeshClsn.Init(this, 0x50000, 0, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    mStarID = (u8)(param1 & 0xf);
    if (mKind != KIND_7 && mKind != KIND_3)
        mStarFlags |= 2;

    kind = mKind;
    if (kind == KIND_0 || (u32)(kind - KIND_5) <= 2) {
        mState = STATE_IDLE;
        if (mKind != KIND_6) {
            b = (s32)(actorID == ACTOR_SILVER_STAR);
            if (b != 0) {
                mVertAccel = -0x2000;
                mTerminalVelocity = -0x28000;
            }
        } else {
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            v.y = v.y + 0xa000;
            sp = (char *)dActor_c::Spawn(ACTOR_STAR_MARKER, 0x40, *(Vector3 *)&v, 0, mSavedAreaId,
                                         -1);
            if (sp != 0) {
                mMarkerID = ((dActor_c *)sp)->uniqueID;
            } else {
                return 0;
            }
            if (NumVsStarsObtained() == 5) {
                mScaleX = 0x1000;
                mScaleY = 0x1000;
                mScaleZ = 0x1000;
            } else {
                mScaleX = 0;
                mScaleY = 0;
                mScaleZ = 0;
            }
        }
        if (mKind != KIND_0)
            mdCc_c.flags |= 1;
    } else if (kind == KIND_1) {
        mState = STATE_BOUNCE;
        mVertSpeed = 0x20000;
        SetDefaultGravity();
        mStateTimer = 0xf;
        mSparkleTimer = 0x32;
        mWithMeshClsn.StartDetectingWater();
    } else if (kind == KIND_2 || kind == KIND_4) {
        mState = STATE_LAUNCH;
        mdCc_c.flags |= 1;
    } else {
        mState = STATE_WAIT_MARKER;
        mStarFlags |= 8;
        mdCc_c.flags |= 1;
        if (mMarkerType != 1)
            mMarkerType = 2;
    }

    mHomeState = mState;
    mSafePosX = mPosX;
    mSafePosY = mPosY;
    mSafePosZ = mPosZ;
    q = &mSafePosX;
    mHomePosX = q[0];
    mHomePosY = q[1];
    mHomePosZ = q[2];
    mMinPosY = data_02092138;
    if (!mBits.visible)
        mdCc_c.flags |= 1;

    if (mStarID < 8 && mKind != KIND_5 && mKind != KIND_3 && (s32)(gGameMode == 1) == 0 &&
        IsStarCollectedInCurLevel(mStarID) != 0) {
        if (SublevelToLevel(gCurrentSublevel) == 0x1d) {
            MarkForDestruction();
            return 0;
        }
        mStarFlags |= 4;
    }

    if (mKind == KIND_0 || mKind == KIND_5 || mKind == KIND_7 || mKind == KIND_1)
        MaybeAddStarMarker();
    ProbeGround();
    if (data_0209cef0 == 0) {
        Event::ClearBit(0x1e);
        Event::ClearBit(EVENT_COLLECTING_STAR);
        if (mKind != KIND_3) {
            if ((s32)(gGameMode == 1) != 0 || (s32)(actorID == ACTOR_SILVER_STAR) != 0) {
                sp = (char *)dActor_c::Spawn(ACTOR_STAR_MARKER, 0x50, mPos, 0, mSavedAreaId, -1);
                if (sp != 0) {
                    mMarkerID = ((dActor_c *)sp)->uniqueID;
                } else {
                    return 0;
                }
            }
        }
    }

    sp = (char *)dActor_c::FindWithID((u32)mMarkerID);
    if (sp != 0)
        ((daStarBase_c *)(sp))->LinkSilverStarAndStarMarker((char *)this);
    if (gCurrentSublevel == 8 && mStarID == 7) {
        if (IsStarCollected(SublevelToLevel(8), 1) == 0 || gCurrentStarId == 1) {
            MarkForDestruction();
            return ret;
        }
    }
    if (gCurrentSublevel == 7 && mStarID == 2) {
        if (gCurrentStarId == 1 || IsStarCollectedInCurLevel(1) == 0) {
            MarkForDestruction();
            return 0;
        }
    }
    if (GetBitInDeathTable() != 0 && mStarID == 1 && gCurrentSublevel == 0x2e) {
        KillAndTrackInDeathTable();
        sp = (char *)dActor_c::FindWithID((u32)mMarkerID);
        if (sp != 0)
            ((daStarBase_c *)(sp))->LinkSilverStarAndStarMarker(0);
    }
    return ret;
}

/* Spawn-time set-up for the marker that shows where a star will appear. param1
 * bits 4..7 pick the flavour: kind 6 only spawns the switch star (actor 0xb2,
 * kind 6, mBits.switchStar) and returns 0; kind 4 starts in state 2, kind 5 in
 * state 3, odd kinds in state 1 (kind & 2 also sets hold) and even kinds in
 * state 0 (kind 0 visible, kind 2 hidden). It probes the floor from 30.0 above
 * itself for the shadow, and destroys itself when the star is already
 * collected on level 0x1d or its death-table bit is set. */
int daStarBase_c::InitResources()
{
    Vector3 pos;
    Vector3 v0;
    Vector3 v4;
    Vector3 v5;
    u32 raw;
    u8 kind;
    int r3;

    raw = ((u32)param1 >> 4) & 0xf;
    kind = (u8)raw;
    mFlags = 0;
    v0.x = 0;
    v0.y = -0x50000;
    v0.z = 0;
    mdCcAcPos_c.Init(this, v0, 0x50000, 0xa0000, 0x100002, 0x8000);
    dBgCh_Gnd ground;
    ground.StartDetectingWater();

    {
        s32 pyb = mPosY;
        s32 pz = mPosZ;
        s32 px = mPosX;
        s32 pyy = pyb + 0x1e000;
        pos.x = px;
        pos.y = pyy;
        pos.z = pz;
    }
    ground.SetObjAndPos(pos, this);
    if (ground.DetectClsn() != 0)
        mGroundY = ground.clsnY;

    r3 = 0;
    mStarID = (u8)(param1 & 0xf);
    mState = 0;

    if (kind == 6) {
        void *sp;
        sp = dActor_c::Spawn(ACTOR_STAR, mStarID | 0x60, mPos, 0, (s8)mAreaId, -1);
        if (sp != 0) {
            ((daStar_c *)sp)->mBits.switchStar = 1;
        }
        mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov002_0211093c), 1, 0x18);
        return 0;
    }

    if (kind == 4) {
        mState = 2;
        mBits.visible = 1;
        v4.x = 0;
        v4.y = -0x50000;
        v4.z = 0;
        mdCcAcPos_c.Init(this, v4, 0x50000, 0xa0000, 0x100004, 0);
        mAppearTimer = 0;
    } else if (kind == 5) {
        mState = 3;
        v5.x = 0;
        v5.y = -0x50000;
        v5.z = 0;
        mdCcAcPos_c.Init(this, v5, 0x50000, 0xa0000, 1, 0);
    } else if (kind & 1) {
        mState = 1;
        if (kind & 2) {
            mBits.hold = 1;
        }
        mBits.refresh = 1;
    } else {
        mBits.visible = ((kind >> 1) & 1) ^ 1;
    }

    if (mState != 0) {
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9a8);
        if (mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov002_0211092c), 1, 0x19) == 0) {
            return 0;
        }
    } else {
        if (mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov002_0211093c), 1, 0x18) == 0) {
            return 0;
        }
    }

    if (mShadowModel.InitCylinder() == 0) {
        return 0;
    }

    if (!mBits.visible) {
        mdCcAcPos_c.flags |= 1;
    }
    r3 = 0;
    mSpawnPos.x = mPosX;
    mSpawnPos.y = mPosY;
    mSpawnPos.z = mPosZ;
    mLinkedStarID = r3;
    mLinkedStarDeathTableID = -1;
    mHitActor = 0;

    if (gGameMode == 1)
        r3 = 1;
    if (r3 == 0 && SublevelToLevel((s8)gCurrentSublevel) == 0x1d &&
        IsStarCollectedInCurLevel(mStarID) != 0) {
        MarkForDestruction();
        return 0;
    }
    if (GetBitInDeathTable() != 0) {
        MarkForDestruction();
        return 0;
    }
    return 1;
}

/* Per-frame update. Looks once for an ice block, clears the cached centre, and
 * while a Yoshi has the star in its mouth only rebuilds the matrices.
 * Otherwise it steps the camera cutscene (StepCameraCutscene), runs the
 * handler for mState from data_ov002_021109d8, moves with UpdatePos, rebuilds
 * the model matrix and refreshes the touch volume (not while an ice block
 * holds the star). */
int daStar_c::Behavior()
{
    CheckIceBlock();
    mCenterX = 0;
    mCenterY = 0;
    mCenterZ = 0;

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        int state = mState;
        if (state >= STATE_COLLECT_BEGIN && state <= STATE_COLLECT_TALK && mEatingPlayer != 0) {
            mEatingPlayer->func_ov002_020d718c();
            mEatingPlayer = 0;
            mFlags &= ~0xe0000;
            UpdateModelMatrix();
            mdCc_c.Clear();
            return 1;
        }
        if ((gNextActorUpdateMask & CUTSCENE_FLAG) != 0) {
            if ((int)((mFlags & CUTSCENE_FLAG) != 0) != 0) {
                dActor_c *p = mEatingPlayer;
                if (p != 0)
                    p->mFlags |= CUTSCENE_FLAG;
            }
        }
        UpdateModelMatrix();
        mdCc_c.Clear();
        return 1;
    }

    mEatingPlayer = 0;
    StepCameraCutscene();
    (this->*data_ov002_021109d8[mState])();
    UpdatePos(0);
    UpdateModelMatrix();
    mdCc_c.Clear();
    {
        VecFx32 v;
        v.x = data_ov002_0210aa0c[0];
        v.y = data_ov002_0210aa0c[1];
        v.z = data_ov002_0210aa0c[2];
        mdCc_c.SetPosRelativeToActor(*(Vector3 *)&v);
    }
    if (mInIceBlock == 0)
        mdCc_c.Update();
    UpdateSilverTrailEffect();
    return 1;
}

/* Per-frame update of the marker. mAppearTimer counts down while this marker's
 * star id is the next one in data_0209f344[data_0209f208], and the marker
 * shows itself at 0 unless hold is set. It builds the model and shadow
 * matrices (the model position is mPos >> 3), and when something with hit bits
 * 0x408000 touches a visible marker that is not in state 2, it records the
 * toucher and calls Collect(). */
int daStarBase_c::Behavior()
{
    if (mBits.refresh) {
        if (mStarID == data_0209f344[data_0209f208]) {
            mAppearTimer = 0;
            if (!mBits.hold) {
                mBits.visible = 1;
                mdCcAcPos_c.flags &= ~1;
            }
        } else {
            mAppearTimer = 0x2a;
        }
        mBits.refresh = 0;
    }
    if (mState != 0) {
        if (mAppearTimer != 0) {
            if (!mBits.visible) {
                if (mStarID == data_0209f344[data_0209f208]) {
                    mAppearTimer -= 1;
                    if (mAppearTimer == 0) {
                        if (!mBits.hold) {
                            mBits.visible = 1;
                            mdCcAcPos_c.flags &= ~1;
                        }
                    }
                }
            }
        }
        Matrix4x3_FromTranslation(&mModel.mat4x3, mPosX >> 3, mPosY >> 3,
                                  mPosZ >> 3);
    } else {
        mAngleY += 0x400;
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
        mModel.mat4x3.t.x = mPosX >> 3;
        mModel.mat4x3.t.y = mPosY >> 3;
        mModel.mat4x3.t.z = mPosZ >> 3;
    }
    if (mBits.visible) {
        *(Mtx43 *)&mShadowMtx = *(Mtx43 *)&IDENTITY_MATRIX4X3;
        mShadowMtx.t.x = mPosX >> 3;
        mShadowMtx.t.y = mPosY >> 3;
        mShadowMtx.t.z = mPosZ >> 3;
        {
            int d = mPosY - mGroundY;
            int rad = 0xa0000;
            if (mState != 0)
                rad = 0xc8000;
            ((dActor_c *)this)
                ->DropShadowRadHeight(*(dExtShadowModel_c *)&mShadowModel,
                                      *(Matrix4x3 *)&mShadowMtx, rad, d + 0x28000, 0xf);
        }
    }
    if (mState != 0) {
        if (mBits.visible) {
            /* Both are fields of the dCcAcPos_c at 0x0d4, which the cartridge's own
               ~daStarBase_c names (tools/dtor_members.py): 0x0f8 is +0x24,
               dCc_c::otherOwner, and 0x0f4 is +0x20, dCc_c::hitFlags. */
            if (mState != 2 && mdCcAcPos_c.otherOwner != 0) {
                char *a = (char *)dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
                if (a != 0) {
                    if ((mdCcAcPos_c.hitFlags & 0x408000) != 0) {
                        mHitActor = (dActor_c *)a;
                        Collect();
                        return 1;
                    }
                }
            }
            mdCcAcPos_c.Clear();
            mdCcAcPos_c.Update();
        }
    }
    return 1;
}

/* Draws mAnmModel2 (collected, translucent) or mAnmModel1, unless the star
 * is scaled to nothing, hidden by mFlags & 0x40000, or not visible. */
int daStar_c::Render()
{
    int locked;
    if (mScaleX == 0 || (locked = (mFlags & 0x40000) != 0) || !mBits.visible)
        return 1;
    else {
        if (!mBits.collectedModel)
            mAnmModel1.Render(&mScale);
        else
            mAnmModel2.Render(&mScale);
        return 1;
    }
}

/* Draws the marker's model while it is visible. */
int daStarBase_c::Render()
{
    if (mBits.visible)
        mModel.Render(0);
    return 1;
}

/* Gives up the map marker slot (kind 8 never had one), unloads the silver star
 * and number models when InitResources loaded them (silver star, or kind 6),
 * and releases the four animation files it loaded. */
int daStar_c::CleanupResources()
{
    int b = (actorID == ACTOR_STAR);
    if (b) {
        int v = mKind;
        if (v != 8) {
            if (v == 6)
                UnloadSilverStarAndNumber();
            UntrackStar(mMarkerSlot);
        }
    } else {
        UntrackStar(mMarkerSlot);
        UnloadSilverStarAndNumber();
    }
    ((SharedFilePtr *)(&data_ov002_02110944))->Release();
    ((SharedFilePtr *)(&data_ov002_02110924))->Release();
    ((SharedFilePtr *)(&data_ov002_02110964))->Release();
    ((SharedFilePtr *)(&data_ov002_02110934))->Release();
    return 1;
}

/* Releases the model files that InitResources loaded for this marker's
 * flavour. */
int daStarBase_c::CleanupResources()
{
    if (mState != 0) {
        ((SharedFilePtr *)(&data_ov002_0211092c))->Release();
        data_ov002_0210d9a8.Release();
    } else {
        ((SharedFilePtr *)(&data_ov002_0211093c))->Release();
    }
    return 1;
}

/* If the linked star exists but has no death-table slot of its own
 * (mDeathTableID < 0), clears the death-table bit this marker remembered for
 * it. */
void daStarBase_c::OnPendingDestroy()
{
    dActor_c *star = dActor_c::FindWithID(mLinkedStarID);
    if (star == 0) return;
    if (star->mDeathTableID >= 0) return;
    DeathTable_ClearBit(mLinkedStarDeathTableID);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 67 -- StateLaunch, 0x020ea9d0, size 0x1bc */
/* -------------------------------------------------------------------------- */
/* STATE_LAUNCH: starts the pop-out. A star with a non-zero id first looks for
 * its marker and destroys itself without one. It then starts the cutscene
 * (mFlags and gNextActorUpdateMask get CUTSCENE_FLAG, mCamSeq 0, sound 3/0x57), jumps
 * at 32.0 with the default gravity and picks the next State: kind 4 aims a hop
 * at 200.0 above its marker (AimArcTo; arc value 100.0 on sublevel
 * 0x11, 70.0 for star id 3 on sublevel 0xb, 400.0 otherwise) and flies there,
 * or lands at once when no marker is set or it is already above it; every
 * other kind rises. */
void daStar_c::StateLaunch() {
    dActor_c *other;
    s32 area;
    struct Vector3 *op;
    struct Vector3 v0;
    struct Vector3 v1;
    struct Vector3 v2;
    struct Vector3 v3;

    if (mStarID != 0) {
        FindMarker();
        if (mMarkerID == 0) {
            MarkForDestruction();
            return;
        }
    }
    mFlags |= CUTSCENE_FLAG;
    gNextActorUpdateMask |= CUTSCENE_FLAG;
    mCamSeq = 0;
    func_02012694(0x57, &mCamSpacePosX);
    mVertSpeed = 0x20000;
    SetDefaultGravity();
    other = dActor_c::FindWithID(mMarkerID);
    if (mKind == KIND_4) {
        if (other == 0 || Vec3_HorzDist(&mPos, &other->mPos) == 0) {
            mVertSpeed = 0x18000;
            mState = STATE_LAND;
        } else {
            mState = STATE_FLY_TO_MARKER;
            op = &other->mPos;
            *(struct Vec1 *)&v0.x = *(struct Vec1 *)&op->x;
            *(struct Vec1 *)&v0.y = *(struct Vec1 *)&op->y;
            *(struct Vec1 *)&v0.z = *(struct Vec1 *)&op->z;
            v0.y = v0.y + 0xc8000;
            area = gCurrentSublevel;
            if (area == 0x11) {
                v1.x = v0.x;
                v1.y = v0.y;
                v1.z = v0.z;
                AimArcTo(&v1, 0x64000);
            } else if (area == 0xb && mStarID == 3) {
                v2.x = v0.x;
                v2.y = v0.y;
                v2.z = v0.z;
                AimArcTo(&v2, 0x46000);
            } else {
                v3.x = v0.x;
                v3.y = v0.y;
                v3.z = v0.z;
                AimArcTo(&v3, 0x190000);
            }
        }
    } else {
        mState = STATE_RISE;
    }
    MaybeAddStarMarker();
    SpawnSoundObj6();
    ProbeGround();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 66 -- StateRise, 0x020ea90c, size 0xc4 */
/* -------------------------------------------------------------------------- */
/* STATE_RISE: once the vertical speed has reached -32.0 or lower it looks for its
 * marker. With no marker, or one directly below, it becomes STATE_LAND with an
 * upward speed of 24.0; otherwise it aims a hop at 200.0 above the marker
 * (AimArcTo, arc value 400.0) and becomes STATE_FLY_TO_MARKER. The
 * trail effect, sound object 6 and the ground probe run every frame. */
void daStar_c::StateRise() {
    if (mVertSpeed <= -0x20000) {
        dActor_c *other = dActor_c::FindWithID(mMarkerID);
        if (other == 0 || Vec3_HorzDist(&mPos, &other->mPos) == 0) {
            mVertSpeed = 0x18000;
            mState = STATE_LAND;
        } else {
            Vector3 *pp = &other->mPos;
            Vector3 v;
            int yv;
            v.x = pp->x;
            yv = pp->y;
            v.y = yv;
            v.z = pp->z;
            v.y = yv + 0xc8000;
            /* By-value staging: the ROM caller copies v to its outgoing area
             * and passes the copy's address (r1 = sp+0xc at the bl). The TU
             * cannot spell the shard's by-value declaration (it collides with
             * the pointer form the other callers need), and copying the real
             * Vector3 would emit its declared-destructor cleanup, so the copy
             * is staged word-wise into plain ints -- proven byte-identical
             * in isolation. */
            int w[3];
            w[0] = v.x;
            w[1] = v.y;
            w[2] = v.z;
            AimArcTo((Vector3 *)w, 0x190000);
            mState = STATE_FLY_TO_MARKER;
        }
    }
    UpdateTrailEffect();
    SpawnSoundObj6();
    ProbeGround();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 65 -- StateFlyToMarker, 0x020ea824, size 0xe8 */
/* -------------------------------------------------------------------------- */
/* STATE_FLY_TO_MARKER: without a marker the star destroys itself. Once the
 * marker is closer than one frame of horizontal travel (mHorzSpeed) it snaps
 * to 200.0 above it, stops horizontally, bounces up at 16.0 (star id 3 on
 * sublevel 0xb) or 24.0 and becomes STATE_LAND. */
void daStar_c::StateFlyToMarker() {
    dActor_c *o = dActor_c::FindWithID(mMarkerID);
    if (o == 0) { MarkForDestruction(); return; }
    if (Vec3_HorzDist(&mPos, &o->mPos) < mHorzSpeed) {
        int* src = (int*)&o->mPosX;
        int* yp = (int*)&mPosY;
        mPosX = src[0];
        mPosY = src[1];
        mPosZ = src[2];
        *yp += 0xc8000;
        mHorzSpeed = 0;
        mVertSpeed = 0x18000;
        if (gCurrentSublevel == 0xb && mStarID == 3)
            mVertSpeed = 0x10000;
        else
            mVertSpeed = 0x18000;
        SetDefaultGravity();
        mState = STATE_LAND;
    }
    UpdateTrailEffect();
    SpawnSoundObj6();
    ProbeGround();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 64 -- StateLand, 0x020ea7ac, size 0x78 */
/* -------------------------------------------------------------------------- */
/* STATE_LAND: while it is falling at 24.0 per frame or faster the motion
 * is zeroed and the touch volume is enabled. When gravity is zero it becomes
 * STATE_IDLE and sets mCamSeq to 0x1d6; the cutscene camera then counts up and
 * restores the saved view at step 0x1f4. */
void daStar_c::StateLand() {
    if (mVertSpeed <= -0x18000) {
        StopMotion();
        mdCc_c.flags &= ~1;
    } else {
        if (mVertAccel == 0) {
            mState = STATE_IDLE;
            mCamSeq = 0x1d6;
        } else {
            SpawnSoundObj6();
        }
    }
    UpdateTrailEffect();
    ProbeGround();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 63 -- StateIdle, 0x020ea420, size 0x38c */
/* -------------------------------------------------------------------------- */
/* STATE_IDLE: the star waits to be collected. Kind 6 is the star that appears
 * and disappears. While it is enabled (five VS stars taken, or its switch on
 * for the switch star) it grows back to full scale, with the camera cutscene,
 * sound 0x41 and a map marker; otherwise it shrinks to nothing with sound 0x42
 * and gives the map marker up. Other kinds: the silver star bounces back up
 * (16.0) whenever it falls to its home height, and the Power Star re-arms
 * sound object 6. The pickup test (CheckPickup) runs last. */
void daStar_c::StateIdle() {
    int spd;
    int spd2;
    int en;
    int lim;
    int step;

    if (mKind == KIND_6) {
        en = 0;
        if (!mBits.switchStar && NumVsStarsObtained() == 5) {
            lim = 0x14;
            step = 0x100;
            en = 1;
        } else {
            if (mBits.switchStar != 0 && mBits.switchOn != 0) {
                lim = 5;
                step = 0x200;
                en = 1;
            }
        }
        if (en != 0) {
            if (mScaleX != 0x1000 && (gNextActorUpdateMask & CUTSCENE_FLAG) == 0) {
                mFlags |= CUTSCENE_FLAG;
                gNextActorUpdateMask |= CUTSCENE_FLAG;
            } else if (mScaleX == 0x1000) {
                mAppearTimer = lim + 0xb;
                mBits.appeared = 1;
                AddStarMarker();
                mdCc_c.flags &= ~1;
            }
            if (mAppearTimer < (unsigned int)lim) {
                mAppearTimer += 1;
                if (mAppearTimer == lim) {
                    if (mCamSeq == 0xffff)
                        mCamSeq = 0x64;
                }
            } else if (mScaleX != 0x1000) {
                spd = mScaleX;
                if (mAppearTimer >= lim + 0xa) {
                    if (ApproachLinear(spd, 0x1000, step) != 0) {
                        AddStarMarker();
                        mdCc_c.flags &= ~1;
                    }
                } else {
                    mAppearTimer += 1;
                }
                {
                    int v = spd;
                    mScaleX = v;
                    mScaleY = v;
                    mScaleZ = v;
                }
            } else {
                if (!mBits.switchStar)
                    mAppearTimer = 0x3d;
                else
                    mAppearTimer = 0;
            }
            if (mAppearTimer >= lim + 0xa &&
                !mBits.appeared) {
                mBits.appeared = 1;
                func_02012790(0x41);
            }
            mStateTimer = 0;
        } else {
            mdCc_c.flags |= 1;
            if (mAppearTimer != 0) {
                mAppearTimer -= 1;
            } else if (mScaleX != 0) {
                int step2;
                if ((u16)mStateTimer == 0)
                    func_02012790(0x42);
                spd2 = mScaleX;
                if (*(u16 *)&mStateTimer <= 0xf) {
                    *(u16 *)&mStateTimer += 1;
                    step2 = 0;
                } else {
                    step2 = 0x100;
                }
                if (ApproachLinear(spd2, 0, step2) != 0) {
                    mBits.appeared = 0;
                    UntrackStar(mMarkerSlot);
                }
                {
                    int v2 = spd2;
                    mScaleX = v2;
                    mScaleY = v2;
                    mScaleZ = v2;
                }
                if ((gNextActorUpdateMask & CUTSCENE_FLAG) == 0) {
                    mFlags |= CUTSCENE_FLAG;
                    gNextActorUpdateMask |= CUTSCENE_FLAG;
                    mCamSeq = 0x64;
                }
            }
        }
    } else {
        int t = actorID;
        t = t == ACTOR_SILVER_STAR;
        if (t != false) {
            if (mPosY <= mHomePosY) {
                mPosY = mHomePosY;
                mVertSpeed = 0x10000;
            }
        } else {
            RearmSoundObj6();
        }
    }
    CheckPickup();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 62 -- StateTouched, 0x020ea410, size 0x10 */
/* -------------------------------------------------------------------------- */
/* STATE_TOUCHED: hands mPlayer to Collect. */
/* StateTouched @ 0x20ea410 (ov002) -- veneer: ldr r1,[r0,#0x438]; b Collect. */
void daStar_c::StateTouched() {
    Collect((void *)mPlayer);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 61 -- PickCollectSoundID, 0x020ea3a4, size 0x6c */
/* -------------------------------------------------------------------------- */
/* Picks the sound id for the collection sound object into mSoundObjSoundID and
 * returns it: for the silver star gNumStageStars[player number] + 0x19, in a VS
 * match 0x4f, otherwise 0x22. */
int daStar_c::PickCollectSoundID() {
    int b = (actorID == ACTOR_SILVER_STAR);
    if (b != 0) {
        unsigned char idx = mPlayer->mPlayerNo;
        mSoundObjSoundID = gNumStageStars[idx] + 0x19;
    } else {
        int b2 = (gGameMode == 1);
        if (b2 != 0) mSoundObjSoundID = 0x4f;
        else mSoundObjSoundID = 0x22;
    }
    return mSoundObjSoundID;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 60 -- StateCollectBegin, 0x020ea100, size 0x2a4 */
/* -------------------------------------------------------------------------- */
/* STATE_COLLECT_BEGIN: the player has touched the star. PickTalkState
 * gives the talk state and daPly_c::Unk_020c9e5c asks whether the player is in
 * the no-control state of that kind. If it is, the sound object is spawned (mode 1 for talk states 1 and 2,
 * mode 2 for 0), the star jumps to 30.0 above the player, is shown at scale
 * 1.0 (3.0 for a mega player), and plays the pose animation (another one under
 * water); it becomes STATE_COLLECT_HOLD. If the player refuses and
 * func_ov002_020ca0f4 says so, the star hovers 200.0 above the player;
 * otherwise, in a VS match, it takes one star off the player's VS count
 * (GiveVsStars -1, when the count is not zero) and bounces away. */
extern "C" {
extern int func_ov002_020ca0f4(void *player);


}

void daStar_c::StateCollectBegin() {
    daPly_c *common;
    u8 state;

    common = mPlayer;
    state = PickTalkState();

    if (common->Unk_020c9e5c(state)) {
        if ((int)state == 1 || (int)state == 2) {
            SpawnCollectSoundObj(0x14);
            mSoundObjMode = 1;
        } else if (state != 3) {
            SpawnCollectSoundObj(0);
            mSoundObjMode = 2;
        }

        int *posY = (int *)&mPosY;

        mState = STATE_COLLECT_HOLD;
        mBits.collectedModel = 0;
        mBits.visible = 1;
        mSeqTimer = 0;

        {
            int *src = (int *)&mPlayer->mPosX;
            int *cache = (int *)&mHomePosX;
            mHomePosX = src[0];
            mHomePosY = src[1];
            mHomePosZ = src[2];
            mPosX = cache[0];
            mPosY = cache[1];
            mPosZ = cache[2];
            *posY += 0x1e000;
        }

        mPrevAngleY = mPlayer->mAngleY;
        mAngleX = 0;

        if (common->mIsMega != 0) {
            mScaleX = 0x3000;
            mScaleY = 0x3000;
            mScaleZ = 0x3000;
        } else {
            mScaleX = 0x1000;
            mScaleY = 0x1000;
            mScaleZ = 0x1000;
        }

        StopMotion();

        if (common->mIsUnderwater != 0) {
            mAnmModel1.SetAnim((BCA_File *)data_ov002_02110924.filePtr, 0x40000000, 0x1000, 0);
        } else {
            mAnmModel1.SetAnim((BCA_File *)data_ov002_02110944.filePtr, 0x40000000, 0x1000, 0);
        }

        {
            mBits.noSpin = 1;
            mShadowModel.~dExtShadowModel_c();
        }

        if (mKind == KIND_9) {
            Sound::ChangeMusicVolume(0, 0x7f000);
        }
    } else {
        if (func_ov002_020ca0f4(common) != 0) {
            int *posY2 = (int *)&mPosY;
            int *s = (int *)&mPlayer->mPosX;
            mPosX = s[0];
            mPosY = s[1];
            mPosZ = s[2];
            *posY2 += 0xc8000;
        } else {
            int ok = (gGameMode == 1);
            if (ok) {
                unsigned char idx = common->mPlayerNo;
                if (gNumStageStars[idx] != 0) {
                    GiveVsStars(idx, -1);
                }
            }
            mState = STATE_BOUNCE;
            mVertSpeed = 0x20000;
            mHorzSpeed = 0xc000;
            SetDefaultGravity();
            mdCc_c.flags &= ~1;
        }
    }

    mParticle[1] = 0;
    mParticle[0] = mParticle[1];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 59 -- StateCollectHold, 0x020ea06c, size 0x94 */
/* -------------------------------------------------------------------------- */
/* STATE_COLLECT_HOLD: copies the player's position and facing, starts the pose
 * animation and becomes STATE_COLLECT_TALK. */
void daStar_c::StateCollectHold() {
    int *s = (int *)&mPlayer->mPosX;
    mPosX = s[0];
    mPosY = s[1];
    mPosZ = s[2];
    mPrevAngleY = mPlayer->mAngleY;
    mState = STATE_COLLECT_TALK;
    mSeqTimer = 0;
    mTalkStep = 0;
    mAnmModel1.Advance();
    SpawnCollectEffects();
    if (mKind == KIND_9) {
        Sound::ChangeMusicVolume(0, 0x7f000);
    }
    ++mMusicTimer;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 58 -- StateCollectTalk, 0x020e9d18, size 0x354 */
/* -------------------------------------------------------------------------- */
/* STATE_COLLECT_TALK: the pose animation plays while the star follows the
 * player (kind 9 keeps adjusting the music volume for 0x78 frames). In a VS
 * match and for the silver star it counts mSeqTimer once the animation has
 * finished. At frame 1 (silver star outside VS, player's star count 4) or
 * frame 5 it gives out VS stars, spawns the score popup at the centre
 * (func_ov002_020e8244) and finishes the collection. For the Power Star
 * outside VS, star id 0, kind 9 and the levels func_ov002_020e9630 accepts
 * read the answer to the message into mBits.answer (sound 0x57 for answer 1,
 * 0x5c for 2) and, once daPly_c::Unk_020c9e5c no longer reports the player in
 * that no-control state, move to STATE_SAVE_MESSAGE. Other stars only keep
 * advancing the animation here, except a marker-placed star (home state 9) and
 * star id 8, which call FinishCollection (it finishes once the animation
 * has ended). */
void daStar_c::StateCollectTalk() {
    u32 r2v;
    u32 b2;
    bool st;
    VecFx32 t1;
    VecFx32 t2;
    int *src;
    u8 st2;

    src = (int *)&mPlayer->mPosX;
    mPosX = src[0];
    mPosY = src[1];
    mPosZ = src[2];
    mPrevAngleY = mPlayer->mAngleY;
    if (mKind == KIND_9) {
        if (mMusicTimer < 0x78) {
            Sound::ChangeMusicVolume(0, 0x7f000);
            mMusicTimer++;
        } else {
            Sound::ChangeMusicVolume(0x40, 0xcb33);
        }
    }
    if (mAnmModel1.Finished() != 0) {
        mSeqTimer++;
    } else {
        func_ov002_020e7eb4();
    }
    r2v = gGameMode == 1;
    if (r2v != false || (b2 = (actorID == ACTOR_SILVER_STAR)) != 0) {
        u32 tmp;
        u16 mode;
        tmp = actorID;
        tmp = tmp == ACTOR_SILVER_STAR;
        if (tmp != false)
            st = 1;
        else
            st = 0;
        mode = mSeqTimer;
        if (mode == 1 && r2v == 0) {
            int idx = mPlayer->mPlayerNo;
            if (gNumStageStars[idx] == 4) {
                GiveVsStars(idx, 1);
                func_ov002_020e8244(&t1, this);
                SpawnNumber(*(Vector3 *)&t1, 5, st, 0, mPlayer);
                FinishCollection();
            }
        } else if (mode == 5) {
            int idx2 = mPlayer->mPlayerNo;
            if (!(gNumStageStars[idx2] == 5 && r2v == false)) {
                if (r2v == false)
                    GiveVsStars(idx2, 1);
                func_ov002_020e8244(&t2, this);
                SpawnNumber(*(Vector3 *)&t2, gNumStageStars[mPlayer->mPlayerNo], st, 0, mPlayer);
                FinishCollection();
            }
        }
    } else {
        if (mKind == KIND_9) {
            st2 = 0x11;
LA:
        if (mBits.answer == 0) {
            *(u16 *)((int)&mStarFlags) =
                (*(u16 *)((int)&mStarFlags) & ~0xc00) | ((data_0209d684 & 3) << 10);
            {
                u32 b3 = mBits.answer;
                if (b3 == 1) {
                    func_02012790(0x57);
                } else if (b3 != 2) {
                    mBits.answer = 0;
                } else {
                    func_02012790(0x5c);
                }
            }
        }
        mSeqTimer = 0xa;
        if (mPlayer->Unk_020c9e5c(st2) == 0) {
            mTalkStep = 2;
            mState = STATE_SAVE_MESSAGE;
            mSeqTimer = 0;
            mPlayer->mFlags &= ~CUTSCENE_FLAG;
            KuppaScript::End();
        }
        mAnmModel1.Advance();
        SpawnCollectEffects();
        return;
        }
        if (mStarID == 0 || func_ov002_020e9630() != 0) {
            st2 = 1;
            // goto: or-chain, comma-assign, st2 flag, do/while, switch, duplicated body still DIFF _ZN8daStar_c16StateCollectTalkEv
            goto LA;
        }
        if (mHomeState == STATE_WAIT_MARKER || mStarID == 8)
            FinishCollection();
    }
    mAnmModel1.Advance();
    SpawnCollectEffects();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 57 -- StateSaveMessage, 0x020e9af4, size 0x224 */
/* -------------------------------------------------------------------------- */
/* STATE_SAVE_MESSAGE: walks the save prompt one mTalkStep at a time. Step 2
 * waits for the talk to end (answer 1 shows saving message 0x295 and goes to
 * 3, answer 2 ends the talk and skips to 4), 3 and 5 wait for the message box
 * to close (data_0209d660), 4 asks func_ov002_020c6e14 whether there is more
 * to say, and 6 hides the star and finishes the collection. Kind 9 keeps
 * adjusting the music. */
void daStar_c::StateSaveMessage() {
    if (mKind == KIND_9) {
        if (mMusicTimer < 0x78) {
            Sound::ChangeMusicVolume(0, 0x7f000);
            mMusicTimer++;
        } else {
            Sound::ChangeMusicVolume(0x40, 0xcb33);
        }
    }
    mPrevAngleY = mPlayer->mAngleY;
    switch (mTalkStep) {
    case 2:
        if (mPlayer->GetTalkState() == -1) {
            u32 t = mBits.answer;
            if (t == 1) {
                Message::DisplaySaving(0x295);
                mTalkStep++;
                mPlayer->mStateFlags |= 0x800;
            } else if (t == 2) {
                mTalkStep += 2;
                Message::EndTalk();
            }
        }
        break;
    case 3:
        if (data_0209d660 == 0) {
            mTalkStep++;
        }
        break;
    case 4:
        if (mPlayer->func_ov002_020c6e14() != 0) {
            mTalkStep++;
        } else {
            mTalkStep += 2;
        }
        break;
    case 5:
        if (data_0209d660 == 0) {
            Message::EndTalk();
            mTalkStep++;
        }
        break;
    case 6:
        mPlayer->mStateFlags &= ~0x800;
        mBits.visible = 0;
        FinishCollection();
        break;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 56 -- StateBounce, 0x020e99e8, size 0x10c */
/* -------------------------------------------------------------------------- */
/* STATE_BOUNCE: the star hops about on the ground. Checks the bounds, moves
 * against the mesh collider, reflects off walls, handles water, and bounces at
 * 14.0 in water or 23.0 on land after each ground contact. The pickup test
 * runs once mStateTimer has counted out, unless the no-pickup flag is set. */
void daStar_c::StateBounce() {
    char *c = (char *)this;
    CheckBounds();
    mPrevPosY = mPosY;
    UpdateWMClsn(mWithMeshClsn, 2);
    if (mWithMeshClsn.IsOnWall()) {
        mPrevAngleY = ((dActor_c *)c)->ReflectAngle(mWallNormalX, mWallNormalZ, mPrevAngleY);
    }
    UpdateWater();
    if (mWithMeshClsn.JustHitGround()) {
        OnBounceLand();
    } else if (mWithMeshClsn.IsOnGround()) {
        mPrevAngleY = ((dActor_c *)c)->ReflectAngle(mWallNormalX, mWallNormalZ, mPrevAngleY);
        if (mBits.water == WATER_IN)
            mVertSpeed = 0xe000;
        else
            mVertSpeed = 0x17000;
    }
    {
        unsigned short *ctr = (unsigned short *)&mStateTimer;
        if (*ctr == 0) {
            if (!mBits.noPickup)
                CheckPickup();
        } else {
            *ctr = *ctr - 1;
        }
    }
    ProbeGround();
    UpdateSparkleEffect();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 55 -- StateWaitMarker, 0x020e9840, size 0x1a8 */
/* -------------------------------------------------------------------------- */
/* STATE_WAIT_MARKER: the star sits hidden until its marker says so. With no
 * marker it looks for one. Once it is this star's turn
 * (data_0209f344[data_0209f208] == mStarID) and the marker's mAppearTimer has
 * run out it appears (sound 3/0x54, map marker). When the marker is on hold
 * the star pops out (32.0 up) into STATE_BOUNCE; in a VS match with a toucher
 * recorded it also leaves at 12.0, facing away from that player's camera. */
void daStar_c::StateWaitMarker() {
    daStarBase_c *actor;

    actor = (daStarBase_c *)dActor_c::FindWithID(mMarkerID);
    if (actor == 0) {
        FindMarker();
        return;
    }
    if (actor == 0) return;

    if (!mBits.visible) {
        if (mStarID != data_0209f344[data_0209f208]) return;
        if (actor->mAppearTimer != 0) return;

        mBits.visible = 1;
        MaybeAddStarMarker();
        func_02012694(0x54, &mCamSpacePos);
        return;
    }

    if (!actor->mBits.hold) return;

    mBits.noPickup = 0;
    mStateTimer = 0xf;
    mState = STATE_BOUNCE;
    mVertSpeed = 0x20000;
    SetDefaultGravity();

    mdCc_c.flags &= ~1;

    {
        int r0 = (gGameMode == 1) ? 1 : 0;
        daPly_c *r3 = static_cast<daPly_c *>(actor->mHitActor);
        if (r0 == 0) return;
        if (r3 == 0) return;

        mBits.noPickup = 1;
        mHorzSpeed = 0xc000;
        mPrevAngleY = GetAngleToCamera(r3->mPlayerNo) + 0x8000;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 54 -- StatePlayAndEnd, 0x020e9804, size 0x3c */
/* -------------------------------------------------------------------------- */
/* STATE_PLAY_AND_END: plays the animation with its effects, then destroys the
 * star. */
void daStar_c::StatePlayAndEnd() {
    mAnmModel1.Advance();
    SpawnPoseEffects();
    if (!mAnmModel1.Finished())
        return;
    MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- StateSpinAndEnd, 0x020e96a0, size 0x164 */
/* -------------------------------------------------------------------------- */
/* STATE_SPIN_AND_END: the silver star taken on a shell. Spins by 0x800 per
 * frame. For 30 frames it hovers 260.0 above the player; at frame 30 it awards
 * the VS star, spawns the score popup and gives its map marker up, then hides;
 * at frame 100 it stops the sound object and destroys itself (VS match) or
 * records itself in the death table. */
void daStar_c::StateSpinAndEnd() {
    int v[3];
    char *p;
    int *src;
    unsigned short t;

    mSeqTimer += 1;
    mAngleY += 0x800;
    t = mSeqTimer;
    if (t >= 0x1e) {
        if (t == 0x1e) {
            GiveVsStars(mPlayer->mPlayerNo, 1);
            func_ov002_020e8244(v, this);
            p = (char *)mPlayer;
            SpawnNumber(*(Vector3 *)v, (unsigned int)gNumStageStars[mPlayer->mPlayerNo], 1, 0,
                        (dActor_c *)p);
            UntrackStar(mMarkerSlot);
        }
        mBits.visible = 0;
        if (mSeqTimer < 0x64) return;
        StopCollectSoundObj();
        if ((int)(gGameMode == 1) != 0) {
            MarkForDestruction();
        } else {
            KillAndTrackInDeathTable();
        }
    } else {
        src = (int *)&mPlayer->mPosX;
        mPosX = src[0];
        mPosY = src[1];
        mPosZ = src[2];
        mPosY += 0x104000;
        SpawnCollectEffects();
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 52 -- func_ov002_020e9630, 0x020e9630, size 0x70 */
/* -------------------------------------------------------------------------- */
/* True on levels 0xf-0x14 and 0x1d, where a collected Power Star starts a
 * talk. */
int daStar_c::func_ov002_020e9630() {
    int lv = SublevelToLevel(gCurrentSublevel);
    if (lv == 0xf || lv == 0x10 || lv == 0x11 || lv == 0x12 ||
        lv == 0x13 || lv == 0x14 || lv == 0x1d)
        return 1;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- FindMarker, 0x020e9590, size 0xa0 */
/* -------------------------------------------------------------------------- */
/* Finds a marker for the star when it has none: the first STAR_MARKER with the
 * same star id that is in state 0 (or, for a star placed by a marker, in a
 * non-zero state). Links both ways. */
void daStar_c::FindMarker() {
    daStarBase_c *found;
    if (dActor_c::FindWithID(mMarkerID))
        return;
    found = 0;
    for (;;) {
        found = (daStarBase_c *)dActor_c::FindWithActorID(ACTOR_STAR_MARKER, found);
        if (!found)
            return;
        if (mStarID == found->mStarID) {
            int v444 = mHomeState;
            if (v444 == 9 && found->mState)
                break;
            if (v444 == 9)
                continue;
            if (!found->mState)
                break;
        }
    }
    mMarkerID = found->uniqueID;
    found->LinkSilverStarAndStarMarker((char *)this);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- AimArcTo, 0x020e947c, size 0x114 */
/* -------------------------------------------------------------------------- */
/* Aims a hop at *p: derives mVertAccel and mVertSpeed from the height
 * difference to p and the arc value n, spreads the horizontal distance over 50
 * frames (mHorzSpeed), sets the terminal speed to -50.0 and turns the star
 * toward p. */
void daStar_c::AimArcTo(Vector3 *p, int n) {
    int dy = ((int *)p)[1] - mPosY;
    if (dy < 0)
        dy = -dy;
    {
        int s0 = cstd::sqrt(n);
        int s1 = cstd::sqrt(n + dy);
        int s2 = cstd::sqrt(n);
        dy = (s0 * 0x32) / (s1 + s2);
    }
    {
        int left = -(n << 1);
        int den = dy * dy;
        n = 0x32 - dy;
        mVertAccel = left / den;
    }
    if (((int *)p)[1] >= mPosY) {
        int v = mVertAccel;
        if (v < 0)
            v = -v;
        mVertSpeed = n * v;
    } else {
        int v = mVertAccel;
        if (v < 0)
            v = -v;
        mVertSpeed = (dy + 1) * v;
    }
    mTerminalVelocity = -0x32000;
    mHorzSpeed = Vec3_HorzDist(&mPos, p) / 50;
    mPrevAngleY = Vec3_HorzAngle(&mPos, p);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- StopMotion, 0x020e9464, size 0x18 */
/* -------------------------------------------------------------------------- */
/* Stops the star: vertical speed, gravity, terminal speed and horizontal speed
 * all zero. */
void daStar_c::StopMotion() {
    mVertSpeed = 0;
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mHorzSpeed = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- SetDefaultGravity, 0x020e9448, size 0x1c */
/* -------------------------------------------------------------------------- */
/* Default gravity: -1.375 per frame squared, terminal speed -32.0. */
void daStar_c::SetDefaultGravity() {
    mVertAccel = -0x1600;
    mTerminalVelocity = -0x20000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- CheckPickup, 0x020e930c, size 0x13c */
/* -------------------------------------------------------------------------- */
/* The pickup test. Reads the touch volume's toucher; hit bit 0x400000 needs a
 * player that is not in a no-control state, event bit 0x1e clear, and
 * Collect accepting the collection, bit 0x8000 only the first two.
 * A kind 6 star with a live marker then calls the marker's Collect. */
void daStar_c::CheckPickup() {
    void* o;
    daPly_c *b;
    int flags;
    unsigned int id;

    id = mdCc_c.otherOwner;
    if (id == 0) return;
    o = dActor_c::FindWithID(id);
    if (o == 0) return;
    b = (daPly_c *)o;

    flags = mdCc_c.hitFlags;
    if (flags & 0x400000) {
        if (b->mIsNoControl != 0) return;
        if (Event::GetBit(0x1e) != 0)
            return;
        if (Collect(o) == 0)
            return;
        if (mKind != KIND_6) return;
        o = dActor_c::FindWithID(mMarkerID);
        if (o == 0)
            return;
        ((daStarBase_c *)o)->Collect();
    } else {
        if (flags & 0x8000) {
            if (b->mIsNoControl != 0) return;
            if (Event::GetBit(0x1e) != 0)
                return;
            if (mKind != KIND_6) return;
            o = dActor_c::FindWithID(mMarkerID);
            if (o == 0)
                return;
            ((daStarBase_c *)o)->Collect();
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- Collect, 0x020e8ef0, size 0x41c */
/* -------------------------------------------------------------------------- */
/* Collects the star for player p. Unlinks the marker and moves to
 * STATE_TOUCHED. A silver star taken on a shell goes to STATE_SPIN_AND_END.
 * Otherwise the player is put in the no-control state for the talk state
 * PickTalkState gives (with message 0x186 for star id 0 or 0x187 for the
 * levels func_ov002_020e9630 accepts), and a Power Star outside VS records the
 * collection (event bits 0x1e and 0x1d, CollectStarInCurLevel, the coin
 * record). The star plays sound 0x2d, enters STATE_COLLECT_BEGIN with the
 * cutscene bits set, and a Power Star with a non-zero talk state heals the
 * player (0x880). Returns 0 when the player cannot be put in the state. */
int daStar_c::Collect(void *p) {
    void* found;
    int r5;
    u8 r4;
    int sb;

    mState = STATE_TOUCHED;
    found = dActor_c::FindWithID(mMarkerID);
    if (found) {
        ((daStarBase_c *)((char *)found))->LinkSilverStarAndStarMarker((char *)0);
    }
    mPlayer = (daPly_c *)p;

    if (((daPly_c *)p)->IsOnShell() != 0) {
        int b = (actorID == ACTOR_SILVER_STAR);
        if (b) {
            mState = STATE_SPIN_AND_END;
            mBits.collectedModel = 0;
            mSeqTimer = 0;
            mParticle[1] = 0;
            mParticle[0] = mParticle[1];
            func_02012790(0x2d);
            mdCc_c.flags |= 1;
            mdCc_c.Clear();
            SpawnCollectSoundObj(0x14);
            mSoundObjMode = 1;
            return 1;
        }
    }

    r5 = 0;
    r4 = PickTalkState();
    {
        int t1;
        int t2;
        if (r4 != 0 && !(t1 = (gGameMode == 1)) && !(t2 = (actorID == ACTOR_SILVER_STAR)) &&
            mHomeState != STATE_WAIT_MARKER) {
            if (mStarID == 0) {
                sb = ((daPly_c *)p)->SetNoControlState(r4, 0x186, 0);
                Message::PrepareTalk();
                r5 = 1;
            } else if (func_ov002_020e9630() != 0) {
                sb = ((daPly_c *)p)->SetNoControlState(r4, 0x187, 0);
                Message::PrepareTalk();
                r5 = 1;
            } else {
                sb = ((daPly_c *)p)->SetNoControlState(r4, -1, 0);
            }
        } else {
            sb = ((daPly_c *)p)->SetNoControlState(r4, -1, 0);
        }
    }

    if (sb != 0) {
        {
            int b1 = (gGameMode == 1);
            if (!b1) {
                Event::SetBit(0x1e);
            } else {
                GiveVsStars(((daPly_c *)p)->mPlayerNo, 1);
            }
        }
        if (r4 == 0) {
            Event::SetBit(EVENT_COLLECTING_STAR);
        }

        {
            int b2 = (gGameMode == 1);
            int b3;
            if (!b2 && !(b3 = (actorID == ACTOR_SILVER_STAR)) && mStarID < 8 &&
                mHomeState != STATE_WAIT_MARKER) {
                gCollectedStarId = mStarID;
                if (IsStarCollectedInCurLevel(mStarID) != 0) {
                    gNewStarCollected = 0;
                } else {
                    gNewStarCollected = 1;
                }
                CollectStarInCurLevel(mStarID);
                if (r5 != 0) {
                    int lvl;
                    if (mBits.coinReward != 0 && found != 0) {
                        ((dActor_c *)found)->TrackInDeathTable();
                    }
                    lvl = SublevelToLevel((signed char)gCurrentSublevel);
                    if (lvl <= 0xe) {
                        int rec = SaveData::GetCoinRecord(lvl);
                        if (rec < NumCoins()) {
                            SaveData::SetCoinRecordIfHigher(
                                lvl, (u8)(gNumCoins[((daPly_c *)p)->mPlayerNo] & 0xff));
                        }
                    }
                }
            }
        }

        if (mHomeState == STATE_WAIT_MARKER) {
            data_0209f208++;
        }
        func_02012790(0x2d);
        mState = STATE_COLLECT_BEGIN;
        mPlayer = (daPly_c *)p;
        {
            int b4 = (gGameMode == 1);
            if (!b4) {
                mPlayer->mFlags |= CUTSCENE_FLAG;
                mFlags |= CUTSCENE_FLAG;
                gNextActorUpdateMask |= CUTSCENE_FLAG;
            }
        }
        mdCc_c.flags |= 1;
        mdCc_c.Clear();
        {
            int b5 = (actorID == ACTOR_STAR);
            if (b5 && r4 != 0) {
                ((daPly_c *)p)->Heal(0x880);
            }
        }
        StopMotion();
        data_0209d684 = 0;
        return 1;
    }
    return 0;
}

s32 daStar_c::OnYoshiTryEat() {
    return 4;
}

/* daStar_c::OnTurnIntoEgg -- vtable slot 19, verified against ov002 relocs.txt:
 * _ZTV8daStar_c (0x0210ab3c) + 0x4c -> 0x020e8edc, exactly this placeholder's
 * former address (former name func_ov002_020e8edc). The ROM body is a
 * tail-call veneer (`ldr ip, [pc]; bx ip`) to CollectByEgg, passing
 * `this`/`player` straight through unchanged.
 * Matched byte-for-byte with mwccarm 2004/b56 (ov002).
 */
void daStar_c::OnTurnIntoEgg(daPly_c &player) {
    return CollectByEgg((int)&player);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- CollectByEgg, 0x020e8e80, size 0x5c */
/* -------------------------------------------------------------------------- */
/* A Yoshi egg took the star (OnTurnIntoEgg): unlinks the marker, stops the
 * motion, clears the hidden flag and collects the star for player a. */
void daStar_c::CollectByEgg(int a) {
    daStarBase_c *marker = (daStarBase_c *)dActor_c::FindWithID(mMarkerID);
    if (marker != 0)
        marker->LinkSilverStarAndStarMarker(0);

    mPlayer = (daPly_c *)a;
    StopMotion();
    mFlags &= ~0x40000;
    Collect((void *)a);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- MaybeAddStarMarker, 0x020e8dd8, size 0xa8 */
/* -------------------------------------------------------------------------- */
/* Adds the star's map marker unless the level hides it: sublevel 5 hides star
 * id 5, and sublevel 0x16 shows star id 4 only once gCurrentEntranceId is 4. Marker
 * type 0 is added only for kinds 2 and 4. */
int daStar_c::MaybeAddStarMarker() {
    signed char g1 = gCurrentSublevel;
    int t;
    if (g1 == 5) {
        if (mStarID == 5) {
            return;
        }
    }
    if (g1 == 0x16) {
        if (mStarID == 4) {
            if (gCurrentEntranceId != 4) {
                return;
            }
            AddStarMarker();
            return;
        }
    }
    if (mMarkerType == 0) {
        t = mKind;
        if ((t != 2) && (t != 4)) {
            return;
        }
    }
    AddStarMarker();
}

/* Claims a free slot of the map-marker table (STAR_MARKERS, 12 entries) for
 * this star. The marker type is 1 for the silver star, 2 for the Power Star
 * and 3 once collected; type 0 uses 3 or 2 from the collected-model flag. A
 * star waiting for its marker does not show it until data_0209f208 is non-
 * zero. */
void daStar_c::AddStarMarker()
{
    s8 i;
    if (mMarkerSlot >= 0) return;
    for (i = 0; i < 0xc; i++) {
        if (STAR_MARKERS[(int)i] != 0)
            continue;

        if (mMarkerType == 0) {
            if (mBits.collectedModel) {
                SetStarMarker((int)i, this, 3);
            } else {
                SetStarMarker((int)i, this, 2);
            }
        } else {
            if (mMarkerType == 2) {
                int f43c;
                if (mBits.collectedModel ||
                    (((f43c = mKind) == 5 || f43c == 7) &&
                     IsStarCollectedInCurLevel(mStarID) != 0))
                    mMarkerType = 3;
            }
            SetStarMarker((int)i, this, mMarkerType);
        }

        mMarkerSlot = i;
        if (mState == STATE_WAIT_MARKER) {
            if (data_0209f208 == 0) return;
        }
        FUN_0202a130();
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- CheckBounds, 0x020e8c34, size 0x6c */
/* -------------------------------------------------------------------------- */
/* Bounds test: a star more than 80000.0 from the origin on X or Z, higher than
 * 80000.0, or below mMinPosY goes to ReturnToHome. */
volatile unsigned int daStar_c::CheckBounds() {
    int v;
    int y;
    v = mPosX;
    y = 0;
    if (y > v) {
        v = -v;
    }
    if (v <= 0x13880000) {
        v = mPosZ;
        if (v < y) {
            v = -v;
        }
        if (v <= 0x13880000) {
            y = mPosY;
            if ((y >= mMinPosY) && (y <= 0x13880000)) {
                return;
            }
        }
    }
    ReturnToHome();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- ReturnToHome, 0x020e8abc, size 0x178 */
/* -------------------------------------------------------------------------- */
/* Out of bounds or on a bad floor: puts the star back. Without a marker it
 * destroys itself. A marker-placed star (home state 9) returns to its home
 * position, hides, waits for the marker again and puts the marker back on show
 * (unless the marker's param1 has bit 0x20). With the relink flag it re-links
 * to a marker; any other star returns to its home position and hops out again
 * (32.0 up). */
void daStar_c::ReturnToHome() {
    daStarBase_c *a;

    a = (daStarBase_c *)dActor_c::FindWithID(mMarkerID);
    if (a == 0) {
        MarkForDestruction();
        return;
    }

    if (mHomeState == STATE_WAIT_MARKER) {
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
        func_02035860(&mWithMeshClsn, &mPosX);
        mState = mHomeState;
        StopMotion();
        mdCc_c.flags |= 1;
        mBits.visible = 0;
        mBits.noPickup = 1;
        mBits.water = WATER_NONE;
        if (a->param1 & 0x20) {
            return;
        }
        {
            a->mBits.hold = 0;
            a->mBits.visible = 1;
        }
        a->mHitActor = 0;
        return;
    }

    {
        if (mBits.relink) {
            RelinkMarker();
            return;
        }
    }

    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;
    func_02035860(&mWithMeshClsn, &mPosX);
    mState = mHomeState;
    mVertSpeed = 0x20000;
    SetDefaultGravity();
    mStateTimer = 0xf;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- OnBounceLand, 0x020e88a8, size 0x214 */
/* -------------------------------------------------------------------------- */
/* The bounce hit the ground. Clears the no-pickup flag, plays sound 3/0x55 and
 * sets the next bounce (14.0 up in water, 23.0 on land, 12.0 forward). On
 * floor types 1 and 9 it aims back at the last safe position
 * (AimArcTo, arc 200.0); on 4 and 5 it goes home
 * (ReturnToHome); on any other floor it saves the spot as safe and sets
 * mPrevAngleY from the closest player when one is within 1200.0 (away from it
 * on a flat floor, toward it on a steep one), or toward the farthest player
 * when none is near. */
void daStar_c::OnBounceLand() {
    struct Vector3 v;
    struct Vector3 w;
    dActor_c *p;
    int r;

    if (mBits.noPickup) {
        mBits.noPickup = 0;
        mStateTimer = 0;
    }
    func_02012694(0x55, &mCamSpacePosX);
    if (mBits.water == WATER_IN) {
        mHorzSpeed = 0xc000;
        mVertSpeed = 0xe000;
    } else {
        mHorzSpeed = 0xc000;
        mVertSpeed = 0x17000;
    }
    r = mWithMeshClsn.GetFloorResult()->surface.clps.GetSurfaceType();
    if (r == 1 || r == 9) {
        w.x = mSafePosX;
        w.y = mSafePosY;
        w.z = mSafePosZ;
        AimArcTo(&w, 0xc8000);
        return;
    }
    if (r == 4 || r == 5) {
        ReturnToHome();
        return;
    }
    SetDefaultGravity();
    mSafePosX = mPosX;
    mSafePosY = mPosY;
    mSafePosZ = mPosZ;
    p = (dActor_c *)ClosestPlayer();
    if (p == 0) return;
    {
        int *s = (int *)&p->mPosX;
        v.x = s[0];
        v.y = s[1];
        v.z = s[2];
    }
    if (Vec3_Dist(&mPos, &v) < 0x4b0000) {
        mPrevAngleY = Vec3_HorzAngle(&v, &mPos);
        if (mFloorNormalY >= *(short*)(data_02082714 + 0x56)) {
            if (gCurrentSublevel != 0x1d)
                return;
            if (mWithMeshClsn.GetFloorResult()->surface.clps.GetFloorClass() != 5)
                return;
        }
        {
            short *a = (short *)&mPrevAngleY;
            *a = *a + 0x8000;
        }
    } else {
        dActor_c *q = FarthestPlayer();
        if (q != 0) {
            mPrevAngleY = Vec3_HorzAngle(&mPos, &q->mPos);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- UpdateWater, 0x020e86ec, size 0x1bc */
/* -------------------------------------------------------------------------- */
/* Water handling for the bounce; mBits.water is a WaterState. On first contact
 * it stops the collider watching for water and probes the surface with a
 * dBgCh_Gnd from 160.0 above the star. A surface at least 60.0 above the star
 * puts it in WATER_IN (weak gravity, slow fall). WATER_IN ends once the saved
 * surface (data_0209f32c) is less than 60.0 above the star. */
void daStar_c::UpdateWater() {
    struct Vector3 v;
    int tx, ty, tz, ta;

    if (mBits.water < WATER_IN) {
        if (mBits.water != WATER_TOUCH) {
            if (mWithMeshClsn.TouchesWater() == 0)
                return;
        }
        if (mBits.water == WATER_NONE) {
            mBits.water = WATER_TOUCH;
            mPosY = mPrevPosY;
            mWithMeshClsn.ClearGroundFlag();
            mWithMeshClsn.ClearJustHitGroundFlag();
            mWithMeshClsn.StopDetectingWater();
        }
        dBgCh_Gnd rc;
        rc.StartDetectingWater();
        ty = mPosY;
        tz = mPosZ;
        tx = mPosX;
        ta = ty + 0xa0000;
        v.x = tx;
        v.y = ta;
        v.z = tz;
        rc.SetObjAndPos(*(Vector3 *)&v, this);
        if (rc.DetectClsn() != 0) {
            if (SurfaceInfo_TestFlag0x20(&rc.surface) != 0) {
                mWaterHeight = rc.clsnY;
                data_0209f32c = mWaterHeight;
                if (mWaterHeight >= mPosY + 0x3c000) {
                    mBits.water = WATER_IN;
                    mVertAccel = -0x700;
                    mTerminalVelocity = -0x10000;
                    mHorzSpeed = 0xc000;
                    mVertSpeed = 0;
                }
            }
        }
    } else {
        if (data_0209f32c < mPosY + 0x3c000) {
            mBits.water = WATER_NONE;
            SetDefaultGravity();
            mWithMeshClsn.StartDetectingWater();
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- FinishCollection, 0x020e8618, size 0xd4 */
/* -------------------------------------------------------------------------- */
/* End of a collection, once the pose animation has finished: stops the sound
 * object, hides the star, gives up the map marker, destroys it (VS match) or
 * records it in the death table, and ends the cutscene (CUTSCENE_FLAG cleared
 * everywhere, event bits 0x1e and 0x1d cleared). */
void daStar_c::FinishCollection() {
    if (mAnmModel1.Finished() == 0)
        return;
    StopCollectSoundObj();
    mBits.visible = 0;
    UntrackStar(mMarkerSlot);
    if ((int)(gGameMode == 1) != 0) {
        MarkForDestruction();
    } else {
        KillAndTrackInDeathTable();
    }
    mPlayer->mFlags &= ~CUTSCENE_FLAG;
    mFlags &= ~CUTSCENE_FLAG;
    gNextActorUpdateMask &= ~CUTSCENE_FLAG;
    Event::ClearBit(0x1e);
    Event::ClearBit(EVENT_COLLECTING_STAR);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- UpdateModelMatrix, 0x020e84ec, size 0x12c */
/* -------------------------------------------------------------------------- */
/* Builds the model matrix for this frame. Kind 8 sits at its position. A
 * matrix pointer in the unnamed dActor_c word at +0xc8 is copied as it is.
 * Otherwise the star spins about Y by 0xc00 per frame, or, with the no-spin
 * flag, faces mPrevAngleY and hangs 50.0 higher. The translucent model takes a
 * copy, then DrawShadow draws the shadow. */
void daStar_c::UpdateModelMatrix() {
    struct Vector3 v;
    s16* ang;
    int t;

    if (mKind == KIND_8) {
        Vec3_Asr(&v, &mPos, 3);
        Matrix4x3_FromTranslation(&mAnmModel1.mat4x3, v.x, v.y, v.z);
    } else if (mCarryMatrix != 0) {
        /* Mtx43 overlay: the class headers above switch Matrix4x3 to the
         * structured math/Matrix.h spelling, whose struct-copy codegen
         * differs from the flat 12-word copy this shard proved. Mtx43 is the
         * same 12 words; proven byte-identical in isolation. */
        *(Mtx43 *)&mAnmModel1.mat4x3 = *(Mtx43 *)mCarryMatrix;
    } else if (!mBits.noSpin) {
        ang = (s16 *)&mAngleY;
        t = *ang + 0xc00;
        *ang = t;
        Matrix4x3_FromRotationY(&mAnmModel1.mat4x3, mAngleY);
        mAnmModel1.mat4x3.t.x = mPosX >> 3;
        mAnmModel1.mat4x3.t.y = mPosY >> 3;
        mAnmModel1.mat4x3.t.z = mPosZ >> 3;
    } else {
        Matrix4x3_FromRotationY(&mAnmModel1.mat4x3, mPrevAngleY);
        mAnmModel1.mat4x3.t.x = mPosX >> 3;
        mAnmModel1.mat4x3.t.y = (mPosY + 0x32000) >> 3;
        mAnmModel1.mat4x3.t.z = mPosZ >> 3;
    }

    *(Mtx43 *)&mAnmModel2.mat4x3 = *(Mtx43 *)&mAnmModel1.mat4x3;
    DrawShadow();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- DrawShadow, 0x020e8398, size 0x154 */
/* -------------------------------------------------------------------------- */
/* Draws the star's shadow on the ground below it. Skipped while hidden,
 * flagged hidden or no-spin. The radius is 100.0 (160.0 for the Power Star)
 * times the scale, less 0.09375 per unit of height above mGroundY (at least
 * 1.0), never below 10.0; the shadow is 40.0 deeper than that height. */
void daStar_c::DrawShadow() {
    char *c = (char *)this;
    int r2, rad, delta, r8, t;
    int flag2;

    if (!mBits.visible)
        return;
    flag2 = mFlags & 0x40000;
    flag2 = flag2 != 0;
    if (flag2 != false)
        return;
    if (mBits.noSpin)
        return;

    r2 = mScaleX;
    rad = r2 * 0x64;
    flag2 = actorID;
    flag2 = flag2 == ACTOR_STAR;
    if (flag2 != false)
        rad = r2 * 0xa0;

    delta = mPosY - mGroundY;
    if (delta <= 0x1000)
        delta = 0x1000;

    r8 = rad - FX_MUL(delta, 0x180);
    if (r8 < 0xa000)
        r8 = 0xa000;

    t = delta + 0x28000;

    *(Mtx43 *)&mShadowMtx = *(Mtx43 *)&IDENTITY_MATRIX4X3;

    mShadowMtx.tx = mPosX >> 3;
    mShadowMtx.ty = mPosY >> 3;
    mShadowMtx.tz = mPosZ >> 3;

    ((dActor_c *)c)
        ->DropShadowRadHeight(*(dExtShadowModel_c *)&mShadowModel, *(Matrix4x3 *)&mShadowMtx, r8, t,
                              0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov002_020e8244, 0x020e8244, size 0x154 */
/* -------------------------------------------------------------------------- */
/* The star's centre, written to *out. Cached in mCenterX/Y/Z (Behavior zeroes
 * the cache each frame). Otherwise it multiplies the first bone's matrix with
 * a translation and Y rotation of the star, scales the result about the star's
 * position (<< 3) and raises Y by 13 times a word of the bone data at +0xc. */
extern "C" {  /* C linkage: the ROM symbol is a bare name, not a mangled member */
void func_ov002_020e8244(void *out, daStar_c *b)
{
    Mtx43 local;
    VecFx32 zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    if (func_0203d024((struct Vector3 *)&b->mCenterX, (struct Vector3 *)&zero) != 0) {
        ((int *)out)[0] = b->mCenterX;
        ((int *)out)[1] = b->mCenterY;
        ((int *)out)[2] = b->mCenterZ;
        return;
    }
    Matrix4x3_FromTranslation(&data_020a0e68, b->mPosX, b->mPosY, b->mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, b->mPrevAngleY);
    local = *(Mtx43 *)b->mAnmModel1.data.transforms;
    MTX_Concat43(&local, &data_020a0e68, &data_020a0e68);
    b->mCenterX = data_020a0e68.t.x;
    b->mCenterY = data_020a0e68.t.y;
    b->mCenterZ = data_020a0e68.t.z;
    VEC_Subtract((struct Vector3 *)&b->mCenterX, &b->mPos, (struct Vector3 *)&b->mCenterX);
    Vec3_LslInPlace((void *)&b->mCenterX, 3);
    VEC_Add((struct Vector3 *)&b->mCenterX, &b->mPos, (struct Vector3 *)&b->mCenterX);
    {
        int *p = &b->mCenterY;
        *p = *(int *)((char *)b->mAnmModel1.data.bones + 0xc) * 0xd + *p;
    }
    ((int *)out)[0] = b->mCenterX;
    ((int *)out)[1] = b->mCenterY;
    ((int *)out)[2] = b->mCenterZ;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- UpdateTrailEffect, 0x020e81e0, size 0x64 */
/* -------------------------------------------------------------------------- */
/* Keeps trail effect 0x113 (mParticle[0]) 13.0 above the star. */
extern "C" void *d_a_starNewHold(int a0, unsigned int a1, int a2, int a3, int a4, void *a5,
                                 void *a6);
void daStar_c::UpdateTrailEffect() {
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y += 0xd000;
    mParticle[0] = Particle::System::New(mParticle[0], kPtclStarTrail, v.x, v.y, v.z, 0, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- SpawnCollectEffects, 0x020e8098, size 0x148 */
/* -------------------------------------------------------------------------- */
/* Collection effects: while the animation runs, effects 0x115 and 0x116
 * (mParticle[0], [1]) at the centre, raised 50.0 (except in
 * STATE_SPIN_AND_END), with the offset from the star scaled by the star's
 * scale. */
void daStar_c::SpawnCollectEffects() {
    Vector3 vc;
    Vector3 v;
    if (mAnmModel1.Finished())
        return;
    func_ov002_020e8244(&v, this);
    vc.x = v.x;
    vc.y = v.y;
    vc.z = v.z;
    if (mState != STATE_SPIN_AND_END)
        vc.y = v.y + 0x32000;
    VEC_Subtract(&vc, &mPos, &vc);
    vc.x = FX_MUL(vc.x, mScaleX);
    vc.y = FX_MUL(vc.y, mScaleY);
    vc.z = FX_MUL(vc.z, mScaleZ);
    VEC_Add(&vc, &mPos, &vc);
    *(void **)&mParticle[0] =
        (void *)Particle::System::New(mParticle[0], kPtclStarCollectA, vc.x, vc.y, vc.z, 0, 0);
    *(void **)&mParticle[1] =
        (void *)Particle::System::New(mParticle[1], kPtclStarCollectB, vc.x, vc.y, vc.z, 0, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- SpawnPoseEffects, 0x020e7fcc, size 0xcc */
/* -------------------------------------------------------------------------- */
/* Pose effects: effect 0x2f (mParticle[0]) at the centre while the animation
 * plays from frame 2 on, and a one-shot effect 0x30 when frame 0x75 is
 * reached. */
void daStar_c::SpawnPoseEffects() {
    void* obj;
    struct Vector3 v1;
    struct Vector3 v2;

    obj = mAnmModel1.data.bones;

    if (!mAnmModel1.Finished() && (u32)((*(u32 *)&mAnmModel1.currFrame << 4) >> 0x10) >= 2 &&
        *(int *)((char *)obj + 0xc) != 0) {
        func_ov002_020e8244(&v1, this);
        mParticle[0] =
            (u32)Particle::System::New(mParticle[0], kPtclStarPose, v1.x, v1.y, v1.z, 0, 0);
        return;
    }

    if (!mAnmModel1.WillHitFrame(0x75))
        return;
    func_ov002_020e8244(&v2, this);
    Particle::System::NewSimple(kPtclStarPoseEnd, v2.x, v2.y, v2.z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- UpdateSparkleEffect, 0x020e7f2c, size 0xa0 */
/* -------------------------------------------------------------------------- */
/* Keeps effect 0x114 (mParticle[2]) 13.0 above the star until mSparkleTimer
 * runs out. */
void daStar_c::UpdateSparkleEffect() {
    volatile s32 x, y, zvar;
    s32 z, yraw;

    if (mSparkleTimer == 0)
        return;
    mSparkleTimer--;
    if (mSparkleTimer == 0)
        mParticle[2] = 0;
    x = mPosX;
    yraw = mPosY;
    y = yraw;
    {
        s32 zraw = mPosZ;
        s32 yadj = yraw + 0xd000;
        z = zraw;
        zvar = zraw;
        y = yadj;
    }
    mParticle[2] =
        (u32)Particle::System::New(*(volatile u32 *)&mParticle[2], kPtclStarSparkle, x, y, z, 0, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- UpdateSilverTrailEffect, 0x020e7eb8, size 0x74 */
/* -------------------------------------------------------------------------- */
/* Silver star only: keeps trail effect 0x10e (mParticle[3]) at the centre. */
void daStar_c::UpdateSilverTrailEffect() {
    Vector3 v;
    int b = (actorID == ACTOR_SILVER_STAR);
    if (b == 0) return;
    func_ov002_020e8244(&v, this);
    mParticle[3] =
        (int)Particle::System::New(mParticle[3], kPtclSilverStarTrail, v.x, v.y, v.z, 0, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov002_020e7eb4, 0x020e7eb4, size 0x4 */
/* -------------------------------------------------------------------------- */
/* Does nothing (STATE_COLLECT_TALK calls it while the pose animation runs). */
void daStar_c::func_ov002_020e7eb4() {
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- StopCollectSoundObj, 0x020e7e58, size 0x5c */
/* -------------------------------------------------------------------------- */
/* Stops the sound object SpawnCollectSoundObj spawned: mode 1 zeroes its
 * mCounterLimit, mode 2 destroys it. mSoundObjID is cleared. */
void daStar_c::StopCollectSoundObj() {
    unsigned int id;
    daSoundObj_c *a;
    if (mSoundObjMode == 0) return;
    id = mSoundObjID;
    if (id == 0) return;
    a = (daSoundObj_c *)dActor_c::FindWithID(id);
    if (a != 0) {
        if (mSoundObjMode == 1) a->mCounterLimit = 0;
        else a->MarkForDestruction();
    }
    mSoundObjID = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- SpawnSoundObj6, 0x020e7e24, size 0x34 */
/* -------------------------------------------------------------------------- */
/* Spawns sound object 6 once; mSoundObj6State records that it exists. */
void daStar_c::SpawnSoundObj6() {
    if (mSoundObj6State != 0xff)
        return;
    if (SpawnSoundObj(6))
        mSoundObj6State = 0x78;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- RearmSoundObj6, 0x020e7e14, size 0x10 */
/* -------------------------------------------------------------------------- */
/* Re-arms SpawnSoundObj6 by setting mSoundObj6State back to 0xff. */
int daStar_c::RearmSoundObj6() {
    mSoundObj6State = 0xff;
    return 1;
}

/* The marker was touched: plays sound 3/0x53, puts the marker on hold and
 * hides it, clears its touch volume and bursts effects 0x12c, 0x12d and 0x12e. */
void daStarBase_c::Collect()
{
    func_02012694(0x53, &mCamSpacePosX);
    {
        mBits.hold = 1;
        mBits.visible = 0;
    }
    mdCcAcPos_c.Clear();
    Particle::System::NewSimple(kPtclStarMarkerCollectA, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(kPtclStarMarkerCollectB, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(kPtclStarMarkerCollectC, mPosX, mPosY, mPosZ);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- ProbeGround, 0x020e7d08, size 0x7c */
/* -------------------------------------------------------------------------- */
/* Ground probe: casts down from 50.0 above the star and stores the floor
 * height in mGroundY (0x7fffffff when there is none). */
void daStar_c::ProbeGround() {
    dBgCh_Gnd rc;
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y += 0x32000;
    rc.SetObjAndPos(v, 0);
    rc.mProbeHeight = 0x3e8000;
    if (rc.DetectClsn())
        mGroundY = rc.clsnY;
    else
        mGroundY = 0x7fffffff;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- PlaceCameraAtStarCamera, 0x020e7c90, size 0x78 */
/* -------------------------------------------------------------------------- */
/* Camera placement from a StarCamera marker (actor 0xb1): looks at the star
 * from the position of the marker whose star id matches. Returns 1 when one
 * was found. */

int daStar_c::PlaceCameraAtStarCamera(void *cam) {
    dActor_c *a = 0;
    for (;;) {
        a = dActor_c::FindWithActorID(ACTOR_CAMERA_MARKER, a);
        if (a == 0) break;
        if (mStarID == (a->param1 & 0xf)) {
            ((dCamera_c *)cam)->SetLookAt(mPos);
            ((dCamera_c *)cam)->SetPos(a->mPos);
            return 1;
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- PlaceCameraOnRing, 0x020e7934, size 0x35c */
/* -------------------------------------------------------------------------- */
/* Camera placement when no StarCamera marker exists: looks at the star and
 * tries up to 20 spots on a ring round it (a quarter turn per attempt,
 * stepping up or down by 300.0 or 600.0 every four) until the line from the
 * star to the spot is not blocked (dBgCh_Lin). After 20 failures the saved
 * camera position is used. */
void daStar_c::PlaceCameraOnRing(void *cam) {
    Vector3 vec[2];
    int flag;
    int dist;
    unsigned int r6 = 0;

    vec[0].x = mPosX;
    vec[0].y = mPosY;
    vec[0].z = mPosZ;
    ((dCamera_c *)cam)->SetLookAt(vec[0]);

    vec[0].y += 0xc8000;
    vec[1] = vec[0];
    dist = Vec3_Dist(&vec[0], (Vector3*)&mCamPosX);

    unsigned short mode = mCamSeq;
    flag = (((mode == 100 && (dist > 0x3e8000 || dist < 0x1f4000)) ||
             (mode != 100 && dist > 0x3e8000)) &&
            IsAreaShowing(mSavedAreaId)) ? 1 : 0;

    while (1) {
        if (r6 >= 0x10)
            vec[0].y -= 0x258000;
        else if (r6 >= 0xc)
            vec[0].y -= 0x12c000;
        else if (r6 >= 8)
            vec[0].y += 0x258000;
        else if (r6 >= 4)
            vec[0].y += 0x12c000;

        if (flag) {
            short ang = Vec3_HorzAngle(&vec[0], (Vector3*)&mCamPosX);
            vec[0].x = FX_SinIdx((u16)(s16)(ang + ((r6 & 3) << 14)) >> 4) * 1000 + vec[0].x;
            vec[0].z = FX_CosIdx((u16)(s16)(ang + ((r6 & 3) << 14)) >> 4) * 1000 + vec[0].z;
            ((dCamera_c *)cam)->SetPos(*(const Vector3 *)&vec[0]);
        } else {
            if (!IsAreaShowing(mAreaId))
                return;
            short ang = Vec3_HorzAngle(&vec[0], (Vector3*)&mCamPosX);
            vec[0].x += FX_MUL(dist, FX_SinIdx((u16)(s16)(ang + ((r6 & 3) << 14)) >> 4));
            vec[0].z += FX_MUL(dist, FX_CosIdx((u16)(s16)(ang + ((r6 & 3) << 14)) >> 4));
            ((dCamera_c *)cam)->SetPos(*(const Vector3 *)&vec[0]);
        }

        if (!IsAreaShowing(mAreaId))
            return;

        {
            dBgCh_Lin rl;
            Vector3 a;
            Vector3 b;
            a.x = mPosX;
            a.y = mPosY;
            a.z = mPosZ;
            b.x = vec[0].x;
            b.y = vec[0].y;
            b.z = vec[0].z;
            rl.SetObjAndLine(a, b, this);
            if (rl.DetectClsn()) {
                r6 = (r6 + 1) & 0xff;
                vec[0] = vec[1];
                if (r6 >= 0x14) {
                    ((dCamera_c *)cam)->SetPos(*(const Vector3*)&mCamPosX);
                    return;
                }
                continue;
            }
            return;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- StepCameraCutscene, 0x020e763c, size 0x2f8 */
/* -------------------------------------------------------------------------- */
/* Steps the camera cutscene (mCamSeq) while CUTSCENE_FLAG is set; 0xffff means
 * none. Step 0 saves the camera's look-at and position, then points it at the
 * star (StarCamera first, PlaceCameraOnRing as the fallback) and goes to 1.
 * Step 1 keeps looking at the star while it flies to its marker. Step 0x64 is
 * step 0 for the appearing star and goes to 0x65, which looks at the star
 * until it is fully shown or hidden, then jumps to 0x1b6. Every other step
 * counts up, so 0x1d6 (set by STATE_LAND) and 0x1b6 reach 0x1f4 after 30 and
 * 62 frames: that restores the saved view, and 0x1f5 ends the cutscene. */
void daStar_c::StepCameraCutscene() {
    VecFx32 v;
    dCamera_c *cam;

    if ((gNextActorUpdateMask & CUTSCENE_FLAG) == 0)
        return;

    if (mCamSeq == 0xffff)
        return;

    v.x = mPosX;
    cam = gCamera;
    v.y = mPosY;
    v.z = mPosZ;

    switch (mCamSeq) {
    case 0:
    {
        int *s1 = (int *)&cam->lookAt;
        int *s2 = (int *)&cam->pos;
        mCamLookAtX = s1[0];
        mCamLookAtY = s1[1];
        mCamLookAtZ = s1[2];
        mCamPosX = s2[0];
        mCamPosY = s2[1];
        mCamPosZ = s2[2];
    }
        PlaceCameraAtStarCamera(cam);
        cam->SetFlag_3();
        if (PlaceCameraAtStarCamera(cam) == 0)
            PlaceCameraOnRing(cam);
        mCamSeq += 1;
        break;
    case 1:
        if (mState != STATE_FLY_TO_MARKER)
            return;
        cam->SetLookAt(*(const Vector3 *)&v);
        break;
    case 0x64:
    {
        int *s1 = (int *)&cam->lookAt;
        int *s2 = (int *)&cam->pos;
        mCamLookAtX = s1[0];
        mCamLookAtY = s1[1];
        mCamLookAtZ = s1[2];
        mCamPosX = s2[0];
        mCamPosY = s2[1];
        mCamPosZ = s2[2];
    }
        cam->SetFlag_3();
        if (PlaceCameraAtStarCamera(cam) == 0)
            PlaceCameraOnRing(cam);
        mCamSeq += 1;
        break;
    case 0x65:
        cam->SetLookAt(*(const Vector3 *)&v);
        if (mScaleX == 0x1000 || mScaleX == 0)
            mCamSeq = 0x1b6;
        break;
    case 0x1f4:
        cam->SetLookAt(*(const Vector3 *)&mCamLookAtX);
        cam->SetPos(*(const Vector3 *)&mCamPosX);
        mCamSeq += 1;
        break;
    case 0x1f5:
        cam->mFlags &= ~8;
        mFlags &= ~CUTSCENE_FLAG;
        gNextActorUpdateMask &= ~CUTSCENE_FLAG;
        mCamSeq = 0xffff;
        mAreaId = mSavedAreaId;
        break;
    default:
        mCamSeq += 1;
        break;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- LinkRandomFreeMarker, 0x020e7554, size 0xe8 */
/* -------------------------------------------------------------------------- */
/* Picks one of up to five free markers at random and links to it. A marker is
 * free when it is in state 3, or is in any state but 0 with hold set, and its
 * star no longer exists. */
void daStar_c::LinkRandomFreeMarker() {
    daStarBase_c *found;
    daStarBase_c *arr[5] = {0};
    int cnt;
    unsigned int idx;

    found = 0;
    cnt = 0;
    do {
        found = (daStarBase_c *)dActor_c::FindWithActorID(ACTOR_STAR_MARKER, found);
        if (found == 0) break;
        if ((found->mState == STATE_LAND && dActor_c::FindWithID(found->mLinkedStarID) == 0) ||
            (found->mState != STATE_LAUNCH && found->mBits.hold &&
             dActor_c::FindWithID(found->mLinkedStarID) == 0)) {
            arr[cnt] = found;
            cnt++;
        }
    } while (cnt < 5);

    idx = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) % (unsigned int)cnt;
    found = arr[idx];
    if (found == 0) return;
    mMarkerID = found->uniqueID;
    found->LinkSilverStarAndStarMarker((char *)this);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- RelinkMarker, 0x020e7454, size 0x100 */
/* -------------------------------------------------------------------------- */
/* Re-links the star to its marker: takes the marker's position. A marker in
 * state 3 hands over the star state it remembered (STATE_IDLE also stops the
 * motion). Any other marker is put back on show and the star waits hidden and
 * unpickable in STATE_WAIT_MARKER. */
void daStar_c::RelinkMarker() {
    daStarBase_c *a = (daStarBase_c *)dActor_c::FindWithID(mMarkerID);
    int* s;
    mBits.water = WATER_NONE;
    s = (int *)&a->mPosX;
    mPosX = s[0];
    mPosY = s[1];
    mPosZ = s[2];
    func_02035860(&mWithMeshClsn, &mPosX);
    if (a->mState == STATE_LAND) {
        mHomeState = a->mLinkedStarState;
        mState = mHomeState;
        if (mState != STATE_IDLE) return;
        StopMotion();
    } else {
        a->mBits.hold = 0;
        a->mBits.visible = 1;
        mState = STATE_WAIT_MARKER;
        mdCc_c.flags |= 1;
        mBits.visible = 0;
        mBits.noPickup = 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- PickTalkState, 0x020e73ac, size 0xa8 */
/* -------------------------------------------------------------------------- */
/* The talk state the player is put in when it takes this star: 3 for star id
 * 8; 2 in a VS match, for the silver star and for a marker-placed star; 1 for
 * star id 0 and the levels func_ov002_020e9630 accepts; 0 otherwise. */
u8 daStar_c::PickTalkState() {
    int c, a, b;
    unsigned char r2 = mStarID;
    if (r2 == 8)
        return 3;

    a = gGameMode == 1;
    if (a != false || (b = (actorID == ACTOR_SILVER_STAR)) != false || (c = mHomeState) == 9)
        return 2;

    if (r2 == 0 || func_ov002_020e9630() != 0)
        return 1;
    return 0;
}

/* Once the eighth red coin is in (NumRedCoins() == 8) spawns this marker's
 * Power Star 120.0 above it. The star takes the map marker, remembers this
 * marker and gets the coin-reward flag. */
void daStarBase_c::SpawnRedCoinStarIfNecessary()
{
    VecFx32 v;
    daStar_c *star;
    int y;
    if (mBits.spawned) return;
    if (NumRedCoins() != 8) return;
    v.x = mPosX;
    y = mPosY;
    v.y = y;
    v.z = mPosZ;
    v.y = y + 0x78000;
    star = (daStar_c *)dActor_c::Spawn(ACTOR_STAR, mStarID, *(Vector3 *)&v, 0, mAreaId, -1);
    if (star == 0) return;
    star->AddStarMarker();
    star->mBits.coinReward = 1;
    star->mMarkerID = uniqueID;
    mBits.spawned = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- SeekNewMarker, 0x020e7218, size 0xc0 */
/* -------------------------------------------------------------------------- */
/* Sends the star looking for a new marker. With gate 0 it first spawns a score
 * popup 200.0 above the player. Sets the relink flag and picks a marker with
 * LinkRandomFreeMarker. */
void daStar_c::SeekNewMarker(char *player, int gate) {
    daPly_c *a = (daPly_c *)player;
    if (gate == 0) {
        struct Vector3 pos[2];
        bool b;
        int* v;
        v = (int *)&a->mPosX;
        pos[0].x = v[0];
        pos[0].y = v[1];
        pos[0].z = v[2];
        b = (actorID != ACTOR_STAR);
        b = (b != 0);
        pos[0].y = pos[0].y + 0xc8000;
        pos[1].x = pos[0].x;
        pos[1].z = pos[0].z;
        pos[1].y = pos[0].y;
        SpawnNumber(pos[1], gNumStageStars[a->mPlayerNo], b, 0x15, a);
    }
    mBits.relink = 1;
    LinkRandomFreeMarker();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- LinkSilverStarAndStarMarker, 0x020e71d4, size 0x44 */
/* -------------------------------------------------------------------------- */
/* Links the marker to the star b, copying its uniqueID, State and (when it has
 * one) death-table id; with 0 it forgets the star. */
void daStarBase_c::LinkSilverStarAndStarMarker(char* b) {
    daStar_c *star = (daStar_c *)b;
    if (star != 0) {
        mLinkedStarID = star->uniqueID;
        mLinkedStarState = star->mState;
        short s = star->mDeathTableID;
        if (s >= 0) mLinkedStarDeathTableID = s;
    } else {
        mLinkedStarDeathTableID = -1;
        mLinkedStarID = 0;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- LoadSilverStarAndNumber, 0x020e71a8, size 0x2c */
/* -------------------------------------------------------------------------- */
/* Loads the silver star and score-number model files. */
extern "C" {  /* C linkage: the ROM symbol is a bare name, not a mangled member */
void LoadSilverStarAndNumber(void)
{
    dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_ov002_0210da28);
    dExtSimpleModel_c::LoadFile(data_ov002_02110954);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- UnloadSilverStarAndNumber, 0x020e717c, size 0x2c */
/* -------------------------------------------------------------------------- */
/* Releases the silver star and score-number model files. */
extern "C" {  /* C linkage: the ROM symbol is a bare name, not a mangled member */
void UnloadSilverStarAndNumber(void)
{
    ((SharedFilePtr *)data_ov002_0210da28)->Release();
    data_ov002_02110954.Release();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- SetSwitchState, 0x020e7104, size 0x78 */
/* -------------------------------------------------------------------------- */
/* Called by the switch with its new state: 0 clears mBits.switchOn and, when
 * no cutscene is running, starts one (mCamSeq 0x64); anything else sets
 * switchOn. */
void daStar_c::SetSwitchState(int r1) {
    if (r1 == 0) {
        mBits.switchOn = 0;
        if (gNextActorUpdateMask & CUTSCENE_FLAG)
            return;
        mFlags |= CUTSCENE_FLAG;
        gNextActorUpdateMask |= CUTSCENE_FLAG;
        mCamSeq = 0x64;
        return;
    }
    mBits.switchOn = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- CollectNoPose, 0x020e7090, size 0x74 */
/* -------------------------------------------------------------------------- */
/* Collects the star for player arg without the pose: starts the cutscene bits,
 * spawns the sound object, stops the star, records the star as collected and
 * plays sound 0x2d. */
void daStar_c::CollectNoPose(daPly_c *arg) {
    mPlayer = arg;
    mPlayer->mFlags |= CUTSCENE_FLAG;
    mFlags |= CUTSCENE_FLAG;
    gNextActorUpdateMask |= CUTSCENE_FLAG;
    SpawnCollectSoundObj(0);
    mSoundObjMode = 1;
    StopMotion();
    CollectStarInCurLevel(mStarID);
    func_02012790(0x2d);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- CheckIceBlock, 0x020e700c, size 0x84 */
/* -------------------------------------------------------------------------- */
/* Once per star: looks for an ice block (actor 0x12) within 200.0 and, if one
 * is there, marks the star as inside it and tells the block. */
void daStar_c::CheckIceBlock() {
    dActor_c *o;
    if (mIceChecked)
        return;
    o = dActor_c::FindWithActorID(ACTOR_ICE_BLOCK_LL, 0);
    while (o) {
        if (Vec3_Dist(&mPos, &o->mPos) < 0xc8000) {
            mInIceBlock = 1;
            ((daObjIceBlock_c *)o)->mContainedActor = this;
            break;
        }
        o = dActor_c::FindWithActorID(ACTOR_ICE_BLOCK_LL, o);
    }
    mIceChecked = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- SpawnCollectSoundObj, 0x020e6fbc, size 0x50 */
/* -------------------------------------------------------------------------- */
/* Spawns sound object 4 once and remembers it in mSoundObjID. It plays the
 * sound id PickCollectSoundID picks at volume arg. */
void daStar_c::SpawnCollectSoundObj(int arg) {
    if (mSoundObjID != 0)
        return;
    daSoundObj_c *s = (daSoundObj_c *)SpawnSoundObj(4);
    if (s == 0)
        return;
    mSoundObjID = s->uniqueID;
    s->mSoundID = PickCollectSoundID();
    s->mVolume = arg;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- InitKind8, 0x020e6edc, size 0xe0 */
/* -------------------------------------------------------------------------- */
/* Kind 8 set-up (param1 & 0x7f == 0x7f): visible, no spin, plays its own
 * animation and goes straight to STATE_PLAY_AND_END. */
void daStar_c::InitKind8() {
    struct Vector3 v;
    mKind = KIND_8;
    mAnmModel1.SetAnim((struct BCA_File *)data_ov002_02110934.filePtr, 0x40000000, 0x1000, 0);
    mAnmModel2.SetAnim((struct BCA_File *)data_ov002_02110934.filePtr, 0x40000000, 0x1000, 0);
    v.x = data_ov002_0210aa0c[0];
    v.y = data_ov002_0210aa0c[1];
    v.z = data_ov002_0210aa0c[2];
    mdCc_c.Init(this, v, 0x64000, 0x96000, 1, 0);
    mState = STATE_PLAY_AND_END;
    mBits.noSpin = 1;
    mBits.visible = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- InitKind9, 0x020e6df8, size 0xe4 */
/* -------------------------------------------------------------------------- */
/* Kind 9 set-up (param1 & 0x7f == 0x6f): visible, no spin, star id 6, starts
 * in STATE_COLLECT_HOLD. */
void daStar_c::InitKind9() {
    mKind = KIND_9;
    mAnmModel1.SetAnim((BCA_File *)data_ov002_02110944.filePtr, 0x40000000, 0x1000, 0);
    mAnmModel2.SetAnim((BCA_File *)data_ov002_02110944.filePtr, 0x40000000, 0x1000, 0);
    {
        Vector3 v;
        v.x = data_ov002_0210aa0c[0];
        v.y = data_ov002_0210aa0c[1];
        v.z = data_ov002_0210aa0c[2];
        mdCc_c.Init(this, v, 0x64000, 0x96000, 1, 0);
    }
    mBits.noSpin = 1;
    mBits.visible = 1;
    mStarID = 6;
    mState = STATE_COLLECT_HOLD;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- ResetAppearance, 0x020e6d88, size 0x70 */
/* -------------------------------------------------------------------------- */
/* Puts the appearing star back to nothing: scale 0, switch off, map marker
 * given up, appear and state timers cleared. */
void daStar_c::ResetAppearance() {
    mScaleX = 0;
    mScaleY = 0;
    mScaleZ = 0;
    mBits.switchOn = 0;
    UntrackStar(mMarkerSlot);
    mBits.appeared = 0;
    mAppearTimer = 0;
    mStateTimer = 0;
}

/* daStarBase_c's destructor is inline in the header. Out of line it emits
 * D2, D0, D1 and defer_codegen off does not flip it. An odr-use of delete
 * then the explicit destructor emits D1 then D0 immediately below this
 * function, which is what the cartridge has at 0x020e6cf4. The function
 * itself is not in the ROM. */
void force_daStarBase_c_dtor_order() {
    daStarBase_c *p = 0;
    delete p;
    p->~daStarBase_c();
    func_02012790(0);
}

/* The nine file-scope resource handles, in retail initializer order. Five
   model files (func_02017acc / func_02017ab4), then four animation files
   (SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim); the manifest
   aliases each wrapper ctor/dtor onto those ROM symbols. */
StarModelFilePtr data_ov002_0211094c(0x8015);
StarModelFilePtr data_ov002_0211095c(0x8017);
StarModelFilePtr data_ov002_02110954(0x8019);
StarModelFilePtr data_ov002_0211093c(ASSET_STAR_BASE_BMD);
StarModelFilePtr data_ov002_0211092c(0x801c);
StarAnimationFileHandle data_ov002_02110944(0x8016);
StarAnimationFileHandle data_ov002_02110924(0x801a);
StarAnimationFileHandle data_ov002_02110964(0x801b);
StarAnimationFileHandle data_ov002_02110934(0x8018);

/* One pointer-to-member per State. mwcc cannot link-time-initialize them, so
   the initializer copies fourteen descriptor records in. */
StateHandler data_ov002_021109d8[14] = {
    &daStar_c::StateLaunch,       /* STATE_LAUNCH */
    &daStar_c::StateRise,         /* STATE_RISE */
    &daStar_c::StateFlyToMarker,  /* STATE_FLY_TO_MARKER */
    &daStar_c::StateLand,         /* STATE_LAND */
    &daStar_c::StateIdle,         /* STATE_IDLE */
    &daStar_c::StateCollectBegin, /* STATE_COLLECT_BEGIN */
    &daStar_c::StateCollectHold,  /* STATE_COLLECT_HOLD */
    &daStar_c::StateCollectTalk,  /* STATE_COLLECT_TALK */
    &daStar_c::StateBounce,       /* STATE_BOUNCE */
    &daStar_c::StateWaitMarker,   /* STATE_WAIT_MARKER */
    &daStar_c::StateTouched,      /* STATE_TOUCHED */
    &daStar_c::StateSaveMessage,  /* STATE_SAVE_MESSAGE */
    &daStar_c::StatePlayAndEnd,   /* STATE_PLAY_AND_END */
    &daStar_c::StateSpinAndEnd,   /* STATE_SPIN_AND_END */
};
