//cpp
// @symbol _ZN8MadPiano8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MadPiano.h"
struct dCc_c;
extern "C" int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern "C" void func_0203568c(void* p, int v);
extern "C" void func_02035684(void* p, int v);
extern "C" void func_ov063_0211d5f4(void* self);

struct dActor_c {
  void UpdatePos(struct dCc_c*);
  struct dActor_c* ClosestPlayer();
};
/* dBgW is the real class now, through this actor's header, which types
   mMeshCollider as its dBgW_KcMbg derived class. IsEnabled and Disable are
   non-virtual there, so the two direct `bl`s are unchanged; redefining the real
   class ICEs mwccarm (CClass.c:3328). */
struct dBgActor_c {
  int IsClsnInRange(int, int);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);


int MadPiano::Behavior()
{
  func_ov063_0211ddf4(((char*)this));
  ((struct dActor_c*)((char*)this))->UpdatePos(0);
  if (Vec3_HorzDist((struct Vector3*)((char*)&unk_6d4), (struct Vector3*)((char*)&mPosX)) > 0x180000) {
    mPosX = mPrevPosX;
    mPosY = mPrevPosY;
    mPosZ = mPrevPosZ;
  }
  {
    int r1 = unk_6cc;
    if (mPosY <= r1) mPosY = r1;
  }
  dBgCh_Actr_UpdateContinuous_Veneer((char*)&mWithMeshClsn);
  func_0203568c(((char*)this)+0x50c, 0x159000);
  func_02035684(((char*)this)+0x50c, 0x159000);
  if (*(unsigned char*)((char*)((struct dActor_c*)((char*)this))->ClosestPlayer()+0x6fb) != 0) {
    if (((struct dBgW*)((char*)&mMeshCollider))->IsEnabled() != 0)
      ((struct dBgW*)((char*)&mMeshCollider))->Disable();
  } else {
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_((struct dBgActor_c*)((char*)this), 0x1f4000, 0);
  }
  func_ov063_0211d88c(((char*)this));
  func_ov063_0211d828(((char*)this));
  func_ov063_0211d5f4(((char*)this));
  return 1;
}
