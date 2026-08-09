//cpp
// @symbol _ZN14KnockDownPlank13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "KnockDownPlank.h"
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern void func_020393d4(int* p, int v);
extern int func_01ffb0a4(void*);
extern int data_ov015_02114534[];
extern int _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
}

int KnockDownPlank::InitResources()
{
  int b = (*(unsigned short*)((char*)&unk_00c) == 0x35) ? 1 : 0;
  if(b) *(int*)((char*)&mVariant) = 1; else *(int*)((char*)&mVariant) = 0;
  int j0 = *(int*)((char*)&mVariant) * 0xc;
  int m = _ZN5Model8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov015_02114534 + j0));
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)((char*)this)+0xd4, m, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  int j = *(int*)((char*)&mVariant) * 0xc;
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov015_02114538 + j));
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)((char*)this)+0x124, k, (char*)((char*)this)+0x2ec, 0x1000, mAngleY, *(void**)((char*)data_ov015_0211453c + j));
  func_020393d4((int*)((char*)&mMeshCollider), (int)&_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  int tmp[3];
  tmp[0] = 0x1000; tmp[1] = 0; tmp[2] = 0;
  func_01ffb0a4((char*)&mMeshCollider);
  func_01ffb07c((char*)((char*)this)+0x124, tmp);
  func_020396d0((int*)((char*)&mMeshCollider), 0xccd);
  *(int*)((char*)&unk_320) = *(int*)((char*)&unk_05c);
  *(int*)((char*)&unk_324) = *(int*)((char*)&unk_060);
  *(int*)((char*)&unk_328) = *(int*)((char*)&unk_064);
  func_ov015_02111fb8(((char*)this), 5);
  return 1;
}
