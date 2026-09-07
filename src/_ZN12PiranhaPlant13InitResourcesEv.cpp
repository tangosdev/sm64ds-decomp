//cpp
// @symbol _ZN12PiranhaPlant13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * This was still `extern "C" int _ZN12PiranhaPlant13InitResourcesEv(char* c)`
 * working raw offsets -- a file renamed .cpp without ever being migrated. It is
 * a real method now and every offset is a field.
 *
 * Unlike FirePiranhaPlantBig's, this one DOES check its loads: a failed SetFile
 * on either model returns 0 rather than carrying on.
 *
 * The tail computes where the plant's fire comes from: 0xe0 along the facing
 * angle out of the shared sin/cos table at data_02082214, and 0x37800 above the
 * spawn position.
 *
 * SetAnim and the three Init calls keep extern "C" declarations with scalar
 * slots: they carry Fix12<int> BY VALUE, which mwccarm passes differently at
 * the call site, so spelling the true types breaks the byte match -- see
 * notes/mwccarm-codegen.md 6az.
 */
#include "PiranhaPlant.h"
#include "SharedFilePtr.h"

extern "C" {
void LoadBlueCoinModel(void* c);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* a, int r, int h, unsigned int e, unsigned int g);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, void* a, const Vector3* v, int r, int h, unsigned int e, unsigned int g);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* a, int b, int cc, void* d, void* e);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int a, int b, unsigned int cc);

extern SharedFilePtr* data_ov084_021302f4[];
extern struct { int id; void* file; } data_ov084_02130dfc;
extern struct { int id; void* file; } data_ov002_0210da38;
extern struct { int id; void* file; } data_ov084_02130df4;
extern s16 data_02082214[];
}

int PiranhaPlant::InitResources()
{
    int i;
    Vector3 v;
    for (i = 0; i < 6; i++)
        Animation::LoadFile(*data_ov084_021302f4[i]);
    LoadBlueCoinModel(this);
    Model::LoadFile(*(SharedFilePtr *)&data_ov084_02130dfc);
    Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210da38);
    if (mModelAnim.SetFile((BMD_File *)data_ov084_02130dfc.file, 1, -1) == 0)
        return 0;
    if (mModel.SetFile((BMD_File *)data_ov002_0210da38.file, 1, -1) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c1, this, 0x32000, 0x64000, 0x200004, 0x423e0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c2, this, 0x82000, 0x64000, 2, 0x423e0);
    v.x = 0; v.y = 0; v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x46000, 0x64000, 0x200002, 0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mState = 0;
    mInitAngleY = mPrevAngleY;
    unk_464 = 0x7fffffff;
    unk_460 = 0;
    mClsnEnabled = 0;
    unk_45d = 1;
    mStateTimer = 0;
    unk_108 = 3;
    unk_46c = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
    mSpawnPos.x = mPosX;
    mSpawnPos.y = mPosY;
    mSpawnPos.z = mPosZ;
    {
        s16 *tbl = data_02082214;
        /* The add sits INSIDE the integer cast, which is load-bearing here:
           not interchangeable with `&mSpawnPos`. */
        Vector3* home = (Vector3*)(((int)this + 0x440));
        mHomePos = *home;
        /* The shift must be LOGICAL so the angle wraps -- on the signed s16 it
           would index the wrong table entry for negative angles. */
        unsigned short angh = *(unsigned short*)&mAngleY;
        int ang = angh >> 4;
        int y0 = mPosY;
        int z0 = mPosZ;
        int cosv = tbl[(ang << 1) + 1];
        int sinv = tbl[ang << 1];
        int z = cosv * 0xe0 + z0;
        int y = y0 + 0x37800;
        int x = sinv * 0xe0 + mPosX;
        mFirePos.x = x;
        mFirePos.y = y;
        mFirePos.z = z;
    }
    unk_474 = 0;
    mParticleHandle = unk_474;
    unk_478 = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
    return 1;
}
