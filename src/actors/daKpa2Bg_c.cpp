//cpp
/* daKpa2Bg_c, span 0x02117980..0x02117cdc.
 * #pragma defer_codegen off: the nine functions are in ROM order.
 * daKpa2Bg_c_classInit at 0x02117cdc stays out.
 *
 * common.h comes first. Matrix4x3 has two 0x30-byte spellings behind one
 * guard; the two matrix helpers whole-struct-assign the flat s32 m[12]
 * form. The class header first reaches the .r/.t spelling and inflates
 * func_ov060_02117a64 and func_ov060_02117ae0.
 */

#pragma defer_codegen off

#include "common.h"
#include "daKpa2Bg_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

extern "C" {

extern SharedFilePtr daKpa2Bg_c_ModelFile;
extern SharedFilePtr daKpa2Bg_c_ClsnFile;
extern Matrix4x3 data_020a0e68;
extern CLPS_Block data_ov046_021115bc;

int Sound_PlayIfNotActive(int handle, int a, int b, int c);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *m, s16 ang);
void Vec3_Asr(void *dst, void *src, int shift);

void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
int func_020393d4(void *, void *);
int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);

void func_ov060_02117a3c(char *self);
void func_ov060_02117a64(char *self);
void func_ov060_02117ae0(char *self);

}

// @symbol _ZN10daKpa2Bg_cD1Ev
daKpa2Bg_c::~daKpa2Bg_c()
{
}

extern "C" {

// @symbol func_ov060_02117a3c
void func_ov060_02117a3c(char *self)
{
    *(int *)(self + 0x56c) =
        Sound_PlayIfNotActive(*(int *)(self + 0x56c), 3, 0x95, 0);
}

// @symbol func_ov060_02117a64
void func_ov060_02117a64(char *self)
{
    Matrix4x3_FromTranslation(&data_020a0e68,
                              *(int *)(self + 0x5c), *(int *)(self + 0x60),
                              *(int *)(self + 0x64));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(self + 0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(self + 0x90));
    *(Matrix4x3 *)(self + 0x2ec) = data_020a0e68;
    ((dBgW_KcMbg *)(self + 0x374))->Transform(
        *(Matrix4x3 *)(self + 0x2ec), *(s16 *)(self + 0x8e));
}

// @symbol func_ov060_02117ae0
void func_ov060_02117ae0(char *self)
{
    int pos[4];
    Vec3_Asr(&pos, self + 0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos[0], pos[1],
                              pos[2]);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(self + 0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(self + 0x90));
    *(Matrix4x3 *)(self + 0x340) = data_020a0e68;
}

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
    s16 *angX = (s16 *)(((int)((char *)this) + 0x8c));
    s16 *angY = (s16 *)(((int)((char *)this) + 0x8e));

    *angX = *angX + mAngleXSpeed;
    *angY = *angY + mAngleYSpeed;

    {
        s16 *angZ = (s16 *)(((int)((char *)this) + 0x90));
        *angZ = *angZ + mAngleZSpeed;
    }

    func_ov060_02117ae0((char *)this);
    func_ov060_02117a64((char *)this);
    return 1;
}

// @symbol _ZN10daKpa2Bg_c13InitResourcesEv
int daKpa2Bg_c::InitResources()
{
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(&daKpa2Bg_c_ModelFile);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel2, mdl, 1, -1);
  func_ov060_02117a64(((char*)this));
  kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&daKpa2Bg_c_ClsnFile);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMovingMeshCollider2, kcl, &mClsnMat, 0x1000, mAngleY, &data_ov046_021115bc);
  func_020393d4(&mMovingMeshCollider2, &_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  ((dBgW *)&mMovingMeshCollider2)->Enable((dActor_c *)(((char*)this)));
  mAngleXSpeed = 0;
  mAngleYSpeed = 0;
  mAngleZSpeed = 0;
  unk_56c = 0;
  return 1;
}
