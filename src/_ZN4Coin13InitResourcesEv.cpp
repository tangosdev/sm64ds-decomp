//cpp
// @symbol _ZN4Coin13InitResourcesEv
/* Coin::InitResources -- vtable slot 0. Sets up one coin from its spawn
 * parameter: mBehaviorType is the low nibble of param1 and selects the physics
 * (bounce height, gravity, terminal velocity) and the flag bits in unk_3ae; the
 * actor ID then selects which of the three coin types it is (0x121 red, 0x122
 * blue, else yellow), which decides the models loaded and, for a red coin,
 * whether it claims a star-tracking slot. The last third builds the two models,
 * the shadow cylinder, the actor collider and the mesh collider, then sets the
 * disappear and no-collision timers.
 *
 * FOUR SITES KEEP RAW OFFSETS, and each one is measured, not left over:
 *
 *   unk_3ae through `(int)c`  -- every read-modify-write of the flag byte is
 *       spelled `*(u8*)(((int)c + 0x3ae))` in the ROM's codegen. Spelled as the
 *       member, the function changes size. The plain `*(u8*)(c + 0x3ae)` sites
 *       DO convert, and have; the launder is per-site, not per-field.
 *   *(u16*)(c + 0x3a8) = 0xffffu  -- the same field the s16 reads reach as
 *       mDisappearTimer, but this one store only reproduces through the raw
 *       unsigned spelling.
 *   *(Blob48*)(c + 0x368) = IDENTITY_MATRIX4X3  -- this is mShadowMat, a Matrix4x3.
 *       As a struct assignment C++ scalarizes the copy and the function changes
 *       size; as a 48-byte blob copy it is the memcpy the ROM has.
 *   *(s32*)(((int)c + 0x190))  -- inside the dCcAc_c sub-object at 0x178, whose
 *       own header does not name that word yet.
 *
 * `#pragma opt_common_subs off` is inherited from the C form and still load-
 * bearing: the ROM re-issues loads this compiler would otherwise CSE. */
#include "Coin.h"
#pragma opt_common_subs off

#define false 0
#define true 1

extern "C" {
typedef struct { void* sfp; void* bmd; } FileEntry;
typedef struct { u32 w[12]; } Blob48;

extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern int SublevelToLevel(int i);
extern void SetStarMarker(int i, void* actor, int v2);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, s32 f1, s32 f2, u32 a, u32 b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, s32 f1, s32 f2, void* v, s32 f3);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void* thiz);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void* thiz);

extern Blob48 IDENTITY_MATRIX4X3;
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern s32 data_0209f40c[];
extern u8 data_0209f2d8;
extern FileEntry* data_ov002_020ff06c[];
extern FileEntry* data_ov002_020ff060[];
extern void* data_ov002_0210d9a8;
}

s32 Coin::InitResources()
{
    char* c = (char*)this;
    s32 r5;
    s32 r4;
    s32 t;
    s8 i;
    s32 j;

    unk_3ae = 0;
    r5 = 0x64000;
    t = (s32)param1 & 0xf;
    mBehaviorType = t;
    r4 = 0x40000;
    t = mBehaviorType;
    if (t >= 9) {
        t = 1;
        mBehaviorType = t;
    }
    t = mBehaviorType;
    if (t == 8) {
        mVertSpeed = 0x14000;
        mVertAccel = -0x4000;
        mTerminalVelocity = -0x37000;
        *(u8*)(((int)c + 0x3ae)) =
            (*(u8*)(((int)c + 0x3ae)) & ~0xe0) | 0x40;
        goto shared140;
    }
    if (t == 1 || t == 7) {
        goto case17;
    }
    if (t == 5) {
        goto case5;
    }
    /* default */
    mVertSpeed = 0x24000;
    *(u8*)(((int)c + 0x3ae)) |= 2;
    goto block_11;
case5:
    {
        u32 flags = param1;
        if (flags & 8) {
            mPrevAngleY = (s16)(((flags & 0x70) << 8) + 0x8000);
        } else {
            mPrevAngleY = (s16)((flags & 0x70) << 8);
        }
    }
block_11:
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x37000;
    *(u8*)(((int)c + 0x3ae)) &= ~0xe0;
    goto shared140;
case17:
    *(u8*)(((int)c + 0x3ae)) =
        (*(u8*)(((int)c + 0x3ae)) & ~0xe0) | 0xe0;
    mFloorPosY = mPosY - 0x1f4000;
    if (mBehaviorType == 7) {
        r5 = 0x32000;
        r4 = 0x28000;
    }
shared140:;

    *(Blob48*)(c + 0x368) = IDENTITY_MATRIX4X3;

    mTrackStarID = -1;
    unk_3ab = 0xff;

    {
        u16 h;
        int b;
        h = actorID;
        b = h;
        b = (b == 0x121);
        if (b) {
            unk_3ab = (u8)((param1 >> 4) & 7);
            _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a8);
            mCoinType = 1;
            if (SublevelToLevel(data_0209f2f8) == 0x13 ||
                unk_3ab == data_0209f220) {
                if (GetBitInDeathTable() == 0) {
                    for (i = 0; i < 0xc; i = (s8)(i + 1)) {
                        if (data_0209f40c[i] == 0) {
                            SetStarMarker(i, this, 4);
                            mTrackStarID = i;
                            break;
                        }
                    }
                }
            }
        } else {
            int b2;
            b2 = h;
            b2 = (b2 == 0x122);
            if (b2) {
                unk_3ab = (u8)((param1 >> 4) & 7);
                mCoinType = 2;
            } else {
                mCoinType = 0;
            }
        }
    }

    j = mCoinType;
    if (j < 2) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel1, data_ov002_020ff06c[j]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel2, data_ov002_020ff060[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
    } else {
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_020ff06c[j]);
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_020ff060[mCoinType]);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel1, data_ov002_020ff06c[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel2, data_ov002_020ff060[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv(&mShadowModel) == 0) {
        return 0;
    }

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, this, r5, r4, 0x100002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x3c000, 0x3c000, 0, 0);
    _ZN10dBgCh_Actr13SetLimMovFlagEv(&mWithMeshClsn);
    _ZN10dBgCh_Actr19StartDetectingWaterEv(&mWithMeshClsn);

    t = mBehaviorType;
    if (t == 8) {
        mDisappearTimer = 0x2d;
        *(s32*)(((int)c + 0x190)) |= 1;
    } else {
        if (t == 6) {
            int b3;
            b3 = data_0209f2d8;
            b3 = (b3 == 1);
            if (b3 == false) {
                *(u16*)(c + 0x3a8) = 0xffffu;
                *(s32*)(((int)c + 0x190)) |= 1;
                goto after438;
            }
        }
        mDisappearTimer = 0xd2;
    after438:;
    }

    *(u8*)(((int)c + 0x3ae)) =
        (*(u8*)(((int)c + 0x3ae)) & ~1) | 1;

    t = mBehaviorType;
    if (t == 1 || t == 7) {
        goto case17b;
    }
    if (t == 5) {
        mNoClsnTimer = 0;
    } else {
        mNoClsnTimer = 0xf;
    }
    goto switch2end;
case17b:
    mNoClsnTimer = 0;
    if (mCoinType == 2 && (u32)unk_3ab < 8) {
        *(u8*)(((int)c + 0x3ae)) &= ~1;
        *(s32*)(((int)c + 0x190)) |= 1;
    }
switch2end:;

    *(u8*)(((int)c + 0x3ae)) &= ~0x1c;
    mPuzzleManagerID = 0;
    return 1;
}
