//cpp
// Timed floor switches and the switch which reveals a star.
// Select the shared flat Matrix4x3 before the actor headers, as dBgActor_c.h requires.
//
// The state handlers and their helpers are daObjSwitch_c members, and the
// .data records at 0x0210987c.. store {init, exec} pointer-to-member pairs
// over them (data_ov002_0210e00c). Reverse source order preserves the retail
// text order.
//
// The unit spans ov002 .text 0x020b9e64..0x020bab0c, 24 functions. The run
// ends with OnStoodOn, the collision callback StoodOnCallback that
// InitResources installs, and the two registry factories (STAR_SWITCH, then
// HANSWITCH). They are the highest-address .text, so they sit at the top of
// the file.
//
// leftovers:
//   - ChangeMusicVolume / IsClsnInRange / KcMbg::SetFile stay computed-spelling
//     externs: the member form changes codegen under 2004/b56 (Fix12<int> by
//     value, wall 6az -- noted at use).
//   - The daStar_c fields at +0x438/+0x440 and unk_49d are unnamed upstream;
//     the int*/unk_ puns stay until daStar_c.h names them.
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjSwitch_c_AssetId {
    ASSET_OBJ_BOX_SWITCH_BMD = 0x045c,
    ASSET_OBJ_BOX_SWITCH_KCL = 0x045d,
    ASSET_OBJ_STAR_SWITCH_BMD = 0x048f,
    ASSET_OBJ_STAR_SWITCH_KCL = 0x0490
};

#include "common.h"
#include "daObjSwitch_c.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "daStar_c.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"

namespace Event { int ClearBit(unsigned int bit); void SetBit(unsigned int bit); }

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c and destroy
 * through SharedFilePtr_Destruct_Clsn. The manifest aliases those undefined
 * members onto the ROM symbols. */
struct SwitchModelFilePtr : SharedFilePtr {
    SwitchModelFilePtr(u32 fileID);
    ~SwitchModelFilePtr();
};

struct SwitchClsnFileHandle : SharedFilePtr {
    SwitchClsnFileHandle(u32 fileID);
    ~SwitchClsnFileHandle();
};

extern "C" {

extern u32 gNextActorUpdateMask;
extern signed char gCurrentSublevel;
extern SharedFilePtr data_ov002_0211092c;
extern daObjSwitch_c::Resources data_ov002_021098e8[];
extern daObjSwitch_c::StateEntry data_ov002_0210e00c[];
extern SwitchModelFilePtr data_ov002_0210dfd4;
extern SwitchClsnFileHandle data_ov002_0210dfc4;
extern SwitchModelFilePtr data_ov002_0210dfcc;
extern SwitchClsnFileHandle data_ov002_0210dfbc;
// Names stay on this unused prototype so this file's @NNN numbers do not move.
int d_a_obj_switchChangeMusicVolumeHold(unsigned int id, int volume);
// local extern: the actual free definition accepts a scalar scale. The
// genuine member call changes InitResources under 2004/b56; see the
// committed experiment.
extern void hold_d_a_obj_switch(void *self, void *file, const Matrix4x3 *matrix, int scale,
                                s16 angle, void *clps);
}

int func_02012310(int handle, int sound, short arg);
unsigned char IsAreaShowing(int area);
extern "C" {

unsigned char DecIfAbove0_Byte(unsigned char *value);
void LoadSilverStarAndNumber();
void UnloadSilverStarAndNumber();
}

int Vec3_Dist(const Vector3 *a, const Vector3 *b);

/* Reconstructed source-style name: SM64DS proves daObjSwitch_c through RTTI,
 * allocation size, vtable identity, and the HANSWITCH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ExclamationSwitch_Spawn.
 *
 * `new daObjSwitch_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x354), dBgActor_c's base constructor, then
 * the vptr store. */
daObjSwitch_c *daObjSwitch_c_classInit_HANSWITCH(void) {
    return new daObjSwitch_c;
}

/* Reconstructed source-style name: SM64DS proves daObjSwitch_c through RTTI,
 * allocation size, vtable identity, and the STAR_SWITCH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StarSwitch_Spawn. */
