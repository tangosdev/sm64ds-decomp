//cpp
/* daObjCtMecha09_c, Tick Tock Clock's moving beam (CT_MECHA09).
 *
 * The whole unit, ov065 .text 0x0211bbdc..0x0211c078, 8 functions: the
 * destructor pair, func_ov065_0211bc88, CleanupResources, Render, Behavior,
 * InitResources and the registry factory daObjCtMecha09_c_classInit. The
 * unit is bounded below by daObjCtMecha08_c's classInit pair (ending at
 * 0x0211bbdc) and above by __sinit_ov065_0211c110 in .init; .text ends at
 * 0x0211c078.
 *
 * The out-of-line destructor is the key function, so this TU emits the
 * vtable and RTTI. Under `#pragma defer_codegen off` it comes out D1, D0,
 * then a D2 the cartridge has no home for, and .text follows source order.
 *
 * InitResources loads one level-overlay word, the CLPS block at 0x02112118,
 * which dsd leaves ambiguous across eleven level overlays. Of those, overlay
 * residency (tools/overlay_residency.py) leaves only ov035, the level overlay
 * that also carries daObjCtMecha10_c, loadable beside ov065, so the reference
 * names data_ov035_02112118.
 */

// leftovers: DropShadowScaleXYZ / IsClsnInRange / dBgW_KcMbg::SetFile stay
// mangled (Fix12<int> by value -- the header forms cannot be called with
// literals; this TU's func_ov065_0211bc88/Behavior/InitResources calls).
// func_ov065_0211bc88 keeps its address name (beam shadow helper, no
// recovered name). The two file-scope handles keep data_ov065_* spellings
// (only names in the tree).

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtMecha09_c_AssetId {
    ASSET_CT_MECHA_OBJ09_BMD = 0x05cc,
    ASSET_CT_MECHA_OBJ09_KCL = 0x05cd
};

#pragma defer_codegen off

#include "daObjCtMecha09_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

/* Resource handles constructed by this TU's static initializer. The
 * constructor and destructor bodies stay out of line; mwcc registers them. */
struct Mecha09ModelFilePtr : SharedFilePtr {
    u32 words[2];

    Mecha09ModelFilePtr(u32 fileID);
    ~Mecha09ModelFilePtr();
};

struct Mecha09ClsnFilePtr : SharedFilePtr {
    u32 words[2];

    Mecha09ClsnFilePtr(u32 fileID);
    ~Mecha09ClsnFilePtr();
};

/* Fix12-by-value calls keep their raw ABI declarations (header forms take
 * Fix12<int> by value, which cannot be called with literals; see
 * include/dActor_c.h). */
extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *matrix, int angle);
extern void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *actor, dExtShadowModel_c *shadow, Matrix4x3 *matrix,
    int scaleX, int scaleY, int scaleZ, u32 opacity);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
extern Mecha09ClsnFilePtr data_ov065_0211d9cc;
extern Mecha09ModelFilePtr data_ov065_0211d9d4;
extern u16 DecIfAbove0_Short(u16 *p);
extern int RandomIntInternal(int *seed);
extern u8 data_0209f2c0;
extern s32 data_ov065_0211d520[];
extern s32 data_0209e650[];
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angleY, CLPS_Block *clps);
}

extern CLPS_Block data_ov035_02112118;

// @symbol _ZN16daObjCtMecha09_cD1Ev
// @symbol _ZN16daObjCtMecha09_cD0Ev
/* The compiler writes the body: this class's vtable, then dBgActor_c's
 * inlined destructor, then the mesh collider and the model. */
daObjCtMecha09_c::~daObjCtMecha09_c()
{
}

// @symbol _ZN16daObjCtMecha09_c19func_ov065_0211bc88Ev
/* Drops the beam's shadow when it is close enough to the ground sample. */
void daObjCtMecha09_c::func_ov065_0211bc88()
{
    int d = mPosY - mGroundY;
    if (d < 0) d = -d;
    if (d > 0x7d0000) return;
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.t.x = mPosX >> 3;
    mShadowMat.t.y = (mGroundY + 0x1000) >> 3;
    mShadowMat.t.z = mPosZ >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMat, 0x1e0000, 0x32000, 0xfa000, 0xf);
}

