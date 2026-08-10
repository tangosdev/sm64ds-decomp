//cpp
// @symbol _ZN7Chuckya13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Claims six shared files -- one model and five animations -- builds the
 * shadow and both collision volumes, seeds mHomePos and mPrevPos from mPos,
 * and enters the starting state.
 *
 * Note it does NOT check any of the loads: unlike KoopaShell::InitResources,
 * which bails on a failed SetFile, this one returns 1 unconditionally.
 *
 * The two Init calls keep extern "C" declarations with scalar slots: both
 * carry Fix12<int> BY VALUE in their real signatures, which mwccarm passes
 * differently at the call site, so spelling the true types breaks the byte
 * match -- notes/mwccarm-codegen.md 6az.
 */
#include "Chuckya.h"
#include "SharedFilePtr.h"

typedef struct PMF PMF;

extern "C" {
extern SharedFilePtr data_ov062_0211ddf0;
extern SharedFilePtr data_ov062_0211dde8;
extern SharedFilePtr data_ov062_0211dde0;
extern SharedFilePtr data_ov062_0211de00;
extern SharedFilePtr data_ov062_0211de08;
extern SharedFilePtr data_ov062_0211ddf8;
extern PMF data_ov062_0211dee0;
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, Actor* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, int r, int h, Vector3_16* p, Vector3_16* q);
extern int Chuckya_ChangeState(void* c, PMF* p);
}

int Chuckya::InitResources()
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov062_0211ddf0), 1, -1);
    _ZN11ShadowModel12InitCylinderEv(&mShadowModel);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov062_0211dde8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov062_0211dde0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov062_0211de00);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov062_0211de08);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov062_0211ddf8);
    mVertAccel = -0x3000;
    mTerminalVelocity = -0x1e000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(&mCylinderClsn, (Actor*)this, 0xc8000, 0xfa000, 0x200004, 0x3010);
    mModel.speed = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(&mMeshClsn, (Actor*)this, 0x118000, 0x118000, 0, 0);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_108 = 1;
    unk_10a = 4;
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    Chuckya_ChangeState(this, &data_ov062_0211dee0);
    return 1;
}
