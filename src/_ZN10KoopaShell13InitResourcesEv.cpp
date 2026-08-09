//cpp
// @symbol _ZN10KoopaShell13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Builds the shell: model, shadow, both collision volumes, initial motion, and
 * the starting state. Any failure returns 0 and the spawn is abandoned.
 *
 * One spawn word does double duty. mSpawnParam bit 0 picks which of the two
 * shared model files to load and is kept in mModelIndex so CleanupResources
 * can release the same one; bit 4 goes to unk_3c5 as a behaviour variant.
 *
 * mPos is copied into mSpawnPos here, which is what makes the spawn point
 * recoverable later.
 *
 * The two Init calls keep extern "C" declarations with scalar slots: both
 * carry Fix12<int> BY VALUE in their real signatures, which mwccarm passes
 * differently at the call site, so spelling the true types breaks the byte
 * match -- notes/mwccarm-codegen.md 6az.
 */
#include "KoopaShell.h"

typedef int Fix12;
struct BMD_File;
struct SharedFilePtr { int h; };
struct Actor;
struct Vector3_16;

struct Model {
    static BMD_File* LoadFile(SharedFilePtr& f);
};
struct ModelBase {
    int SetFile(BMD_File* f, int a, int b);
};
struct ShadowModel {
    int InitCylinder();
};
struct MovingCylinderClsn {
    void Init(Actor* a, Fix12 r, Fix12 h, unsigned int d, unsigned int e);
};
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *, Actor* a, Fix12 r, Fix12 h, unsigned int d, unsigned int e);

struct WithMeshClsn {
    void Init(Actor* a, Fix12 b, Fix12 c, Vector3_16* d, Fix12 e);
    void StartDetectingWater();
};
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *, Actor* a, Fix12 b, Fix12 c, Vector3_16* d, Fix12 e);

extern "C" void func_ov102_0214d1f8(void* c, void* p);

extern SharedFilePtr* data_ov102_0214d70c[];
extern void* data_ov102_0214ea68;

int KoopaShell::InitResources()
{
    BMD_File* bmd;
    mModelIndex = (unsigned char)(mSpawnParam & 1);
    unk_3c5 = (unsigned char)((mSpawnParam >> 4) & 1);
    bmd = Model::LoadFile(*data_ov102_0214d70c[mModelIndex]);
    if (((ModelBase*)&mModel)->SetFile(bmd, 1, 1) == 0)
        return 0;
    if (((ShadowModel*)&mShadowModel)->InitCylinder() == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj((MovingCylinderClsn*)&mCylinderClsn, (Actor*)this, 0x3c000, 0x46000, 0x100004, 0xa083c0);
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mSpeed = -0x2000;
    unk_0a0 = -0x32000;
    unk_100 = 0x14;
    unk_3c0 = 0;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_((WithMeshClsn*)&mMeshClsn, (Actor*)this, 0x28000, 0x28000, 0, 0);
    ((WithMeshClsn*)&mMeshClsn)->StartDetectingWater();
    func_ov102_0214d1f8(this, &data_ov102_0214ea68);
    unk_3d4 = 0;
    unk_3d0 = unk_3d4;
    unk_3cc = unk_3d0;
    unk_3c8 = unk_3cc;
    return 1;
}
