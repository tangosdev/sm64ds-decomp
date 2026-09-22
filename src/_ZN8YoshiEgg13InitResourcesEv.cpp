//cpp
// @symbol _ZN8YoshiEgg13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 0. This was the last YoshiEgg function still written as a C free
 * function reaching every field through a raw offset -- it named not one member,
 * which is why it could not even include its own header once YoshiEgg became a
 * real dEnemyBase_c subclass.
 */
#include "YoshiEgg.h"
#include "SharedFilePtr.h"

extern "C" {
/* ov002's egg model table. Each entry is a SharedFilePtr, whose +4 is the file
   the shared loader put there; index 1 is the variant that also gets the cuboid
   shadow rather than the cylinder. */
extern char *data_ov002_021000a0[];
extern char data_ov002_0210e6b0[];
extern char data_ov002_0210eb78[];

/* All three carry Fix12<int> parameters, so their definitions stay mangled free
   functions -- see the note in include/ShadowModel.h about CW homing class-typed
   by-value parameters. A CALL is unaffected, it emits the same symbol either way.
   extern "C" so the reference is mangled once and not twice. */
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        dCcAc_c *self, dActor_c *actor, int radius, int height, u32 d, u32 e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        dBgCh_Actr *self, dActor_c *actor, int radius, int height, void *a, void *b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        ModelAnim *self, void *animFile, int flags, int speed, u32 startFrame);

unsigned char _ZN8dActor_c9TrackStarEjj(dActor_c *self, u32 star, u32 kind);

int func_ov002_020ec654(void *self);
int func_ov002_020ec628(void *self);
int func_ov002_020ec610(void *self);
void LoadBlueCoinModel(void *self);
}

#pragma opt_strength_reduction off

int YoshiEgg::InitResources()
{
    int idx;
    int i;

    unk_428 = (u8)(param1 >> 4);

    idx = 0;
    if (func_ov002_020ec654((char *)this) != 0)
        idx = 1;

    Animation::LoadFile(*(SharedFilePtr *)data_ov002_0210e6b0);
    Animation::LoadFile(*(SharedFilePtr *)data_ov002_0210eb78);
    if (mModelAnim.SetFile(*(BMD_File **)(data_ov002_021000a0[idx] + 4), 1, -1) == 0)
        return 0;

    /* The predicate is asked a SECOND time rather than reusing idx: the ROM calls
       0x020ec654 twice, and folding it into the index above loses a bl. */
    if (func_ov002_020ec654((char *)this) == 0) {
        if (mShadowModel.InitCylinder() == 0)
            return 0;
    } else {
        if (mShadowModel.InitCuboid() == 0)
            return 0;
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, *(void **)(data_ov002_0210eb78 + 4), 0, 0x1000, 0);

    mAreaId = -1;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    unk_3f0 = param1 & 3;

    switch (unk_3f0) {
    case 0:
    case 1:
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0x46000, 0x8c000, 0x200002, 0xa08000);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
            &mWithMeshClsn, this, 0x28000, 0x28000, 0, 0);
        break;
    case 2:
        mScaleX = 0x2000;
        mScaleY = 0x2000;
        mScaleZ = 0x2000;
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0x78000, 0xa0000, 0x200002, 0xa08000);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
            &mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
        break;
    default:
        break;
    }

    for (i = 0; i < 5; i++)
        unk_3fc[i] = 0;

    unk_3ea = mAngleX;
    unk_3ec = mAngleY;
    unk_3ee = mAngleZ;
    unk_3e4 = mAngleX;
    unk_3e6 = mAngleY;
    unk_3e8 = mAngleZ;
    unk_41f = 0xf;

    if (func_ov002_020ec628((char *)this) != 0)
        LoadBlueCoinModel((char *)this);
    if (func_ov002_020ec610((char *)this) != 0)
        mStarSlot = _ZN8dActor_c9TrackStarEjj(this, 0, 1);
    return 1;
}
