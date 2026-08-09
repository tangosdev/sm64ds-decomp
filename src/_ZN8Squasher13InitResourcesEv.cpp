//cpp
// @symbol _ZN8Squasher13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_FloatOnWaterPlatformWdwRectangle.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Squasher.h"
#include "MeshColliderBase.h"
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void* f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* m, int bmd, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* s);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* f);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* mc, int kcl, struct Matrix4x3* mtx, int scale, short s, void* clps);
void func_020393d4(int* p, int v);
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);
}

int Squasher::InitResources()
{
  int bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov023_02112088);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, bmd, 1, -1);
  _ZN11ShadowModel10InitCuboidEv((char*)&mShadowModel);
  func_ov023_02111308(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  int kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&_ZN32FloatOnWaterPlatformWdwRectangleD1Ev);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x124, kcl, (struct Matrix4x3*)((char*)&unk_2ec), 0x1000, mAngleY, &data_ov064_0211ba4c);
  func_020393d4((int*)((char*)&mMeshCollider), (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  ((MeshColliderBase *)(((char*)this)+0x124))->Enable((Actor *)(((char*)this)));
  unk_31e = 0;
  unk_320 = 0;
  unk_322 = 0;
  return 1;
}
