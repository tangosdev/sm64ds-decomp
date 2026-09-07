//cpp
// @symbol _ZN9WaterBomb13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * The local `Obj` shadow struct this file used to cast `this` to is gone --
 * `struct WaterBomb : dEnemyBase_c` types every offset it restated. Its tail fields
 * (0x3a8..0x3b6) moved into the header, since the shadow was the only place
 * they were ever written down.
 */
#include "WaterBomb.h"
#include "SharedFilePtr.h"

/* ModelBase::SetFile is declared void in include/ModelBase.h -- which is what its
   own matched definition compiles as -- but the ROM leaves DoSetFile's int in r0
   and this call site reads it, so the value-returning entry keeps the mangled
   spelling until that signature is settled. */
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, BMD_File *f, int a, int b);

/* Signatures deliberately kept as the local declarations: the ROM names carry
   by-value class parameters (Fix12<int>), which mwccarm passes differently at
   the call site, so declaring the true types breaks the byte match.
   See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, dActor_c* a, s32 r, s32 h, unsigned int d, unsigned int e);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *, dActor_c* a, s32 b, s32 c, Vector3_16* d, s32 e);

extern char data_ov002_0210da38[];
extern char data_ov098_0213c91c[];

int WaterBomb::InitResources()
{
    BMD_File* bmd;
    int kind;

    kind = param1 & 0xf;
    unk_3c8 = kind;

    if (unk_3c8 == 2)
    {
        bmd = (BMD_File*)Model::LoadFile(*(SharedFilePtr*)data_ov002_0210da38);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, bmd, 1, 0x16) == 0)
            return 0;
    }
    else
    {
        Model::LoadFile(*(SharedFilePtr*)data_ov002_0210da38);
        bmd = (BMD_File*)Model::LoadFile(*(SharedFilePtr*)data_ov098_0213c91c);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, bmd, 1, 0x16) == 0)
            return 0;
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (unk_3c8 == 0)
        unk_3c4 = 0;
    else
        unk_3c4 = 1;

    if (unk_3c8 != 0)
    {
        if (unk_3c8 == 2)
        {
            mScaleX = 0x800;
            mScaleY = 0x800;
            mScaleZ = 0x800;
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, (dActor_c*)this, 0x14000, 0x28000, 0x200004, 0);
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (dActor_c*)this, 0x1e000, 0x1e000, 0, 0);
        }
        else
        {
            mScaleX = 0x1000;
            mScaleY = 0x1000;
            mScaleZ = 0x1000;
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, (dActor_c*)this, 0x28000, 0x50000, 0x204004, 0);
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (dActor_c*)this, 0x32000, 0x32000, 0, 0);
        }
    }

    mStateTimer = 0;
    unk_3b6 = 0;
    unk_3b4 = 0;
    unk_3a8 = mPosX;
    unk_3ac = mPosY;
    unk_3b0 = mPosZ;

    if ((unsigned int)(unk_3c8 - 1) <= 1)
    {
        mVertAccel = -0x3000;
        mTerminalVelocity = -0x50000;
    }

    return 1;
}
