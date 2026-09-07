//cpp
// @symbol _ZN14TTC_MovingBeam13InitResourcesEv
/* recovered: real C++ method on the shared actor and collision-query classes
 *
 * The previous C++-mode source still hand-spelled this function's mangled
 * name and rebuilt TTC_MovingBeam and dBgCh_Gnd as private byte layouts. The
 * shared declarations now carry the period-accurate inheritance and lifecycle
 * evidence: TTC_MovingBeam owns its model/collider through dBgActor_c, while
 * the local dBgCh_Gnd automatically emits the ROM's C1 and D1 calls.
 *
 * dBgW_KcMbg::SetFile remains behind its exact ABI symbol because its Fix12i
 * by-value parameter is a known mwccarm language-mode wall. Everything else
 * below is expressed through the real C++ types and named members.
 */
#include "TTC_MovingBeam.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angleY, CLPS_Block *clps);

extern SharedFilePtr data_ov065_0211d9d4;
extern SharedFilePtr data_ov065_0211d9cc;
extern CLPS_Block func_02112118;

int TTC_MovingBeam::InitResources()
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
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, &func_02112118);

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
