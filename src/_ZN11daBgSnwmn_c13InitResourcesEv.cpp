//cpp
// @symbol _ZN11daBgSnwmn_c13InitResourcesEv
/* recovered: named members + real C++ method */
/* daBgSnwmn_c::InitResources() -- vtable slot 0. Bails out (spawning actor
 * 0x111 in its place) if this level's star 5 is already collected. Otherwise
 * loads two models and the texture sequence, sets up the cylinder shadow and
 * collision, then drops the snowman onto the ground: a throwaway dBgCh_Gnd
 * probe 0x14000 above the spawn point finds the floor, and mPosY is set from
 * the hit (or the probe height) and raised 0xc3000. */
#include "daBgSnwmn_c.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"

extern "C" {
extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, void *pos, void *ang, int a, int b);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, u32 u);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *act, void *pos, int f1, int f2, u32 u1, u32 u2);
extern int data_ov072_02122c70[];
}

s32 daBgSnwmn_c::InitResources()
{
    Vector3 pos;
    void *m;

    if (IsStarCollectedInLevel(0xa, 5) == 0)
    {
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x111, 0, &mPosX, &mAngleX, mAreaId, -1);
        _ZN7fBase_c18MarkForDestructionEv(this);
    }

    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov072_02122c48);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel1, m, 1, 1);
    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov072_02122c40);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel2, m, 1, 1);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov072_02122c50);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void *)data_ov072_02122c48[1], (void *)data_ov072_02122c50[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTexSeq, (void *)data_ov072_02122c50[1], 0, 0x1000, 0);

    if (_ZN11ShadowModel12InitCylinderEv(&mShadow) == 0)
        return 0;

    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mCylClsn, this, data_ov072_02122c70, 0xc3000, 0x17c000, 0x800004, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mPosY += 0xc3000;
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mScaleX = 0x1800;
    mScaleY = 0x1800;
    mScaleZ = 0x1800;
    func_ov072_021208d8(this);
    return 1;
}
