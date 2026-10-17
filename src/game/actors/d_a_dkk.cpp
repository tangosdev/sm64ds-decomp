//cpp
/* DONKAKU / Grindel, the sliding crusher leaf of daDsnBase_c (sibling
 * daDsn_c / Thwomp). Overlay actor 162. daDgr_c in this overlay is DONGURU.
 *
 * ~daDkk_c() is inline in daDkk_c.h, which emits D1 0x021118c8 then D0
 * 0x02111928. InitResources is the key function. Definitions run from the
 * highest address back, which is the order this compiler emits.
 */
// leftovers:
// - func_ov025_021119f4 keeps `unk_39e--`. `unk_39e = unk_39e - 1` and
//   `(u8)(unk_39e - 1)` shrink 0x90 to 0x84.
// - func_ov025_02111a84 keeps `unk_39f++` and `Vector3 v[2]`. Adding one
//   as `unk_39f = unk_39f + 1` shrinks 0xe0 to 0xd4. Two separate Vector3
//   locals DIFF 8 words.
// - Earthquake and Particle::System::NewSimple stay mangled. Fix12<int> by
//   value grows 02111a84 (0xe0 to 0xe8, and 0xe0 to 0x104). The ROM plays
//   the impact through func_0201267c, not Sound::Play.
// - Behavior's IsClsnInRange stays the scalar (void *, Fix12i, Fix12i)
//   call with (0, 0). Fix12<int> by value grows Behavior 0xc0 to 0xdc.
// - InitResources writes the ray's far point before the near point, and
//   calls GetClsnPos twice. The first result is unread. One call, or
//   writing the near point first, shrinks the function.
// - func_ov091_02132ff4, func_ov091_02132e98 and func_ov091_02132e64 stay
//   C calls into daDsnBase_c. The other four func_ov091_* calls are already
//   daDsnBase_c methods.
// - unk_394, unk_39c, unk_39e and unk_39f stay the header's names: ground
//   height, turn target, pause count, landing count.
// - data_ov025_02113814 is the file-table handle. g_profile_DONKAKU stays
//   overlay data.


/* daDkk_c.h pulls common.h before Model.h, so Matrix4x3 is the flat
 * s32 m[12]. Keep dBgCh_Lin.h below that include. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDkk_c_AssetId {
    ASSET_DONKAKU_BMD = 0x02dc,
    ASSET_DONKAKU_KCL = 0x02dd
};

#include "daDkk_c.h"
#include "common.h"
#include "decl_common.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. data_ov025_02113814 stays
 * the file table decl_common.h already declares. */
struct DkkModelFilePtr : SharedFilePtr {
    unsigned int words[2];
    DkkModelFilePtr(unsigned int fileId);
    ~DkkModelFilePtr();
};
struct DkkClsnFileHandler : SharedFilePtr {
    unsigned int words[2];
    DkkClsnFileHandler(unsigned int fileId);
    ~DkkClsnFileHandler();
};
typedef char DkkModelFilePtr_size_must_be_8[sizeof(DkkModelFilePtr) == 8 ? 1 : -1];
typedef char DkkClsnFileHandler_size_must_be_8[sizeof(DkkClsnFileHandler) == 8 ? 1 : -1];

bool ApproachLinear(short &value, short target, short step);

/* decl_common.h already declares the three daDsnBase_c C helpers and
 * data_ov025_02113814. func_0201267c's definition takes (unsigned, const
 * Vector3 *). This TU's calls do not read a result, so the local extern
 * spells (int, void *). */
extern "C" {
extern void func_0201267c(int a, void *b);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *p, Fix12i a, Fix12i b);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *thiz, const Vector3 &v, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int x, int y, int z);
}

// @symbol daDkk_c_classInit
/* DONKAKU factory. The `new` is the whole body. */
extern "C" daDkk_c *daDkk_c_classInit()
{
    return new daDkk_c();
}

// @symbol _ZN7daDkk_c13InitResourcesEv
/* Stores the file table, then either starts already airborne or raycasts
 * down for mProbeHeight. */
