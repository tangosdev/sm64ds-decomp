//cpp
// @symbol _ZN6Bowser13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * The other half of Bowser::CleanupResources. Every handle this loads is one
 * the cleanup releases, in the same order and with the same counts -- one
 * single, a 0x1c-entry table, a six-entry table, then two more singles, the
 * last of which (data_ov089_02132c50) lives in ov089 rather than this overlay.
 * That pairing is why the siblings' CleanupResources are bare `return 1`s:
 * Bowser loads the whole fight's resources, so Bowser frees them.
 *
 * The two loops are reproduced rather than unrolled, for the same reason as in
 * the cleanup: 0x1c and 6 are the counts the ROM's own comparisons test.
 *
 * `Vector3 pos` was a local shadow typedef; it is the real types.h Vector3
 * here, which is layout-identical (Fix12i is s32) and costs nothing.
 *
 * The fields this used to spell as unk_ are the base classes' and are named now:
 * unk_09c / unk_0a0 are Actor::mVertAccel and Actor::mTerminalVelocity -- and the
 * values written here, -0x2000 and -0x3c000, are fix12 gravity and terminal
 * velocity, which is the same evidence Actor.h cites from BooCage and MadPiano.
 * mParam is ActorBase::param1, unk_004 is ActorBase::uniqueID, and unk_0cc is
 * Actor::mAreaId -- which is why it is read as a signed char and handed straight
 * to Actor::Spawn's areaID parameter.
 *
 * The early `return 0` when ShadowModel::InitCylinder fails is the ROM's -- the
 * only failure path in the function.
 */
#include "Bowser.h"
#include "TextureSequence.h"

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int b, unsigned int d);
extern void _ZN9Animation8SetFlagsEi(void *self, int flags);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *act, void *pos, int c3, int d, unsigned int e, unsigned int f);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, int a, int b, void *d1, void *d2);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *dir, int e, int f);
extern void func_ov060_02111cc0(void *c, int a, int b);
extern void func_ov060_021123dc(void *c);
extern void func_02011d50(void *a);

extern int data_ov060_0211ac78[];
extern int data_ov060_021192dc[];
extern int data_ov060_0211927c[];
extern int data_ov060_0211b208[];
extern int data_ov089_02132c50[];
extern int data_ov060_0211ac28[];
}

int Bowser::InitResources()
{
    int i;
    Vector3 pos;
    void *a1;
    void *a2;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(&this->mModelAnim,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov060_0211ac78), 1, 0x16);

    for (i = 0; i < 0x1c; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void *)data_ov060_021192dc[i]);

    for (i = 0; i < 6; i++)
        _ZN15TextureSequence8LoadFileER13SharedFilePtr((void *)data_ov060_0211927c[i]);

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov060_0211b208);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov089_02132c50);

    if (_ZN11ShadowModel12InitCylinderEv(&this->mShadowModel) == 0)
        return 0;

    func_ov060_02111cc0(this, 0x10, 0);

    TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78[1],
                             *(BTP_File *)data_ov060_0211ac28[1]);

    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &this->mTextureSequence, (void *)data_ov060_0211ac28[1], 0, 0x1000, 0);

    _ZN9Animation8SetFlagsEi(&this->mTextureSequence, 0x40000000);

    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        &this->mMovingCylinderClsnWithPos, this, &pos, 0x78000, 0x118000, 0x200004, 0);

    this->unk_3b0 = this->mPosX;
    this->unk_3b4 = this->mPosY;
    this->unk_3b8 = this->mPosZ;
    this->mVertAccel = -0x2000;
    this->mTerminalVelocity = -0x3c000;
    this->mTargetPlayer = 0;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        &this->mWithMeshClsn, this, 0x50000, 0x50000, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv(&this->mWithMeshClsn);

    this->unk_40c = 0;
    this->unk_414 = (char)(this->param1 & 3);
    this->unk_416 = (char)(((unsigned int)this->param1 >> 2) & 1);
    this->unk_3fc = 0;
    this->unk_423 = 0;
    this->unk_426 = 1;
    this->unk_427 = 0;
    this->mScaleX = 0x1000;
    this->mScaleY = 0x1000;
    this->mScaleZ = 0x1000;
    this->unk_3f8 = 0x1000;
    this->unk_429 = 1;
    func_ov060_021123dc(this);

    this->unk_424 = 0;
    this->unk_444 = 0;

    a1 = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
        0x118, 0, &this->mPosX, 0, this->mAreaId, -1);
    *(int *)((char *)a1 + 0x2cc) = this->uniqueID;

    a2 = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
        0x116, 0, &this->mPosX, 0, this->mAreaId, -1);
    this->unk_3a8 = *(int *)((char *)a2 + 4);
    *(int *)((char *)a2 + 0x108) = this->uniqueID;
    this->unk_42a = 5;
    this->unk_42b = 0;
    this->unk_448 = 0;
    this->unk_446 = 0;
    this->unk_44c = 0;
    this->unk_450 = 0;
    func_02011d50(a2);
    return 1;
}
