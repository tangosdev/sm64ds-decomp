//cpp
// Timed floor switches and the switch which reveals a star.
// This TU contains 18 written bodies and two destructor variants emitted from
// daObjSwitch_c.h. Reverse source order preserves the retail text order with 2004/b56.
// Select the shared flat Matrix4x3 before the actor headers, as dBgActor_c.h requires.
//
// Leftover: the func_ov002_020b9xxx helpers and data_ov002_021098e8 table
//   keep linker names; naming belongs at their definitions.
// Leftover: ChangeMusicVolume / IsClsnInRange / KcMbg::SetFile keep
//   computed spellings (Fix12<int> by value, wall 6az -- noted at use).
#include "common.h"
#include "daObjSwitch_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "PowerStar.h"

extern daObjSwitch_c::StateEntry data_ov002_0210e00c[];

namespace cstd { Fix12i fdiv(Fix12i numerator, Fix12i denominator); }

namespace Event { int ClearBit(unsigned int bit); void SetBit(unsigned int bit); }

extern "C" {
    extern int data_0209b454;
    extern signed char data_0209f2f8;
    extern SharedFilePtr data_ov002_0211092c;
    extern daObjSwitch_c::Resources data_ov002_021098e8[];
    int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int id, int volume);
    int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
    // The actual free definition accepts a scalar scale. The genuine member
    // call changes InitResources under 2004/b56; see the committed experiment.
    void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *self, void *file, const Matrix4x3 *matrix, int scale, s16 angle, void *clps);
    // The existing setter stores a callback address as a 32-bit word at +0x1c.
    void func_020393c4(int *self, int callback);
    void func_ov002_020baa98(void *, void *, void *);
    int func_02012310(int handle, int sound, int arg);
    void func_ov002_020e6d88(void *star);
    void func_ov002_020e7104(void *star, int active);
    unsigned char IsAreaShowing(int area);
    unsigned char DecIfAbove0_Byte(unsigned char *value);
    void LoadSilverStarAndNumber();
    void UnloadSilverStarAndNumber();
    void func_ov002_020ba4d8(char *self, int state);
    void func_ov002_020ba01c(char *self, int axes, Fix12i frames, Fix12i from, Fix12i to);
    void func_ov002_020b9f80(char *self);
    int func_ov002_020b9f00(char *self);
    void func_ov002_020ba520(char *self);
}

// @symbol _ZN13daObjSwitch_c13InitResourcesEv
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
            Model::LoadFile(data_ov002_0211092c);
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
    f = Model::LoadFile(*data_ov002_021098e8[idx].model);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelPosAndRotY();
    func_ov002_020b9f80(reinterpret_cast<char *>(this));

    idx = mResourceIdx;
    f = dBgW_Kc::LoadFile(*data_ov002_021098e8[idx].collision);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, f, &mClsnMat, 0x199, mAngleY,
        data_ov002_021098e8[idx].clps);

    func_020393c4(reinterpret_cast<int *>(&mMeshCollider),
                  reinterpret_cast<int>(&func_ov002_020baa98));

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

// @symbol _ZN13daObjSwitch_c8BehaviorEv
int daObjSwitch_c::Behavior()
{
    dActor_c *a;
    int id344;
    int v;

    if (IsAreaShowing(mHomeAreaId) == 0) {
        mAreaId = mHomeAreaId;
        mTimer = 1;
        func_ov002_020ba01c(reinterpret_cast<char *>(this), 2, 1, 0x333, 0x1000);
        func_ov002_020ba4d8(reinterpret_cast<char *>(this), 0);
        a = dActor_c::FindWithID(mTargetActorID);
        if (a != 0) {
            func_ov002_020e6d88(a);
        }
    }

    {
        int isType = (int)(actorID == 0xc);
        if (isType != 0) {
            if (func_ov002_020b9f00(reinterpret_cast<char *>(this)) != 0) {
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
            func_ov002_020ba01c(reinterpret_cast<char *>(this), 2, 3, 0x333, 0x1000);
            UpdateModelPosAndRotY();
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0) {
                func_ov002_020b9f80(reinterpret_cast<char *>(this));
            }
            return 1;
        }
        if (dActor_c::FindWithID(id344) == 0) {
            mTargetActorID = 0;
            mAreaId = mHomeAreaId;
            return 1;
        }
    }

    if ((data_0209b454 & 0x4000000) == 0) {
        mTimer++;
    }
    func_ov002_020ba520(reinterpret_cast<char *>(this));
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0) {
        func_ov002_020b9f80(reinterpret_cast<char *>(this));
    }

    if (mSwitchType == 2) {
        if (mMusicFadeDone == 0) {
            v = mMusicVolume;
            if (v != 0x40) {
                if (v == 0x7f) {
                    if ((data_0209b454 & 0x4000000) == 0) {
                        mMusicFadeDone = _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(v, 0x64cc);
                    }
                }
            } else {
                mMusicFadeDone = _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(v, 0xc999);
            }
        }
    }

    mPlayerNearby = 0;
    return 1;
}

