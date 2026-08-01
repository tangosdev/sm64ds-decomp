//cpp
// @symbol _ZN8MadPiano8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MadPiano.h"
struct CylinderClsn;
extern "C" int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern "C" void WithMeshClsn_UpdateContinuous_Veneer(void* p);
extern "C" void func_0203568c(void* p, int v);
extern "C" void func_02035684(void* p, int v);
extern "C" void func_ov063_0211d5f4(void* self);

struct Actor {
  void UpdatePos(struct CylinderClsn*);
  struct Actor* ClosestPlayer();
};
struct MeshColliderBase {
  int IsEnabled();
  void Disable();
};
struct Platform {
  int IsClsnInRange(int, int);
};

int MadPiano::Behavior()
{
  func_ov063_0211ddf4(((char*)this));
  ((struct Actor*)((char*)this))->UpdatePos(0);
  if (Vec3_HorzDist((struct Vector3*)((char*)&unk_6d4), (struct Vector3*)((char*)&mPosX)) > 0x180000) {
    mPosX = unk_068;
    mPosY = unk_06c;
    mPosZ = unk_070;
  }
  {
    int r1 = unk_6cc;
    if (mPosY <= r1) mPosY = r1;
  }
  WithMeshClsn_UpdateContinuous_Veneer((char*)&mWithMeshClsn);
  func_0203568c(((char*)this)+0x50c, 0x159000);
  func_02035684(((char*)this)+0x50c, 0x159000);
  if (*(unsigned char*)((char*)((struct Actor*)((char*)this))->ClosestPlayer()+0x6fb) != 0) {
    if (((struct MeshColliderBase*)((char*)&mMeshCollider))->IsEnabled() != 0)
      ((struct MeshColliderBase*)((char*)&mMeshCollider))->Disable();
  } else {
    ((struct Platform*)((char*)this))->IsClsnInRange(0x1f4000, 0);
  }
  func_ov063_0211d88c(((char*)this));
  func_ov063_0211d828(((char*)this));
  func_ov063_0211d5f4(((char*)this));
  return 1;
}
