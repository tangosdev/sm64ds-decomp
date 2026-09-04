//cpp
/* Reconstructed ov072/daBgSnwmn_c translation unit.
 *
 * The RTTI-backed class run is 0x02120824..0x02120c00. The registry-backed
 * factory at 0x02120c00 allocates exactly sizeof(daBgSnwmn_c), installs this
 * class's vtable, constructs its five members, and ends at the next class's
 * D1 at 0x02120c58. That direct registry/factory evidence supports testing
 * the combined nine-function TU; tu_map alone does not prove the join.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * factory first. The inline destructor declared last in daBgSnwmn_c emits the
 * retail D1/D0 pair first and emits no D2 body.
 */

#include "daBgSnwmn_c.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"

struct Matrix4x3;
struct ShadowModel;

extern "C" {
extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 id, u32 param, void *pos, void *ang, int areaID, int unk);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *file);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(
    void *self, void *file, int unk0, int unk1);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *file);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
    void *modelFile, void *textureFile);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    void *self, void *textureFile, int animation, int speed, u32 flags);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, void *actor, const void *offset, int radius, int height,
    u32 flags, u32 unk);
extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern int _ZN15TextureSequence6UpdateER15ModelComponents(
    void *self, void *modelComponents);
extern void Matrix4x3_FromRotationY(void *matrix, int angle);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, ShadowModel *shadow, Matrix4x3 *matrix, int radius,
    int height, u32 flags);
extern void func_ov072_021208d8(void *self);
}

extern const Vector3 data_ov072_02122c70;

struct SnwmnProfile {
    daBgSnwmn_c *(*classInit)();
    s16 profileID;
    s16 groupFlags;
    u32 actorFlags;
    Fix12i cullRadiusX;
    Fix12i cullRadiusY;
    u32 executeOrder;
    u32 drawOrder;
};

typedef char SnwmnProfile_size_must_be_0x1c[
    sizeof(SnwmnProfile) == 0x1c ? 1 : -1];

/* Reconstructed source-style names. SM64DS directly preserves the class RTTI,
 * BIG_SNOWMAN ID, descriptor relationship, and factory behavior. */
// @symbol daBgSnwmn_c_classInit
extern "C" daBgSnwmn_c *daBgSnwmn_c_classInit()
{
    return new daBgSnwmn_c();
}

extern "C" SnwmnProfile g_profile_BIG_SNOWMAN = {
    daBgSnwmn_c_classInit,
    0x0110,
    0x0083,
    3,
    1,
    0x0012c000,
    0x01000000,
    0x00001000
};

// @symbol _ZN11daBgSnwmn_c13InitResourcesEv
s32 daBgSnwmn_c::InitResources()
{
    struct Position {
        s32 x;
        s32 y;
        s32 z;
    } pos;
    void *model;

    if (IsStarCollectedInLevel(0xa, 5) == 0) {
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0x111, 0, &mPosX, &mAngleX, mAreaId, -1);
        _ZN7fBase_c18MarkForDestructionEv(this);
    }

    model = _ZN5Model8LoadFileER13SharedFilePtr(data_ov072_02122c48);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel1, model, 1, 1);
    model = _ZN5Model8LoadFileER13SharedFilePtr(data_ov072_02122c40);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel2, model, 1, 1);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov072_02122c50);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
        (void *)data_ov072_02122c48[1], (void *)data_ov072_02122c50[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTexSeq, (void *)data_ov072_02122c50[1], 0, 0x1000, 0);

    if (_ZN11ShadowModel12InitCylinderEv(&mShadow) == 0)
        return 0;

    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mCylClsn, this, &data_ov072_02122c70, 0xc3000, 0x17c000,
        0x800004, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(*reinterpret_cast<Vector3 *>(&pos), 0);
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

// @symbol _ZN11daBgSnwmn_c8BehaviorEv
s32 daBgSnwmn_c::Behavior()
{
    mTexSeq.Advance();
    mCylClsn.SetPosRelativeToActor(data_ov072_02122c70);
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}

struct SnwmnModelRender {
    virtual int Unk0();
    virtual int Unk1();
    virtual int Unk2();
    virtual int Unk3();
    virtual int Unk4();
    virtual int Render(void *scale);
};

// @symbol _ZN11daBgSnwmn_c6RenderEv
s32 daBgSnwmn_c::Render()
{
    _ZN15TextureSequence6UpdateER15ModelComponents(
        (char *)this + 0x174, (char *)this + 0xdc);
    reinterpret_cast<SnwmnModelRender *>((char *)this + 0xd4)->Render(
        (char *)this + 0x80);
    reinterpret_cast<SnwmnModelRender *>((char *)this + 0x124)->Render(
        (char *)this + 0x80);
    return 1;
}

// @symbol _ZN11daBgSnwmn_c16OnPendingDestroyEv
void daBgSnwmn_c::OnPendingDestroy()
{
}

// @symbol _ZN11daBgSnwmn_c16CleanupResourcesEv
s32 daBgSnwmn_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c48);
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c40);
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c50);
    return 1;
}

// @symbol func_ov072_021208d8
extern "C" void func_ov072_021208d8(void *self)
{
    unsigned char *actor = static_cast<unsigned char *>(self);

    Matrix4x3_FromRotationY(actor + 0xf0, *reinterpret_cast<short *>(actor + 0x8e));
    *reinterpret_cast<int *>(actor + 0x114) =
        *reinterpret_cast<int *>(actor + 0x5c) >> 3;
    *reinterpret_cast<int *>(actor + 0x118) =
        (*reinterpret_cast<int *>(actor + 0x60) + 0x13b000) >> 3;
    *reinterpret_cast<int *>(actor + 0x11c) =
        *reinterpret_cast<int *>(actor + 0x64) >> 3;

    Matrix4x3_FromRotationY(actor + 0x140, *reinterpret_cast<short *>(actor + 0x8e));
    *reinterpret_cast<int *>(actor + 0x164) =
        *reinterpret_cast<int *>(actor + 0x5c) >> 3;
    *reinterpret_cast<int *>(actor + 0x168) =
        *reinterpret_cast<int *>(actor + 0x60) >> 3;
    *reinterpret_cast<int *>(actor + 0x16c) =
        *reinterpret_cast<int *>(actor + 0x64) >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        actor, reinterpret_cast<ShadowModel *>(actor + 0x188),
        reinterpret_cast<Matrix4x3 *>(actor + 0x140), 0xe6000, 0x12c000, 0xf);
}
