//cpp
// @symbol _ZN14SquarePathLift13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SquarePathLift.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(int* p, void* v);
extern void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
struct SFP { void* a; void* b; void* c; };
extern struct SFP data_ov052_021125a0;
}

int SquarePathLift::InitResources()
{
  void* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov052_021125a0.a);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, f, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  void* f2 = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov052_021125a0.b);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, f2, &mClsnMat, 0x199, mAngleY, data_ov052_021125a0.c);
  func_020393d4((int*)((char*)&(*(u8 *)&mMeshCollider)), &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  _ZN7PathPtr6FromIDEj(((char*)this)+0x320, (*(s32 *)&param1)&0xff);
  mPathDir = 1;
  mHorzSpeed = 0xa000;
  return 1;
}
