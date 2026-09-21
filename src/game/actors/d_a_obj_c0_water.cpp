//cpp
/**
 * d_a_obj_c0_water.cpp
 * Object - Castle basement water
 *
 * ov012/daObjC0Water_c, 7 function(s), .text 0x02111450..0x02111768.
 * RTTI ov012:0x021123c4 names daObjC0Water_c; _ZTS at 0x021123d0;
 * _ZTV address point 0x02112408; g_profile_C0_WATER at 0x021123e4.
 * overlay_actors.md: ov012 BASEMENT_WATER(35). Ugly RTTI name is final.
 *
 * mwccarm emits ordinary text sections in reverse source order, so the
 * ROM-high factory is defined first. The inline destructor in
 * include/daObjC0Water_c.h emits the retail D1/D0 pair first and emits no
 * D2 body.
 *
 * deslop leftovers:
 * - TextureTransformer::SetFile / dBgW_KcMbg::SetFile 6az: InitResources
 *   passes Fix12<int> by value; the header method form size-DIFFs.
 * - Sound::PlayLong / Sound::PlaySecretSound come from Sound.h, which now
 *   declares both. The ROM mangle is RK7Vector3 (const Vector3 &); a pointer
 *   argument is byte-identical, so this caller alone does not prove the
 *   reference spelling -- the header's definition-side reading does.
 *   SharedFilePtr.h has no fields: Prepare reads the BMD at
 *   data_ov012_021124d0 +4.
 * - data_ov012_021124c8 / 021124d0 / 02111c24 / 02111c90 KCL / BMD / BTA /
 *   CLPS handles; this TU consumes them, overlay .bss/.data owns them.
 * - data_0209caa0[2] & 0x80000 is the same group flag
 *   daObjC0_Switch_c::OnGroundPounded writes; this water drains when it is
 *   set. data_0209f32c is the arm9 water-height word this TU publishes.
 * - no Player.h.
 * - homeless _ZN10dBgActor_cD2Ev from the inline dtor chain (deadstrip).
 */

/* daObjC0Water_c.h FIRST: it pulls in dBgActor_c.h, which must reach
   common.h ahead of Model.h or the wrong Matrix4x3 spelling wins. */
#include "daObjC0Water_c.h"
#include "SharedFilePtr.h"
#include "dMap_c.h"
#include "Sound.h"

struct BMD_File;
struct BTA_File;
struct KCL_File;
struct CLPS_Block;

extern "C" {
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer *self, BTA_File *file, int flags, int speed, u32 startFrame);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale, s16 angY,
    CLPS_Block *clps);

extern SharedFilePtr data_ov012_021124d0;
extern SharedFilePtr data_ov012_021124c8;
extern BTA_File      data_ov012_02111c24;
extern CLPS_Block    data_ov012_02111c90;
extern int           data_0209caa0[];
extern int           data_0209f32c;
}

// @symbol daObjC0Water_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: BasementWater_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 832 = 0x340 is the class's own size, loaded into fBase_c's size_t
 * operator new; dBgActor_c's C2, this class's vptr store, and
 * TextureTransformer C1 on the member at 0x320 are the implicit
 * constructor. The null check is the one `new` itself emits. Declaring a
 * constructor of our own would emit a `bl` the factory does not have. */
extern "C" daObjC0Water_c *daObjC0Water_c_classInit()
{
    return new daObjC0Water_c();
}

// @symbol _ZN14daObjC0Water_c13InitResourcesEv
int daObjC0Water_c::InitResources()
{
    Model::LoadFile(data_ov012_021124d0);
    dBgW_Kc::LoadFile(data_ov012_021124c8);
    /* SharedFilePtr.h has no fields; +4 is the loaded file pointer. */
    mModel.SetFile(*(BMD_File **)((char *)&data_ov012_021124d0 + 4), 1, 0x14);
    TextureTransformer::Prepare(**(BMD_File **)((char *)&data_ov012_021124d0 + 4),
                                data_ov012_02111c24);
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, &data_ov012_02111c24, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        *(KCL_File **)((char *)&data_ov012_021124c8 + 4),
        &mClsnMat, 0x1000, mAngleY, &data_ov012_02111c90);
    mMeshCollider.Enable(this);
    mLoweredY = mPosY - 0x4b0000;
    if (data_0209caa0[2] & 0x80000) {
        mPosY = mLoweredY;
    }
    return 1;
}

// @symbol _ZN14daObjC0Water_c8BehaviorEv
int daObjC0Water_c::Behavior()
{
    if (mWasJustDrained != 0)
        Sound::PlaySecretSound(this, &mSoundTimer);
    if (mPosY <= mLoweredY) {
        mPosY = mLoweredY;
    } else if (data_0209caa0[2] & 0x80000) {
        mSoundID = Sound::PlayLong(
            mSoundID, 3, 0x96, *(const Vector3 *)&mCamSpacePosX, 0);
        mPosY -= 0x5000;
        mWasJustDrained = 1;
        if (mPosY <= mLoweredY) {
            mPosY = mLoweredY;
            dMap_c::UpdateLevelSpecific();
        }
    }
    data_0209f32c = mPosY;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    return 1;
}

// @symbol _ZN14daObjC0Water_c6RenderEv
int daObjC0Water_c::Render()
{
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjC0Water_c16CleanupResourcesEv
int daObjC0Water_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov012_021124d0.Release();
    data_ov012_021124c8.Release();
    return 1;
}
