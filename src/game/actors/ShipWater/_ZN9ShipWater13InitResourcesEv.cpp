//cpp
// @symbol _ZN9ShipWater13InitResourcesEv
/* ShipWater::InitResources -- load the water model and its BTA texture
 * animation, load and place the mesh collider, and remember the starting
 * surface height so Behavior can raise the water from it.
 *
 * THE COLLISION FILE'S HANDLE WAS NAMED FROM THE WRONG OVERLAY. The C form
 * passed `_ZN18TiltingPlatformBfsD0Ev', an ov045 FUNCTION that happens to sit
 * at 0x02111c80 -- the same address as ov017's own `data_ov017_02111c80', which
 * is what this actually is. Overlays share address space, so both names
 * relocate to the same word and the byte gate could not tell them apart (see
 * "Reloc slots are wildcards" in notes/mwccarm-codegen.md). Named correctly here.
 *
 * The two SetFile calls stay extern-C mangled free functions -- the same
 * arrangement LavaSeesaw and Scuttlebug use, for the same reason: both take
 * Fix12<int> by value, and Fix12<int> is an aggregate with no converting
 * constructor from the int literals these calls pass.
 *
 * The `+ 4' on the model handle is SharedFilePtr's loaded-file pointer.
 * include/SharedFilePtr.h declares no fields on purpose -- the layout is not
 * recovered and the files that spell it locally disagree -- so the offset stays
 * raw and says what it is rather than inventing a member.
 */
#include "ShipWater.h"
#include "SharedFilePtr.h"
#include "Model.h"

extern "C" {
extern SharedFilePtr data_ov017_02111c80;   /* the KCL handle */
extern SharedFilePtr data_ov017_02111c88;   /* the BMD handle */
extern BTA_File data_ov017_02111a60;
extern CLPS_Block data_ov017_02111a94;

void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer *self, BTA_File *file, int flags, int speed, u32 startFrame);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale, s16 angY,
    CLPS_Block *clps);
}

int ShipWater::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov017_02111c88), 1, -1);
    TextureTransformer::Prepare(**(BMD_File **)((char *)&data_ov017_02111c88 + 4),
                                data_ov017_02111a60);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, &data_ov017_02111a60, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(data_ov017_02111c80),
        &mClsnMat, 0x1000, mAngleY, &data_ov017_02111a94);
    mMeshCollider.Enable(this);
    mOriginalPosY = mPosY;
    return 1;
}
