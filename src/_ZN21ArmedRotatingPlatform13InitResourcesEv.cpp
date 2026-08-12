//cpp
// @symbol _ZN10DonutBlock13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "DonutBlock.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern int func_020393d4(void*, void*);
}
extern void* data_ov036_02113d78[];
extern "C" {
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);
}

int DonutBlock::InitResources()
{
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_02113d78[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, mdl, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(this);
  _ZN8Platform19UpdateClsnPosAndRotEv(this);
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov036_02113d78[1]);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, data_ov036_02113d78[2]);
  func_020393d4(&mMeshCollider, &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  unk_31e = -0x80;
  if (mAngleZ != 0) unk_31e = mAngleZ;
  return 1;
}
