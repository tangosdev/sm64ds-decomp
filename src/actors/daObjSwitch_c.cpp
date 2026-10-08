//cpp
// Timed floor switches and the switch which reveals a star.
// Select the shared flat Matrix4x3 before the actor headers, as dBgActor_c.h requires.
//
// The 13 func_ov002_020b9xxx/020baxxx helpers are daObjSwitch_c members: each
// takes the object as arg0 and recasts it, and the .data records at
// 0x0210987c.. store {init, exec} pointer-to-member pairs over them
// (data_ov002_0210e00c). Reverse source order preserves the retail text order.
//
// comment leftovers:
//   - ChangeMusicVolume / IsClsnInRange / KcMbg::SetFile stay computed-spelling
//     externs: the member form changes codegen under 2004/b56 (Fix12<int> by
//     value, wall 6az -- noted at use).
//   - func_020393c4 stores a callback address as a 32-bit word at +0x1c; a
//     typed callback signature has no byte proof, so the int* decl stays.
//   - func_ov002_020baa98 is a foreign-overlay callback whose address is taken;
//     it keeps its free extern.
//   - The daStar_c fields at +0x438/+0x440 and unk_49d are unnamed upstream;
//     the int*/unk_ puns stay until daStar_c.h names them.
#include "common.h"
#include "daObjSwitch_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "daStar_c.h"

namespace cstd { Fix12i fdiv(Fix12i numerator, Fix12i denominator); }

namespace Event { int ClearBit(unsigned int bit); void SetBit(unsigned int bit); }

/* Model handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c and destroy
 * through SharedFilePtr_Destruct_Clsn. The manifest aliases those undefined
 * members onto the ROM symbols. */
struct SwitchModelFilePtr : SharedFilePtr {
    u32 words[2];

    SwitchModelFilePtr(u32 fileID);
    ~SwitchModelFilePtr();
};

struct SwitchClsnFileHandle : SharedFilePtr {
    u32 words[2];

    SwitchClsnFileHandle(u32 fileID);
    ~SwitchClsnFileHandle();
};

