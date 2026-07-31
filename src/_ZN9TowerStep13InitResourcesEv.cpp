//cpp
// @symbol _ZN9TowerStep13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TowerStep.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* c, void* file, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* c, void* kcl, void* m, int fix, short s, void* clps);
extern void func_020393d4(int* p, void* v);
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
}

int TowerStep::InitResources()
{
  void* f;
  f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov015_02114a8c);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, f, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov015_02114a84);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      ((char*)this) + 0x124, f, ((char*)this) + 0x2ec, 0x199, unk_08e, &data_ov015_02113654);
  func_020393d4((int*)((char*)&mMeshCollider), (void*)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  unk_31e = 0x3c;
  return 1;
}