// @symbol _ZN16daObjCtMecha09_c16CleanupResourcesEv
int daObjCtMecha09_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d9d4.Release();
    data_ov065_0211d9cc.Release();
    return 1;
}

// @symbol _ZN16daObjCtMecha09_c6RenderEv
int daObjCtMecha09_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjCtMecha09_c8BehaviorEv
/* Clock setting data_0209f2c0 picks a vertical speed from data_ov065_0211d520.
 * Setting 2 rerolls direction and a countdown; the last five frames of a leg
 * hold the beam still. Travel is clamped to [mStartPosY, mEndPosY]. */
int daObjCtMecha09_c::Behavior()
{
    u8 setting = data_0209f2c0;
    mVertSpeed = mDirection * data_ov065_0211d520[setting];

    if (setting == 2) {
        if (DecIfAbove0_Short(&mLegTimer) == 0) {
            u16 rnd = (u32)RandomIntInternal(data_0209e650) >> 16;
            if (rnd >= 0x7fff)
                mDirection = 1;
            else
                mDirection = -1;
            mLegTimer = (rnd % 6 + 1) * 30;
            mLegLength = mLegTimer;
        } else if (mLegTimer >= mLegLength - 5) {
            mVertSpeed = 0;
        }
    }

    UpdatePos(0);

    {
        int y = mPosY;
        int lo = mStartPosY;
        int hi = mEndPosY;
        int in = 0;
        if (y >= lo)
            in = (y <= hi);
        if (in == 0) {
            mPosY = (y < lo) ? lo : ((y > hi) ? hi : y);
            mDirection = -mDirection;
        }
    }

    UpdateModelPosAndRotY();
    func_ov065_0211bc88();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN16daObjCtMecha09_c13InitResourcesEv
/* Loads the beam's model and mesh collider, sets the travel range from
 * param1's low byte (0 or 0xff means the default 0x1f4000), and casts a
 * dBgCh_Gnd ray from 0xa000 below the beam for the shadow's ground height.
 * The local dBgCh_Gnd emits the ROM's C1 and D1 calls. dBgW_KcMbg::SetFile
 * stays behind its exact ABI symbol because of its Fix12 by-value parameter. */
int daObjCtMecha09_c::InitResources()
{
    BMD_File *bmd;
    KCL_File *kcl;
    Vector3 pos;

    bmd = (BMD_File *)Model::LoadFile(data_ov065_0211d9d4);
    mModel.SetFile(bmd, 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov065_0211d9cc);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, &data_ov035_02112118);

    mStartPosY = mPosY;
    {
        int distance = param1 & 0xff;
        if (distance != 0xff && distance != 0)
            mEndPosY = distance * 0xa000 + mStartPosY;
        else
            mEndPosY = mStartPosY + 0x1f4000;
    }
    mTerminalVelocity = -0x3c000;
    mVertSpeed = 0x6000;
    mDirection = 1;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y -= 0xa000;
    dBgCh_Gnd ray;
    ray.SetObjAndPos(pos, 0);
    mGroundY = pos.y;
    if (ray.DetectClsn())
        mGroundY = ray.clsnY;
    return 1;
}

// @symbol daObjCtMecha09_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjCtMecha09_c through
 * RTTI, allocation size, vtable identity, and the CT_MECHA09 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical aliases: TTC_MovingBeam, TTC_MovingBeam_Spawn. */
extern "C" daObjCtMecha09_c *daObjCtMecha09_c_classInit()
{
    return new daObjCtMecha09_c();
}

Mecha09ModelFilePtr data_ov065_0211d9d4(ASSET_CT_MECHA_OBJ09_BMD);
Mecha09ClsnFilePtr data_ov065_0211d9cc(ASSET_CT_MECHA_OBJ09_KCL);
