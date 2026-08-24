//cpp
// @symbol _ZN19FirePiranhaPlantBig13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * This was still `extern "C" int _ZN19FirePiranhaPlantBig13InitResourcesEv(
 * char *c)` working raw offsets -- a file that had been renamed .cpp without
 * ever being migrated. It is a real method now and every offset is a field.
 *
 * The three actorIDs pick the variant: 0xfc and 0xfd are the two scripted
 * plants, and anything else is the free-standing one that gets a collision
 * volume sized 0x64000 square and consults the death table for whether it has
 * already been beaten.
 *
 * SetAnim and both Init calls keep extern "C" declarations with scalar slots:
 * they carry Fix12<int> BY VALUE, which mwccarm passes differently at the call
 * site, so spelling the true types breaks the byte match -- see
 * notes/mwccarm-codegen.md 6az.
 */
#include "FirePiranhaPlantBig.h"
#include "SharedFilePtr.h"

extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int b, unsigned int cc);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *a, int r, int h, unsigned int e, unsigned int g);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *a, Vector3 *v, int r, int h, unsigned int e, unsigned int g);
void LoadBlueCoinModel(void *c);

extern SharedFilePtr data_ov084_02130dfc;
extern SharedFilePtr *data_ov084_021302f4[];
extern SharedFilePtr data_ov002_0210da38;
extern struct { int id; void *file; } data_ov084_02130df4;
}

int FirePiranhaPlantBig::InitResources()
{
    int i;
    Vector3 v;
    int id;
    int cond;

    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov084_02130dfc), 1, -1);

    for (i = 0; i < 6; i++)
        Animation::LoadFile(*data_ov084_021302f4[i]);

    Model::LoadFile(data_ov002_0210da38);
    LoadBlueCoinModel(this);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, data_ov084_02130df4.file, 0x40000000, 0x1000, 0);

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0, 0, 0x200001, 0x66fe0);

    v.x = 0;
    v.y = 0;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, &v, 0x4b000, 0x64000, 0x200002, 0x66fe0);

    mScale = 0;
    unk_1e8 = 0;
    mState = 0;
    unk_1f0 = 0;
    unk_1f4 = 0;
    unk_21a = 0;
    unk_21b = 0;
    unk_21c = 0;
    unk_21d = 0;
    unk_21e = 1;
    unk_228 = 0;
    unk_224 = unk_228;

    id = actorID;
    cond = (id == 0xfc);
    if (cond != 0) {
        mClsnRadiusFactor = 0x3c;
        mClsnHeightFactor = 0xaa;
        mMaxScale = 0x800;
        unk_214 = 0x52;
        mState = 1;
        /* The add sits INSIDE the integer cast, which is load-bearing: this is
           not interchangeable with `mdCcAc_c.vulnFlags |= 0x8000`
           even though 0x190 is that field (+0x1c). */
        *(int *)(((int)this + 0x190)) |= 0x8000;
    } else {
        cond = (id == 0xfd);
        if (cond != 0) {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0xaa;
            mMaxScale = 0x1000;
            unk_214 = 0xa4;
            mState = 1;
        } else {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0x96;
            mMaxScale = 0x2000;
            unk_214 = 0x147;
            mdCcAcPos_c.radius = 0x64000;
            mdCcAcPos_c.height = 0x64000;
            if (GetBitInDeathTable() != 0)
                mAlive = 0;
            else
                mAlive = 1;
        }
    }

    unk_21f = (unsigned char)(param1 & 0xf);
    return 1;
}
