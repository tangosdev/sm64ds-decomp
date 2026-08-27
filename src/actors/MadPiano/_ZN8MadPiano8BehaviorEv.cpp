//cpp
// @symbol _ZN8MadPiano8BehaviorEv
/* Advance the piano and its collision, constrain it to its home area, and
 * disable the inherited moving mesh while the nearest player is vanished. */
#include "MadPiano.h"
#include "Player.h"

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *clsn);
void func_0203568c(dBgCh_Actr *clsn, int radius);
void func_02035684(dBgCh_Actr *clsn, int height);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
void func_ov063_0211ddf4(MadPiano *self);
void func_ov063_0211d88c(MadPiano *self);
void func_ov063_0211d828(MadPiano *self);
void func_ov063_0211d5f4(MadPiano *self);
}


int MadPiano::Behavior()
{
  func_ov063_0211ddf4(this);
  UpdatePos(0);
  if (Vec3_HorzDist(&mHomePos, (struct Vector3*)&mPosX) > 0x180000) {
    mPosX = mPrevPosX;
    mPosY = mPrevPosY;
    mPosZ = mPrevPosZ;
  }
  {
    int r1 = mMinPosY;
    if (mPosY <= r1) mPosY = r1;
  }
  dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
  func_0203568c(&mWithMeshClsn, 0x159000);
  func_02035684(&mWithMeshClsn, 0x159000);
  if (ClosestPlayer()->mIsVanish != 0) {
    if (mMeshCollider.IsEnabled() != 0)
      mMeshCollider.Disable();
  } else {
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x1f4000, 0);
  }
  func_ov063_0211d88c(this);
  func_ov063_0211d828(this);
  func_ov063_0211d5f4(this);
  return 1;
}