// @symbol _ZN13daObjSwitch_c6RenderEv
int daObjSwitch_c::Render()
{
    mModel.Render(&mDrawScale);
    return 1;
}

// @symbol _ZN13daObjSwitch_c16CleanupResourcesEv
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

// @symbol func_ov002_020ba520
extern "C" void func_ov002_020ba520(char *self)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(self);
    int state = actor->mState;
    (actor->*data_ov002_0210e00c[state].exec)();
}

// @symbol func_ov002_020ba4d8
extern "C" void func_ov002_020ba4d8(char *self, int nextState)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(self);
    actor->mState = nextState;
    int state = actor->mState;
    (actor->*data_ov002_0210e00c[state].init)();
}

// @symbol func_ov002_020ba4c0
extern "C" void func_ov002_020ba4c0(char *p)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(p);
    actor->mTimer = 0;
    actor->mPressTimer = 5;
}

// @symbol func_ov002_020ba3fc
extern "C" void func_ov002_020ba3fc(char *c)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(c);
    int ok = 1;
    dActor_c *p;
    actor->mTimer = 0;
    actor->mPressTimer = 5;
    actor->mMusicFadeDone = 0;
    p = dActor_c::FindWithActorID(0xb, 0);
    while (p != 0) {
        if (p != actor) {
            if (static_cast<daObjSwitch_c *>(p)->mState != 0) ok = 0;
        }
        p = dActor_c::FindWithActorID(0xb, p);
    }
    if (ok != 0) {
        p = dActor_c::FindWithActorID(0xc, 0);
        while (p != 0) {
            if (p != actor) {
                if (static_cast<daObjSwitch_c *>(p)->mState != 0) ok = 0;
            }
            p = dActor_c::FindWithActorID(0xc, p);
        }
    }
    if (ok != 0) actor->mMusicVolume = 0x7f;
}

// @symbol func_ov002_020ba3a8
extern "C" void func_ov002_020ba3a8(daObjSwitch_c *c)
{
    if (c->mPlayerNearby) {
        if (DecIfAbove0_Byte(&c->mPressTimer) != 0) return;
        func_ov002_020ba4d8(reinterpret_cast<char *>(c), 1);
        return;
    }
    c->mPressTimer = 5;
}

// @symbol func_ov002_020ba2d0
extern "C" void func_ov002_020ba2d0(char *c)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(c);
    func_ov002_020ba01c(c, 2, 3, 0x1000, 0x333);
    if (actor->mTimer != 3) return;
    Sound::PlayBank3(0x3e, *reinterpret_cast<const Vector3 *>(&actor->mCamSpacePosX));
    if ((int)(actor->actorID == 0xc) != 0) {
        dActor_c *p = actor->mTargetActor;
        if (p != 0) {
            func_ov002_020e7104(p, 1);
            actor->mAreaId = -1;
        }
    } else {
        if (data_0209f2f8 == 0xd && actor->mPosX == -0x140000) {
            actor->SpawnSoundObj(0);
        }
        Event::SetBit(actor->mEventBit);
    }
    func_ov002_020ba4d8(c, 2);
}

// @symbol func_ov002_020ba2ac
extern "C" void func_ov002_020ba2ac(char *p)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(p);
    actor->mTimer = 0;
    actor->mPressTimer = 5;
    actor->mMusicFadeDone = 0;
    actor->mMusicVolume = 64;
}

