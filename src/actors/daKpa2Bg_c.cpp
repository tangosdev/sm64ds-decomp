//cpp
/* daKpa2Bg_c -- KOOPA2BG, a dBgActor_c in ov060 that adds mAngleX/Y/ZSpeed to its
 * angles every frame (InitResources zeroes the speeds). The model matrix
 * follows the X and Z angles; the moving mesh collider follows all three. The class name is the ROM's own RTTI
 * spelling (evidence in include/daKpa2Bg_c.h).
 *
 * ROM span 0x02117980..0x02117d1c: D1 through InitResources, then the registry
 * factory daKpa2Bg_c_classInit -- ten functions in ROM order under
 * `#pragma defer_codegen off`.
 *
 * Known limits:
 * - common.h must come first. Matrix4x3 has two 0x30-byte spellings behind
 *   one guard; the two matrix helpers whole-struct-assign the flat s32 m[12]
 *   form, and the class header alone reaches the .r/.t spelling, which
 *   inflates func_ov060_02117a64 and func_ov060_02117ae0.
 * - The three helpers keep their ROM-address names as member names.
 * - Behavior keeps the rematerialised angle pointers, see the comment there.
 * - dBgW_KcMbg::SetFile stays a mangled bridge.
 */

#pragma defer_codegen off

#include "common.h"
#include "daKpa2Bg_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct Kpa2BgModelFilePtr : SharedFilePtr {
    u32 words[2];
    Kpa2BgModelFilePtr(u32 fileID);
    ~Kpa2BgModelFilePtr();
};
struct Kpa2BgCollisionFilePtr : SharedFilePtr {
    u32 words[2];
    Kpa2BgCollisionFilePtr(u32 fileID);
    ~Kpa2BgCollisionFilePtr();
};

extern "C" {

extern Kpa2BgModelFilePtr daKpa2Bg_c_ModelFile;
extern Kpa2BgCollisionFilePtr daKpa2Bg_c_ClsnFile;
extern Matrix4x3 data_020a0e68;
extern CLPS_Block data_ov046_021115bc;

int Sound_PlayIfNotActive(int handle, int a, int b, int c);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *m, s16 ang);
void Vec3_Asr(void *dst, void *src, int shift);

int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
int func_020393d4(void *, void *);

}

// @symbol _ZN10daKpa2Bg_cD1Ev
daKpa2Bg_c::~daKpa2Bg_c()
{
}

/* Plays sound 0x95 through the handle kept in mSoundHandle. */
// @symbol _ZN10daKpa2Bg_c19func_ov060_02117a3cEv
void daKpa2Bg_c::func_ov060_02117a3c()
{
    mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 3, 0x95, 0);
}

/* Rebuilds the collision matrix from the position and the X and Z angles,
   then moves the moving mesh collider to it with the Y angle. */
// @symbol _ZN10daKpa2Bg_c19func_ov060_02117a64Ev
void daKpa2Bg_c::func_ov060_02117a64()
{
    Matrix4x3_FromTranslation(&data_020a0e68,
                              mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    mClsnMat = data_020a0e68;
    mMovingMeshCollider2.Transform(mClsnMat, mAngleY);
}

/* Rebuilds the model matrix the same way from the position shifted down by 3
   bits. The Y angle is not part of this matrix build. */
// @symbol _ZN10daKpa2Bg_c19func_ov060_02117ae0Ev
void daKpa2Bg_c::func_ov060_02117ae0()
{
    int pos[4];
    Vec3_Asr(&pos, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos[0], pos[1],
                              pos[2]);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    mModel2.mat4x3 = data_020a0e68;
}

// @symbol _ZN10daKpa2Bg_c16CleanupResourcesEv
s32 daKpa2Bg_c::CleanupResources()
{
    if (mMovingMeshCollider2.IsEnabled()) {
        mMovingMeshCollider2.Disable();
    }
    daKpa2Bg_c_ModelFile.Release();
    daKpa2Bg_c_ClsnFile.Release();
    return 1;
}

// @symbol _ZN10daKpa2Bg_c6RenderEv
s32 daKpa2Bg_c::Render()
{
    mModel2.Render(0);
    return 1;
}

// @symbol _ZN10daKpa2Bg_c8BehaviorEv
s32 daKpa2Bg_c::Behavior()
{
    /* mAngleX / mAngleY / mAngleZ, reached through rematerialised pointers.
       The plain member form (`mAngleX = mAngleX + mAngleXSpeed;`) is five
       words SHORTER than the ROM here -- measured, 0x54 against 0x68 -- so
       the three stores stay spelled the way the cartridge computes them. */
    s16 *angX = &mAngleX;
    s16 *angY = &mAngleY;

    *angX = *angX + mAngleXSpeed;
    *angY = *angY + mAngleYSpeed;

    {
        s16 *angZ = &mAngleZ;
        *angZ = *angZ + mAngleZSpeed;
    }

    func_ov060_02117ae0();
    func_ov060_02117a64();
    return 1;
}

// @symbol _ZN10daKpa2Bg_c13InitResourcesEv
int daKpa2Bg_c::InitResources()
{
    mModel2.SetFile((BMD_File *)Model::LoadFile(daKpa2Bg_c_ModelFile), 1, -1);
    func_ov060_02117a64();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMovingMeshCollider2,
        (KCL_File *)dBgW_Kc::LoadFile(daKpa2Bg_c_ClsnFile), &mClsnMat, 0x1000,
        mAngleY, &data_ov046_021115bc);
    func_020393d4(&mMovingMeshCollider2, (void *)&dBgW::UpdatePosAndAngs);
    mMovingMeshCollider2.Enable(this);
    mAngleXSpeed = 0;
    mAngleYSpeed = 0;
    mAngleZSpeed = 0;
    mSoundHandle = 0;
    return 1;
}

/* recovered: vtable identified, globals resolved */
/* `new daKpa2Bg_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x570), dBgActor_c's base constructor, the vptr store,
 * then the Model and dBgW_KcMbg member constructors. */
// @symbol daKpa2Bg_c_classInit
extern "C" daKpa2Bg_c *daKpa2Bg_c_classInit(void)
{
    return new daKpa2Bg_c;
}

/* The KOOPA2BG model and collision file handles; the extern block above
 * declares them for CleanupResources / InitResources. */
Kpa2BgModelFilePtr daKpa2Bg_c_ModelFile(1571);
Kpa2BgCollisionFilePtr daKpa2Bg_c_ClsnFile(1572);
