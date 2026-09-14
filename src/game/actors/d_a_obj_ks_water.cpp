//cpp
/* Production translation unit for ov017/daObjKsWater_c, hand-curated.
 * 7 function(s), .text 0x021111a0..0x021114b8.
 *
 * The draining water inside the sunken ship (profile KS_MIZU, actor 62).
 * RTTI ov017:0x02111bb4 names daObjKsWater_c; _ZTS at 0x02111bc0;
 * _ZTV address point 0x02111bf8; g_profile_KS_MIZU at 0x02111bd4.
 * overlay_actors.md: ov017 SHIP_WATER(62). Ugly RTTI name is final.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * inline destructor in include/daObjKsWater_c.h emits the retail D1/D0 pair
 * first and emits no D2 body.
 *
 * deslop leftovers:
 * - TextureTransformer::SetFile / dBgW_KcMbg::SetFile 6az: InitResources
 *   passes Fix12<int> by value; the header method form size-DIFFs.
 * - Sound::PlayLong: Behavior's long water sound; Sound.h has no PlayLong.
 * - SharedFilePtr +4: InitResources' TextureTransformer::Prepare reads the
 *   BMD at data_ov017_02111c88 +4; SharedFilePtr.h has no fields.
 * - data_ov017_02111c80 / 02111c88 / 02111a60 / 02111a94 KCL / BMD / BTA / CLPS
 *   handles; this TU consumes them, overlay .bss/.data owns them.
 * - no Player.h.
 * - homeless _ZN10dBgActor_cD2Ev from the inline dtor chain (deadstrip).
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN14daObjKsWater_cD1Ev.cpp
 *   [1] 0x021111ec  src/_ZN14daObjKsWater_cD0Ev.cpp
 *   [2] 0x0211124c  src/_ZN14daObjKsWater_c16CleanupResourcesEv.cpp
 *   [3] 0x02111290  src/_ZN14daObjKsWater_c6RenderEv.cpp
 *   [4] 0x021112c4  src/_ZN14daObjKsWater_c8BehaviorEv.cpp
 *   [5] 0x021113c0  src/_ZN14daObjKsWater_c13InitResourcesEv.cpp
 *   [6] 0x02111480  src/daObjKsWater_c_classInit.c
 */

#include "daObjKsWater_c.h"
#include "SharedFilePtr.h"
#include "TreasureChest.h"

struct BMD_File;
struct BTA_File;
struct KCL_File;
struct CLPS_Block;

extern "C" {
unsigned _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned a, unsigned b, unsigned c,
                                           void *pos, unsigned e);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer *self, BTA_File *file, int flags, int speed, u32 startFrame);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale, s16 angY,
    CLPS_Block *clps);

extern SharedFilePtr data_ov017_02111c80;
extern SharedFilePtr data_ov017_02111c88;
extern BTA_File      data_ov017_02111a60;
extern CLPS_Block    data_ov017_02111a94;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjKsWater_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: ShipWater_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02111484 loads 0x340 -- the class's own size -- into the header's leaf
 * operator new; 0x02111494 calls dBgActor_c's C2, the store at 0x021114a0
 * lays down this class's vptr, and TextureTransformer C1 runs on the member
 * at 0x320. The null check is the one `new` itself emits. The implicit
 * constructor is what keeps the intervening construction inlined; declaring
 * a constructor of our own would emit a `bl` the factory does not have. */
extern "C" daObjKsWater_c *daObjKsWater_c_classInit()
{
    return new daObjKsWater_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c13InitResourcesEv
int daObjKsWater_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov017_02111c88), 1, -1);
    /* SharedFilePtr.h has no fields; +4 is the loaded BMD pointer. */
    TextureTransformer::Prepare(**(BMD_File **)((char *)&data_ov017_02111c88 + 4),
                                data_ov017_02111a60);
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, &data_ov017_02111a60, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(data_ov017_02111c80),
        &mClsnMat, 0x1000, mAngleY, &data_ov017_02111a94);
    mMeshCollider.Enable(this);
    mOriginalPosY = mPosY;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c8BehaviorEv
int daObjKsWater_c::Behavior()
{
    if (mChestsOpen == 0) {
        int ok = 1;
        dActor_c *p = dActor_c::FindWithActorID(0xd, 0);
        while (p != 0) {
            int state = ((TreasureChest *)p)->mState;
            int cond = 1;
            if (state != 1) {
                if (state != 2) cond = 0;
            }
            if (cond == 0) ok = 0;
            p = dActor_c::FindWithActorID(0xd, p);
        }
        if (ok != 0) mChestsOpen = 1;
    } else {
        int d = mOriginalPosY - mPosY;
        if (d < 0) d = -d;
        if (d < 0x92e000) {
            mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                mSoundID, 3, 0x96, (void *)&mCamSpacePosX, 0);
            mPosY -= 0x5000;
        }
    }
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c6RenderEv
int daObjKsWater_c::Render()
{
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c16CleanupResourcesEv
int daObjKsWater_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov017_02111c88.Release();
    data_ov017_02111c80.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_cD1Ev
// @symbol _ZN14daObjKsWater_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKsWater_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.
 */