// @symbol func_ov002_020ba1ac
extern "C" void func_ov002_020ba1ac(char *self)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(self);
    int diff;
    if (actor->mSwitchType == 0) return;
    if (actor->mSwitchType == 1) {
        if (actor->mPlayerNearby == 0) {
            func_ov002_020ba4d8(self, 3);
            return;
        }
    }
    diff = actor->mTimeLimit - actor->mTimer;
    if (!(data_0209b454 & 0x4000000)) {
        if (diff == 0x2d) {
            actor->mTickSoundHandle = 0;
        } else if (diff < 0x2d) {
            actor->mTickSoundHandle = func_02012310(actor->mTickSoundHandle, 0x39, 0);
        } else {
            actor->mTickSoundHandle = func_02012310(actor->mTickSoundHandle, 0x38, 0);
        }
    }
    if (actor->mTimer > actor->mTimeLimit) {
        func_ov002_020ba4d8(self, 4);
        return;
    }
    if (actor->mTargetActor == 0) return;
    if (static_cast<PowerStar *>(actor->mTargetActor)->unk_440 != 5) return;
    func_ov002_020ba4d8(self, 4);
}

// @symbol func_ov002_020ba0f8
extern "C" void func_ov002_020ba0f8(char *c)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(c);
    func_ov002_020ba01c(c, 2, 3, 0x333, 0x1000);
    if (actor->mTimer != 3) return;
    Sound::PlayBank3(0x3e, *reinterpret_cast<const Vector3 *>(&actor->mCamSpacePosX));
    {
        int b = (actor->actorID == 0xc);
        if (b) {
            dActor_c *p = actor->mTargetActor;
            // This foreign star field is still unnamed in PowerStar.h.
            if (p != 0 && *reinterpret_cast<int *>(reinterpret_cast<char *>(p) + 0x438) == 0) {
                func_ov002_020e7104(p, 0);
            }
            actor->mAreaId = actor->mHomeAreaId;
        } else {
            Event::ClearBit(actor->mEventBit);
        }
    }
    func_ov002_020ba4d8(c, 0);
}

// @symbol func_ov002_020ba0bc
extern "C" void func_ov002_020ba0bc(char *c)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(c);
    unsigned short v = actor->actorID;
    int b = (v == 0xc);
    if (!b) Event::ClearBit(actor->mEventBit);
    func_ov002_020ba4d8(c, 3);
}

// @symbol func_ov002_020ba01c
extern "C" void func_ov002_020ba01c(char *c, int mask, Fix12i b, Fix12i base, Fix12i target)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(c);
    Fix12i d = cstd::fdiv((Fix12i)actor->mTimer, b);
    Fix12i diff = target - base;
    Fix12i e = (Fix12i)(((long long)diff * d + 0x800) >> 12);
    Fix12i v = base + cstd::fdiv(e, 0x1000);
    Fix12i h = (Fix12i)(((long long)v * 0x3c000 + 0x800) >> 12);
    actor->mDisplacementY = 0x3c000 - h;
    if (mask & 1) actor->mDrawScale.x = v;
    if (mask & 2) actor->mDrawScale.y = v;
    if (mask & 4) actor->mDrawScale.z = v;
}

// @symbol _ZN13daObjSwitch_c15OnGroundPoundedER8dActor_c
void daObjSwitch_c::OnGroundPounded(dActor_c &other)
{
    if (mState != 0) return;
    func_ov002_020ba4d8(reinterpret_cast<char *>(this), 1);
}

// @symbol func_ov002_020b9f80
extern "C" void func_ov002_020b9f80(char *self)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(self);

    actor->mClsnMat = actor->mModel.mat4x3;
    actor->mClsnMat.m[9] = actor->mPosX;
    actor->mClsnMat.m[10] = actor->mPosY - actor->mDisplacementY;
    actor->mClsnMat.m[11] = actor->mPosZ;
    actor->mMeshCollider.Transform(actor->mClsnMat, actor->mAngleY);
}

// @symbol func_ov002_020b9f00
extern "C" int func_ov002_020b9f00(char *self)
{
    daObjSwitch_c *actor = reinterpret_cast<daObjSwitch_c *>(self);
    dActor_c *star;
    if (actor->mTargetActorID) return 0;
    star = dActor_c::FindWithActorID(0xb2, 0);
    while (star) {
        if (actor->mStarID == static_cast<PowerStar *>(star)->unk_49d) {
            actor->mTargetActorID = star->uniqueID;
            return 1;
        }
        star = dActor_c::FindWithActorID(0xb2, star);
    }
    return 0;
}
