//cpp
#include "types.h"
// @symbol _ZN13UpDownLiftBbh13InitResourcesEv
/* recovered: real C++ method over the reconstructed platform hierarchy */
#include "UpDownLiftBbh.h"
#include "SharedFilePtr.h"

/* dBgW_KcMbg::SetFile takes Fix12<int> by value. A faithful member call hits
   mwccarm wall 6az, so this call retains the exact scalar ABI spelling. */
extern "C" {
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angleY, CLPS_Block *clps);
extern void func_020393d4(void *p, void *v);
extern void func_020393c4(void *p, void *v);
extern void func_ov095_02136788(void *a, void *b, void *c);
}

extern SharedFilePtr *data_ov095_02136f68[];
extern SharedFilePtr *data_ov095_02136f74[];
extern CLPS_Block *data_ov095_021375a4[];

int UpDownLiftBbh::InitResources()
{
  int idx;
  void *new_var;
  u16 t;
  int b;
  unk_349 = 0;
  t = actorID;
  b = (int) (t == 0x20);
  if (b != 0)
  {
    mVariant = 0;
    goto load;
  }
  b = (int) (t == 0x21);
  if (b != 0)
  {
    mVariant = 1;
    goto load;
  }
  b = (int) (t == 0x83);
  if (b != 0)
  {
    mVariant = 2;
    unk_349 = 1;
    goto load;
  }
  return 0;
  load:
  idx = mVariant;

  mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov095_02136f68[idx]), 1, -1);
  new_var = (void *)&dBgW::UpdatePosWithTransform;
  UpdateClsnPosAndRot();
  idx = mVariant;
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(*data_ov095_02136f74[idx]),
      &mClsnMat, 0x199, mAngleY, data_ov095_021375a4[idx]);
  func_020393d4(&mMeshCollider, new_var);
  func_020393c4(&mMeshCollider, (void *)&func_ov095_02136788);
  mRider = 0;
  mState = 0;
  /* Top of the shaft is where it was placed; the spawn word says how far down
     it travels, and it stops halfway on the way back up. */
  if (unk_349 == 2)
  {
    mTopY = mPosY + ((u16)mPrevAngleZ << 12);
  }
  else
  {
    mTopY = mPosY;
  }
  mBottomY = mTopY - ((u16)mPrevAngleX << 12);
  mMiddleY = (mTopY + mBottomY) / 2;
  unk_346 = 0;
  mIsArmed = 1;
  mIsRidden = 0;
  mSoundHandle = 0;
  return 1;
}