daObjSwitch_c *daObjSwitch_c_classInit_STAR_SWITCH(void) {
    return new daObjSwitch_c;
}

/* The collision callback InitResources installs in mMeshCollider's slot. The
 * slot passes three arguments; OnStoodOn wants the last two.
 * long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
void daObjSwitch_c::StoodOnCallback(dBgW *collider, dActor_c *owner, dActor_c *other) {
    static_cast<daObjSwitch_c *>(owner)->OnStoodOn(other);
}
#pragma pop

/* The player (actor 0xbf) stepped on the switch: raise mPlayerNearby, which
 * the press states read. Underwater, only a metal or mega player is heavy
 * enough, and the player must be within 0x98 units of the switch. */
void daObjSwitch_c::OnStoodOn(dActor_c *other) {
    daPly_c *player = static_cast<daPly_c *>(other);
    unsigned int isPlayer = other->actorID == 0xbf;
    if (isPlayer == 0) return;
    if (player->mIsUnderwater != 0) {
        if (player->mIsMetal == 0) {
            if (player->mIsMega == 0) return;
        }
    }
    if (Vec3_Dist(&mPos, &other->mPos) < 0x98000)
        mPlayerNearby = 1;
}

int daObjSwitch_c::InitResources()
{
    u8 idx;
    void *f;

    mDrawScale.x = 0x1000;
    mDrawScale.y = 0x1000;
    mDrawScale.z = 0x1000;
    mTargetActorID = 0;
    mTargetActor = 0;
    mHomeAreaId = mAreaId;

    {
        // Keep the integer boolean: the direct condition changes the retail body.
        int b = 0;
        if (actorID == 0xc) b = 1;
        if (b) {
            mFlags |= 0x4000000;
            mSwitchType = 2;
            mStarID = param1;
            if (mStarID == 0xff)
                mStarID = 0;
            mTimeLimit = (param1 >> 8) & 0xff;
            LoadSilverStarAndNumber();
            dExtSimpleModel_c::LoadFile(data_ov002_0211092c);
            mResourceIdx = 1;
        } else {
            mSwitchType = param1 & 3;
            mEventBit = (param1 >> 3) & 0xf;
            mTimeLimit = (param1 >> 8) & 0xff;
            Event::ClearBit(mEventBit);
            mResourceIdx = 0;
        }
    }

    idx = mResourceIdx;
    f = dExtSimpleModel_c::LoadFile(*data_ov002_021098e8[idx].model);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnMatrix();

    idx = mResourceIdx;
    f = dBgW_Kc::LoadFile(*data_ov002_021098e8[idx].collision);
    mMeshCollider.SetFile((KCL_File *)(f), mClsnMat, 0x199, mAngleY,
                          *(CLPS_Block *)(data_ov002_021098e8[idx].clps));

    mMeshCollider.SetStoodOnCallback(&StoodOnCallback);

    {
        u16 h = mTimeLimit;
        if (h == 0xff || h == 0)
            mTimeLimit = 0x190;
        else
            mTimeLimit *= 0xa;
    }
    mPressTimer = 5;
    mPosY += 0x5000;
    mMusicFadeDone = 1;
    return 1;
}

int daObjSwitch_c::Behavior()
{
    dActor_c *a;
    int id344;
    int v;

    if (IsAreaShowing(mHomeAreaId) == 0) {
        mAreaId = mHomeAreaId;
        mTimer = 1;
        ScaleOverTime(2, 1, 0x333, 0x1000);
        SetState(0);
        a = dActor_c::FindWithID(mTargetActorID);
        if (a != 0) {
            static_cast<daStar_c *>(a)->ResetAppearance();
        }
    }

    {
        int isType = (int)(actorID == 0xc);
        if (isType != 0) {
            if (FindTargetStar() != 0) {
                a = dActor_c::FindWithID(mTargetActorID);
                if (a != 0) {
                    mTargetActor = a;
                }
            }
        }
    }

    if ((int)(actorID == 0xc) != 0) {
        id344 = mTargetActorID;
        if (id344 == 0) {
            ScaleOverTime(2, 3, 0x333, 0x1000);
            UpdateModelPosAndRotY();
            if (IsClsnInRange(0, 0) != 0) {
                UpdateClsnMatrix();
            }
            return 1;
        }
        if (dActor_c::FindWithID(id344) == 0) {
            mTargetActorID = 0;
            mAreaId = mHomeAreaId;
            return 1;
        }
    }

    if ((gNextActorUpdateMask & 0x4000000) == 0) {
        mTimer++;
    }
    CallStateBehavior();
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0) != 0) {
        UpdateClsnMatrix();
    }

    if (mSwitchType == 2) {
        if (mMusicFadeDone == 0) {
            v = mMusicVolume;
            if (v != 0x40) {
                if (v == 0x7f) {
                    if ((gNextActorUpdateMask & 0x4000000) == 0) {
                        mMusicFadeDone = Sound::ChangeMusicVolume(v, 0x64cc);
                    }
                }
            } else {
                mMusicFadeDone = Sound::ChangeMusicVolume(v, 0xc999);
            }
        }
    }

    mPlayerNearby = 0;
    return 1;
}