extern "C" {
    extern int data_0209b454;
    extern signed char data_0209f2f8;
    extern SharedFilePtr data_ov002_0211092c;
    extern daObjSwitch_c::Resources data_ov002_021098e8[];
    extern daObjSwitch_c::StateEntry data_ov002_0210e00c[];
    extern SwitchModelFilePtr data_ov002_0210dfd4;
    extern SwitchClsnFileHandle data_ov002_0210dfc4;
    extern SwitchModelFilePtr data_ov002_0210dfcc;
    extern SwitchClsnFileHandle data_ov002_0210dfbc;
    // local extern: Sound.h/dBgActor_c.h declare the members, but Fix12<int>
    // by value through the member call changes codegen under 2004/b56.
    int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int id, int volume);
    int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
    // local extern: the actual free definition accepts a scalar scale. The
    // genuine member call changes InitResources under 2004/b56; see the
    // committed experiment.
    void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *self, void *file, const Matrix4x3 *matrix, int scale, s16 angle, void *clps);
    // The existing setter stores a callback address as a 32-bit word at +0x1c.
    void func_020393c4(int *self, int callback);
    void func_ov002_020baa98(void *, void *, void *);
    int func_02012310(int handle, int sound, int arg);
    unsigned char IsAreaShowing(int area);
    unsigned char DecIfAbove0_Byte(unsigned char *value);
    void LoadSilverStarAndNumber();
    void UnloadSilverStarAndNumber();
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
    func_ov002_020b9f80();

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
        func_ov002_020ba01c(2, 1, 0x333, 0x1000);
        func_ov002_020ba4d8(0);
        a = dActor_c::FindWithID(mTargetActorID);
        if (a != 0) {
            static_cast<daStar_c *>(a)->func_ov002_020e6d88();
        }
    }

    {
        int isType = (int)(actorID == 0xc);
        if (isType != 0) {
            if (func_ov002_020b9f00() != 0) {
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
            func_ov002_020ba01c(2, 3, 0x333, 0x1000);
            UpdateModelPosAndRotY();
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0) {
                func_ov002_020b9f80();
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
    func_ov002_020ba520();
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0) {
        func_ov002_020b9f80();
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

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba520Ev
void daObjSwitch_c::func_ov002_020ba520()
{
    int state = mState;
    (this->*data_ov002_0210e00c[state].exec)();
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba4d8Ei
void daObjSwitch_c::func_ov002_020ba4d8(int nextState)
{
    mState = nextState;
    int state = mState;
    (this->*data_ov002_0210e00c[state].init)();
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba4c0Ev
void daObjSwitch_c::func_ov002_020ba4c0()
{
    mTimer = 0;
    mPressTimer = 5;
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba3fcEv
void daObjSwitch_c::func_ov002_020ba3fc()
{
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

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba3a8Ev
void daObjSwitch_c::func_ov002_020ba3a8()
{
    if (mPlayerNearby) {
        if (DecIfAbove0_Byte(&mPressTimer) != 0) return;
        func_ov002_020ba4d8(1);
        return;
    }
    mPressTimer = 5;
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba2d0Ev
void daObjSwitch_c::func_ov002_020ba2d0()
{
    func_ov002_020ba01c(2, 3, 0x1000, 0x333);
    if (mTimer != 3) return;
    Sound::PlayBank3(0x3e, *reinterpret_cast<const Vector3 *>(&mCamSpacePosX));
    if ((int)(actorID == 0xc) != 0) {
        dActor_c *p = mTargetActor;
        if (p != 0) {
            static_cast<daStar_c *>(p)->func_ov002_020e7104(1);
            mAreaId = -1;
        }
    } else {
        if (data_0209f2f8 == 0xd && mPosX == -0x140000) {
            SpawnSoundObj(0);
        }
        Event::SetBit(mEventBit);
    }
    func_ov002_020ba4d8(2);
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba2acEv
void daObjSwitch_c::func_ov002_020ba2ac()
{
    mTimer = 0;
    mPressTimer = 5;
    mMusicFadeDone = 0;
    mMusicVolume = 64;
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba1acEv
void daObjSwitch_c::func_ov002_020ba1ac()
{
    int diff;
    if (mSwitchType == 0) return;
    if (mSwitchType == 1) {
        if (mPlayerNearby == 0) {
            func_ov002_020ba4d8(3);
            return;
        }
    }
    diff = mTimeLimit - mTimer;
    if (!(data_0209b454 & 0x4000000)) {
        if (diff == 0x2d) {
            mTickSoundHandle = 0;
        } else if (diff < 0x2d) {
            mTickSoundHandle = func_02012310(mTickSoundHandle, 0x39, 0);
        } else {
            mTickSoundHandle = func_02012310(mTickSoundHandle, 0x38, 0);
        }
    }
    if (mTimer > mTimeLimit) {
        func_ov002_020ba4d8(4);
        return;
    }
    if (mTargetActor == 0) return;
    if (static_cast<daStar_c *>(mTargetActor)->unk_440 != 5) return;
    func_ov002_020ba4d8(4);
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba0f8Ev
void daObjSwitch_c::func_ov002_020ba0f8()
{
    func_ov002_020ba01c(2, 3, 0x333, 0x1000);
    if (mTimer != 3) return;
    Sound::PlayBank3(0x3e, *reinterpret_cast<const Vector3 *>(&mCamSpacePosX));
    {
        int b = (actorID == 0xc);
        if (b) {
            dActor_c *p = mTargetActor;
            // This foreign star field is still unnamed in daStar_c.h.
            if (p != 0 && *reinterpret_cast<int *>(reinterpret_cast<char *>(p) + 0x438) == 0) {
                static_cast<daStar_c *>(p)->func_ov002_020e7104(0);
            }
            mAreaId = mHomeAreaId;
        } else {
            Event::ClearBit(mEventBit);
        }
    }
    func_ov002_020ba4d8(0);
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba0bcEv
void daObjSwitch_c::func_ov002_020ba0bc()
{
    unsigned short v = actorID;
    int b = (v == 0xc);
    if (!b) Event::ClearBit(mEventBit);
    func_ov002_020ba4d8(3);
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020ba01cEiiii
void daObjSwitch_c::func_ov002_020ba01c(int mask, Fix12i b, Fix12i base, Fix12i target)
{
    Fix12i d = cstd::fdiv((Fix12i)mTimer, b);
    Fix12i diff = target - base;
    Fix12i e = (Fix12i)(((long long)diff * d + 0x800) >> 12);
    Fix12i v = base + cstd::fdiv(e, 0x1000);
    Fix12i h = (Fix12i)(((long long)v * 0x3c000 + 0x800) >> 12);
    mDisplacementY = 0x3c000 - h;
    if (mask & 1) mDrawScale.x = v;
    if (mask & 2) mDrawScale.y = v;
    if (mask & 4) mDrawScale.z = v;
}

// @symbol _ZN13daObjSwitch_c15OnGroundPoundedER8dActor_c
void daObjSwitch_c::OnGroundPounded(dActor_c &other)
{
    if (mState != 0) return;
    func_ov002_020ba4d8(1);
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020b9f80Ev
void daObjSwitch_c::func_ov002_020b9f80()
{
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY - mDisplacementY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

// @symbol _ZN13daObjSwitch_c19func_ov002_020b9f00Ev
int daObjSwitch_c::func_ov002_020b9f00()
{
    dActor_c *star;
    if (mTargetActorID) return 0;
    star = dActor_c::FindWithActorID(0xb2, 0);
    while (star) {
        if (mStarID == static_cast<daStar_c *>(star)->unk_49d) {
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
SwitchModelFilePtr data_ov002_0210dfd4(0x45c);
SwitchClsnFileHandle  data_ov002_0210dfc4(0x45d);
SwitchModelFilePtr data_ov002_0210dfcc(0x48f);
SwitchClsnFileHandle  data_ov002_0210dfbc(0x490);

daObjSwitch_c::StateEntry data_ov002_0210e00c[5] = {
    {&daObjSwitch_c::func_ov002_020ba3fc, &daObjSwitch_c::func_ov002_020ba3a8},
    {&daObjSwitch_c::func_ov002_020ba4c0, &daObjSwitch_c::func_ov002_020ba2d0},
    {&daObjSwitch_c::func_ov002_020ba2ac, &daObjSwitch_c::func_ov002_020ba1ac},
    {&daObjSwitch_c::func_ov002_020ba4c0, &daObjSwitch_c::func_ov002_020ba0f8},
    {&daObjSwitch_c::func_ov002_020ba4c0, &daObjSwitch_c::func_ov002_020ba0bc},
};