int daDkk_c::InitResources()
{
    mFileTable = (s32)data_ov025_02113814;
    int r = Init();
    if (param1 & 1) {
        mState = 6;
    } else {
        mState = 0;
        dBgCh_Lin ray;
        Vector3 va;
        Vector3 vb;
        /* Far point first, then the near point. */
        int x = mPosX;
        vb.x = x;
        int y = mPosY;
        vb.y = y;
        int z = mPosZ;
        va.x = x;
        vb.z = z;
        va.y = y;
        va.z = z;
        vb.y = y + 0x7d0000;
        ray.SetObjAndLine(va, vb, this);
        if (ray.DetectClsn()) {
            /* The first result is unused. */
            Vector3 p1 = ray.GetClsnPos();
            Vector3 p2 = ray.GetClsnPos();
            mProbeHeight = p2.y - 0x190000;
        }
    }
    return r;
}

// @symbol _ZN7daDkk_c8BehaviorEv
/* Eight states, then the shared model and collision update. */
int daDkk_c::Behavior()
{
    switch (mState) {
    case 0: func_ov091_02133020(); break;
    case 1: func_ov091_02132ff4((char *)this); break;
    case 2: func_ov091_02132f04(); break;
    case 3: func_ov091_02132e98((char *)this); break;
    case 4: func_ov091_02132e64((char *)this); break;
    case 5: func_ov025_02111a84(); break;
    case 6: func_ov025_021119f4(); break;
    case 7: func_ov025_021119a4(); break;
    }
    UpdateModelPosAndRotY();
    func_ov091_02133098();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0 ||
        func_ov091_02132dc0() != 0) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN7daDkk_c19func_ov025_02111a84Ev
/* mState 5, the fall. Integrates the drop, and on reaching the stored ground
 * height snaps to it, shakes the camera, spawns the impact particle and hands
 * over to state 6. */
void daDkk_c::func_ov025_02111a84()
{
    Vector3 v[2];
    UpdatePos(0);
    if (mVertSpeed >= 0)
        mVertAccel = -0x4000;
    else
        mVertAccel = -0x8000;
    if (mPosY > unk_394)
        return;
    mPosY = unk_394;
    v[1].x = mPosX;
    v[1].y = mPosY;
    v[1].z = mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, v[1], 0x7d0000);
    unk_39e = 0x3c;
    unk_39f++;
    mState = 6;
    v[0].x = mPosX;
    v[0].y = mPosY;
    v[0].z = mPosZ;
    v[0].y = v[0].y + 0x3c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, v[0].x, v[0].y, v[0].z);
    func_0201267c(0xc7, &mCamSpacePosX);
}

// @symbol _ZN7daDkk_c19func_ov025_021119f4Ev
/* mState 6, the pause after landing. Counts 0x39e down; at zero it either
 * starts the next fall (state 5) or, on the fourth pass, turns to face the
 * opposite way (state 7). */
void daDkk_c::func_ov025_021119f4()
{
    unk_39e--;
    if (unk_39e != 0) return;
    if (unk_39f != 4) {
        mState = 5;
        mVertSpeed = 0x3c000;
        func_0201267c(0xf4, &mCamSpacePosX);
        return;
    }
    mState = 7;
    unk_39c = (s16)(mAngleY + 0x8000);
}

// @symbol _ZN7daDkk_c19func_ov025_021119a4Ev
/* mState 7, the turn. Steps mAngleY toward the target angle in unk_39c; once
 * ApproachLinear reports it has arrived, copies it into mPrevAngleY, clears the
 * pass counter and goes back to state 6. */
int daDkk_c::func_ov025_021119a4()
{
    int arrived = ApproachLinear(mAngleY, unk_39c, 0x400);
    if (arrived == 0) return arrived;
    mPrevAngleY = mAngleY;
    mState = 6;
    unk_39f = 0;
    unk_39e = 0x28;
    return 0x28;
}

// @symbol _ZN7daDkk_c16OnAimedAtWithEggEv
/* Egg-aim height, 206 units. The return is added to the egg's Y. */
int daDkk_c::OnAimedAtWithEgg()
{
    return 0xce000;
}

/* D1 and D0 come from the inline destructor in daDkk_c.h. */
// @symbol _ZN7daDkk_cD1Ev
// @symbol _ZN7daDkk_cD0Ev

/* Retail construction order: model 0x2dc, then collision 0x2dd.
 * mwcc emits __sinit_d_a_dkk.cpp. */
DkkModelFilePtr gDonkakuModelFile(ASSET_DONKAKU_BMD);
DkkClsnFileHandler gDonkakuCollisionFile(ASSET_DONKAKU_KCL);