int daObjSwitch_c::Render()
{
    mModel.Render(&mDrawScale);
    return 1;
}

int daObjSwitch_c::CleanupResources()
{
    int t;
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov002_021098e8[mResourceIdx].model->Release();
    data_ov002_021098e8[mResourceIdx].collision->Release();
    t = actorID == 0xc;
    if (t != false) {
        UnloadSilverStarAndNumber();
        data_ov002_0211092c.Release();
    }
    return 1;
}

void daObjSwitch_c::CallStateBehavior() {
    int state = mState;
    (this->*data_ov002_0210e00c[state].exec)();
}

void daObjSwitch_c::SetState(int nextState) {
    mState = nextState;
    int state = mState;
    (this->*data_ov002_0210e00c[state].init)();
}

void daObjSwitch_c::ResetTimers() {
    mTimer = 0;
    mPressTimer = 5;
}

void daObjSwitch_c::StateIdleInit() {
    int ok = 1;
    dActor_c *p;
    mTimer = 0;
    mPressTimer = 5;
    mMusicFadeDone = 0;
    p = dActor_c::FindWithActorID(0xb, 0);
    while (p != 0) {
        if (p != this) {
            if (static_cast<daObjSwitch_c *>(p)->mState != 0) ok = 0;
        }
        p = dActor_c::FindWithActorID(0xb, p);
    }
    if (ok != 0) {
        p = dActor_c::FindWithActorID(0xc, 0);
        while (p != 0) {
            if (p != this) {
                if (static_cast<daObjSwitch_c *>(p)->mState != 0) ok = 0;
            }
            p = dActor_c::FindWithActorID(0xc, p);
        }
    }
    if (ok != 0) mMusicVolume = 0x7f;
}

void daObjSwitch_c::StateIdleMain() {
    if (mPlayerNearby) {
        if (DecIfAbove0_Byte(&mPressTimer) != 0) return;
        SetState(1);
        return;
    }
    mPressTimer = 5;
}

void daObjSwitch_c::StatePressedMain() {
    ScaleOverTime(2, 3, 0x1000, 0x333);
    if (mTimer != 3) return;
    Sound::PlayBank3(0x3e, mCamSpacePos);
    if ((int)(actorID == 0xc) != 0) {
        dActor_c *p = mTargetActor;
        if (p != 0) {
            static_cast<daStar_c *>(p)->SetSwitchState(1);
            mAreaId = -1;
        }
    } else {
        if (gCurrentSublevel == 0xd && mPosX == -0x140000) {
            SpawnSoundObj(0);
        }
        Event::SetBit(mEventBit);
    }
    SetState(2);
}

void daObjSwitch_c::StateTickingInit() {
    mTimer = 0;
    mPressTimer = 5;
    mMusicFadeDone = 0;
    mMusicVolume = 64;
}

