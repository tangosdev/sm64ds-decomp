//cpp
// @symbol _ZN19FloatOnLavaPlatform13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatOnLavaPlatform.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern int func_020393d4(void*, void*);
extern int func_020393c4(void*, void*);
}
extern char data_ov022_02114558[];
extern "C" {
extern int _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);
}

int FloatOnLavaPlatform::InitResources()
{
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov022_02114558);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, mdl, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov022_02114550);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, data_ov064_0211bb0c);
  func_020393d4(&mMeshCollider, &_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  func_020393c4(&mMeshCollider, &func_ov022_0211193c);
  unk_320 = mPosY;
  return 1;
}
