//cpp
// @symbol _ZN7daDgr_c13InitResourcesEv
/* daDgr_c::InitResources -- vtable slot 0. Real C++ method over the shared
 * header: loads the swinging platform's model and its KCL, points the mesh
 * collider's update hook at dBgW::UpdatePosAndAngs, zeroes the facing angle the
 * KCL was baked against, and parks the swing state -- resting height, angular
 * speed, phase, stage, direction and both particle handles.
 *
 * (This used to be an extern "C" free function over a raw `char *` with every
 * field reached by literal offset. Converting it to a real method and naming
 * the fields is byte-exact under the pinned 2004/b56 -- checked with
 * build_pin.verify in tools/build_pin.py.)
 *
 * The two func_ov025_* helpers still take a `char *`: decl_common.h spells them
 * that way and this file does not own that header. */
#include "daDgr_c.h"
#include "decl_common.h"

typedef int Fix12i;
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct CLPS_Block;
struct ModelBase;

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" KCL_File* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg*, KCL_File*, const Matrix4x3&, Fix12i, short, CLPS_Block&);
extern "C" void func_020393d4(int* p, int v);

extern SharedFilePtr data_ov025_02113a68;
extern SharedFilePtr data_ov025_02113a60;
extern CLPS_Block data_ov025_02112c28;
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

s32 daDgr_c::InitResources()
{
    func_ov025_02111344((char*)this);
    func_ov025_021112e0((char*)this);
    {
        BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov025_02113a68);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)&mModel, bmd, 1, -1);
    }
    {
        KCL_File* kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov025_02113a60);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, mClsnMat, 0x1000, mAngleY, data_ov025_02112c28);
    }
    func_020393d4((int*)&mMeshCollider,
                  (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mAngleY = 0;
    mBasePosY = mPosY;
    mAngleXSpeed = 0;
    mPhaseTimer = 0;
    mSwingStage = 0;
    mSwingDir = 0;
    mDustParticle2 = 0;
    mDustParticle1 = mDustParticle2;
    return 1;
}