void daObjSwitch_c::StateTickingMain() {
    int diff;
    if (mSwitchType == 0) return;
    if (mSwitchType == 1) {
        if (mPlayerNearby == 0) {
            SetState(3);
            return;
        }
    }
    diff = mTimeLimit - mTimer;
    if (!(gNextActorUpdateMask & 0x4000000)) {
        if (diff == 0x2d) {
            mTickSoundHandle = 0;
        } else if (diff < 0x2d) {
            mTickSoundHandle = func_02012310(mTickSoundHandle, 0x39, 0);
        } else {
            mTickSoundHandle = func_02012310(mTickSoundHandle, 0x38, 0);
        }
    }
    if (mTimer > mTimeLimit) {
        SetState(4);
        return;
    }
    if (mTargetActor == 0) return;
    if (static_cast<daStar_c *>(mTargetActor)->mState != 5) return;
    SetState(4);
}

void daObjSwitch_c::StateUnpressedMain() {
    ScaleOverTime(2, 3, 0x333, 0x1000);
    if (mTimer != 3) return;
    Sound::PlayBank3(0x3e, mCamSpacePos);
    {
        int b = (actorID == 0xc);
        if (b) {
            dActor_c *p = mTargetActor;
            if (p != 0 && static_cast<daStar_c *>(p)->mPlayer == 0) {
                static_cast<daStar_c *>(p)->SetSwitchState(0);
            }
            mAreaId = mHomeAreaId;
        } else {
            Event::ClearBit(mEventBit);
        }
    }
    SetState(0);
}

void daObjSwitch_c::StateExpiredMain() {
    unsigned short v = actorID;
    int b = (v == 0xc);
    if (!b) Event::ClearBit(mEventBit);
    SetState(3);
}

void daObjSwitch_c::ScaleOverTime(int mask, Fix12i frames, Fix12i base, Fix12i target) {
    Fix12i d = FX_Div((Fix12i)mTimer, frames);
    Fix12i diff = target - base;
    Fix12i e = FX_MUL(diff, d);
    Fix12i v = base + FX_Div(e, 0x1000);
    Fix12i h = FX_MUL(v, 0x3c000);
    mDisplacementY = 0x3c000 - h;
    if (mask & 1) mDrawScale.x = v;
    if (mask & 2) mDrawScale.y = v;
    if (mask & 4) mDrawScale.z = v;
}

void daObjSwitch_c::OnGroundPounded(dActor_c &other)
{
    if (mState != 0) return;
    SetState(1);
}

void daObjSwitch_c::UpdateClsnMatrix() {
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY - mDisplacementY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

int daObjSwitch_c::FindTargetStar() {
    dActor_c *star;
    if (mTargetActorID) return 0;
    star = dActor_c::FindWithActorID(0xb2, 0);
    while (star) {
        if (mStarID == static_cast<daStar_c *>(star)->mStarID) {
            mTargetActorID = star->uniqueID;
            return 1;
        }
        star = dActor_c::FindWithActorID(0xb2, star);
    }
    return 0;
}

/* The four file-scope handles, by Resources entry at data_ov002_021098e8:
 *   dfd4/dfc4  entry 0's model and collision (files 0x45c, 0x45d)
 *   dfcc/dfbc  entry 1's model and collision (files 0x48f, 0x490)
 * Their constructors and the destructor registrations make mwcc emit this TU's
 * static initializer; the five-entry state table adds the ten
 * pointer-to-member copies it performs afterwards. */
SwitchModelFilePtr data_ov002_0210dfd4(ASSET_OBJ_BOX_SWITCH_BMD);
SwitchClsnFileHandle  data_ov002_0210dfc4(ASSET_OBJ_BOX_SWITCH_KCL);
SwitchModelFilePtr data_ov002_0210dfcc(ASSET_OBJ_STAR_SWITCH_BMD);
SwitchClsnFileHandle  data_ov002_0210dfbc(ASSET_OBJ_STAR_SWITCH_KCL);

daObjSwitch_c::StateEntry data_ov002_0210e00c[5] = {
    {&daObjSwitch_c::StateIdleInit, &daObjSwitch_c::StateIdleMain},
    {&daObjSwitch_c::ResetTimers, &daObjSwitch_c::StatePressedMain},
    {&daObjSwitch_c::StateTickingInit, &daObjSwitch_c::StateTickingMain},
    {&daObjSwitch_c::ResetTimers, &daObjSwitch_c::StateUnpressedMain},
    {&daObjSwitch_c::ResetTimers, &daObjSwitch_c::StateExpiredMain},
};
