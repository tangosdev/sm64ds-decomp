//cpp
/* Production translation unit for ov102/daObjSimpleBg_c.
 * 6 function(s), .text 0x02148a80..0x02148e9c. One mesh and one moving
 * collider, shared by the six profiles named in the header. BK_TOWER (actor
 * 0x31) in sublevel 7 does not spawn above y 0xdac000 until star 1 is
 * collected, unless the level was entered for that star.
 *
 * NAME: _ZTS15daObjSimpleBg_c is "15daObjSimpleBg_c" at ov102 0x0214e0cc;
 * _ZTI at 0x0214e0c0 reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The tree previously called the class FortressTower
 * (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02148a80), D0
 * (0x02148ac4), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 * The six daObjSimpleBg_c_classInit_* factories follow at 0x02148e9c and stay
 * one-function sources.
 *
 * Leftover: dBgW_KcMbg::SetFile takes Fix12<int> by value, so it stays
 *   mangled; the method spelling was measured to size-DIFF.
 * Leftover: dBgActor_c::IsClsnInRangeOnScreen stays mangled because
 *   dBgActor_c.h does not declare it.
 * The 0xc-stride file rows are local: only this TU indexes those tables.
 */

#include "decl_common.h"
#include "daObjSimpleBg_c.h"
#include "SharedFilePtr.h"

struct FileRow {
    SharedFilePtr *file;
    u8 pad[8];
};
typedef char FileRow_size_must_be_0xc[sizeof(FileRow) == 0xc ? 1 : -1];

struct ClpsRow {
    CLPS_Block *block;
    u8 pad[8];
};
typedef char ClpsRow_size_must_be_0xc[sizeof(ClpsRow) == 0xc ? 1 : -1];

extern "C" {
extern FileRow data_ov102_0214e188[];
extern FileRow data_ov102_0214e18c[];
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern u8 data_0209f2d8[];
int IsStarCollectedInCurLevel(int starID);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mat, int scale, s16 angY,
    CLPS_Block *clps);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
}

/* ROM order. Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN15daObjSimpleBg_cD1Ev
// @symbol _ZN15daObjSimpleBg_cD0Ev
daObjSimpleBg_c::~daObjSimpleBg_c()
{
}

// @symbol _ZN15daObjSimpleBg_c16CleanupResourcesEv
int daObjSimpleBg_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov102_0214e188[mVariant].file->Release();
    data_ov102_0214e18c[mVariant].file->Release();
    return 1;
}

// @symbol _ZN15daObjSimpleBg_c6RenderEv
int daObjSimpleBg_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjSimpleBg_c8BehaviorEv
int daObjSimpleBg_c::Behavior()
{
    int range = 0;
    switch (actorID) {
    case 0x31:  range = 0x900000; break;
    case 0x3b:  range = 0x600000; break;
    case 0x3d:  range = 0x600000; break;
    case 0x12a: range = 0x600000; break;
    case 0x154: range = 0x300000; break;
    case 0x155: range = 0x300000; break;
    }
    int on = (data_0209f2d8[0] == 1);
    if (on) {
        if (!mMeshCollider.IsEnabled())
            mMeshCollider.Enable(this);
    } else {
        _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, range, 0);
    }
    return 1;
}

// @symbol _ZN15daObjSimpleBg_c13InitResourcesEv
int daObjSimpleBg_c::InitResources()
{
    u16 id = actorID;
    u8 idx;

    switch (id) {
    case 0x31:  mVariant = 2; break;
    case 0x3b:  mVariant = 1; break;
    case 0x3d:  mVariant = 0; break;
    case 0x12a: mVariant = 3; break;
    case 0x154: mVariant = 4; break;
    case 0x155: mVariant = 5; break;
    }

    idx = mVariant;
    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov102_0214e188[idx].file), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    idx = mVariant;
    if (idx == 0 || idx == 2) {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov102_0214e18c[idx].file);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
            ((ClpsRow *)data_ov102_0214e190)[idx].block);
    } else {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov102_0214e18c[idx].file);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
            ((ClpsRow *)data_ov102_0214e190)[idx].block);
    }

    {
        int b31 = (int)(actorID == 0x31);
        if (b31 != 0 && data_0209f2f8 == 7) {
            if (data_0209f220 != 1) {
                if (IsStarCollectedInCurLevel(1) != 0)
                    goto ret1;
            }
            if (mPosY >= 0xdac000)
                return 0;
        }
    }
ret1:
    return 1;
}
